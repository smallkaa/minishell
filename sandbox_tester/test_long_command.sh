#!/bin/bash

# Create a very long command: echo a a a a a ... (repeated many times)
WORD="a"
REPEAT=2000000  # This will likely go over 1MB with spaces
OUTFILE="long_command.sh"

# Generate the command
{
    echo -n "echo"
    for ((i=0; i<$REPEAT; i++)); do
        echo -n " $WORD"
    done
    echo
} > "$OUTFILE"

echo "✅ Generated $OUTFILE with $(wc -c < $OUTFILE) bytes"

# Show a preview (first 100 chars)
echo -n "Command preview: "
head -c 100 "$OUTFILE"
echo "..."

echo "🚀 To run it:"
echo "./minishell < $OUTFILE"
