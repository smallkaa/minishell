#include "minishell.h"

void	execute(t_cmd *cmd, int in_fd)
{
	if (cmd->binary == NULL)
	{
		printf("DEBUG: execute() cmd->binary == NULL status: %d\n", cmd->minishell->exit_status);
		command_not_found_handle(cmd);
	}
	if (in_fd != 0)
	{
		if (dup2(in_fd, STDIN_FILENO) == -1)
		{
			if (close(in_fd) == -1)
				print_error_exit("close", EXIT_FAILURE);
			print_error_exit("dup2", EXIT_FAILURE);
		}
		if (close(in_fd) == -1)
			print_error_exit("close", EXIT_FAILURE);
	}
	if (is_builtin(cmd))
		exec_builtin(cmd);
	execve(cmd->binary, cmd->argv, cmd->minishell->env);
}

static void	exec_fork_child(t_cmd *cmd, int in_fd, int fd[2])
{
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
		handle_redirections(cmd, in_fd);
	execute(cmd, in_fd);
}

static void	fork_and_execute(t_cmd *cmd, int in_fd, int fd[2])
{
	pid_t	pid;
	int		status;

	pid = fork();
	if (pid == -1)
		print_error_exit("fork", EXIT_FAILURE);
	if (pid == 0)
		exec_fork_child(cmd, in_fd, fd);
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
		update_last_exit_status(cmd, 128 + WTERMSIG(cmd->minishell->exit_status));
}

void	exec_cmd(t_cmd *cmd)
{
	int	fd[2];
	int	in_fd;

	cmd->in_pipe = true;
	in_fd = 0;
	while (cmd)
	{
		if (cmd->next && pipe(fd) == -1)
			print_error_exit("pipe", EXIT_FAILURE);
		fork_and_execute(cmd, in_fd, fd);
		cmd = cmd->next;
	}
	// cleanup_heredoc(cmd);
}
