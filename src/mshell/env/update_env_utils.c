/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   update_env_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imunaev- <imunaev-@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 14:50:08 by Ilia Munaev       #+#    #+#             */
/*   Updated: 2025/05/12 13:57:48 by imunaev-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file env_utils.c
 * @brief Utility functions for creating and freeing environment variables.
 *
 * This file provides helper functions used when managing environment variables
 * in Minishell, including creation of `KEY=VALUE` formatted strings
 * and cleanup of dynamically allocated environment arrays.
 */
#include "minishell.h"

/**
 * @brief Frees a partially filled environment array.
 *
 * Used when building the env array fails partway through.
 *
 * @param env The environment array to free.
 * @param count Number of filled entries in the array.
 */
void	free_partial_env(char **env, int count)
{
	int	i;

	if (!env)
		return ;
	i = 0;
	while (i < count)
	{
		free(env[i]);
		i++;
	}
	free(env);
}

/**
 * @brief Creates a formatted environment entry string.
 *
 * Constructs a new string in the format `KEY=VALUE` using the key and value
 * from the provided environment variable structure.
 *
 * @param var Pointer to the `t_mshell_var` structure containing key-value data.
 * @return A newly allocated string containing `KEY=VALUE`, or NULL on failure.
 */
char	*create_env_entry(t_mshell_var *mshell_var)
{
	size_t	total_len;
	char	*entry;
	char	*val;
	int		len_key;
	int		len_val;

	if (!mshell_var || !mshell_var->key)
		return (NULL);
	if (mshell_var->value)
		val = mshell_var->value;
	else
		val = "";
	len_key = ft_strlen(mshell_var->key);
	len_val = ft_strlen(val);
	total_len = len_key + len_val + 2;
	entry = malloc(total_len * sizeof(char));
	if (!entry)
	{
		print_error("-minishell: create_env_entry, malloc failed\n");
		return (NULL);
	}
	ft_strlcpy(entry, mshell_var->key, total_len);
	ft_strlcat(entry, "=", total_len);
	ft_strlcat(entry, val, total_len);
	return (entry);
}
