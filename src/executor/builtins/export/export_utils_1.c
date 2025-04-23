/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils_1.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ilia Munaev <ilyamunaev@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 14:45:29 by Ilia Munaev       #+#    #+#             */
/*   Updated: 2025/04/23 14:45:30 by Ilia Munaev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file export_utils_1.c.c
 * @brief Sorting and printing logic for the `export` built-in in Minishell.
 *
 * This file implements logic to collect, sort, and print the environment
 * variables in a format compatible with the Bash `export` command.
 * Variables are printed in lexicographically sorted order as:
 *
 * - `declare -x VAR="value"` if a value is present.
 * - `declare -x VAR` if no value is set.
 *
 * The sorting is done using a simple bubble sort algorithm for simplicity.
 */
#include "minishell.h"

/**
 * @brief Frees an array of dynamically allocated strings.
 *
 * Used to deallocate the memory allocated for environment variable keys
 * after printing.
 *
 * @param keys Array of strings to free.
 * @param num_kyes Number of elements in the `keys` array.
 */
static void	free_keys(char **keys, int num_kyes)
{
	int	i;

	if (!keys)
		return ;
	i = 0;
	while (i < num_kyes)
	{
		free(keys[i]);
		i++;
	}
	free(keys);
}

/**
 * @brief Sorts an array of strings alphabetically using bubble sort.
 *
 * This function performs an in-place sort of the provided array using
 * the bubble sort algorithm and `ft_strcmp()` for comparison.
 *
 * @param keys Array of environment variable keys.
 * @param count Number of elements in the array.
 */
static void	bubble_sort(char **keys, int count)
{
	int		i;
	int		j;
	char	*temp;

	i = 0;
	while (i < count - 1)
	{
		j = 0;
		while (j < (count - i - 1))
		{
			if (ft_strcmp(keys[j], keys[j + 1]) > 0)
			{
				temp = keys[j];
				keys[j] = keys[j + 1];
				keys[j + 1] = temp;
			}
			j++;
		}
		i++;
	}
}

/**
 * @brief Prints sorted environment variables in `declare -x` format.
 *
 * This function formats and prints each key-value pair:
 * - If a value is assigned: `declare -x key="value"`
 * - If no value is assigned: `declare -x key`
 *
 * The `_` variable is skipped from printing to mimic Bash behavior.
 *
 * @param mshell Pointer to the Minishell structure.
 * @param keys Array of sorted environment variable keys.
 * @param count Number of keys in the array.
 */
static void	print_sorted_env(t_mshell *mshell, char **keys, int count)
{
	int		i;
	char	*value;

	i = 0;
	while (i < count)
	{
		value = ms_getenv(mshell, keys[i]);
		if (ft_strcmp(keys[i], "_") != 0)
		{
			if (value)
				(void)printf("declare -x %s=\"%s\"\n", keys[i], value);
			else
				(void)printf("declare -x %s\n", keys[i]);
		}
		i++;
	}
}

/**
 * @brief Collects, sorts, and prints environment variables in export format.
 *
 * Handles the logic for `export` with no arguments:
 * - Gathers all environment keys from the hash table.
 * - Sorts them alphabetically.
 * - Prints each key and its value in Bash-like format.
 * - Frees any temporary memory allocated during the process.
 *
 * Prints errors if the shell context or hash table is invalid.
 *
 * @param mshell Pointer to the Minishell shell state structure.
 */
void	handle_sorted_env(t_mshell *mshell)
{
	char		**keys;
	int			count;

	if (!mshell || !mshell->hash_table)
	{
		print_error("minishell: export: no mshell or hash_table found\n");
		return ;
	}
	collect_keys(mshell->hash_table, &keys, &count);
	if (!keys)
	{
		print_error("minishell: export: keys memory allocation failed\n");
		return ;
	}
	bubble_sort(keys, count);
	print_sorted_env(mshell, keys, count);
	free_keys(keys, count);
}
