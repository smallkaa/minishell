/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setup_hash_table_utils_1.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ilia Munaev <ilyamunaev@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 14:50:18 by Ilia Munaev       #+#    #+#             */
/*   Updated: 2025/05/10 03:14:11 by Ilia Munaev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file setup_hash_table_utils_1.c
 * @brief Utility functions for parsing environment variable strings
 *        and computing hash values for environment storage.
 */
#include "minishell.h"

/**
 * @brief Allocates and initializes a new shell variable.
 *
 * Duplicates the key and value (if present), and sets flags and next pointer.
 *
 * @param key Variable name (non-null).
 * @param value Variable value (can be NULL).
 * @param assigned 1 if value is assigned, 0 otherwise.
 * @return Pointer to new variable, or NULL on failure.
 */
t_mshell_var	*create_new_var(char *key, char *value, int assigned)
{
	t_mshell_var	*new_var;

	new_var = malloc(sizeof(t_mshell_var));
	if (!new_var)
		return (print_error("Error: new_var malloc failed\n"), NULL);
	new_var->key = ft_strdup(key);
	if (!new_var->key)
		return (free(new_var), NULL);
	if (value)
	{
		new_var->value = ft_strdup(value);
		if (!new_var->value)
		{
			free(new_var->key);
			free(new_var);
			return (NULL);
		}
	}
	else
		new_var->value = NULL;
	new_var->val_assigned = assigned;
	new_var->next = NULL;
	return (new_var);
}

/**
 * @brief Frees a t_mshell_var struct and its key/value.
 *
 * Used for safe cleanup of temporary key-value pairs.
 *
 * @param pair Pointer to the t_mshell_var to free.
 */
void	free_pair_and_return_null(t_mshell_var *pair)
{
	if (!pair)
		return ;
	free(pair->key);
	free(pair->value);
	free(pair);
}

/**
 * @brief Splits a key=value string into a key and value.
 *
 * Allocates and initializes a t_mshell_var struct. Handles both
 * cases where '=' is present and absent. Only key is required.
 *
 * @param kv_pair The input string to split.
 * @return A pointer to t_mshell_var or NULL on failure.
 */
static t_mshell_var	*alloc_and_split_pair(char *kv_pair)
{
	t_mshell_var	*mshell_var;
	char			*equal_sign;

	mshell_var = malloc(sizeof(t_mshell_var)); // tested 
	if (!mshell_var)
	{
		print_error("minishell: mshell_var malloc failed\n");
		return (NULL);
	}
	equal_sign = ft_strchr(kv_pair, '=');
	if (equal_sign)
	{
		mshell_var->key = ft_substr(kv_pair, 0, equal_sign - kv_pair);
		mshell_var->value = ft_strdup(equal_sign + 1);
	}
	else
	{
		mshell_var->key = ft_strdup(kv_pair);
		mshell_var->value = NULL;
	}
	return (mshell_var);
}

/**
 * @brief Wraps alloc_and_split_pair and checks memory integrity.
 *
 * Frees allocated memory and prints error if key/value allocation fails.
 *
 * @param kv_pair The input string to split.
 * @return A valid t_mshell_var pointer or NULL on failure.
 */
t_mshell_var	*split_key_value(char *kv_pair)
{
	t_mshell_var	*mshell_var;

	mshell_var = alloc_and_split_pair(kv_pair);
	if (!mshell_var)
		return (NULL);
	if (!mshell_var->key || (ft_strchr(kv_pair, '=') && !mshell_var->value))
	{
		if (mshell_var->key)
			free(mshell_var->key);
		if (mshell_var->value)
			free(mshell_var->value);
		free(mshell_var);
		print_error("-minishell: split_key_value, strdup/substr failed\n");
		return (NULL);
	}
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
