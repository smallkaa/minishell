#!/usr/bin/env bash
#
# test_echo_builtin.sh
# --------------------
# A test script for validating the "echo" built-in command in Minishell.
#
# Usage:
#   ./test_echo_builtin.sh
#
# Explanation:
#   - This script uses a helper function `test_echo` to run a command in a
#     Bash subshell (bash -c "cmd").
#   - It compares the actual output to the expected output.
#   - If they match, it prints [PASS], otherwise [FAIL].
#   - The script also verifies that `echo` behaves consistently with and without `-n`.

###############################################################################
# Function: test_echo
# -------------------
# Arguments:
#   1) The actual command to run (e.g. "echo hello")
#   2) The expected output (expected printed text)
#   3) A short description of the test
#
# Steps:
#   1) Print the test case being run.
#   2) Run "bash -c $cmd" in a subshell, capturing its output.
#   3) Compare the result to the expected output.
#   4) Print [PASS] or [FAIL] accordingly.
###############################################################################
test_echo() {
  local cmd="$1"
  local expected="$2"
  local desc="$3"

  # Print test description
  echo "Running Test: $desc..."
  echo "     Command: $cmd"
  echo "      Expect: \"$expected\""

  # Run the command in a Bash subshell, capturing its output
  actual=$(bash -c "$cmd")

  # Compare actual vs. expected output
  if [[ "$actual" == "$expected" ]]; then
    echo "      PASS: \"$actual\" == \"$expected\""
  else
    echo "      FAIL: \"$actual\" != \"$expected\""
  fi

  echo  # Blank line for readability
}

###############################################################################
# Main Script
###############################################################################
echo "=== echo Built-in Tests ==="
echo

# ----------------------------------------------------------------------------
# Basic echo tests
# ----------------------------------------------------------------------------
test_echo "echo Hello, World!" "Hello, World!" "Basic echo"
test_echo "echo 'Hello, World!'" "Hello, World!" "Single-quoted string"
test_echo "echo \"Hello, World!\"" "Hello, World!" "Double-quoted string"

# ----------------------------------------------------------------------------
# Handling of `-n` flag (no trailing newline)
# ----------------------------------------------------------------------------
test_echo "echo -n Hello" "Hello" "Echo without newline"
test_echo "echo -n 'Hello, World!'" "Hello, World!" "Echo without newline (quoted)"
test_echo "echo -n" "" "Echo -n with no arguments (should print nothing)"
test_echo "echo -n ''" "" "Echo -n with empty string"

# ----------------------------------------------------------------------------
# Handling multiple arguments
# ----------------------------------------------------------------------------
test_echo "echo Hello World" "Hello World" "Multiple arguments"
test_echo "echo -n Hello World" "Hello World" "Multiple arguments without newline"
test_echo "echo -n Hello       World" "Hello World" "Multiple spaces should be preserved"

# ----------------------------------------------------------------------------
# Handling special characters
# ----------------------------------------------------------------------------
test_echo "echo \$HOME" "$HOME" "Environment variable substitution"
test_echo "echo 'Hello\tWorld'" "Hello\tWorld" "Tab inside string (not interpreted)"
test_echo "echo '\\\\'" "\\\\" "Backslash should be printed as single \\"

# ----------------------------------------------------------------------------
# Handling empty arguments
# ----------------------------------------------------------------------------
test_echo "echo ''" "" "Empty string"
test_echo "echo ' '" " " "Single space"
test_echo "echo \"  \"" "  " "Double spaces"

# ----------------------------------------------------------------------------
# Handling newlines
# ----------------------------------------------------------------------------
test_echo "echo -n -n Hello" "Hello" "Multiple -n flags"
test_echo "echo Hello && echo World" "Hello"$'\n'"World" "Multiple echo commands"
test_echo "echo -n Hello && echo World" "Hello"$'\n'"World" "First line no newline"

# ----------------------------------------------------------------------------
# Argument Parsing (invalid flags)
# ----------------------------------------------------------------------------
test_echo "echo -z Hello" "-z Hello" "Invalid flag should be treated as argument"
test_echo "echo -- Hello" "-- Hello" "Double-dash should be treated as argument"
test_echo "echo '-nHello'" "-nHello" "Flag without space should be treated as argument"

echo "All tests completed."
