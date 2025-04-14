#include "minishell.h"

static void	handle_pipe_creation(t_cmd *cmd, int *pipe_fd)
{
	if (cmd->next)
	{
		if (pipe(pipe_fd) == -1)
		{
			perror("-exec_in_pipes: pipe");
			exit(EXIT_FAILURE);
		}
	}
}

uint8_t	close_unused_fds(int in_fd, int *pipe_fd)
{
	if (in_fd != STDIN_FILENO && close(in_fd) == -1)
	{
		perror("-close_unused_fds: close in_fd");
		return (EXIT_FAILURE);
	}
	if (pipe_fd[0] != -1 && close(pipe_fd[0]) == -1)
	{
		perror("-close_unused_fds: close pipe_fd[0]");
		return (EXIT_FAILURE);
	}
	if (pipe_fd[1] != -1 && close(pipe_fd[1]) == -1)
	{
		perror("-close_unused_fds: close pipe_fd[1]");
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}

void	close_fds_and_prepare_next(int *in_fd, int *pipe_fd)
{
	if (pipe_fd[1] >= 0 && close(pipe_fd[1]) == -1)
	{
		perror("-exec_in_pipes: close pipe_fd[1]");
		_exit(EXIT_FAILURE);
	}
	if (*in_fd != STDIN_FILENO && *in_fd > STDIN_FILENO && close(*in_fd) == -1)
	{
		perror("-exec_in_pipes: close in_fd");
		_exit(EXIT_FAILURE);
	}
	*in_fd = pipe_fd[0];
	pipe_fd[0] = -1;
	pipe_fd[1] = -1;
}

void	close_all_heredoc_fds(t_cmd *cmd_list)
{
	t_list	*rlist;
	t_redir	*redirection;

	while (cmd_list)
	{
		rlist = cmd_list->redirs;
		while (rlist)
		{
			redirection = rlist->content;
			if (is_heredoc(redirection))
			{
				if (redirection->fd >= 0)
				{
					if (close(redirection->fd) == -1)
						perror("close_all_heredoc_fds: close");
					redirection->fd = -1;
				}
			}
			rlist = rlist->next;
		}
		cmd_list = cmd_list->next;
	}
}

uint8_t	exec_in_pipes(t_cmd *cmd_list)
{
	int			pipe_fd[2];
	pid_t		pids[MAX_CMDS];
	int			idx;
	t_pipe_info	info;
	t_cmd		*cmd;

	cmd = cmd_list;
	pipe_fd[0] = -1;
	pipe_fd[1] = -1;
	idx = 0;
	info.in_fd = STDIN_FILENO;
	info.pipe_fd = pipe_fd;
	info.pids = pids;
	info.idx = &idx;
	info.cmd_list = cmd_list;
	while (cmd)
	{
		handle_pipe_creation(cmd, pipe_fd);
		handle_child_and_track(cmd, &info);
		close_fds_and_prepare_next(&info.in_fd, pipe_fd);
		cmd = cmd->next;
	}
	close_all_heredoc_fds(cmd_list);
	return (wait_for_children(pids, idx));
}
