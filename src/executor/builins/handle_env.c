#include "minishell.h"

/**
 * Prints the environment variables.
 * - Iterates through the `envp` array and prints each variable.
 *
 * @param envp  The NULL-terminated array of environment variables.
 */
void	print_env(char **envp)
{
	while (*envp)
	{
		printf("%s\n", *envp);
		envp++;
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
	char	**envp;

	envp = cmd->minishell->envp;
	envp = NULL;
	if (!envp)
	{
		update_last_exit_status(cmd, EXIT_FAILURE);
		print_error("minishell: no environment variables found\n");
		if (cmd->in_pipe)
			exit (EXIT_FAILURE);
		return ;
	}
	print_env(envp);
	update_last_exit_status(cmd, EXIT_SUCCESS);
	if (cmd->in_pipe)
		exit (EXIT_SUCCESS);
	return ;
}
