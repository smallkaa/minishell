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
	int	fd;

	fd = open(r->filename, O_RDONLY);
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
	int	flags;
	int	fd;

	flags = O_WRONLY | O_CREAT;
	if (r->type == R_APPEND)
		flags |= O_APPEND;
	else
		flags |= O_TRUNC;
	fd = open(r->filename, flags, 0644);
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

// uint8_t	apply_redirections(t_cmd *cmd)
// {
// 	t_list	*node;
// 	int		last_in_fd;
// 	int		last_out_fd;
// 	t_redir	*r;

// 	node = cmd->redirs;
// 	last_in_fd  = -1;
// 	last_out_fd = -1;
// 	if (!cmd->argv[0])
// 		free_minishell(cmd->minishell); // must be here if > file
// 	while (node)
// 	{
// 		r = node->content;
// 		if (r->type == R_INPUT || r->type == R_HEREDOC)
// 		{
// 			safe_close(&last_in_fd);
// 			if ((r->type == R_HEREDOC
// 				 ? handle_heredoc_redirection(r)
// 				 : handle_file_input_redirection(r)) == REDIR_ERR)
// 			{
// 				return (EXIT_FAILURE);
// 			}
// 			last_in_fd = r->fd;
// 		}
// 		else if (r->type == R_OUTPUT || r->type == R_APPEND)
// 		{
// 			safe_close(&last_out_fd);
// 			if (handle_output_redirection(r) == REDIR_ERR)
// 			{
// 				return (EXIT_FAILURE);
// 			}
// 			last_out_fd = -1;
// 		}
// 		node = node->next;
// 	}
// 	return (EXIT_SUCCESS);
// }
uint8_t	apply_redirections(t_cmd *cmd)
{
	t_list	*node;
	int		last_in_fd;
	int		last_out_fd;
	t_redir	*r;
	int		result;

	node = cmd->redirs;
	last_in_fd  = -1;
	last_out_fd = -1;

	// if (!cmd->argv[0])
	// 	free_minishell(cmd->minishell); // must be here if > file
	while (node)
	{
		r = node->content;
		if (r->type == R_INPUT || r->type == R_HEREDOC)
		{
			safe_close(&last_in_fd);
			if (r->type == R_HEREDOC)
				result = handle_heredoc_redirection(r);
			else
				result = handle_file_input_redirection(r);
			if (result == REDIR_ERR)
				return (EXIT_FAILURE);
			last_in_fd = r->fd;
		}
		else if (r->type == R_OUTPUT || r->type == R_APPEND)
		{
			safe_close(&last_out_fd);
			if (handle_output_redirection(r) == REDIR_ERR)
				return (EXIT_FAILURE);
			last_out_fd = -1;
		}
		node = node->next;
	}
	return (EXIT_SUCCESS);
}
