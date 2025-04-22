/**
 * @file setup_hash_table.c
 * @brief Environment variable management via hash table for Minishell.
 *
 * This file contains functions to initialize and populate a hash table with
 * environment variables, update variables, and manage internal shell state
 * efficiently.
 */
#include "minishell.h"

/**
 * @brief Creates a new key-value variable node for the hash table.
 *
 * This function allocates and initializes a `t_mshell_var` structure
 * with a given key and value. The `assigned` flag indicates whether
 * the variable was assigned a value or just declared.
 *
 * @param key The environment variable name (must not be NULL).
 * @param value The value of the variable (can be NULL).
 * @param assigned Whether the variable has a value assigned (1) or not (0).
 * @return Pointer to the new variable node, or NULL on allocation failure.
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
 * @brief Inserts or updates an environment variable in the hash table.
 *
 * If the key already exists, its value and assignment flag are updated.
 * Otherwise, a new variable node is inserted at the beginning of the bucket.
 *
 * @param mshell Pointer to the Minishell instance.
 * @param key The name of the variable to set.
 * @param value The value to assign (can be NULL).
 * @param assigned 1 if a value is assigned, 0 if it's just declared.
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
 * @brief Populates the hash table from the environment array (`mshell->env`).
 *
 * Iterates through `mshell->env`, splits key-value pairs, and adds them
 * to the hash table. Also ensures `OLDPWD` is initialized using `HOME` if found.
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
 * @brief Initializes an empty hash table structure.
 *
 * Allocates memory and sets all bucket pointers to NULL.
 *
 * @return Pointer to the initialized hash table, or NULL on failure.
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
 * @brief Initializes and sets up the Minishell hash table.
 *
 * This function:
 * - Allocates the hash table.
 * - Loads variables from `mshell->env` into it.
 * - Updates the shell’s environment array from the hash table.
 *
 * @param mshell Pointer to the Minishell structure.
 * @return `EXIT_SUCCESS` if setup is successful, `EXIT_FAILURE` otherwise.
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
