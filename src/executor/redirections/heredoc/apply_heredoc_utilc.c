#include "minishell.h"

void	safe_close(int *fd)
{
	if (fd && *fd >= 0)
	{
		if (close(*fd) == -1)
			return ;
		*fd = -1;
	}
}

bool	heredoc_exceeds_limit(size_t total_written)
{
	return (total_written >= HEREDOC_MAX_SIZE);
}

bool	is_heredoc(t_redir *redirection)
{
	return (redirection->type == R_HEREDOC);
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
				safe_close(&redirection->fd);
			rlist = rlist->next;
		}
		cmd_list = cmd_list->next;
	}
}
