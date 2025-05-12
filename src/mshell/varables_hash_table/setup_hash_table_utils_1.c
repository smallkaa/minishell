/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setup_hash_table_utils_1.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imunaev- <imunaev-@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 14:50:18 by Ilia Munaev       #+#    #+#             */
/*   Updated: 2025/05/12 14:04:07 by imunaev-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file setup_hash_table_utils_1.c
 * @brief Utility functions for parsing environment variable strings
 *        and computing hash values for environment storage.
 */
#include "minishell.h"

/**
 * @brief Handles splitting and allocation when '=' is absent.
 *
 * @param kv_pair The input string to duplicate as key.
 * @return A pointer to t_mshell_var or NULL on failure.
 */
static t_mshell_var	*split_without_equal(char *kv_pair,
	t_mshell_var *mshell_var)
{
	mshell_var->key = ft_strdup(kv_pair);
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
static t_mshell_var	*split_with_equal(char *kv_pair,
	char *equal_sign,
	t_mshell_var *mshell_var)
{
	mshell_var->key = ft_substr(kv_pair, 0, equal_sign - kv_pair);
	if (!mshell_var->key)
	{
		print_error("-minishell: mshell_var->key ft_substr failed\n");
		free_ptr((void **)&mshell_var);
		return (NULL);
	}
	mshell_var->value = ft_strdup(equal_sign + 1);
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
