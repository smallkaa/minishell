/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setup_hash_table_utils_1.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ilia Munaev <ilyamunaev@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 14:50:18 by Ilia Munaev       #+#    #+#             */
/*   Updated: 2025/05/10 22:06:34 by Ilia Munaev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file setup_hash_table_utils_1.c
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

	var = malloc(sizeof(t_mshell_var)); // tested FINAL
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

	mshell_var = alloc_var_struct(key); // tested FINAL
	if (!mshell_var)
		return (NULL);
	if (value)
	{
		dup = ft_strdup(value); //
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
static t_mshell_var	*allocate_env_var(void)
{
	t_mshell_var	*mshell_var;

	mshell_var = malloc(sizeof(t_mshell_var)); // tested FINAL
	if (!mshell_var)
	{
		print_error("-minishell: mshell_var malloc failed\n");
		return (NULL);
	}
	ft_memset(mshell_var, 0, sizeof(t_mshell_var));
	return (mshell_var);
}

/**
 * @brief Handles splitting and allocation when '=' is absent.
 *
 * @param kv_pair The input string to duplicate as key.
 * @return A pointer to t_mshell_var or NULL on failure.
 */
static t_mshell_var	*split_without_equal(char *kv_pair, t_mshell_var *mshell_var)
{
	mshell_var->key = ft_strdup(kv_pair); // tested FINAL
	if (!mshell_var->key)
	{
		print_error("-minishell: mshell_var->key ft_strdup failed\n");
		free_pair(&mshell_var);
		return (NULL);
	}
	mshell_var->value = NULL;
	return (mshell_var);
}

/**
 * @brief Handles splitting and allocation when '=' is present.
 *
 * @param kv_pair The original string.
 * @param equal_sign Pointer to '=' inside kv_pair.
 * @return A pointer to t_mshell_var or NULL on failure.
 */
static t_mshell_var	*split_with_equal(char *kv_pair, char *equal_sign, t_mshell_var *mshell_var)
{
	mshell_var->key = ft_substr(kv_pair, 0, equal_sign - kv_pair); // tested FINAL
	if (!mshell_var->key)
	{
		print_error("-minishell: mshell_var->key ft_substr failed\n");
		free_ptr((void **)&mshell_var);
		return (NULL);
	}
	mshell_var->value = ft_strdup(equal_sign + 1); // tested FINAL
	if (!mshell_var->value)
	{
		print_error("-minishell: mshell_var->value ft_strdup failed\n");
		free_pair(&mshell_var);
		return (NULL);
	}
	return (mshell_var);
}

/**
 * @brief Splits a key=value string into a key and value.
 *
 * Delegates '=' and non-'=' handling to helper functions.
 *
 * @param kv_pair The input string to split.
 * @return A pointer to t_mshell_var or NULL on failure.
 */
static t_mshell_var	*alloc_and_split_pair(char *kv_pair)
{
	char			*equal_sign;
	t_mshell_var	*mshell_var;

	mshell_var = allocate_env_var();
	if (!mshell_var)
		return (NULL);
	equal_sign = ft_strchr(kv_pair, '=');
	if (equal_sign)
		return (split_with_equal(kv_pair, equal_sign, mshell_var));
	else
		return (split_without_equal(kv_pair, mshell_var));
}

/**
 * @brief Wraps alloc_and_split_pair and checks memory integrity.
 *
 * Frees allocated memory and prints error if key/value allocation fails.
 *
 * @param kv_pair Key=value environment string to split.
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
		free_pair(&mshell_var);
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
