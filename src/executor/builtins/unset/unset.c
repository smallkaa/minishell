/**
 * @file handle_unset.c
 * @brief Functions for handling the `unset` built-in command in Minishell.
 */
#include "minishell.h"

static bool	is_invalid_option(const char *name)
{
	return (name && name[0] == '-' && name[1]);
}

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
 * @brief Removes a variable from the Minishell environment's hash table.
 *
 * This function searches for a variable with the given `key` in the hash
 * table.
 * If found, it removes the variable by unlinking it from the corresponding
 * bucket list and frees its allocated memory. After deletion,
 * `update_env(mshell)` is called to synchronize the shell's environment
 * array.
 *
 * @param mshell Pointer to the Minishell structure containing the hash table.
 * @param key The name of the variable to be removed.
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

static uint8_t	do_unset_loop(t_cmd *cmd)
{
	int	i;

	i = 1;
	while (cmd->argv[i])
	{
		if (is_invalid_option(cmd->argv[i]))
		{
			unset_error(cmd->argv[i]);
			return (2);
		}
		if (is_valid_identifier(cmd->argv[i]))
			remove_var_from_ht(cmd->minishell, cmd->argv[i]);
		i++;
	}
	return (EXIT_SUCCESS);
}

/**
 * @brief Handles the `unset` built-in command in Minishell.
 *
 * The `unset` command removes one or more environment variables from the
 * shell.
 * - If no arguments are provided, the function returns `EXIT_SUCCESS`.
 * - If the command structure (`cmd`) or Minishell instance is missing, it
 * prints an error.
 * - Otherwise, it loops through the arguments and removes each specified
 * variable.
 *
 * If `unset` is executed in a subshell (e.g., inside a pipeline), the
 * process may exit
 * after execution, depending on the shell's implementation.
 *
 * **Usage Example:**
 * ```
 * unset VAR1 VAR2 VAR3
 * ```
 *
 * @param cmd Pointer to the command structure (contains argv, minishell
 *			instance, etc.).
 * @return `EXIT_SUCCESS` if all requested variables were removed or no
 *		 arguments were provided.
 *		 `EXIT_FAILURE` if an error occurs (e.g., missing command structure).
 */
uint8_t	handle_unset(t_cmd *cmd)
{
	if (!cmd || !cmd->argv)
	{
		print_error("minishell: unset: no cmd or minishell instanse\n");
		return (EXIT_FAILURE);
	}
	if (!cmd->argv[1])
		return (EXIT_SUCCESS);
	return (do_unset_loop(cmd));
}
