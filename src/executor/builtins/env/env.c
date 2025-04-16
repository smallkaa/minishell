/**
 * @file handle_env.c
 * @brief Functions for handling the `env` built-in command in Minishell.
 */
#include "minishell.h"

void	print_env(t_cmd *cmd)
{
	int	i;

	i = 0;
	while (cmd->minishell->env[i])
	{
		(void)printf("%s\n", cmd->minishell->env[i]);
		i++;
	}
}

/**
 * @brief Handles the `env` built-in command to display environment variables.
 *
 * The `env` command prints all assigned environment variables stored in
 * the shell’s hash table. Variables that are unset or unassigned are not
 * displayed.
 *
 * **Behavior:**
 * - If called **without arguments**, it prints all assigned environment
 *   variables.
 * - If called **with arguments**, an error message is displayed.
 *
 * @param cmd Pointer to the command structure containing execution context.
 * @return `EXIT_SUCCESS` (0) if variables were displayed successfully.
 *         `EXIT_FAILURE` (1) if arguments were provided (invalid usage).
 */
uint8_t	handle_env(t_cmd *cmd)
{
	uint8_t	exit_status;

	if (!cmd)
		return (no_cmd_error("env"));
	if (!cmd->argv)
		return (no_argv_error("env"));
	if (cmd->argv[1])
	{
		print_error("env: ‘");
		print_error(cmd->argv[1]);
		print_error("‘: No such file or directory\n");
		return (127);
	}
	print_env(cmd);
	exit_status = EXIT_SUCCESS;
	return (exit_status);
}
