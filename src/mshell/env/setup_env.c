/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setup_env.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imunaev- <imunaev-@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 14:49:47 by Ilia Munaev       #+#    #+#             */
/*   Updated: 2025/05/12 12:42:14 by imunaev-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file setup_env.c
 * @brief Helpers for duplicating and managing the environment array.
 *
 * This file includes utilities for:
 * - Determining the size of a string array (`char **`).
 * - Creating a duplicate of the system environment (`envp`) for internal use.
 */
#include "minishell.h"

/**
 * @brief Calculates the number of strings in a NULL-terminated string array.
 *
 * This function is typically used to determine the number of environment
 * variables in `envp`.
 *
 * @param arr The array of strings (e.g., environment variables).
 * @return The number of elements in the array
 * (not including the NULL terminator).
 */
size_t	ft_arr_size(char **arr)
{
	size_t	len;

	len = 0;
	while (arr && arr[len])
		len++;
	return (len);
}

/**
 * @brief Allocates memory for the environment array.
 *
 * @param count Number of envp entries.
 * @return Pointer to the allocated array, or NULL on failure.
 */
static char	**alloc_env_array(size_t count)
{
	char	**env;

	env = malloc((count + 1) * sizeof(char *));
	if (!env)
	{
		print_error("-minishell: setup_env, env malloc failed\n");
		return (NULL);
	}
	ft_bzero(env, (count + 1) * sizeof(char *));
	return (env);
}

/**
 * @brief Copies envp strings to the allocated array.
 *
 * @param dst Destination array.
 * @param src Source array (envp).
 * @param count Number of strings to copy.
 * @return true on success, false on failure.
 */
static bool	copy_env_strings(char **dst, char **src, size_t count)
{
	size_t	i;

	i = 0;
	while (i < count)
	{
		dst[i] = ft_strdup(src[i]);
		if (!dst[i])
		{
			print_error("-minishell: setup_env, strdup failed\n");
			free_str_array_range(dst, i);
			return (false);
		}
		i++;
	}
	dst[i] = NULL;
	return (true);
}

/**
 * @brief Duplicates the given envp array.
 *
 * @param envp Original envp array.
 * @return Newly allocated and duplicated environment array.
 */
char	**setup_env(char **envp)
{
	size_t	envp_len;
	char	**env;

	envp_len = ft_arr_size(envp);
	env = alloc_env_array(envp_len);
	if (!env)
		return (NULL);
	if (!copy_env_strings(env, envp, envp_len))
	{
		free_env(&env);
		return (NULL);
	}
	return (env);
}
