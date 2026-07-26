#include "explainer.h"
#include <stdio.h>

void explain_translation(const char* original_input, const ExecutionIR* ir, const AdaptedCommand* adapted, CapabilitySupport cap) {
    printf("\n--- CmdBridge Explanation ---\n");
    printf("Input: %s\n", original_input);
    printf("Intent: %d\n", ir->operation);
    
    printf("\nCapability: ");
    if (cap == CAPABILITY_SUPPORTED) {
        printf("Supported\n");
    } else if (cap == CAPABILITY_PARTIALLY_SUPPORTED) {
        printf("Partially Supported\n");
    } else {
        printf("Unsupported\n");
        return; // Nothing to explain for adapter if unsupported
    }

    printf("Reason: ");
    switch (adapted->score.reason) {
        case REASON_DIRECT_MAPPING:
            printf("DIRECT_MAPPING\n");
            break;
        case REASON_APPROXIMATE_TRANSLATION:
            printf("APPROXIMATE_TRANSLATION\n");
            break;
        case REASON_FLAG_NOT_AVAILABLE:
            printf("FLAG_NOT_AVAILABLE\n");
            break;
        case REASON_UNSUPPORTED_PERMISSION_MODEL:
            printf("UNSUPPORTED_PERMISSION_MODEL\n");
            break;
        case REASON_OS_LIMITATION:
            printf("OS_LIMITATION\n");
            break;
        default:
            printf("UNKNOWN\n");
            break;
    }

    if (adapted->score.reason != REASON_DIRECT_MAPPING) {
        printf("Detail: %s %s does not implement `%s` natively (or uses a different syntax).\n", adapted->target_os, adapted->target_shell, original_input);
        printf("Suggested Alternative: %s\n", adapted->score.suggested_alternative);
    }
    
    printf("\nAdapted Command:\n");
    printf("  %s\n", adapted->native_command);
    
    printf("\nAnalysis Score:\n");
    printf("  Native Compatibility: %d%%\n", adapted->score.native_score);
    printf("  Confidence: %d%%\n\n", adapted->score.confidence);
}
