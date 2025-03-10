#include "minishell.h"

static void	safe_close(int fd, t_cmd *cmd, char *msg)
{
	if (fd != STDIN_FILENO && fd != STDOUT_FILENO && fd != STDERR_FILENO && fd >= 0)
	{
		if (close(fd) == -1)
		{
			cmd->minishell->exit_status = EXIT_FAILURE;
			print_error_exit(msg, EXIT_FAILURE);
		}
	}
}

static void	safe_dup2(int oldfd, int newfd, t_cmd *cmd, char *msg)
{
	if (dup2(oldfd, newfd) == -1)
	{
		/*
		 * If dup2 fails, try closing oldfd. It might not help much,
		 * but at least we avoid leaving it open. Then report the error.
		 */
		safe_close(oldfd, cmd, msg);
		cmd->minishell->exit_status = EXIT_FAILURE;
		print_error_exit(msg, EXIT_FAILURE);
	}
}

static void	execute(t_cmd *cmd, int in_fd)
{
	if (cmd->binary == NULL)
    {
        fprintf(stderr, "%s: command not found\n", cmd->argv[0]);
        cmd->minishell->exit_status = 127;
        return;
    }
	/*
	 * Only dup2 if in_fd is not STDIN.
	 */
	if (in_fd != STDIN_FILENO)
	{
		safe_dup2(in_fd, STDIN_FILENO, cmd, "dup2 1");
		safe_close(in_fd, cmd, "close 2");
	}

	/* If it's a builtin, just execute the builtin. */
	if (is_builtin(cmd))
		exec_builtin(cmd);
	else
	{
		/* Otherwise, run the external command. */
		execve(cmd->binary, cmd->argv, cmd->minishell->env);
		cmd->minishell->exit_status = EXIT_FAILURE;
		print_error_exit(cmd->binary, EXIT_FAILURE);
	}
}

/*
 * This runs in the child after a fork. It sets up output redirection if
 * there's a next command (i.e., we need to write to the pipe), then calls
 * handle_redirections, then execute().
 */
static void	exec_fork_child(t_cmd *cmd, int in_fd, int fd[2])
{
	/*
	 * If there's a next command, dup2 our pipe's write-end to STDOUT,
	 * then close both ends of the pipe.
	 */
	if (cmd->next)
	{
		safe_dup2(fd[1], STDOUT_FILENO, cmd, "dup2");
		safe_close(fd[1], cmd, "close 4");
		safe_close(fd[0], cmd, "close 5");
	}

	/*
	 * If this command has redirections, let handle_redirections()
	 * set them up (it may change in_fd or set new fds).
	 */
	if (cmd->in_redir || cmd->out_redir)
		handle_redirections(cmd, in_fd);

	execute(cmd, in_fd);
}

/*
 * fork_and_execute():
 *   - Forks off a child for the current command.
 *   - Waits for the child to finish, stores exit status.
 *   - Closes the write-end of the pipe if there is a next command.
 *   - Returns the read-end of the pipe (for use by the next command),
 *     or -1 if no next command.
 */
static int	fork_and_execute(t_cmd *cmd, int in_fd, int fd[2])
{
	pid_t	pid;
	int		status;

	pid = fork();
	if (pid == -1)
	{
		cmd->minishell->exit_status = EXIT_FAILURE;
		print_error_exit("fork", EXIT_FAILURE);
	}
	if (pid == 0)
		exec_fork_child(cmd, in_fd, fd);

	/* Parent code: close pipe write-end if there's a next command. */
	if (cmd->next)
		safe_close(fd[1], cmd, "close 6");

	/* Wait for child to finish. */
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		cmd->minishell->exit_status = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		cmd->minishell->exit_status = 128 + WTERMSIG(status);

	if (cmd->next)
	{
		/*
		 * If there's a next command, this command's read-end
		 * becomes the next command's in_fd.
		 */
		if (in_fd != STDIN_FILENO)
			safe_close(in_fd, cmd, "close 8");
		return (fd[0]);
	}
	else
	{
		/* If no next command, close the read-end if valid. */
		if (fd[0] >= 0)
			safe_close(fd[0], cmd, "close 7");
		return (-1);
	}
}

/*
 * exec_cmd():
 *   - Loops over the linked list of commands.
 *   - Creates a pipe for each command if there's a next command.
 *   - Calls fork_and_execute() to run the command.
 *   - Closes and updates in_fd accordingly.
 */
void	exec_cmd(t_cmd *cmd)
{
	int	fd[2];
	int	in_fd;
	int	new_in_fd;

	in_fd = 0;
	while (cmd)
	{
		if (cmd->next && pipe(fd) == -1)
		{
			cmd->minishell->exit_status = EXIT_FAILURE;
			print_error_exit("pipe", EXIT_FAILURE);
		}
		new_in_fd = fork_and_execute(cmd, in_fd, fd);

		if (in_fd != 0)
			safe_close(in_fd, cmd, "close 9");

		in_fd = new_in_fd;
		cmd = cmd->next;
	}

	/* At the very end, if in_fd is still open, close it. */
	if (in_fd >= 0 && in_fd != STDIN_FILENO)
		safe_close(in_fd, cmd, "close 10");
}
