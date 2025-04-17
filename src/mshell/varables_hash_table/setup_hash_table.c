/**
 * @file setup_hash_table.c
 * @brief Environment variable management for Minishell.
 */
#include "minishell.h"

/**
 * @brief Creates a new environment variable structure.
 *
 * Allocates memory for a new `t_mshell_var` structure,
 * initializes it with the given key-value pair, and sets the assignment flag.
 *
 * @param key The key of the environment variable.
 * @param value The value of the environment variable. Can be NULL.
 * @param assigned Flag indicating if the variable is assigned a value.
 * @return A pointer to the newly created variable, or NULL if allocation fails.
 */
t_mshell_var	*create_new_var(char *key, char *value, int assigned)
{
	t_mshell_var	*new_var;

	new_var = malloc(sizeof(t_mshell_var));
	if (!new_var)
	{
		print_error("Error: new_var malloc failed\n");
		return (NULL);
	}
	new_var->key = ft_strdup(key);
	if (value)
		new_var->value = ft_strdup(value);
	else
		new_var->value = NULL;
	new_var->val_assigned = assigned;
	new_var->next = NULL;
	return (new_var);
}

/**
 * @brief Adds or updates an environment variable in the hash table.
 *
 * If the variable already exists, its value is updated. Otherwise,
 * a new variable is created and added to the hash table.
 *
 * @param mshell Pointer to the Minishell structure.
 * @param key The key of the variable to set.
 * @param value The value of the variable. Can be NULL.
 * @param assigned Flag indicating if the variable is assigned a value.
 */
void	set_variable(t_mshell *mshell, char *key, char *value, int assigned)
{
	unsigned int	index;
	t_mshell_var	*curr;
	t_mshell_var	*new_var;

	index = hash_function(key);
	curr = mshell->hash_table->buckets[index];
	while (curr)
	{
		if (ft_strcmp(curr->key, key) == 0)
		{
			if (value)
			{
				free(curr->value);
				curr->value = ft_strdup(value);
			}
			curr->val_assigned = assigned;
			return ;
		}
		curr = curr->next;
	}
	new_var = create_new_var(key, value, assigned);
	if (!new_var)
		return ;
	new_var->next = mshell->hash_table->buckets[index];
	mshell->hash_table->buckets[index] = new_var;
}

/**
 * @brief Loads environment variables into the hash table.
 *
 * Iterates through the `mshell->env` array, extracts key-value pairs,
 * and adds them to the hash table.
 *
 * @param mshell Pointer to the Minishell structure.
 */
static void	load_env_into_ht(t_mshell *mshell)
{
	t_mshell_var	*tmp;
	int				i;
	char			*home;

	i = 0;
	while (mshell->env[i])
	{
		tmp = split_key_value(mshell->env[i]);
		if (tmp)
		{
			set_variable(mshell, tmp->key, tmp->value, 1);
			free(tmp->key);
			free(tmp->value);
			free(tmp);
		}
		i++;
	}
	home = ms_getenv(mshell, "HOME");
	if (!home)
	{
		print_error("minishell: load_env_into_ht: retrieve home dir failed\n");
		return ;
	}
	set_variable(mshell, "OLDPWD", home, 1);
}

/**
 * @brief Initializes a new hash table for environment variables.
 *
 * Allocates memory for the hash table and initializes all buckets to NULL.
 *
 * @return A pointer to the newly created hash table, or NULL on failure.
 */
static t_hash_tbl	*init_hash_tbl(void)
{
	t_hash_tbl	*ht;
	int			i;

	ht = malloc(sizeof(t_hash_tbl));
	if (!ht)
	{
		(void)print_error("hash_table malloc failed\n");
		return (NULL);
	}
	i = 0;
	while (i < HASH_SIZE)
	{
		ht->buckets[i] = NULL;
		i++;
	}
	return (ht);
}

/**
 * @brief Sets up the hash table and populates it with environment variables.
 *
 * This function initializes the hash table and loads existing environment
 * variables into it.
 *
 * @param mshell Pointer to the Minishell structure.
 * @return `EXIT_SUCCESS` on success, `EXIT_FAILURE` on failure.
 */
int	setup_hash_table(t_mshell *mshell)
{
	mshell->hash_table = init_hash_tbl();
	if (!mshell->hash_table)
		return (EXIT_FAILURE);
	load_env_into_ht(mshell);
	update_env(mshell);
	return (EXIT_SUCCESS);
}
