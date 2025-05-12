/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setup_hash_table_utils_3.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imunaev- <imunaev-@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/08 09:49:13 by Ilia Munaev       #+#    #+#             */
/*   Updated: 2025/05/12 14:04:34 by imunaev-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file setup_hash_table_utils_3.c
 * @brief Utility functions for parsing environment variable strings
 *        and computing hash values for environment storage.
 */
#include "minishell.h"

/**
 * @brief Allocates a new t_mshell_var and duplicates the key.
 *
 * @param key The variable name (non-null).
 * @return A partially initialized t_mshell_var or NULL on failure.
 */
static t_mshell_var	*alloc_var_struct(char *key)
{
	t_mshell_var	*var;
	char			*dup;

	var = malloc(sizeof(t_mshell_var));
	if (!var)
	{
		print_error("-minishell: new_var malloc failed\n");
		return (NULL);
	}
	ft_memset(var, 0, sizeof(t_mshell_var));
	dup = ft_strdup(key);
	if (!dup)
	{
		print_error("-minishell: new_var->key ft_strdup failed\n");
		free_ptr((void **)&var);
		return (NULL);
	}
	free(var->key);
	var->key = dup;
	return (var);
}

/**
 * @brief Creates and initializes a new shell variable with key
 * and optional value.
 *
 * @param key Variable name (non-null).
 * @param value Variable value (can be NULL).
 * @param assigned 1 if value is assigned, 0 otherwise.
 * @return Pointer to new variable, or NULL on failure.
 */
t_mshell_var	*create_new_var(char *key, char *value, int assigned)
{
	t_mshell_var	*mshell_var;
	char			*dup;

	mshell_var = alloc_var_struct(key);
	if (!mshell_var)
		return (NULL);
	if (value)
	{
		dup = ft_strdup(value);
		if (!dup)
		{
			print_error("-minishell: new_var->value ft_strdup failed\n");
			free_pair(&mshell_var);
			return (NULL);
		}
		free(mshell_var->value);
		mshell_var->value = dup;
	}
	mshell_var->val_assigned = assigned;
	return (mshell_var);
}

/**
 * @brief Allocates and zero-initializes a t_mshell_var.
 *
 * @return Pointer to new variable, or NULL on failure.
 */
t_mshell_var	*allocate_env_var(void)
{
	t_mshell_var	*mshell_var;

	mshell_var = malloc(sizeof(t_mshell_var));
	if (!mshell_var)
	{
		print_error("-minishell: mshell_var malloc failed\n");
		return (NULL);
	}
	ft_memset(mshell_var, 0, sizeof(t_mshell_var));
	return (mshell_var);
}

/**
 * @brief Hash function for computing bucket index.
 *
 * Implements the djb2 algorithm by Dan Bernstein. Iterates through each
 * character of the key and computes a hash value, then reduces it using
 * modulo with the hash table size.
 *
 * @param key The input key string.
 * @return An unsigned int representing the hash bucket index.
 */
unsigned int	hash_function(const char *key)
{
	unsigned long	hash;
	int				c;

	hash = 5381;
	while (*key)
	{
		c = *key++;
		hash = ((hash << 5) + hash) + c;
	}
	return (hash % HASH_SIZE);
}
