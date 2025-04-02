#include "minishell.h"

static uint8_t	apply_mode(t_redir *redir, int *mode)
{
	*mode = O_WRONLY | O_CREAT;
	if (redir->type == R_OUTPUT)
		*mode |= O_TRUNC;
	else if (redir->type == R_APPEND)
		*mode |= O_APPEND;
	else
	{
		print_error("Error: unknown redirection mode\n");
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}

int	open_output_file(t_redir *redir, int mode)
{
	int	out_fd;

	out_fd = open(redir->filename, mode, 0644);
	if (out_fd < 0)
	{
		print_error("-minishell: ");
		perror(redir->filename);
	}
	return (out_fd);
}

uint8_t	apply_output_fd(int out_fd)
{
	if (dup2(out_fd, STDOUT_FILENO) == -1)
	{
		if (close(out_fd) == -1)
			perror_return("-minishell: close", EXIT_FAILURE);
		perror_return("-minishell: dup2", EXIT_FAILURE);
	}
	if (close(out_fd) == -1)
		perror_return("-minishell: close", EXIT_FAILURE);
	return (EXIT_SUCCESS);
}

uint8_t	process_out_redirection(t_redir *redir)
{
	int		out_fd;
	int		mode;

	if (apply_mode(redir, &mode) == EXIT_FAILURE)
		return (EXIT_FAILURE);
	out_fd = open_output_file(redir, mode);
	if (out_fd < 0)
		return (EXIT_FAILURE);
	if (apply_output_fd(out_fd) != EXIT_SUCCESS)
		return (EXIT_FAILURE);
	return (EXIT_SUCCESS);
}

uint8_t	apply_out_redirection(t_cmd *cmd)
{
	t_list	*current_redir;
	t_redir	*redir;
	uint8_t	exit_status;

	exit_status = EXIT_SUCCESS;
	if (!cmd->redirs)
		return (EXIT_SUCCESS);
	current_redir = cmd->redirs;
	while (current_redir)
	{
		redir = (t_redir *)current_redir->content;
		if (redir->type == R_OUTPUT || redir->type == R_APPEND)
		{
			exit_status = process_out_redirection(redir);
			if (exit_status != EXIT_SUCCESS)
				return (exit_status);
		}
		current_redir = current_redir->next;
	}
	return (exit_status);
}
