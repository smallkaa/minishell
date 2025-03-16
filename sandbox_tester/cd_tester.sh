#!/usr/bin/env bash
#
# test_cd_paths.sh
# ----------------
# A test script for validating the "cd" built-in command in Minishell
# when given only a relative or absolute path.
#
# Usage:
#   ./test_cd_paths.sh
#
# Explanation:
#   - This script uses a helper function `test_cd_path` to run a `cd` command
#     in a **new Bash subshell** and capture the resulting working directory.
#   - It compares the actual working directory to the expected value.
#   - If they match, it prints [PASS], otherwise [FAIL].
#   - The script ensures that `cd` is only tested with **absolute and relative paths**.
#   - It also tests **directories without access permissions**.

###############################################################################
# Function: test_cd_path
# ----------------------
# Arguments:
#   1) The command to run (e.g., "cd /tmp")
#   2) The expected output (expected working directory after `cd`)
#   3) A short description of the test
#
# Steps:
#   1) Print the test case being run.
#   2) Run `bash -c 'cd path; pwd'` in a **new Bash subshell**, capturing its output.
#   3) Compare the result to the expected working directory.
#   4) Print [PASS] or [FAIL] accordingly.
###############################################################################
test_cd_path() {
  local cmd="$1"
  local expected="$2"
  local desc="$3"

  # Print test case description
  echo "Running Test: $desc..."
  echo "     Command: $cmd"
  echo "      Expect: \"$expected\""

  # Run the command in a new Bash subshell
  actual=$(bash -c "$cmd; pwd" 2>/dev/null)

  # Compare actual vs. expected output
  if [[ "$actual" == "$expected" ]]; then
    echo "      PASS: \"$actual\" == \"$expected\""
  else
    echo "      FAIL: \"$actual\" != \"$expected\""
  fi

  echo  # Blank line for readability
}

###############################################################################
# Function: test_cd_fail
# ----------------------
# Arguments:
#   1) The command to run (e.g., "cd /root")
#   2) The expected error message (from `cd`)
#   3) A short description of the test
#
# Steps:
#   1) Print the test case being run.
#   2) Run `bash -c 'cd path'` in a **new Bash subshell**, capturing its stderr.
#   3) Compare the error message to the expected output.
#   4) Print [PASS] or [FAIL] accordingly.
###############################################################################
test_cd_fail() {
  local cmd="$1"
  local expected="$2"
  local desc="$3"

  # Print test case description
  echo "Running Test: $desc..."
  echo "     Command: $cmd"
  echo "      Expect: \"$expected\""

  # Run the command in a new Bash subshell, capturing the error message
  actual=$(bash -c "$cmd" 2>&1)

  # Compare actual vs. expected error message
  if [[ "$actual" == *"$expected"* ]]; then
    echo "      PASS: \"$actual\" contains \"$expected\""
  else
    echo "      FAIL: \"$actual\" does NOT contain \"$expected\""
  fi

  echo  # Blank line for readability
}

###############################################################################
# Main Script
###############################################################################
echo "=== cd Path Tests (Absolute & Relative) ==="
echo

# ----------------------------------------------------------------------------
# Setup Temporary Directories
# ----------------------------------------------------------------------------
mkdir -p /tmp/cd_test/dir1
mkdir -p /tmp/cd_test/dir2/subdir
mkdir -p /tmp/cd_test/no_access

# Set permissions to **no access**
chmod 000 /tmp/cd_test/no_access

# ----------------------------------------------------------------------------
# Test Absolute Paths
# ----------------------------------------------------------------------------
test_cd_path "cd /tmp" "/tmp" "cd to /tmp (absolute path)"
test_cd_path "cd /tmp/cd_test/dir1" "/tmp/cd_test/dir1" "cd to absolute directory"
test_cd_path "cd /tmp/cd_test/dir2/subdir" "/tmp/cd_test/dir2/subdir" "cd to nested absolute directory"

# ----------------------------------------------------------------------------
# Test Relative Paths
# ----------------------------------------------------------------------------
test_cd_path "cd /tmp/cd_test; cd dir1" "/tmp/cd_test/dir1" "cd to relative directory"
test_cd_path "cd /tmp/cd_test/dir2; cd subdir" "/tmp/cd_test/dir2/subdir" "cd to nested relative directory"
test_cd_path "cd /tmp/cd_test; cd .." "/tmp" "cd .. moves up one directory"

# ----------------------------------------------------------------------------
# Test Directory with No Access
# ----------------------------------------------------------------------------
test_cd_fail "cd /tmp/cd_test/no_access" "Permission denied" "cd to no-access directory should fail"

# ----------------------------------------------------------------------------
# Cleanup Test Directories
# ----------------------------------------------------------------------------
chmod 755 /tmp/cd_test/no_access  # Restore permissions before deleting
rm -rf /tmp/cd_test

echo "All tests completed."
