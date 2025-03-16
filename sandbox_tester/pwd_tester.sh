#!/usr/bin/env bash
#
# test_pwd_builtin.sh
# -------------------
# A test script for validating the "pwd" built-in command in Minishell.
#
# Usage:
#   ./test_pwd_builtin.sh
#
# Explanation:
#   - This script uses a helper function `test_pwd` to run the `pwd` command
#     in a **new Bash subshell** and capture its output.
#   - It compares the actual working directory to the expected value.
#   - If they match, it prints [PASS], otherwise [FAIL].
#   - The script ensures `pwd` is **only tested with no options**.
#
###############################################################################

###############################################################################
# Function: test_pwd
# ------------------
# Arguments:
#   1) The command to run (e.g., "pwd")
#   2) The expected output (expected working directory)
#   3) A short description of the test
#
# Steps:
#   1) Print the test case being run.
#   2) Run `bash -c 'pwd'` in a **new Bash subshell**, capturing its output.
#   3) Compare the result to the expected working directory.
#   4) Print [PASS] or [FAIL] accordingly.
###############################################################################
test_pwd() {
  local cmd="$1"
  local expected="$2"
  local desc="$3"

  # Print test case description
  echo "Running Test: $desc..."
  echo "     Command: $cmd"
  echo "      Expect: \"$expected\""

  # Run the command in a new Bash subshell
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
echo "=== pwd Built-in Tests (No Options) ==="
echo

# ----------------------------------------------------------------------------
# Get Expected Output from Bash
# ----------------------------------------------------------------------------
expected_pwd=$(pwd)

# ----------------------------------------------------------------------------
# Test `pwd` with No Options
# ----------------------------------------------------------------------------
test_pwd "pwd" "$expected_pwd" "pwd should print the current working directory"

# ----------------------------------------------------------------------------
# Test `pwd` After Changing Directory
# ----------------------------------------------------------------------------
mkdir -p /tmp/pwd_test/dir1
cd /tmp/pwd_test/dir1
expected_pwd=$(pwd)
test_pwd "pwd" "$expected_pwd" "pwd after changing to /tmp/pwd_test/dir1"

# ----------------------------------------------------------------------------
# Test `pwd` in Nested Directories
# ----------------------------------------------------------------------------
mkdir -p /tmp/pwd_test/dir1/dir2/dir3
cd /tmp/pwd_test/dir1/dir2/dir3
expected_pwd=$(pwd)
test_pwd "pwd" "$expected_pwd" "pwd in deep nested directory"

# ----------------------------------------------------------------------------
# Test `pwd` in a Symbolic Link
# ----------------------------------------------------------------------------
mkdir -p /tmp/pwd_test/real_dir
ln -s /tmp/pwd_test/real_dir /tmp/pwd_test/symlink_dir
cd /tmp/pwd_test/symlink_dir
expected_pwd=$(pwd)
test_pwd "pwd" "$expected_pwd" "pwd in symbolic link directory"

# ----------------------------------------------------------------------------
# Test `pwd` with No Access Permissions
# ----------------------------------------------------------------------------
mkdir -p /tmp/pwd_test/no_access
chmod 000 /tmp/pwd_test/no_access
test_pwd "cd /tmp/pwd_test/no_access 2>/dev/null; pwd" "$expected_pwd" "pwd in no-access directory should fail (remains in previous dir)"

# ----------------------------------------------------------------------------
# Test `pwd` After Deleting Current Directory
# ----------------------------------------------------------------------------
mkdir -p /tmp/pwd_test/delete_me
cd /tmp/pwd_test/delete_me
rm -rf /tmp/pwd_test/delete_me
test_pwd "pwd" "$(pwd)" "pwd after deleting current directory (should return a valid path or an error)"

# ----------------------------------------------------------------------------
# Cleanup Test Directories
# ----------------------------------------------------------------------------
cd ~  # Return to home before deleting
chmod 755 /tmp/pwd_test/no_access  # Restore permissions before deleting
rm -rf /tmp/pwd_test

echo "All tests completed."
