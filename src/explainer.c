#include "explainer.h"
#include <stdio.h>

void explain_translation(const char* original_input, const ExecutionIR* ir, const AdaptedCommand* adapted) {
    printf("\nTranslation:\n");
    printf("  %s (%s) \xE2\x86\x92 %s %s (%s)\n\n", 
           ir->input_shell, original_input, 
           adapted->target_os, adapted->target_shell, adapted->native_command);
           
    printf("Reason:\n");
    printf("  %s %s does not implement `%s` natively (or uses a different syntax).\n", adapted->target_os, adapted->target_shell, original_input);
    printf("  `%s` provides equivalent functionality.\n\n", adapted->native_command);
    
    printf("Analysis Score:\n");
    printf("  Native Compatibility: %d%%\n", adapted->score.native_score);
    printf("  Confidence: %d%%\n\n", adapted->score.confidence);
}
