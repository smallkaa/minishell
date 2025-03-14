/**
 * @file handle_env.c
 * @brief Functions for displaying environment variables in Minishell.
 */
#include "minishell.h"

/**
 * @brief Prints all assigned environment variables from a single hash table bucket.
 *
 * Iterates through the linked list of environment variables in the given bucket
 * and prints each assigned variable in the format `KEY=VALUE`. If the variable has
 * no value, it is displayed as `KEY=`.
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
 * @brief Handles the `env` built-in command to display environment variables
 *        or attempt to execute a command.
 *
 * If `env` is run without arguments, it prints all assigned environment variables.
 * If an argument is provided, it tries to execute the given command in the current environment.
 *
 * @param cmd Pointer to the command structure containing execution context.
 * @return `EXIT_SUCCESS` if successful, `127` if the command does not exist.
 */
uint8_t	handle_env(t_cmd *cmd)
{
	t_hash_table	*ht;
	int				i;
	uint8_t			exit_status;
	t_cmd			*cmd_to_execute;

	// Case: No arguments -> Print environment variables
	if (!cmd->argv[1])
	{
		ht = cmd->minishell->hash_table;
		i = 0;
		while (i < HASH_SIZE)
		{
			print_env_bucket(ht->buckets[i]);
			i++;
		}
		exit_status = EXIT_SUCCESS;
	}
	else
	{
		cmd_to_execute = run_parser(cmd->minishell, cmd->argv[1]);
		exit_status = run_executor(cmd_to_execute);
	}
	cmd->minishell->exit_status = exit_status;
	if (cmd->in_pipe)
		exit(exit_status);
	return (exit_status);
}
