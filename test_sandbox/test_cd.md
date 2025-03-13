# Manual Testing Checklist for `cd` Built-in Command


---

### Normal Operation

**1\. Simple directory change**
```bash
# passed
cd /
pwd
# Expected output: /
# exit 0
```

**2\. Change to a subdirectory**
```bash
mkdir test_dir
cd testfile
pwd  # Expected output: current_directory/testdir
```

**3\. Complex relative navigation**
```bash
mkdir -p a/b/c
cd a/b/c
cd ../../..
pwd  # Expected: back to initial directory
```

---

### 📌 Missing Arguments

**4\. `cd` without arguments (should go to `$HOME`)**
```bash
cd
pwd  # Expected output: HOME directory
```

**5\. `cd` when `$HOME` is unset**
```bash
unset HOME
cd  # Expected error: HOME not set
```

---

### 📌 Non-directory Targets

**6\. Attempt to `cd` into a file**
```bash
touch file.txt
cd file.txt  # Expected error: Not a directory
```

**7\. Non-existent directory**
```bash
cd nonexistent  # Expected error: No such file or directory
```

---

### 📌 Permission-related Cases

**8\. Directory without execute permissions**
```bash
mkdir restricted
chmod 666 restricted_dir
cd restricted_dir  # Expected error: Permission denied
```

**9\. Directory with read but no execute permission**
```bash
mkdir noexec
chmod 644 no_exec_dir
cd noexec_dir  # Expected error: Permission denied
```

---

### 📌 Special Paths

**10\. Current directory (.)**
```bash
cd .
pwd  # Expected: no change
```

**11\. Parent directory (..)**
```bash
mkdir -p testdir/subdir
cd test_dir
cd ..
pwd  # Expected: Parent directory
```

---

### 📌 Absolute Path Handling

**12\. Absolute path to `/tmp`**
```bash
cd /tmp
pwd  # Expected: /tmp
```

**12\. Non-existent absolute path**
```bash
cd /does/not/exist  # Expected error: No such file or directory
```

---

### 📌 Special Characters and Spaces

**13\. Directory name with spaces**
```bash
mkdir "dir with spaces"
cd "dir with spaces"
pwd  # Expected: path ends with "dir with spaces"
```

**14\. Directory with special characters**
```bash
mkdir "dir!@#\$%^&*"
cd "dir!@#\$%^&*()"
pwd  # Expected: correct directory path
```

---

### 📌 Handling Variables

**15\. `cd` with environment variable**
```bash
mkdir -p testdir
export DIR=testdir
cd $DIR
pwd  # Expected: inside testdir
```

**16\. Unset or empty variable**
```bash
unset DIR
cd $DIR  # Expected: treated as `cd` with no args, should go to HOME or error
```

---

### 📌 `cd -` Previous Directory

**16\. Return to previous directory**
```bash
mkdir dir1 dir2
cd dir1
cd ../dir2
cd -
pwd  # Expected: dir1
```

**17\. `cd -` without previous directory**
```bash
unset OLDPWD
cd -  # Expected error: OLDPWD not set
```

---

### 📌 Edge cases with $HOME

**18\. Empty HOME variable**
```bash
export HOME=
cd  # Expected error: HOME not set
```

**19\. Unset `$HOME`**
```bash
unset HOME
cd  # Expected error: HOME not set
```

---

### 📌 Path Length Limits

**20\. Extremely long path name**
```bash
mkdir $(printf 'a%.0s' {1..255})
cd $(printf 'a%.0s' {1..300})  # Expected: Filename too long or No such file or directory
```

---

### 📋 Testing Notes
- Check the return code with `echo $?` after each command.
- Compare outputs and error messages with Bash.

Use these tests for thorough manual validation of your `cd` built-in.

