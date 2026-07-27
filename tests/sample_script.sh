#!/bin/bash

# Setup environment
export MY_DIR="build"

echo "Creating build directory..."
mkdir $MY_DIR

if [ -d $MY_DIR ]; then
    echo "Directory created."
fi

cp config.json $MY_DIR/
rm -rf temp/

# Done
