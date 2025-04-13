#include "minishell.h"

static uint8_t apply_input_redirection(t_redir *redir)
{
	int fd;

	if (redir->type == R_HEREDOC)
	{
		if (redir->fd == -1)
		{
			fprintf(stderr, "apply_input_redirection: heredoc fd already closed\n");
			return (EXIT_FAILURE);
		}
		if (dup2(redir->fd, STDIN_FILENO) == -1)
		{
			perror("apply_input_redirection: dup2 heredoc");
			return (EXIT_FAILURE);
		}
		if (close(redir->fd) == -1)
		{
			perror("apply_input_redirection: close heredoc");
			return (EXIT_FAILURE);
		}
		// redir->fd = -2;
	}
	else if (redir->type == R_INPUT)
	{
		fd = open(redir->filename, O_RDONLY);
		if (fd == -1)
		{
			perror(redir->filename);
			return (EXIT_FAILURE);
		}
		if (dup2(fd, STDIN_FILENO) == -1)
		{
			perror("apply_input_redirection: dup2 input");
			close(fd); // still try to close even after dup2 failure
			return (EXIT_FAILURE);
		}
		if (close(fd) == -1)
		{
			perror("apply_input_redirection: close input file");
			return (EXIT_FAILURE);
		}
	}
	return (EXIT_SUCCESS);
}


static uint8_t	apply_output_redirection(t_redir *redir)
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
		return (perror_return(redir->filename, EXIT_FAILURE));
	if (dup2(fd, STDOUT_FILENO) == -1)
	{
		close(fd);
		return (perror_return("dup2 output", EXIT_FAILURE));
	}
	close(fd);
	return (EXIT_SUCCESS);
}

uint8_t	apply_redirections(t_cmd *cmd)
{
	t_list	*redir_list;
	t_redir	*redir;
	uint8_t	exit_status;

	redir_list = cmd->redirs;
	exit_status = EXIT_SUCCESS;
	while (redir_list)
	{
		redir = redir_list->content;
		if (redir->type == R_INPUT || redir->type == R_HEREDOC)
			exit_status = apply_input_redirection(redir);
		else if (redir->type == R_OUTPUT || redir->type == R_APPEND)
			exit_status = apply_output_redirection(redir);
		if (exit_status != EXIT_SUCCESS)
			return (exit_status);
		redir_list = redir_list->next;
	}
	return (exit_status);
}
