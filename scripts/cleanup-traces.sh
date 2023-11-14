#!/bin/bash

# Check if the correct number of arguments are provided
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <directory_path> <keep_list_file>"
    exit 1
fi

directory_path=$1
keep_list_file=$2

# Check if the directory exists
if [ ! -d "$directory_path" ]; then
    echo "Error: Directory '$directory_path' not found."
    exit 1
fi

# Check if the keep list file exists
if [ ! -e "$keep_list_file" ]; then
    echo "Error: Keep list file '$keep_list_file' not found."
    exit 1
fi

# Iterate over files in the directory
for file in "$directory_path"/*; do
    # Extract the file name from the path
    filename=$(basename "$file")

    # Check if the filename is not in the keep list
    if ! grep -qFx "$filename" "$keep_list_file"; then
        echo "Deleting $filename"
        rm "$file"
    fi
done

echo "Cleanup complete!"
