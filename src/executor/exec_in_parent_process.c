#include "minishell.h"

uint8_t	exec_in_parent_process(t_cmd *cmd)
{
	uint8_t	exit_status;

	exit_status = apply_redirections(cmd);

	if(exit_status == EXIT_FAILURE)
		return (exit_status);
	else
		return (exec_builtin(cmd));
}
