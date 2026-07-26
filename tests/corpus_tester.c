#include "translator.h"
#include "adapter.h"
#include "test_framework.h"
#include "cJSON.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* read_file(const char* filename) {
    FILE* f = fopen(filename, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long length = ftell(f);
    fseek(f, 0, SEEK_SET);
    char* data = (char*)malloc(length + 1);
    fread(data, 1, length, f);
    data[length] = '\0';
    fclose(f);
    return data;
}

void test_corpus() {
    ASSERT_TRUE(translator_init("../config/dictionary"));

    char* json_data = read_file("../tests/corpus.json");
    ASSERT_TRUE(json_data != NULL);
    if (!json_data) return;

    cJSON* root = cJSON_Parse(json_data);
    ASSERT_TRUE(root != NULL);
    if (!root) {
        free(json_data);
        return;
    }

    cJSON* test_case;
    cJSON_ArrayForEach(test_case, root) {
        const char* input = cJSON_GetObjectItem(test_case, "input")->valuestring;
        const char* host_os = cJSON_GetObjectItem(test_case, "host_os")->valuestring;
        const char* host_shell = cJSON_GetObjectItem(test_case, "host_shell")->valuestring;
        cJSON* expected_ir = cJSON_GetObjectItem(test_case, "expected_ir");
        const char* expected_command = cJSON_GetObjectItem(test_case, "expected_command")->valuestring;

        printf("Testing Corpus: %s -> %s\n", input, host_shell);

        ExecutionIR ir;
        bool parsed = translate_input_to_ir(input, &ir);
        ASSERT_TRUE(parsed);

        if (parsed) {
            cJSON* op = cJSON_GetObjectItem(expected_ir, "operation");
            if (op) ASSERT_EQ(op->valueint, ir.operation);

            cJSON* rec = cJSON_GetObjectItem(expected_ir, "recursive");
            if (rec) ASSERT_EQ(cJSON_IsTrue(rec), ir.recursive);

            cJSON* force = cJSON_GetObjectItem(expected_ir, "force");
            if (force) ASSERT_EQ(cJSON_IsTrue(force), ir.force);

            cJSON* sh = cJSON_GetObjectItem(expected_ir, "show_hidden");
            if (sh) ASSERT_EQ(cJSON_IsTrue(sh), ir.show_hidden);
            
            cJSON* tgt = cJSON_GetObjectItem(expected_ir, "target");
            if (tgt) ASSERT_STR_EQ(tgt->valuestring, ir.target);

            AdaptedCommand adapted;
            bool adapted_ok = adapt_ir_to_native(&ir, host_os, host_shell, "../config/dictionary", &adapted);
            ASSERT_TRUE(adapted_ok);
            if (adapted_ok) {
                ASSERT_STR_EQ(expected_command, adapted.native_command);
            }
        }
    }

    cJSON_Delete(root);
    free(json_data);
    translator_cleanup();
}

void run_all_tests() {
    RUN_TEST(test_corpus);
}

TEST_MAIN()
