// #include "minishell.h"

// uint8_t	close_unused_fds(int in_fd, int *pipe_fd)
// {
// 	if (in_fd != STDIN_FILENO && close(in_fd) == -1)
// 	{
// 		perror("-close_unused_fds: close in_fd");
// 		return (EXIT_FAILURE);
// 	}
// 	if (pipe_fd[0] != -1 && close(pipe_fd[0]) == -1)
// 	{
// 		perror("-close_unused_fds: close pipe_fd[0]");
// 		return (EXIT_FAILURE);
// 	}
// 	if (pipe_fd[1] != -1 && close(pipe_fd[1]) == -1)
// 	{
// 		perror("-close_unused_fds: close pipe_fd[1]");
// 		return (EXIT_FAILURE);
// 	}
// 	return (EXIT_SUCCESS);
// }

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

// void	close_fds_and_prepare_next(int *in_fd, int *pipe_fd)
// {
// 	if (pipe_fd[1] >= 0 && close(pipe_fd[1]) == -1)
// 	{
// 		perror("-exec_in_pipes: close pipe_fd[1]");
// 		_exit(EXIT_FAILURE);
// 	}
// 	if (*in_fd != STDIN_FILENO && *in_fd > STDIN_FILENO && close(*in_fd) == -1)
// 	{
// 		perror("-exec_in_pipes: close in_fd");
// 		_exit(EXIT_FAILURE);
// 	}
// 	*in_fd = pipe_fd[0];
// 	pipe_fd[0] = -1;
// 	pipe_fd[1] = -1;
// }
