#include "minishell.h"

/**
 * Prints the environment variables.
 * - Iterates through the `env` array and prints each variable.
 *
 * @param env  The NULL-terminated array of environment variables.
 */
void	print_env(char **env)
{
	while (*env)
	{
		printf("%s\n", *env);
		env++;
	}
}

/**
 * Handles the `env` built-in command.
 * - Retrieves and prints the environment variables using `print_env()`.
 * - Updates the last exit status.
 * - If the command is part of a pipe, it exits after execution.
 *
 * @param cmd   The command structure containing minishell's context.
 */
void	handle_env(t_cmd *cmd)
{
	char	**env;

	env = cmd->minishell->env;
	if (!env)
	{
		update_last_exit_status(cmd, EXIT_FAILURE);
		print_error("Error: no environment variables found\n");
		if (cmd->in_pipe)
			exit (EXIT_FAILURE);
		return ;
	}
	print_env(env);
	update_last_exit_status(cmd, EXIT_SUCCESS);
	if (cmd->in_pipe)
		exit (EXIT_SUCCESS);
	return ;
}
