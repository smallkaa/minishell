/**
 * @file unset.c
 * @brief Implementation of the `unset` built-in command for Minishell.
 *
 * This file defines the logic for handling `unset`, which removes one or more
 * environment variables from the shell’s internal hash table and updates the
 * environment accordingly. Invalid options and variable names
 * are gracefully handled.
 */
#include "minishell.h"

/**
 * @brief Checks if the given argument is an unsupported option.
 *
 * Minishell does not support flags/options for `unset`. Therefore,
 * any argument starting with `-` and followed by characters is treated as
 * invalid.
 *
 * @param name The argument string to check.
 * @return `true` if the argument is an invalid option, `false` otherwise.
 */
static bool	is_invalid_option(const char *name)
{
	return (name && name[0] == '-' && name[1]);
}

/**
 * @brief Validates a string as a valid environment variable name.
 *
 * A valid identifier must:
 * - Start with a letter or underscore.
 * - Contain only alphanumeric characters or underscores.
 *
 * @param name The variable name to validate.
 * @return `true` if valid, `false` otherwise.
 */
static bool	is_valid_identifier(const char *name)
{
	int	i;

	if (!name || (!ft_isalpha(name[0]) && name[0] != '_'))
		return (false);
	i = 1;
	while (name[i])
	{
		if (!ft_isalnum(name[i]) && name[i] != '_')
			return (false);
		i++;
	}
	return (true);
}

/**
 * @brief Removes a variable from the shell's environment hash table.
 *
 * Looks for a variable with the specified `key` in the hash table and unlinks
 * it from the corresponding bucket. After successful removal, it updates the
 * exported environment (`mshell->env`) via `update_env()`.
 *
 * @param mshell Pointer to the shell instance.
 * @param key The name of the environment variable to remove.
 * @return Always returns `EXIT_SUCCESS`, even if the key is not found.
 */
static uint8_t	remove_var_from_ht(t_mshell *mshell, char *key)
{
	unsigned long	index;
	t_mshell_var	*prev;
	t_mshell_var	*current;

	prev = NULL;
	index = hash_function(key);
	current = mshell->hash_table->buckets[index];
	while (current)
	{
		if (current->key && ft_strcmp(current->key, key) == 0)
		{
			if (prev == NULL)
				mshell->hash_table->buckets[index] = current->next;
			else
				prev->next = current->next;
			free_mshell_var(current);
			update_env(mshell);
			return (EXIT_SUCCESS);
		}
		prev = current;
		current = current->next;
	}
	return (EXIT_SUCCESS);
}

/**
 * @brief Iterates over `unset` arguments and processes each one.
 *
 * Skips invalid options. Valid identifiers are removed from the environment.
 * Invalid identifiers (e.g., `1VAR`) are silently ignored per Bash behavior.
 *
 * @param cmd Pointer to the command structure containing arguments.
 * @return `EXIT_SUCCESS` if all arguments are processed.
 *         `EXIT_FAILURE` if an invalid option is detected.
 */
static uint8_t	do_unset_loop(t_cmd *cmd)
{
	int	i;

	i = 1;
	while (cmd->argv[i])
	{
		if (is_invalid_option(cmd->argv[i]))
			return (unset_error(cmd->argv[i]));
		if (is_valid_identifier(cmd->argv[i]))
			remove_var_from_ht(cmd->minishell, cmd->argv[i]);
		i++;
	}
	return (EXIT_SUCCESS);
}

/**
 * @brief Main handler for the `unset` built-in command.
 *
 * Removes one or more environment variables from the shell. Handles edge cases:
 * - No arguments → no operation.
 * - Missing shell context → prints an error.
 *
 * @param cmd Pointer to the command structure with arguments and shell context.
 * @return `EXIT_SUCCESS` if successful or no arguments were given.
 *         `EXIT_FAILURE` if command context is missing or an error occurs.
 */
uint8_t	handle_unset(t_cmd *cmd)
{
	if (!cmd)
		return (no_cmd_error("unset"));
	if (!cmd->argv[1])
		return (EXIT_SUCCESS);
	return (do_unset_loop(cmd));
}
