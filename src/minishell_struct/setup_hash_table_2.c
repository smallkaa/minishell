#include "minishell.h"
/**
 * @brief Creates a new variable for the hash table.
 *
 * Allocates memory and initializes a new environment variable structure.
 *
 * @param mshell_var The key-value pair to store.
 * @param val_assigned Flag indicating if the value is assigned.
 * @return A pointer to the newly created variable or NULL if allocation fails.
 */
t_mshell_var	*create_new_variable(t_mshell_var *mshell_var, int val_assigned)
{
	t_mshell_var	*new_var;

	new_var = malloc(sizeof(t_mshell_var));
	if (!new_var)
	{
		print_error("Error (set_variable): new_var malloc failed\n");
		return (NULL);
	}
	new_var->key = ft_strdup(mshell_var->key);
	if (mshell_var->value)
		new_var->value = ft_strdup(mshell_var->value);
	else
	{
		new_var->value = NULL;
		new_var->val_assigned = val_assigned;
		new_var->next = NULL;
	}
	return (new_var);
}

/**
 * @brief Creates an environment entry in the format `KEY=VALUE`.
 *
 * Allocates memory for a new string combining `key=value`, ensuring
 * proper memory allocation and safety.
 *
 * @param var The environment variable structure containing the key-value pair.
 * @return A newly allocated string containing `KEY=VALUE`, or NULL if allocation fails.
 */
char	*create_env_entry(t_mshell_var *var)
{
	const char	*val;
	size_t		total_len;
	char		*entry;

	if (var->value)
		val = var->value;
	else
		val = "";
	total_len = ft_strlen(var->key) + ft_strlen(val) + 2;
	entry = malloc(total_len);
	if (!entry)
		return (NULL);
	ft_strlcpy(entry, var->key, total_len);
	ft_strlcat(entry, "=", total_len);
	ft_strlcat(entry, val, total_len);
	return (entry);
}

/**
 * @brief Adds an environment entry to the `new_env` array.
 *
 * Allocates memory for an environment variable formatted as `KEY=VALUE`
 * and stores it in the provided `new_env` array at the given index.
 *
 * @param current The current environment variable node.
 * @param new_env The environment array where the entry will be stored.
 * @param index Pointer to the index where the new entry should be placed.
 * @return `true` if successful, `false` if memory allocation fails.
 */
bool	add_env_entry(t_mshell_var *current, char **new_env, int *index)
{
	new_env[*index] = create_env_entry(current);
	if (!new_env[*index])
	{
		print_error("minishell: update_environ, malloc failed\n");
		return (false);
	}
	(*index)++;
	return (true);
}

/**
 * @brief Processes a single bucket in the hash table and
 *        adds entries to `new_env`.
 *
 * Iterates through all variables in a bucket and adds them to
 * `new_env` if assigned.
 *
 * @param bucket The linked list of environment variables in a bucket.
 * @param new_env The array where environment variables are stored.
 * @param index Pointer to the current index in `new_env`.
 * @return `true` if all entries were added successfully,
 *         `false` if a failure occurred.
 */
bool	process_env_bucket(t_mshell_var *bucket, char **new_env, int *index)
{
	t_mshell_var	*current;

	current = bucket;
	while (current)
	{
		if (current->val_assigned)
		{
			if (!add_env_entry(current, new_env, index))
				return (false);
		}
		current = current->next;
	}
	return (true);
}

/**
 * @brief Populates the `new_env` array with environment variables from the hash table.
 *
 * Iterates through the hash table, processing each bucket and adding assigned variables
 * to `new_env`. If memory allocation fails, it frees the partially created array.
 *
 * @param minishell Pointer to the main shell structure containing the hash table.
 * @param new_env The array to be populated with environment variables.
 */
void	populate_env_array(t_mshell *minishell, char **new_env)
{
	int	index;
	int	i;

	index = 0;
	i = 0;
	while (i < HASH_SIZE)
	{
		if (!process_env_bucket(minishell->hash_table->buckets[i], new_env, &index))
		{
			free_old_env(new_env);
			return ;
		}
		i++;
	}
	new_env[index] = NULL;
}
