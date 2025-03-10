#include "minishell.h"

/**
 * @brief Adds or updates an environment variable in the shell's hash table.
 *
 * @param minishell The shell structure.
 * @param mshell_var The variable structure containing key and value.
 * @param val_assigned Whether the variable should be assigned a value.
 */
void	set_variable(t_mshell *minishell, t_mshell_var *mshell_var, int val_assigned)
{
	unsigned int	index;
	t_mshell_var	*current;
	t_hash_table	*hash_table;
	t_mshell_var	*new_var;

	index = hash_function(mshell_var->key);
	hash_table = minishell->hash_table;
	current = hash_table->buckets[index];
	while (current)
	{
		if (ft_strcmp(current->key, mshell_var->key) == 0)
		{
			update_existing_variable(current, mshell_var, val_assigned, minishell);
			return ;
		}
		current = current->next;
	}
	new_var = create_new_variable(mshell_var, val_assigned);
	if (!new_var)
		return ;
	new_var->next = hash_table->buckets[index];
	hash_table->buckets[index] = new_var;
	if (val_assigned)
		update_env(minishell);
}

/**
 * @brief Extracts and stores environment variables in the hash table.
 *
 * Iterates over `minishell->env`, splitting each `KEY=VALUE` pair and storing
 * it in the hash table.
 *
 * @param minishell Pointer to the shell structure containing
 *                  the environment variables.
 */
static void	store_env_variables(t_mshell *minishell)
{
	t_mshell_var	*mshell_var;
	int				i;

	i = 0;
	while (minishell->env[i])
	{
		mshell_var = split_key_value(minishell->env[i]);
		if (mshell_var)
		{
			set_variable(minishell, mshell_var, 0);
			free(mshell_var->key);
			free(mshell_var->value);
			free(mshell_var);
		}
		i++;
	}
}

/**
 * @brief Marks all environment variables in the hash table as assigned.
 *
 * Iterates through each bucket in the hash table and updates the `val_assigned` flag.
 *
 * @param minishell Pointer to the shell structure containing the hash table.
 */
static void	mark_all_variables_assigned(t_mshell *minishell)
{
	t_mshell_var	*curr;
	int				i;

	i = 0;
	while (i < HASH_SIZE)
	{
		curr = minishell->hash_table->buckets[i];
		while (curr)
		{
			curr->val_assigned = 1;
			curr = curr->next;
		}
		i++;
	}
}

/**
 * @brief Populates the minishell's hash table with environment variables.
 *
 * This function processes the existing environment variables and stores them
 * in the hash table as key-value pairs. It then marks each variable as assigned.
 *
 * @param minishell The shell structure containing the environment variables.
 */
static void	fillup_hash_table(t_mshell *minishell)
{
	store_env_variables(minishell);
	mark_all_variables_assigned(minishell);
	update_env(minishell);
}

/**
 * @brief Initializes the minishell's hash table and
 *        populates it with environment variables.
 *
 * Allocates memory for the hash table, initializes its buckets to NULL,
 * and calls `fillup_hash_table()` to populate it.
 *
 * @param minishell The shell structure to initialize the hash table for.
 * @return A pointer to the initialized hash table, or NULL on failure.
 */
t_hash_table	*setup_hash_table(t_mshell *minishell)
{
	t_hash_table	*hash_table;
	int				i;

	hash_table = malloc(sizeof(t_hash_table));
	if (!hash_table)
	{
		print_error("Error (setup_hash_table): hash_table malloc failed\n");
		return (NULL);
	}
	i = 0;
	while (i < HASH_SIZE)
	{
		hash_table->buckets[i] = NULL;
		i++;
	}
	minishell->hash_table = hash_table;
	fillup_hash_table(minishell);
	return (hash_table);
}
