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

uint8_t	apply_out_redirection(t_cmd *cmd)
{
	int		out_fd;
	int		mode;
	t_list	*current_redir;
	t_redir	*redir;
	uint8_t result = EXIT_SUCCESS;

	if (!cmd->redirs)
		return (EXIT_SUCCESS);

	current_redir = cmd->redirs;
	while (current_redir)
	{
		redir = (t_redir *)current_redir->content;

		if (redir->type == R_OUTPUT || redir->type == R_APPEND)
		{
			if (apply_mode(redir, &mode) == EXIT_FAILURE)
				return (EXIT_FAILURE);

			out_fd = open(redir->filename, mode, 0644);
			if (out_fd < 0)
			{
				print_error("-minishell: ");
				perror(redir->filename);
				return (EXIT_FAILURE);
			}
			if (dup2(out_fd, STDOUT_FILENO) == -1)
			{
				if (close(out_fd) == -1)
					perror_return("-minishell: close", EXIT_FAILURE);
				perror_return("-minishell: dup2", EXIT_FAILURE);
			}
			if (close(out_fd) == -1)
				perror_return("-minishell: close", EXIT_FAILURE);

			// After successfully applying the redirection, continue to the next one.
			result = EXIT_SUCCESS;
		}

		current_redir = current_redir->next;
	}

	return (result);
}

// fprintf(stderr, "errno: %d (%s)\n", errno, strerror(errno));
