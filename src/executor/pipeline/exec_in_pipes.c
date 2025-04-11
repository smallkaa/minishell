#include "minishell.h"

uint8_t	exec_in_pipes(t_cmd *cmd)
{
	int in_fd;
	int pipe_fd[2];
	pid_t pids[MAX_CMDS];
	int idx;

	in_fd = STDIN_FILENO;
	pipe_fd[0] = -1;
	pipe_fd[1] = -1;
	idx = 0;
	while (cmd)
	{
		handle_pipe_creation(cmd, pipe_fd);
		handle_child_and_track(cmd, in_fd, pipe_fd, pids, &idx);
		if (close_heredoc_fds(cmd) == EXIT_FAILURE)
			_exit(EXIT_FAILURE);
		close_fds_and_prepare_next(&in_fd, pipe_fd);
		cmd = cmd->next;
	}
	return (wait_for_children(pids, idx));
}
