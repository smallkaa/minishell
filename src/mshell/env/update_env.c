/**
 * @file update_env.c
 * @brief Synchronizes environment variables from hash table to env array.
 *
 * This file contains logic for updating the Minishell's `env` array
 * from its internal hash table representation. It only includes variables
 * marked as assigned (i.e., exported with a value).
 */
#include "minishell.h"

/**
 * @brief Adds an assigned environment variable to the new environment array.
 *
 * Formats the variable into a `KEY=VALUE` string and stores it in `new_env`.
 * If memory allocation fails, the function returns `false`.
 *
 * @param var Pointer to the environment variable structure.
 * @param new_env The array where the formatted entry will be stored.
 * @param idx Pointer to the current index in `new_env`.
 * @return `true` if the entry was added successfully, `false` on failure.
 */
bool	add_env_entry(t_mshell_var *var, char **new_env, int *idx)
{
	char	*entry;

	entry = create_env_entry(var);
	if (!entry)
		return (false);
	new_env[*idx] = entry;
	(*idx)++;
	return (true);
}

/**
 * @brief Processes a single bucket in the hash table and adds
 * entries to `new_env`.
 *
 * Iterates through all variables in a bucket and adds assigned variables
 * to `new_env` in the format `KEY=VALUE`.
 *
 * @param bucket The linked list of environment variables in a bucket.
 * @param new_env The array where environment variables are stored.
 * @param idx Pointer to the current index in `new_env`.
 * @return `true` if all entries were added successfully, `false`
 * if a failure occurred.
 */
bool	process_env_bucket(t_mshell_var *bucket, char **new_env, int *idx)
{
	t_mshell_var	*current;

	current = bucket;
	while (current)
	{
		if (current->val_assigned)
		{
			if (!add_env_entry(current, new_env, idx))
				return (false);
		}
		current = current->next;
	}
	return (true);
}

/**
 * @brief Populates the new environment array with assigned variables.
 *
 * Iterates through the hash table, extracts assigned variables, and
 * formats them into `KEY=VALUE` strings, storing them in `new_env`.
 *
 * @param mshell Pointer to the Minishell structure containing the hash table.
 * @param new_env The newly allocated array for storing environment variables.
 * @return `true` if successful, `false` if a memory allocation fails.
 */
static bool	populate_env_array(t_mshell *mshell, char **new_env)
{
	int	i;
	int	idx;

	i = 0;
	idx = 0;
	while (i < HASH_SIZE)
	{
		if (!process_env_bucket(mshell->hash_table->buckets[i], new_env, &idx))
			return (false);
		i++;
	}
	new_env[idx] = NULL;
	return (true);
}

/**
 * @brief Counts the number of exported environment variables.
 *
 * Iterates through the hash table to count variables marked as assigned.
 *
 * @param ht Pointer to the hash table containing environment variables.
 * @return The number of exported environment variables.
 */
static int	count_exported_vars(t_hash_tbl *ht)
{
	int				i;
	int				count;
	t_mshell_var	*curr;

	count = 0;
	i = 0;
	while (i < HASH_SIZE)
	{
		curr = ht->buckets[i];
		while (curr)
		{
			if (curr->val_assigned)
				count++;
			curr = curr->next;
		}
		i++;
	}
	return (count);
}

/**
 * @brief Updates the environment variable array.
 *
 * Allocates a new environment array, populates it with assigned variables
 * from the hash table, and replaces the old environment array.
 *
 * @param mshell Pointer to the Minishell structure.
 */
void	update_env(t_mshell *mshell)
{
	int		count;
	char	**new_env;
	bool	populated;

	count = count_exported_vars(mshell->hash_table);
	new_env = malloc(sizeof(char *) * (count + 1));
	if (!new_env)
	{
		print_error("-minishell: update_env: malloc failed\n");
		return ;
	}
	populated = populate_env_array(mshell, new_env);
	if (!populated)
	{
		free_old_env(new_env);
		return ;
	}
	free_old_env(mshell->env);
	mshell->env = new_env;
}
