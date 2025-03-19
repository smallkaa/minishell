
## Run cmd

```bash
echo hi > file1 > file2 > file3
```

The shell processes redirections from left to right:
1. Opens **`file1`**, redirects **stdout** (`FD 1`).
2. Opens **`file2`**, **overrides** `file1`.
3. Opens **`file3`**, **overrides** `file2`.
4. Final output goes to `file3`. `file1` and `file2` remain empty.

---

## Updated Structures
We will use a **linked list** for redirections:

```c
typedef struct s_redir
{
	t_redir_type	type;	 // >, >>, <, <<
	char			*filename;
	struct s_redir  *next;
}	t_redir;

typedef struct s_cmd
{
	char			**argv;
	char			*binary;
	// delete t_redir	*in_redir;
	// delete t_redir	*out_redir;

	// add a new linked list
	t_redir		 *redirs;  // Linked list of redirections

	struct s_cmd	*next;
	t_mshell		*minishell;
	bool			in_pipe; // maybe no needed ....
}	t_cmd;
```

Tracks multiple redirections per command.

Handles heredoc (`<<`) separately.

## Handle redirs
 Process redirections left to right.

 Use `dup2()` to redirect file descriptors.

```c
void	apply_redirections(t_redir *redirs)
{
	int	fd;
	t_redir	*current;

	current = redirs;
	while (current)
	{
		if (current->type == REDIR_OUT) // >
			fd = open(current->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		else if (current->type == REDIR_APPEND) // >>
			fd = open(current->filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
		else if (current->type == REDIR_IN) // <
			fd = open(current->filename, O_RDONLY);

		if (fd == -1) { perror("open"); exit(1); }
		dup2(fd, STDOUT_FILENO); // Redirect stdout to file
		close(fd);
		current = current->next;
	}
}
```

Opens each file, replacing stdout (`FD 1`).

Only the last redirection takes effect.

---

## Exec cmds

```c
void	exec_cmds(t_cmd *cmd)
{
	pid_t	pid;

	pid = fork();
	if (pid == 0) // Child process
	{
		apply_redirections(cmd->redirs);  // Set up redirections
		foo_exec_command(cmd->argv[0], cmd->argv);  // Run command
		perror("error");
		exit(1);
	}
	else
		waitpid(pid, NULL, 0); // Parent waits
}
```

---

## Handling Heredocs (`<<`)
**Reads input into a temporary file**

**Redirects stdin to the temp file**

```c
void	handle_heredoc(t_redir *redir)
{
	int fd;

	fd = open("/tmp/heredoc", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	char buffer[1024];

	printf("> ");
	while ()
	{
		// do smth
	}
	close(fd);

	fd = open("/tmp/heredoc", O_RDONLY);
	dup2(fd, STDIN_FILENO);
	close(fd);
}
```
Stops reading when delimiter is detected.


