#include "minishell.h"

uint8_t	apply_redirections(t_cmd *cmd)
{

	if (!cmd)
	{
		print_error("minishell: cmd struct not found\n");
		return (EXIT_FAILURE);
	}

	if (cmd->in_redir && cmd->in_redir->type == R_HEREDOC)
	{
		return (apply_heredoc(cmd));
	}

	else if (cmd->in_redir && cmd->in_redir->type == R_INPUT)
	{
		printf ("---PAVEL is GOOD! You handled '<'");
		// return (EXIT_SUCCESS);
		// return (handle_input_redirectiion(cmd));
	}

	else if (cmd->out_redir && cmd->out_redir->type == R_APPEND)
	{
		printf ("---PAVEL is GOOD! You handled '>>'");
		return (EXIT_SUCCESS);
		// return (handle_append_redirection(cmd));
	}

	else if (cmd->out_redir && cmd->out_redir->type == R_OUTPUT)
	{
		printf("---PAVEL is GOOD! You handled '>'");
		return (EXIT_SUCCESS);
		// return (handle_out_redirection(cmd));
	}
	printf("---No redirectons found\n"); // test
	return (EXIT_SUCCESS);
}
