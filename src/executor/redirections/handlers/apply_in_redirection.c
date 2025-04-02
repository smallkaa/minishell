#include "minishell.h"

uint8_t	apply_in_redirection(t_cmd *cmd)
{
	int		in_fd;
	t_list	*current_redir;
	t_redir	*redir;
	uint8_t result = EXIT_SUCCESS;

	if (!cmd->redirs)
		return (EXIT_SUCCESS);

	current_redir = cmd->redirs;
	while (current_redir)
	{
		redir = (t_redir *)current_redir->content;
		if (redir->type == R_INPUT)
		{
			in_fd = open(redir->filename, O_RDONLY);
			if (in_fd < 0)
			{
				print_error("-minishell: ");
				perror(redir->filename);
				return (EXIT_FAILURE);
			}
			if (dup2(in_fd, STDIN_FILENO) == -1)
			{
				if (close(in_fd) == -1)
					perror_return("-minishell: close", EXIT_FAILURE);
				perror_return("-minishell: dup2", EXIT_FAILURE);
			}
			if (close(in_fd) == -1)
				perror_return("-minishell: close", EXIT_FAILURE);

			// Continue to the next redirection if there is one.
			result = EXIT_SUCCESS;
		}
		current_redir = current_redir->next;
	}

	return (result);
}
