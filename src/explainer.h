#ifndef EXPLAINER_H
#define EXPLAINER_H

#include "ir.h"
#include "adapter.h"

// Generates a structured explanation comparing the original input to the adapted command
void explain_translation(const char* original_input, const ExecutionIR* ir, const AdaptedCommand* adapted, CapabilitySupport cap);

#endif // EXPLAINER_H
