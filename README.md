# Functions

## Terminal input management

- __readline__
```c
char *readline (const char *prompt);
```
The function reads a line from the terminal and return it, using prompt as a prompt.

- __rl_clear_history__
```c
void rl_clear_history (void);
```
The function clears the history list by deleting all of the entries, in the same manner as the History library’s clear_history() function.

- __rl_on_new_line_with_prompt__
```c
int rl_on_new_line_with_prompt (void);
```
The function tells the update functions that we have moved onto a new (empty) line, usually after outputting a newline.

- __rl_replace_line__
```c
void rl_replace_line (const char *text, int clear_undo);
```
The function replaces the contents of rl_line_buffer with text.

- __rl_redisplay__
```c
void rl_redisplay (void);
```
The function changes what’s displayed on the screen to reflect the current contents of rl_line_buffer.

- __add_history__
```c
void add_history (const char *string);
```
The function places string at the end of the history list.

## Standard I/O

- __printf__
```c
int printf(const char *format, ...);
```
The function produces output according to a format; writes output to stdout, the standard output stream.

- __write__
```c
ssize_t write(int fd, const void *buf, size_t nbyte);
```
The function writes nbyte bytes from the buffer pointed to by buf to the file associated with the open file descriptor, fd.

## File Handling

- __open__
```c
int open(const char *path, int oflag, ... );
```
The function creates an open file description that refers to a file and a file descriptor that refers to that open file description.

- __read__
```c
ssize_t read(int fd, void *buf, size_t nbyte);
```
The function reads up to count bytes from file descriptor fd into the buffer starting at buf.

- __close__
```c
int close(int fd);
```
The function closes a file descriptor fd, so that it no longer refers to any file and may be reused.

- __access__
```c
int access(const char *pathname, int mode);
```
The function checks whether the calling process can access the file pathname.

## Directory & Path Management

- __getcwd__
```c
char *getcwd(char *buf, size_t size);
```
The function copies an absolute pathname of the current working directory to the array pointed to by buf which is of length size.

- __chdir__
```c
int chdir(const char *path);
```
The function changes the current working directory of the calling process to the directory specified in path.

- __opendir__
```c
DIR *opendir(const char *name);
```
The function function opens a directory stream corresponding to the directory name, and returns a pointer to the directory stream.

- __readdir__
```c
struct dirent *readdir(DIR *dirp);
```
The function returns a pointer to a dirent structure representing the next directory entry in the directory stream  pointed to by dirp.

- __closedir__
```c
int closedir(DIR *dirp);
```
The function closes the directory stream associated with dirp.

## File & Metadata Information

- __stat__
```c
int stat(const char *pathname, struct stat *statbuf);
```
The function retrieve information about the file pointed to by pathname.

- __lstat__
```c
int lstat(const char *pathname, struct stat *statbuf);
```
The function is identical to stat(), except that if  if pathname is a symbolic link, then it returns information about the link itself, not the file that the link refers to.

- __fstat__
```c
int fstat(int fd, struct stat *statbuf);
```
The function is identical to stat(), except that the file about which information is to be retrieved is specified by the file descriptor fd.

- __unlink__
```c
int unlink(const char *path);
```
The function removes a link to a file. If path names a symbolic link, unlink() shall removes the symbolic link named by path and shall not affect any file or directory named by the contents of the symbolic link. Otherwise, unlink() shall remove the link named by the pathname pointed to by path and shall decrement the link count of the file referenced by the link.

##  Process Management

- __fork__
```c
pid_t fork(void);
```
The function creates a new process by duplicating the calling process.

- __wait__
```c
pid_t wait(int *wstatus);
```
The system call suspends execution of the calling thread until one of its children terminates. The call wait(&wstatus) is equivalent to: waitpid(-1, &wstatus, 0);

- __waitpid__
```c
pid_t waitpid(pid_t pid, int *wstatus, int options);
```
The system call suspends execution of the calling thread until a child specified by pid argument has changed state.

- __wait3__
```c
 pid_t wait3(int *wstatus, int options, struct rusage *rusage);
```
_This function are nonstandard; in new programs, the use of  waitpid(2) or waitid(2) is preferable._

It is equivalent to: waitpid(-1, wstatus, options);

- __wait4__
```c
 pid_t wait4(pid_t pid, int *wstatus, int options, struct rusage *rusage);
```
_This function are nonstandard; in new programs, the use of  waitpid(2) or waitid(2) is preferable._

It is equivalent to: waitpid(pid, wstatus, options);

- __execve__
```c
 int execve(const char *pathname, char *const argv[], char *const envp[]);
```
The function  executes the program referred to by pathname.  This causes the program that is currently being run by the calling process to be replaced with a new program, with newly initialized stack, heap, and (initialized and uninitialized) data segments.

- __kill__
```c
int kill(pid_t pid, int sig);
```
The system call can be used to send any signal to any process group or process.

- __exit__
```c
void exit(int status);
```
The function causes normal process termination and the least significant byte of status (i.e., status & 0xFF) is returned to the parent.

## Signal Handling

- __signal__
```c
sighandler_t signal(int signum, sighandler_t handler);
```
_Avoid its use: use sigaction(2) instead._

The function sets the disposition of the signal signum to handler, which is either SIG_IGN, SIG_DFL, or the address of a programmer defined function (a "signal handler").

- __sigaction__
```c
int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
```
The system call is used to change the action taken by a process on receipt of a specific signal.

- __sigemptyset__
```c
int sigemptyset(sigset_t *set);
```
The function initializes the signal set given by set to empty with all signals excluded from the set.

- __sigaddset__
```c
int sigaddset(sigset_t *set, int signum);
```
The function add signal signum from set.

## Pipe & Terminal Interaction

- __pipe__
```c
struct fd_pair pipe();
```
The funcrion creates a pipe, a unidirectional data channel that can be used for interprocess communication. The array pipefd is  used to return two file descriptors referring  to the ends of the pipe.

pipefd[0] refers to the read end of the pipe.

pipefd[1] refers to the  write  end  of the pipe.

Data written to the write end of the pipe is buffered by the kernel until it is read from the read end of the pipe.

- __dup__
```c
int dup(int oldfd);
```
The  dup() system call creates a copy of the file descriptor oldfd, using the lowest-numbered unused file descriptor for the new descriptor.

- __dup2__
```c
int dup2(int oldfd, int newfd);
```
The dup2() system call performs the same task as dup(), but instead of using the lowest-numbered unused file descriptor, it uses the file  descriptor number specified in newfd. If the file descriptor newfd was previously open, it is silently closed before being reused.

- __isatty__
```c
int isatty(int fd);
```
The function tests whether fd is an open file descriptor referring to a terminal.

- __ttyname__
```c
char *ttyname(int fd);
```
The function returns a pointer to the null-terminated
pathname of the terminal device that is open on the file descriptor fd, or NULL on error (for example, if fd is not connected to a terminal).

- __ttyslot__
```c
int ttyslot(void);
```
_The legacy function ttyslot() returns the index of the current user's entry in some file._

- __ioctl__
```c
int ioctl(int fd, unsigned long request, ...);
```
The system call manipulates the underlying device parameters of special files. In particular, many operating characteristics of character special files (e.g., terminals) may be controlled with ioctl() requests. The argument fd must be an  open  file  descriptor.

The second argument is a device-dependent request code.

The third argument is an untyped pointer to memory. It's traditionally char *argp.

## Environment Variables

- __getenv__
```c
char *getenv(const char *name);
```
The function searches the environment list to find the environment variable name, and returns a pointer to the corresponding value string.

## Terminal Settings & Formatting

- __tcsetattr__
```c
int tcsetattr(int fd, int optional_actions, const struct termios *termios_p);
```
The function sets the parameters associated with the terminal (unless support is required from the underlying hardware that  is not available) from the termios structure referred to by termios_p.

- __tcgetattr__
```c
int tcgetattr(int fd, struct termios *termios_p);
```
The function gets the parameters associated with the object referred by fd and stores them in  the termios structure referenced by termios_p.

- __tgetent__
```c
int tgetent(char *bp, const char *name);
```
The routine loads the entry for name. It returns 1 on success, 0 if there is no such entry, and -1 if the terminfo database could not be found. The emulation ignores the buffer pointer bp.

- __tgetflag__
```c
int tgetflag(char *id);
```
The routine gets the boolean entry for id, or zero if it is not available.

- __tgetnum__
```c
int tgetnum(char *id)
```
The routine gets the numeric entry for id, or -1 if it is not available.

- __tgetstr__
```c
char *tgetstr(char *id, char **area);
```
The routine returns the string entry for id, or zero if it is not available. Use tputs to output the returned string. The return value will also be copied to the buffer pointed to by area, and the area value will be updated to point past the null ending this value.

- __tgoto__
```c
char *tgoto(const char *cap, int col, int row);
```
The routine instantiates the parameters into the given capability. The output from this routine is to be passed to tputs.

- __tputs__
```c
int tputs(const char *str, int affcnt, int (*putc)(int));
```
The routine is described on the curs_terminfo(3X) manual page. It can retrieve capabilities by either termcap or terminfo name.

## Memory managment

- __malloc__
```c
void *malloc(size_t size);
 ```
The function allocates size bytes and returns a pointer to the allocated memory.

- __free__
```c
void free(void *ptr);
```
 The function shall cause the space pointed to by ptr to be deallocated; that is, made available for further allocation.

## Error output

- __strerror__
```c
char *strerror(int errnum);
```
The function returns a pointer to a string that describes the error code passed in the argument errnum, possibly using the LC_MESSAGES part of the current locale to select the appropriate  language.


- __perror__
```c
void perror(const char *s);
```
The function produces a message on standard error describing the last error encountered during a call to a system or library function.

# Structure for Minishell

## Core Shell
### 1. Initialization & Prompt Handling
- Setup input loop (`while (1)`)
- Display shell prompt (`$_`)
- Read user input
- Ignore empty lines
- Handle Ctrl+D to exit

### 2. Input Parsing & Tokenization
- Split input into tokens (commands & arguments)
- Handle whitespace & separators
- Identify builtins and external commands
- Store history (`readline()`, `add_history()`)

### 3. Executing Commands
- Find executables in `$PATH`
- Handle absolute and relative paths
- Fork and execute external commands (`fork*()`, `execve*()`, `waitpid*()`)

---

## Shell Features
### 4. Pipes & Process Control (`|`)
- Parse `cmd1 | cmd2`
- Create pipes (`pipe()`)
- Redirect stdout of first command -> stdin of second

### 5. Input & Output Redirections (`<`, `>`, `>>`, `<<`)
- Open files (`open*()`)
- Redirect stdin/stdout (`dup2*()`)
- Handle **append mode (`>>`)** and **heredoc (`<<`)**

### 6. Handling Quotes (`""`, `''`)
- Preserve spaces inside quotes
- Ignore metacharacters inside `'`
- Expand `$VAR` inside `"` but not `'`

### 7. Expanding Variables (`$VAR`, `$?`)
- Expand environment variables
- Implement `$?` for the last exit status

---

## More Features
### 8. Builtin Commands
- Implement builtins (`cd`, `pwd`, `export`, `unset`, `env`, `echo`, `exit`)
- No `fork()` for builtins

### 9. Signal Handling (`Ctrl+C`, `Ctrl+D`, `Ctrl+\`)
- Handle **SIGINT (`Ctrl+C`)** -> Display new prompt
- Handle **SIGQUIT (`Ctrl+\`)** -> Ignore or quit process
- Handle **EOF (`Ctrl+D`)** -> Exit shell

## Bonus Features
### 10. Logical Operators (`&&`, `||`)
- Execute `cmd2` only if `cmd1` succeeds (`&&`) or fails (`||`)
- Handle **(parentheses)** for grouping


### 11. Wildcards (`*`)
- Expand `*` to match filenames in current directory (`opendir()`, `readdir()`)
