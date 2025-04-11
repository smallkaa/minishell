#include "minishell.h"

void	child_process(t_cmd *cmd, int in_fd, int *pipe_fd)
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

void	handle_child_and_track(t_cmd *cmd, t_pipe_info *info)
{
	info->pids[*info->idx] = fork();
	if (info->pids[*info->idx] == -1)
	{
		perror("-exec_in_pipes: fork");
		exit(EXIT_FAILURE);
	}
	if (info->pids[*info->idx] == 0)
		child_process(cmd, info->in_fd, info->pipe_fd);
	// if (close_heredoc_fds(cmd) == EXIT_FAILURE)
	// 	_exit(EXIT_FAILURE);
	(*info->idx)++;
}
