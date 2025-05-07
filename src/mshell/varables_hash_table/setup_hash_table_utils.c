/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setup_hash_table_utils.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ilia Munaev <ilyamunaev@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 14:50:18 by Ilia Munaev       #+#    #+#             */
/*   Updated: 2025/05/07 21:10:11 by Ilia Munaev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file setup_hash_table_utils.c
 * @brief Utility functions for parsing environment variable strings
 *        and computing hash values for environment storage.
 */
#include "minishell.h"

/**
 * @brief Splits a `KEY=VALUE` string into a `t_mshell_var` structure.
 *
 * This function is used to convert a string representation of an
 * environment variable into a structured key-value pair.
 *
 * Behavior:
 * - If the input string contains `'='`, it is split at the first occurrence.
 *   The part before the '=' becomes the key, the part after becomes the value.
 * - If there is no `'='`, the entire string becomes the key,
 *   and the value is set to NULL.
 *
 * @param kv_pair The string to split (e.g., "HOME=/Users/user").
 * @return A pointer to a newly allocated `t_mshell_var` structure,
 *         or NULL on allocation failure.
 */
t_mshell_var	*split_key_value(char *kv_pair)
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
		mshell_var->value = ft_strdup(equal_sign + 1); // tested
	}
	else
	{
		mshell_var->key = ft_strdup(kv_pair); // tested
		mshell_var->value = NULL;
	}
	if (!mshell_var->key || (equal_sign && !mshell_var->value))
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
