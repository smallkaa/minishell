#include "minishell.h"

static int	handle_heredoc_redirection(t_redir *redir)
{
	if (redir->fd == -1)
		return (error_return("herdoc: fd already closed\n", REDIR_ERR));
	if (dup2(redir->fd, STDIN_FILENO) == -1)
		return (perror_return("herdoc: dup2", REDIR_ERR));
	safe_close(&redir->fd);
	return (EXIT_SUCCESS);
}

static int	handle_file_input_redirection(t_redir *redir)
{
	int	fd;

	fd = open(redir->filename, O_RDONLY);
	if (fd == -1)
		return (perror_return(redir->filename, REDIR_ERR));
	if (dup2(fd, STDIN_FILENO) == -1)
	{
		safe_close(&fd);
		return (perror_return("in_redir: dup2 input", REDIR_ERR));
	}
	safe_close(&fd);
	return (EXIT_SUCCESS);
}

static int	apply_input_redirection(t_redir *redir)
{
	if (redir->type == R_HEREDOC)
		return (handle_heredoc_redirection(redir));
	else if (redir->type == R_INPUT)
		return (handle_file_input_redirection(redir));
	return (EXIT_SUCCESS);
}

static int	apply_output_redirection(t_redir *redir)
{
	int	fd;
	int	flags;

	flags = O_WRONLY | O_CREAT;
	if (redir->type == R_APPEND)
		flags |= O_APPEND;
	else
		flags |= O_TRUNC;
	fd = open(redir->filename, flags, 0644);
	if (fd == -1)
		return (perror_return(redir->filename, REDIR_ERR));
	if (dup2(fd, STDOUT_FILENO) == -1)
	{
		safe_close(&fd);
		return (perror_return("dup2 output", REDIR_ERR));
	}
	safe_close(&fd);
	return (EXIT_SUCCESS);
}

uint8_t	apply_redirections(t_cmd *cmd)
{
	t_list	*redir_list;
	t_redir	*redir;
	int		exit_status;

	redir_list = cmd->redirs;
	exit_status = EXIT_SUCCESS;
	while (redir_list)
	{
		redir = redir_list->content;
		if (redir->type == R_INPUT || redir->type == R_HEREDOC)
			exit_status = apply_input_redirection(redir);
		else if (redir->type == R_OUTPUT || redir->type == R_APPEND)
			exit_status = apply_output_redirection(redir);
		if (exit_status == REDIR_ERR)
			return (EXIT_FAILURE);
		redir_list = redir_list->next;
	}
	return ((uint8_t)exit_status);
}
