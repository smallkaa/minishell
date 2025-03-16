#!/usr/bin/env bash
#
# test_exit_builtin.sh
# --------------------
# A test script for validating the "exit" built-in command in Bash.
# Now prints each test case before running it.
#
# Usage:
#   ./test_exit_builtin.sh
#
# Explanation:
#   - This script uses a helper function `test_exit` to run a command in a
#     Bash subshell (bash -c "cmd").
#   - It compares the resulting exit code to an expected value.
#   - If they match, it prints [PASS], otherwise [FAIL].
#
#   The script now prints "==> Running Test: ..." before each test case.

###############################################################################
# Function: test_exit
# -------------------
# Arguments:
#   1) The actual command to run (e.g. "exit 42")
#   2) The expected exit status
#   3) A short description of the test
#
# Steps:
#   1) Print the test case being run
#   2) Run "bash -c $cmd" in a subshell, capturing its exit code
#   3) Compare the result to the expected exit code
#   4) Print [PASS] or [FAIL] accordingly
###############################################################################
test_exit() {
  local cmd="$1"
  local expected="$2"
  local desc="$3"

  # Print out which test case is being run:
  echo "Running Test: $desc..."
  echo "     Command: $cmd"
  echo "      Expect: $expected"

  # Run the command in a Bash subshell, capturing its exit code
  bash -c "$cmd" >/dev/null 2>&1
  local actual=$?

  # Compare actual vs. expected
  if [[ "$actual" -eq "$expected" ]]; then
    echo "      PASS:   $actual == $expected"
  else
    echo "      FAIL:   $actual != $expected"
  fi

  echo  # Blank line for readability
}

###############################################################################
# Main Script
###############################################################################
echo "=== exit Built-in Tests ==="
echo

# ----------------------------------------------------------------------------
# Standard Behavior
# ----------------------------------------------------------------------------
test_exit "exit 0" 0 "Exits with status 0"
test_exit "exit 42" 42 "Exits with status 42"
test_exit "exit +42" 42 "Positive number with plus sign"
test_exit "exit -42" $((256 - 42)) "Negative number => mod 256"
test_exit "exit 255" 255 "Exits with status 255 => stays 255"
test_exit "exit -255" $((256 - 255)) "Exits with mod 256 => 1"
test_exit "exit -1" 255 "Exits with status 255 => mod 256"

# ----------------------------------------------------------------------------
# Numeric Argument Edge Cases
# ----------------------------------------------------------------------------
test_exit "exit --42" 2 "--42 => numeric argument required"
test_exit "exit -+42" 2 "-+ => numeric argument required"
test_exit "exit +" 2 "Empty plus => numeric argument required"
test_exit "exit -" 2 "Empty minus => numeric argument required"
test_exit "exit - + " 2 "- plus => numeric argument required"
test_exit "exit 0000000000000000042" 42 "Leading zeros => valid 42"
test_exit "exit -0000000000000000042" $((256 - 42)) "Leading zeros negative => mod 256"

# ----------------------------------------------------------------------------
# Large Numeric Arguments
# ----------------------------------------------------------------------------
test_exit "exit 256" 0 "256 % 256 => 0"
test_exit "exit 9223372036854775807" 255 "Huge positive => becomes 255"
test_exit "exit -9223372036854775808" 0 "Huge negative => mod 256 => 0"
test_exit "exit 9999999999999999999" 2 "Out of range => numeric arg required => 2"

# ----------------------------------------------------------------------------
# Invalid Numeric Formats
# ----------------------------------------------------------------------------
test_exit "exit 1.0" 2 "Decimal => numeric argument required"
test_exit "exit 42abc" 2 "Mixed alnum => numeric argument required"
test_exit "exit abc42" 2 "Prefixed alpha => numeric argument required"

# ----------------------------------------------------------------------------
# Argument Parsing
# ----------------------------------------------------------------------------
test_exit "exit 1 2" 1 "Too many arguments => status 1, no exit"
test_exit "exit 1 a" 1 "Too many => status 1, no exit"
test_exit "exit a 1" 2 "First arg invalid => numeric arg required => 2"
test_exit "exit \"1 2\"" 2 "\"1 2\" => numeric arg required => 2"
test_exit "exit '123 '" 123 "Trailing space => still 123"

# ----------------------------------------------------------------------------
# Special Symbols as Arguments
# ----------------------------------------------------------------------------
test_exit "exit ---" 2 "All dashes => numeric argument required => 2"
test_exit "exit ///" 2 "Slashes => numeric argument required => 2"
test_exit "exit -+-+42" 2 "Hybrid sign => numeric argument required => 2"
test_exit "exit \"++42\"" 2 "++ prefix => numeric argument required => 2"

# ----------------------------------------------------------------------------
# Edge Tests with Quoting
# ----------------------------------------------------------------------------
test_exit "exit \"42abc\"" 2 "Quoted alpha => numeric arg required => 2"
test_exit "exit \" 42\"" 42 "Leading space => parse 42"
test_exit "exit \"\"" 0 "Empty => no args => exit with 0"
test_exit "exit \"1 2\"" 2 "\"1 2\" => numeric arg required => 2"
test_exit "exit '1 2'" 2 "Single-quoted => numeric arg required => 2"
test_exit "exit \" \"" 0 "Whitespace => no numeric => exit 0? (Shell-dependent)"

# ----------------------------------------------------------------------------
# Check Behavior Consistency
# ----------------------------------------------------------------------------
test_exit "exit \$(echo 42)" 42 "Command substitution => 42"

export VAR=123
test_exit "exit \$VAR" 123 "\$VAR => 123"

export VAR="hello"
test_exit "exit \$VAR" 2 "\$VAR => 'hello' => numeric arg required => 2"

test_exit "exit \$HOME" 2 "\$HOME => /home/... => numeric arg required => 2"

echo "All tests completed."
