#include "minishell.h"

uint8_t	apply_redirections(t_cmd *cmd)
{
	t_redir *redir;
	t_list	*current_redir;
	int		result;

	if (!cmd)
	{
		print_error("minishell: cmd struct not found\n");
		return (EXIT_FAILURE);
	}
	if (!cmd->redirs)
		return (EXIT_SUCCESS);

	current_redir = cmd->redirs;
	while (current_redir)
	{
		redir = (t_redir *)current_redir->content;

		if (redir->type == R_INPUT)
			result = apply_in_redirection(cmd);
		else if (redir->type == R_OUTPUT || redir->type == R_APPEND)
			result = apply_out_redirection(cmd);
		else if (redir->type == R_HEREDOC)
			result = apply_heredoc(cmd);
		else
		{
			print_error("Error: apply_redirections: invalid redirection type\n");
			return (EXIT_FAILURE);
		}

		// Check if the redirection application failed
		if (result != EXIT_SUCCESS)
			return (result);

		current_redir = current_redir->next; // Move to the next redirection
	}

	return (EXIT_SUCCESS);
}

