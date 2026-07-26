# CmdBridge Design Principles

## Intents Over Translation

CmdBridge is not fundamentally about translating command names—it's about mapping semantic operations onto the capabilities of different operating systems and shells. 

Instead of thinking about how `ls` becomes `Get-ChildItem`, think about how the semantic intent of "List Directory" (`INTENT_LIST_DIRECTORY`) is expressed in Bash vs. PowerShell.

The internal representation is always an `OperationIntent` paired with logical flags (`recursive`, `force`, etc.). 

### Architecture Flow
`Input -> Translator (IR) -> Safety -> Capability -> Adapter -> Execution`

- **Adapters never parse raw strings**: They only work with the Intermediate Representation (IR).
- **Safety never performs translation**: It only checks the semantic risk level of the IR.
- **Translators never execute commands**: They only map raw input to IR.
- **Dictionaries are data, not code**: Every new command maps to an Intent via JSON definition files.

This modular architecture ensures extensibility, security, and consistent cross-platform compatibility.
