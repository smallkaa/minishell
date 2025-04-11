#include "minishell.h"

static void	child_process(t_cmd *cmd, int in_fd, int *pipe_fd)
{
	if (cmd->next && dup2(pipe_fd[1], STDOUT_FILENO) == -1)
	{
		perror("-exec_in_pipes: dup2 pipe_fd[1]");
		_exit(EXIT_FAILURE);
	}
	if (in_fd != STDIN_FILENO && dup2(in_fd, STDIN_FILENO) == -1)
	{
		perror("-exec_in_pipes: dup2 in_fd");
		_exit(EXIT_FAILURE);
	}
	if (apply_redirections(cmd) != EXIT_SUCCESS)
		_exit(EXIT_FAILURE);
	if (close_unused_fds(in_fd, pipe_fd) != EXIT_SUCCESS)
		_exit(EXIT_FAILURE);
	execute_command(cmd);
}

void	handle_child_and_track(t_cmd *cmd, int in_fd, int *pipe_fd, pid_t *pids, int *idx)
{
	pids[*idx] = fork();
	if (pids[*idx] == -1)
	{
		perror("-exec_in_pipes: fork");
		exit(EXIT_FAILURE);
	}
	if (pids[*idx] == 0)
		child_process(cmd, in_fd, pipe_fd);
	(*idx)++;
}
