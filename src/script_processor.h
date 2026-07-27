#ifndef SCRIPT_PROCESSOR_H
#define SCRIPT_PROCESSOR_H

#include "ir.h"
#include "adapter.h"
#include "capability.h"
#include <stdbool.h>

typedef enum {
    LINE_BLANK = 0,
    LINE_COMMENT,
    LINE_SHEBANG,
    LINE_VARIABLE,
    LINE_COMMAND,
    LINE_PIPELINE,
    LINE_CONTROL_FLOW,
    LINE_UNKNOWN
} LineClassification;

typedef enum {
    STATUS_PRESERVED = 0,
    STATUS_TRANSLATED,
    STATUS_APPROXIMATE,
    STATUS_UNSUPPORTED
} MigrationStatus;

typedef struct {
    int line_number;
    char original_line[1024];
    LineClassification classification;
    
    // Translation results (if classification == LINE_COMMAND)
    ExecutionIR ir;
    char translated_command[1024];
    ReasonCode reason;
    MigrationStatus status;
} MigrationEntry;

typedef struct {
    MigrationEntry* entries;
    int entry_count;
    int entry_capacity;
    
    // Summary Stats
    int total_lines;
    int total_commands;
    int translated;
    int approximate;
    int unsupported;
    int preserved;
} MigrationReport;

// Core API
bool analyze_script(const char* filepath, MigrationReport* out_report);
bool migrate_script(const char* filepath, const char* target_os, const char* target_shell, const char* dict_path, MigrationReport* out_report);
void free_migration_report(MigrationReport* report);

// Output generation
bool write_migrated_script(const MigrationReport* report, const char* output_path);
void print_migration_summary(const MigrationReport* report);
bool export_migration_report_json(const MigrationReport* report, const char* json_path);

#endif // SCRIPT_PROCESSOR_H
