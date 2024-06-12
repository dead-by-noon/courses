#!/bin/bash

# Check if both arguments are provided
if [ $# -ne 2 ]; then
    echo "Error: Please provide the directory path and the search string."
    exit 1
fi

# Get the directory path and search string
filesdir="$1"
searchstr="$2"

# Check if filesdir exists and is a directory
if [ ! -d "$filesdir" ]; then
    echo "Error: $filesdir is not a valid directory."
    exit 1
fi

# Function to count the number of matching lines in a file
count_matching_lines() {
    local file="$1"
    local count=0
    while IFS= read -r line; do
        if [[ $line == *"$searchstr"* ]]; then
            ((count++))
        fi
    done < "$file"
    echo "$count"
}

# Function to recursively count files and matching lines
count_files_and_lines() {
    local dir="$1"
    local file_count=0
    local line_count=0

    # Loop through all files and directories
    while IFS= read -r -d '' entry; do
        if [ -f "$entry" ]; then
            ((file_count++))
            lines=$(count_matching_lines "$entry")
            ((line_count += lines))
        fi
    done < <(find "$dir" -type f -print0)

    echo "The number of files are $file_count and the number of matching lines are $line_count"
}

# Invoke the main function
count_files_and_lines "$filesdir"
