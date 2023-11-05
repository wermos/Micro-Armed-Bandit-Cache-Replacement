#!/bin/bash

# Directory where the test files are located
directory_path="./traces"

# Initialize counter
# We know that there are 207 traces in the CRC 2 list
file_count=0

# Iterate through each file in the directory
for file in "$directory_path"/*; do
    # Check if the item is a file
    if [ -f "$file" ]; then
        ((file_count++))  # Increment the counter
        echo "File $file_count: $file"  # Print the file name with the count

        ./bin/champsim < "$file" > output.txt

        # Run the Python script to process the output
        python_script="./scripts/analysis.py"
        output_file="output.txt"

        # Run the Python script and check its output
        python_output=$(python "$python_script" "$output_file")

        # If Python script gives the okay, add the filename to a text file
        if [ "$python_output" = "OK" ]; then
            echo "$file" >> "./memory_intensive_traces.txt"
        else
            echo "$file" >> "./other_traces.txt"
        fi

        rm "$output_file"
    fi
done
