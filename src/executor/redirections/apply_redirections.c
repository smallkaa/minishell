#include "minishell.h"

static int	handle_heredoc_redirection(t_redir *r)
{
	if (r->fd == -1)
		return (EXIT_SUCCESS);
	if (dup2(r->fd, STDIN_FILENO) == -1)
		return (perror_return("heredoc dup2", REDIR_ERR));
	safe_close(&r->fd);
	return (EXIT_SUCCESS);
}

static int	handle_file_input_redirection(t_redir *r)
{

	int fd = open(r->filename, O_RDONLY);
	if (fd == -1)
		return (perror_return(r->filename, REDIR_ERR));
	if (dup2(fd, STDIN_FILENO) == -1)
	{
		safe_close(&fd);
		return (perror_return("in dup2", REDIR_ERR));
	}
	safe_close(&fd);
	return (EXIT_SUCCESS);
}

static int	handle_output_redirection(t_redir *r)
{
	int flags = O_WRONLY | O_CREAT |
	            (r->type == R_APPEND ? O_APPEND : O_TRUNC);
	int fd = open(r->filename, flags, 0644);
	if (fd == -1)
		return (perror_return(r->filename, REDIR_ERR));
	if (dup2(fd, STDOUT_FILENO) == -1)
	{
		safe_close(&fd);
		return (perror_return("out dup2", REDIR_ERR));
	}
	safe_close(&fd);
	return (EXIT_SUCCESS);
}

uint8_t	apply_redirections(t_cmd *cmd)
{
	t_list	*node = cmd->redirs;
	int		last_in_fd  = -1;
	int		last_out_fd = -1;
	if (!cmd->argv[0])
	{
		free_minishell(cmd->minishell); // must be here if > file
	}
	while (node)
	{
		t_redir *r = node->content;

		if (r->type == R_INPUT || r->type == R_HEREDOC)
		{
			safe_close(&last_in_fd);                    /* new ► close old stdin */
			if ((r->type == R_HEREDOC
				 ? handle_heredoc_redirection(r)
				 : handle_file_input_redirection(r)) == REDIR_ERR)
			{
				free_minishell(cmd->minishell); 
				return (EXIT_FAILURE);
			}
			last_in_fd = r->fd;                         /* r->fd is now −1      */
		}
		else if (r->type == R_OUTPUT || r->type == R_APPEND)
		{

			safe_close(&last_out_fd);                   /* new ► close old stdout */
			if (handle_output_redirection(r) == REDIR_ERR)
				return (EXIT_FAILURE);
			last_out_fd = -1;                           /* already closed       */
		}
		node = node->next;
	}
	return (EXIT_SUCCESS);
}
