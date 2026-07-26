#ifndef CAPABILITY_H
#define CAPABILITY_H

#include "ir.h"
#include "dictionary.h"

// Negotiates if a target shell/os supports the given ExecutionIR intent.
CapabilitySupport negotiate_capability(const ExecutionIR* ir, const char* target_os, const char* target_shell, const char* dictionary_dir);

#endif // CAPABILITY_H
