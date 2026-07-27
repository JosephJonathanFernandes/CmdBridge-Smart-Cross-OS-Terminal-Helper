#include "script_processor.h"
#include "translator.h"
#include "adapter.h"
#include "capability.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static void init_report(MigrationReport* report) {
    report->entry_capacity = 128;
    report->entry_count = 0;
    report->entries = (MigrationEntry*)malloc(report->entry_capacity * sizeof(MigrationEntry));
    report->total_lines = 0;
    report->total_commands = 0;
    report->translated = 0;
    report->approximate = 0;
    report->unsupported = 0;
    report->preserved = 0;
}

static void add_entry(MigrationReport* report, const MigrationEntry* entry) {
    if (report->entry_count >= report->entry_capacity) {
        report->entry_capacity *= 2;
        report->entries = (MigrationEntry*)realloc(report->entries, report->entry_capacity * sizeof(MigrationEntry));
    }
    report->entries[report->entry_count++] = *entry;
}

void free_migration_report(MigrationReport* report) {
    if (report && report->entries) {
        free(report->entries);
        report->entries = NULL;
    }
}

// Basic classifier
static LineClassification classify_line(const char* line) {
    char trimmed[1024];
    strncpy(trimmed, line, sizeof(trimmed) - 1);
    trimmed[sizeof(trimmed)-1] = '\0';
    trim_whitespace(trimmed);
    
    if (strlen(trimmed) == 0) return LINE_BLANK;
    if (strncmp(trimmed, "#!", 2) == 0) return LINE_SHEBANG;
    if (trimmed[0] == '#') return LINE_COMMENT;
    
    // Very basic heuristics for variables and control flow
    if (strchr(trimmed, '=') != NULL && !strchr(trimmed, ' ')) {
        // e.g. VAR=value
        return LINE_VARIABLE;
    }
    
    if (strstr(trimmed, "if ") == trimmed || strstr(trimmed, "if[") == trimmed || 
        strstr(trimmed, "for ") == trimmed || strstr(trimmed, "while ") == trimmed ||
        strcmp(trimmed, "then") == 0 || strcmp(trimmed, "else") == 0 || 
        strcmp(trimmed, "fi") == 0 || strcmp(trimmed, "done") == 0) {
        return LINE_CONTROL_FLOW;
    }
    
    if (strchr(trimmed, '|')) {
        return LINE_PIPELINE;
    }
    
    // If not matching above, treat as a potential command
    return LINE_COMMAND;
}

static bool read_and_classify_script(const char* filepath, MigrationReport* report) {
    FILE* file = fopen(filepath, "r");
    if (!file) {
        return false;
    }
    
    char line[1024];
    int line_number = 1;
    
    while (fgets(line, sizeof(line), file)) {
        // Strip trailing newline for internal processing, but we might want to keep it.
        // Let's strip trailing \r and \n
        size_t len = strlen(line);
        while (len > 0 && (line[len-1] == '\n' || line[len-1] == '\r')) {
            line[len-1] = '\0';
            len--;
        }
        
        MigrationEntry entry;
        memset(&entry, 0, sizeof(MigrationEntry));
        entry.line_number = line_number++;
        strncpy(entry.original_line, line, sizeof(entry.original_line) - 1);
        
        entry.classification = classify_line(line);
        entry.status = STATUS_PRESERVED; // Default
        
        report->total_lines++;
        if (entry.classification == LINE_COMMAND) {
            report->total_commands++;
        } else {
            report->preserved++;
        }
        
        add_entry(report, &entry);
    }
    
    fclose(file);
    return true;
}

bool analyze_script(const char* filepath, MigrationReport* out_report) {
    init_report(out_report);
    return read_and_classify_script(filepath, out_report);
}

bool migrate_script(const char* filepath, const char* target_os, const char* target_shell, const char* dict_path, MigrationReport* out_report) {
    init_report(out_report);
    
    if (!read_and_classify_script(filepath, out_report)) {
        return false;
    }
    
    for (int i = 0; i < out_report->entry_count; i++) {
        MigrationEntry* entry = &out_report->entries[i];
        
        if (entry->classification == LINE_COMMAND) {
            // Try translating
            if (translate_input_to_ir(entry->original_line, &entry->ir)) {
                CapabilitySupport cap = negotiate_capability(&entry->ir, target_os, target_shell, dict_path);
                
                if (cap == CAPABILITY_SUPPORTED) {
                    AdaptedCommand adapted;
                    if (adapt_ir_to_native(&entry->ir, target_os, target_shell, dict_path, &adapted)) {
                        strncpy(entry->translated_command, adapted.native_command, sizeof(entry->translated_command) - 1);
                        entry->reason = adapted.score.reason;
                        entry->status = STATUS_TRANSLATED;
                        out_report->translated++;
                    } else {
                        entry->status = STATUS_UNSUPPORTED;
                        out_report->unsupported++;
                    }
                } else if (cap == CAPABILITY_PARTIALLY_SUPPORTED) {
                    AdaptedCommand adapted;
                    if (adapt_ir_to_native(&entry->ir, target_os, target_shell, dict_path, &adapted)) {
                        strncpy(entry->translated_command, adapted.native_command, sizeof(entry->translated_command) - 1);
                        entry->reason = adapted.score.reason;
                        entry->status = STATUS_APPROXIMATE;
                        out_report->approximate++;
                    } else {
                        entry->status = STATUS_UNSUPPORTED;
                        out_report->unsupported++;
                    }
                } else {
                    entry->status = STATUS_UNSUPPORTED;
                    entry->reason = REASON_OS_LIMITATION;
                    out_report->unsupported++;
                }
            } else {
                // Translator didn't understand it
                entry->status = STATUS_UNSUPPORTED;
                entry->reason = REASON_UNKNOWN;
                out_report->unsupported++;
            }
        }
    }
    
    return true;
}

const char* reason_code_to_string(ReasonCode code) {
    switch (code) {
        case REASON_DIRECT_MAPPING: return "REASON_DIRECT_MAPPING";
        case REASON_APPROXIMATE_TRANSLATION: return "REASON_APPROXIMATE_TRANSLATION";
        case REASON_FLAG_NOT_AVAILABLE: return "REASON_FLAG_NOT_AVAILABLE";
        case REASON_UNSUPPORTED_PERMISSION_MODEL: return "REASON_UNSUPPORTED_PERMISSION_MODEL";
        case REASON_OS_LIMITATION: return "REASON_OS_LIMITATION";
        case REASON_UNKNOWN: return "REASON_UNKNOWN";
        default: return "UNKNOWN";
    }
}

bool write_migrated_script(const MigrationReport* report, const char* output_path) {
    FILE* file = fopen(output_path, "w");
    if (!file) return false;
    
    for (int i = 0; i < report->entry_count; i++) {
        const MigrationEntry* entry = &report->entries[i];
        
        if (entry->classification == LINE_COMMAND) {
            if (entry->status == STATUS_TRANSLATED) {
                // Attempt to preserve leading whitespace
                int leading_spaces = 0;
                while (entry->original_line[leading_spaces] == ' ' || entry->original_line[leading_spaces] == '\t') {
                    fputc(entry->original_line[leading_spaces], file);
                    leading_spaces++;
                }
                fprintf(file, "%s\n", entry->translated_command);
            } else if (entry->status == STATUS_APPROXIMATE) {
                fprintf(file, "# CmdBridge Warning: Approximate Translation\n");
                fprintf(file, "# Reason: %s\n", reason_code_to_string(entry->reason));
                int leading_spaces = 0;
                while (entry->original_line[leading_spaces] == ' ' || entry->original_line[leading_spaces] == '\t') {
                    fputc(entry->original_line[leading_spaces], file);
                    leading_spaces++;
                }
                fprintf(file, "%s\n", entry->translated_command);
            } else if (entry->status == STATUS_UNSUPPORTED) {
                fprintf(file, "# CmdBridge Error: Unsupported Command\n");
                fprintf(file, "# Reason: %s\n", reason_code_to_string(entry->reason));
                fprintf(file, "# Original: %s\n", entry->original_line);
            }
        } else {
            // Preserve other lines
            if (entry->classification == LINE_CONTROL_FLOW || entry->classification == LINE_VARIABLE || entry->classification == LINE_PIPELINE) {
                fprintf(file, "# CmdBridge Notice: Preserved unsupported structure\n");
            }
            fprintf(file, "%s\n", entry->original_line);
        }
    }
    
    fclose(file);
    return true;
}

void print_migration_summary(const MigrationReport* report) {
    printf("Migration Summary\n");
    printf("-----------------\n");
    printf("Input lines:   %d\n", report->total_lines);
    printf("Commands:      %d\n", report->total_commands);
    printf("Translated:    %d\n", report->translated);
    printf("Approximate:   %d\n", report->approximate);
    printf("Unsupported:   %d\n", report->unsupported);
    printf("Preserved:     %d\n", report->preserved);
}

bool export_migration_report_json(const MigrationReport* report, const char* json_path) {
    FILE* f = fopen(json_path, "w");
    if (!f) return false;
    
    fprintf(f, "{\n");
    fprintf(f, "  \"total_lines\": %d,\n", report->total_lines);
    fprintf(f, "  \"commands\": %d,\n", report->total_commands);
    fprintf(f, "  \"translated\": %d,\n", report->translated);
    fprintf(f, "  \"approximate\": %d,\n", report->approximate);
    fprintf(f, "  \"unsupported\": %d,\n", report->unsupported);
    fprintf(f, "  \"preserved\": %d,\n", report->preserved);
    fprintf(f, "  \"entries\": [\n");
    
    for (int i = 0; i < report->entry_count; i++) {
        const MigrationEntry* entry = &report->entries[i];
        fprintf(f, "    {\n");
        fprintf(f, "      \"line\": %d,\n", entry->line_number);
        // Basic escaping for JSON
        fprintf(f, "      \"status\": %d\n", entry->status);
        fprintf(f, "    }%s\n", (i == report->entry_count - 1) ? "" : ",");
    }
    
    fprintf(f, "  ]\n");
    fprintf(f, "}\n");
    
    fclose(f);
    return true;
}
