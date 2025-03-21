#include "minishell.h"

uint8_t	apply_redirections(t_cmd *cmd)
{
	// if (!cmd || !cmd->in_redir || !cmd->out_redir)
	// {
	// 	print_error("minishell: redirections not found\n");
	// 	return (EXIT_FAILURE);
	// }
	if (cmd->in_redir->type == R_HEREDOC)
		return (apply_heredoc(cmd));
	// else if (cmd->in_redir->type == R_INPUT)
	// 	return (handle_input_redirectiion(cmd));
	// else if (cmd->out_redir->type == R_APPEND)
	// 	return (handle_append_redirection(cmd));
	// else if (cmd->out_redir->type == R_OUTPUT)
	// 	return (handle_out_redirection(cmd));
	else
	{
		print_error("Error: process_redirections failed\n");
		return(EXIT_FAILURE);
	}
}
