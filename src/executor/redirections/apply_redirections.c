#include "minishell.h"

uint8_t	apply_single_redirection(t_cmd *cmd, t_redir *redir)
{
	uint8_t	exit_status;

	if (redir->type == R_INPUT)
		exit_status = apply_in_redirection(cmd);
	else if (redir->type == R_OUTPUT || redir->type == R_APPEND)
		exit_status = apply_out_redirection(cmd);
	else if (redir->type == R_HEREDOC)
		exit_status = apply_heredoc(cmd);
	else
	{
		print_error("Error: apply_redirections: invalid redirection type\n");
		return (EXIT_FAILURE);
	}
	return (exit_status);
}

uint8_t	apply_redirections(t_cmd *cmd)
{
	t_redir	*redir;
	t_list	*current_redir;
	uint8_t	exit_status;

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
		exit_status = apply_single_redirection(cmd, redir);
		if (exit_status != EXIT_SUCCESS)
			return (exit_status);
		current_redir = current_redir->next;
	}
	return (EXIT_SUCCESS);
}
