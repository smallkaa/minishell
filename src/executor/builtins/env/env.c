/**
 * @file handle_env.c
 * @brief Functions for handling the `env` built-in command in Minishell.
 */
#include "minishell.h"

/**
 * @brief Prints all assigned environment variables from a single hash
 * table bucket.
 *
 * Iterates through the linked list of environment variables in the
 * given bucket and prints each assigned variable in the format:
 * ```
 * KEY=VALUE
 * ```
 * If a variable exists but has no assigned value, it is displayed as:
 * ```
 * KEY=
 * ```
 *
 * @param var Pointer to the first variable in the bucket.
 */
static void	print_env_bucket(t_mshell_var *var)
{
	while (var)
	{
		if (var->val_assigned)
		{
			if (var->value)
				printf("%s=%s\n", var->key, var->value);
			else
				printf("%s=\n", var->key);
		}
		var = var->next;
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
	t_hash_table	*ht;
	int				i;
	uint8_t			exit_status;

	if (cmd->argv[1])
	{
		print_error("Usage: ./minishell: env\n");
		exit_status = EXIT_FAILURE;
		return (exit_status);
	}
	ht = cmd->minishell->hash_table;
	i = 0;
	while (i < HASH_SIZE)
	{
		print_env_bucket(ht->buckets[i]);
		i++;
	}
	exit_status = EXIT_SUCCESS;
	return (exit_status);
}
