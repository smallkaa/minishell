#include "minishell.h"

static int	new_heredoc_fd(const char *delim)
{
	int	pipe_fd[2];

	if (pipe(pipe_fd) == -1)
		return (perror_return("new_heredoc_fd: pipe", WRITE_HERED_ERR));
	if (write_heredoc_to_pipe(pipe_fd[1], delim) == WRITE_HERED_ERR)
	{
		safe_close(&pipe_fd[0]);
		safe_close(&pipe_fd[1]);
		return (perror_return("new_heredoc_fd: write", WRITE_HERED_ERR));
	}
	safe_close(&pipe_fd[1]);
	return (pipe_fd[0]);
}

static bool	assign_heredoc_fd(t_redir *redirection)
{
	redirection->fd = new_heredoc_fd(redirection->filename);
	if (redirection->fd == WRITE_HERED_ERR)
		return (false);
	return (true);
}

static bool	handle_cmd_heredocs(t_cmd *cmd)
{
	t_list	*redir_list;
	t_redir	*redirection;

	redir_list = cmd->redirs;
	while (redir_list)
	{
		redirection = redir_list->content;
		if (is_heredoc(redirection))
		{
			if (!assign_heredoc_fd(redirection))
				return (false);
		}
		redir_list = redir_list->next;
	}
	return (true);
}

uint8_t	apply_heredocs(t_cmd *cmd)
{
	t_cmd	*initial_cmd_list;

	initial_cmd_list = cmd;
	if (!cmd)
		return (error_return("apply_heredocs: cmd not found\n", EXIT_FAILURE));
	while (cmd)
	{
		if (!handle_cmd_heredocs(cmd))
		{
			close_all_heredoc_fds(initial_cmd_list);
			return (EXIT_FAILURE);
		}
		cmd = cmd->next;
	}
	return (EXIT_SUCCESS);
}
