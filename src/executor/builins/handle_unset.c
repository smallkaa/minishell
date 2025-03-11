/**
 * @file handle_unset.c
 * @brief Functions for handling the `unset` built-in command in Minishell.
 */
#include "minishell.h"

/**
 * @brief Removes a variable from the hash table completely.
 *
 * Unlinks the node matching `key` from the hash table's bucket list and
 * frees its memory. Calls `update_env(mshell)` to resync the environment array.
 *
 * @param mshell Pointer to the Minishell structure.
 * @param key The variable name to unset.
 */
static void	remove_var_from_ht(t_mshell *mshell, char *key)
{
	unsigned long	index;
	t_mshell_var	*prev;
	t_mshell_var	*current;

	prev = NULL;
	if (!mshell || !key)
		return;
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
			free(current->key);
			free(current->value);
			free(current);
			update_env(mshell);
			return;
		}
		prev = current;
		current = current->next;
	}
}

/**
 * @brief Loops over all `unset` arguments and removes each variable.
 *
 * @param cmd Pointer to the command structure.
 * @return The final exit status after removing all requested variables.
 */
static uint8_t	do_unset_loop(t_cmd *cmd)
{
	uint8_t	status;
	int		i;

	status = EXIT_SUCCESS;
	i = 1;
	while (cmd->argv[i])
	{
		remove_var_from_ht(cmd->minishell, cmd->argv[i]);
		i++;
	}
	cmd->minishell->exit_status = status;
	return (status);
}

/**
 * @brief Handles the `unset` built-in command.
 *
 * This function checks basic validity, then iterates through all arguments in
 * `cmd->argv`, removing each specified variable from the Minishell environment.
 * If called within a pipeline, the function exits the process after performing
 * the unset operations.
 *
 * Usage: `unset VAR1 [VAR2 VAR3 ...]`
 *
 * @param cmd Pointer to the command structure (contains argv, minishell, etc.).
 * @return `EXIT_SUCCESS` if all unsets succeed (or if no arguments),
 *         otherwise `EXIT_FAILURE`.
 */
uint8_t	handle_unset(t_cmd *cmd)
{
	uint8_t	status;

	status = EXIT_SUCCESS;
	if (!cmd || !cmd->minishell)
	{
		status = EXIT_FAILURE;
		if (cmd && cmd->in_pipe)
			exit(status);
		return (status);
	}
	if (!cmd->argv[1])
	{
		if (cmd->in_pipe)
			exit(status);
		return (status);
	}
	status = do_unset_loop(cmd);
	if (cmd->in_pipe)
		exit(status);
	return (status);
}
