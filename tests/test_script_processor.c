#include "script_processor.h"
#include "test_framework.h"

void test_analyze_script() {
    MigrationReport report;
    
    bool result = analyze_script("tests/sample_script.sh", &report);
    ASSERT_TRUE(result);
    ASSERT_EQ(16, report.total_lines);
    ASSERT_EQ(6, report.total_commands);
    ASSERT_EQ(10, report.preserved);
    
    free_migration_report(&report);
}

void run_all_tests() {
    RUN_TEST(test_analyze_script);
}

TEST_MAIN()
