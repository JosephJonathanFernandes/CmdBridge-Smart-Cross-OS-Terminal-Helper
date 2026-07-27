#!/bin/bash

# Setup environment
$env: MY_DIR="build"

# CmdBridge Error: Unsupported Command
# Reason: REASON_UNKNOWN
# Original: echo "Creating build directory..."
New-Item -ItemType Directory $MY_DIR

# CmdBridge Notice: Preserved unsupported structure
if [ -d $MY_DIR ]; then
# CmdBridge Error: Unsupported Command
# Reason: REASON_UNKNOWN
# Original:     echo "Directory created."
# CmdBridge Notice: Preserved unsupported structure
fi

Copy-Item config.json $MY_DIR/
Remove-Item temp/

# Done
