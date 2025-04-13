#include "minishell.h"

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

// uint8_t	close_heredoc_fds(t_cmd *cmd)
// {
// 	t_list	*rlist;
// 	t_redir	*r;

// 	rlist = cmd->redirs;
// 	while (rlist)
// 	{
// 		r = rlist->content;
// 		if (r->type == R_HEREDOC && r->fd != -1)
// 		{
// 			printf("DEBUG: close_heredoc_fds: r->fd  == '%d'\n", r->fd);
// 			if (close(r->fd) == -1)
// 			{
// 				perror("-close_heredoc_fds: close");
// 				return (EXIT_FAILURE);
// 			}
// 			r->fd = -1;
// 		}
// 		rlist = rlist->next;
// 	}
// 	return (EXIT_SUCCESS);
// }

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

void close_all_heredoc_fds(t_cmd *cmd_list)
{
	// print_pid ("I am in close_all_heredoc_fds");
	// printf("DEBUG: intering close_all_heredoc_fds\n");
	while (cmd_list)
	{
		t_list *rlist = cmd_list->redirs;
		while (rlist)
		{
			t_redir *r = rlist->content;
			if (r->type == R_HEREDOC)
			{
				if (r->fd >= 0)
				{
					// fprintf(stderr, "DEBUG: close_all_heredoc_fds: closing remaining heredoc fd -> '%d'\n", r->fd);
					if (close(r->fd) == -1)
						perror("close_all_heredoc_fds: close");
					r->fd = -1;
				}
				// else
				// {
				// 	fprintf(stderr, "DEBUG: close_all_heredoc_fds: heredoc fd already -1, nothing to close\n");
				// }
			}
			rlist = rlist->next;
		}
		cmd_list = cmd_list->next;
	}
}


uint8_t exec_in_pipes(t_cmd *cmd_list)
{
	int pipe_fd[2] = {-1, -1};
	pid_t pids[MAX_CMDS];
	int idx = 0;
	t_pipe_info info;
	t_cmd *cmd = cmd_list;

	info.in_fd = STDIN_FILENO;
	info.pipe_fd = pipe_fd;
	info.pids = pids;
	info.idx = &idx;
	info.cmd_list = cmd_list;

	while (cmd)
	{
		// printf("\n---------------CMD TO EXEC: '%s' -----------\n", cmd->argv[0]);
		// print_pid("exec_in_pipes: forking");

		handle_pipe_creation(cmd, pipe_fd);
		handle_child_and_track(cmd, &info);
		close_fds_and_prepare_next(&info.in_fd, pipe_fd);

		// ⚠️ DO NOT close heredoc FDs here — parent must wait until the end
		// If you close them here, the next command (in the same pipeline)
		// might still need them open. Keep the cleanup centralized below.

		cmd = cmd->next;
	}

	// print_pid("\n-------------I am a parent");


	// printf("DEBUG: exec_in_pipes: I am befor wait_for_children\n");
	close_all_heredoc_fds(cmd_list);

	return wait_for_children(pids, idx);
}
