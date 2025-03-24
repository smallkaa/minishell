#include "minishell.h"

static uint8_t	pre_exec_input_validation(t_cmd *cmd)
{
	if (!cmd || !cmd->in_redir || !cmd->in_redir->filename)
    {
        print_error("Error: pre_exec_input_validation: redirection not properly initialized.\n");
        return (EXIT_FAILURE);
    }
	return (EXIT_SUCCESS);

}

static uint8_t	pre_exec_out_validation(t_cmd *cmd)
{
	if (!cmd || !cmd->out_redir || !cmd->out_redir->filename)
    {
        print_error("Error: pre_exec_out_validation: redirection not properly initialized.\n");
        return (EXIT_FAILURE);
    }
	return (EXIT_SUCCESS);
}

uint8_t	pre_exec_validation(t_cmd *cmd, t_redir_type type)
{
	if (type == R_HEREDOC || type == R_INPUT)
		return (pre_exec_input_validation(cmd));
	else if (type == R_APPEND || type == R_OUTPUT)
		return (pre_exec_out_validation(cmd));
	else
	{
		print_error("Error: pre_exec_validation\n");
		return (EXIT_FAILURE);
	}
}
