#!/bin/bash

# Check if both arguments are provided
if [ -z "$1" ] || [ -z "$2" ]; then
  echo "Error: Please provide both filename and text string as arguments."
  exit 1
fi

# Extract arguments
writefile="$1"
writestr="$2"


# Create parent directories (if needed)
parent_dir=$(dirname "$writefile")  # Get the directory path from writefile
mkdir -p "$parent_dir" || {
  echo "Error: Could not create directory structure for '$writefile'."
  exit 1
}

# Create the file with content, handle errors
if ! echo "$writestr" > "$writefile"; then
  echo "Error: Could not create file '$writefile'."
  exit 1
fi

echo "File '$writefile' created successfully."

exit 0
