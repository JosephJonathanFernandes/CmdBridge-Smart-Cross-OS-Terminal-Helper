#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include "ir.h"
#include "dictionary.h"

// Initialize the translator by loading all dictionaries
bool translator_init(const char* dictionary_dir);

// Cleanup dictionaries
void translator_cleanup();

// Parses a raw input string and resolves it into an ExecutionIR
// If the input is a known shell command, it extracts flags and targets
bool translate_input_to_ir(const char* raw_input, ExecutionIR* out_ir);

#endif // TRANSLATOR_H
