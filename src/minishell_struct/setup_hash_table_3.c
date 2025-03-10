#include "minishell.h"

/**
 * @brief Hash function to map keys to an index in the hash table.
 *
 * Uses the `djb2` algorithm to compute a hash value for a given key.
 *
 * @param key The key (environment variable name).
 * @return A hash index between 0 and `HASH_SIZE - 1`.
 */
unsigned int	hash_function(const char *key)
{
	unsigned long	hash;
	int				c;

	hash = 5381;
	while ((c = *key++))
		hash = ((hash << 5) + hash) + c;
	return (hash % HASH_SIZE);
}

/**
 * @brief Frees the memory allocated for the environment variables array.
 *
 * Iterates through the `env` array, freeing each entry, and
 *          then frees the array itself.
 *
 * @param env The array of environment variables to free.
 */
void	free_old_env(char **env)
{
	int	i;

	if (!env)
		return ;
	i = 0;
	while (env[i])
	{
		free(env[i]);
		i++;
	}
	free(env);
}

/**
 * @brief Updates the shell's environment variable array.
 *
 * Allocates a new environment array, populates it with variables
 * from the hash table,
 * and replaces the old environment array with the new one.
 *
 * @param minishell The main shell structure containing environment variables.
 */
void	update_env(t_mshell *minishell)
{
	int		count;
	char	**new_env;

	count = count_exported_vars(minishell->hash_table);
	new_env = malloc((count + 1) * sizeof(char *));
	if (!new_env)
	{
		print_error("minishell: update_environ, malloc failed\n");
		return ;
	}
	populate_env_array(minishell, new_env);
	free_old_env(minishell->env);
	minishell->env = new_env;
}

/**
 * @brief Updates an existing environment variable in the hash table.
 *
 * If the variable exists, updates its value and ensures it's marked as assigned.
 *
 * @param current The existing variable to update.
 * @param mshell_var The new variable data.
 * @param val_assigned Flag indicating whether the variable should be
 *                     marked as assigned.
 * @param minishell Pointer to the shell structure.
 */
void	update_existing_variable(t_mshell_var *current, t_mshell_var *mshell_var, int val_assigned, t_mshell *minishell)
{
	if (mshell_var->value)
	{
		free(current->value);
		current->value = ft_strdup(mshell_var->value);
	}
	current->val_assigned = val_assigned;
	if (val_assigned)
		update_env(minishell);
}
