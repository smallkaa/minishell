/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setup_env.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ilia Munaev <ilyamunaev@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 14:49:47 by Ilia Munaev       #+#    #+#             */
/*   Updated: 2025/05/07 20:55:37 by Ilia Munaev      ###   ########.fr       */
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
 * @brief Duplicates the environment variable array.
 *
 * This function creates a deep copy of the system `envp` array so it can be
 * safely modified within Minishell.
 *
 * Memory is allocated for each string and for the array itself. The returned
 * array must be freed manually.
 *
 * @param envp The original environment variable array (from `main()`).
 * @return A newly allocated deep copy of `envp`, or `NULL`
 * on allocation failure.
 */
char	**setup_env(char **envp)
{
	int		i;
	int		envp_len;
	char	**env;

	envp_len = ft_arr_size(envp);
	env = malloc((envp_len + 1) * sizeof(char *)); // tested
	if (!env)
	{
		print_error("Error (setup_envp): env malloc failed\n");
		return (NULL);
	}
	i = 0;
	while (envp[i])
	{
		env[i] = ft_strdup(envp[i]); // tested
		i++;
	}
	env[i] = NULL;
	return (env);
}
