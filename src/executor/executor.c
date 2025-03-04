#include "minishell.h"

/**
 * Handles input and output redirections.
 * If a file redirection is present, it adjusts the corresponding
 * file descriptors.
 *
 * @param cmd	The command whose redirections need to be handled.
 * @param in_fd	The file descriptor for input redirection.
 * @param envp	The environment variables array.
 */
void	handle_redirection(t_cmd *cmd, int in_fd, char **envp)
{
	int	status;

	if (cmd->in_redir)
		handle_in_redirection(cmd, envp);
	else
	{
		status = dup2(in_fd, STDIN_FILENO);
		if (status == -1)
		{
			if (close(in_fd) == -1)
				print_error_exit("close", EXIT_FAILURE);
			print_error_exit("dup2", EXIT_FAILURE);
		}
	}
	if (cmd->out_redir)
		handle_out_redirection(cmd);
}

/**
 * Forks a new process to execute the given command.
 * - Redirects output to the next command in a pipeline if needed.
 * - Handles input/output redirections.
 * - Calls `execve()` to execute the command.
 *
 * @param cmd	The command to execute.
 * @param in_fd	The file descriptor for input redirection.
 * @param fd	The pipe file descriptors for inter-process communication.
 * @param envp	The environment variables array.
 */
static void	exec_fork_child(t_cmd *cmd, int in_fd, int fd[2], char **envp)
{
	// printf("[DEBUG]: exec_fork_child() input cmd: [%s]\n", cmd->argv[0]);
	if (cmd->next)
	{
		if (dup2(fd[1], STDOUT_FILENO) == -1)
		{
			if (close(fd[1]) == -1)
				print_error_exit("close", EXIT_FAILURE);
			print_error_exit("dup2", EXIT_FAILURE);
		}
		if (close(fd[1]) == -1)
			print_error_exit("close", EXIT_FAILURE);
		if (close(fd[0]) == -1)
			print_error_exit("close", EXIT_FAILURE);
	}
	if (cmd->in_redir || cmd->out_redir)
		handle_redirection(cmd, in_fd, envp);
	execute(cmd, in_fd, envp);
}

/**
 * Forks and executes a command, setting up pipes for
 * inter-process communication.
 * - Waits for the child process to complete before continuing.
 * - Stores the last exit status in shell stuct
 * 	on success - WEXITSTATUS, else 128 + WTERMSIG.
 *
 * @param cmd	The command to execute.
 * @param in_fd	The file descriptor for input redirection.
 * @param fd	The pipe file descriptors for communication.
 * @param envp	The environment variables array.
 */
static void	fork_and_execute(t_cmd *cmd, int in_fd, int fd[2], char **envp)
{
	pid_t	pid;
	int		status;

	pid = fork();
	if (pid == -1)
		print_error_exit("fork", EXIT_FAILURE);
	if (pid == 0)
		exec_fork_child(cmd, in_fd, fd, envp);
	if (cmd->next)
	{
		if (close(fd[1]) == -1)
			print_error_exit("close", EXIT_FAILURE);
		in_fd = fd[0];
	}
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		update_last_exit_status(cmd, WEXITSTATUS(status));
	else if (WIFSIGNALED(status))
		update_last_exit_status(cmd, 128 + WTERMSIG(cmd->minishell->exit_stat));
}

/**
 * Executes a series of commands, handling pipelines and
 * forking child processes.
 * - Handles multiple commands in a loop.
 * - Sets up pipes when needed.
 * - Waits for child processes to complete.
 *
 * @param cmd	The first command in the command list.
 * @param envp	The environment variables array.
 */
static void	exec_cmd(t_cmd *cmd, char **envp)
{
	int	fd[2];
	int	in_fd;

	cmd->in_pipe = true;
	in_fd = 0;
	while (cmd)
	{
		if (cmd->next && pipe(fd) == -1)
			print_error_exit("pipe", EXIT_FAILURE);
		fork_and_execute(cmd, in_fd, fd, envp);
		cmd = cmd->next;
	}
	// cleanup_heredoc(cmd);
}

/**
 * Entry point for command execution.
 * - If a single built-in command is executed, it runs directly
 * in the current shell.
 * - Otherwise, it executes the full command pipeline using subshell.
 *
 * @param cmd The first command in the command list.
 * @param envp The environment variables array.
 */
int	run_executor(t_cmd *cmd, char **envp)
{
	cmd->in_pipe = false;
	if (is_builtin(cmd) && !cmd->next)
	{
		exec_builtin(cmd);
		printf("[DEBUG]: run_executor exit status (%d)\n", cmd->minishell->exit_stat);

		return (cmd->minishell->exit_stat);
	}
	exec_cmd(cmd, envp);

	printf("[DEBUG]: run_executor exit status (%d)\n", cmd->minishell->exit_stat);

	return (cmd->minishell->exit_stat);
}
