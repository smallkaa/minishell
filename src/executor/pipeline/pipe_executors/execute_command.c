#include "minishell.h"

static void	handle_builtin_and_exit(t_cmd *cmd)
{
	uint8_t	exit_status;

	exit_status = exec_builtins(cmd, 0);
	free_minishell(cmd->minishell);
	_exit(exit_status);
}

void	exec_cmd(t_cmd *cmd)
{
	uint8_t	exit_status;

	exit_status = validate_dots(cmd);
	if (exit_status != EXIT_SUCCESS)
		_exit(exit_status);
	execve(cmd->binary, cmd->argv, cmd->minishell->env);
	child_execve_error(cmd);
}

void	execute_command(t_cmd *cmd)
{

	if (is_minishell_executable(cmd) && update_shlvl(cmd) == EXIT_FAILURE)
		_exit(EXIT_FAILURE);
	if (ft_strcmp(cmd->argv[0], "") == 0)
		handle_empty_command(cmd);
	if (!cmd->binary)
	{
		if (is_builtin(cmd))
			handle_builtin_and_exit(cmd);
		else
			cmd_missing_command_error(cmd);
	}
	exec_cmd(cmd);
}
