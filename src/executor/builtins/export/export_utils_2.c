/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils_2.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imunaev- <imunaev-@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 14:45:34 by Ilia Munaev       #+#    #+#             */
/*   Updated: 2025/05/12 14:15:03 by imunaev-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file export_utils_1.c
 * @brief Collects environment variable keys from the shell's hash table.
 *
 * This file implements the logic to extract all variable keys from the
 * shell's internal hash table. The keys are collected into a dynamically
 * allocated array, which is used by the `export` command for sorting
 * and display.
 */
#include "minishell.h"

/**
 * @brief Counts all keys in the hash table.
 *
 * Iterates through each bucket and counts the total number of entries
 * stored in the hash table.
 *
 * @param ht Pointer to the hash table structure.
 * @return The total number of keys (environment variables) found.
 */

static int	count_total_keys(t_hash_tbl *ht)
{
	int				i;
	int				total_keys;
	t_mshell_var	*var;

	i = 0;
	total_keys = 0;
	while (i < HASH_SIZE)
	{
		var = ht->buckets[i];
		while (var)
		{
			total_keys++;
			var = var->next;
		}
		i++;
	}
	return (total_keys);
}

/**
 * @brief Allocates memory for storing all environment variable keys.
 *
 * Calculates the number of keys and allocates a `char **` array of that size.
 * If allocation fails, an error is printed and `false` is returned.
 *
 * @param ht Pointer to the hash table.
 * @param keys Output parameter to store the allocated array.
 * @param count Output parameter to store the number of keys.
 * @return `true` if allocation succeeded, `false` otherwise.
 */
static bool	allocate_keys_array(t_hash_tbl *ht, char ***keys, int *count)
{
	if (!ht || !keys || !count)
		return (false);
	*count = count_total_keys(ht);
	*keys = malloc(sizeof(char *) * (*count));
	if (!(*keys))
	{
		print_error("-minishell: export: key memory allocation failed\n");
		return (false);
	}
	ft_memset(*keys, 0, sizeof(char *) * (*count));
	return (true);
}

/**
 * @brief Fills an array with all keys from the hash table.
 *
 * Iterates over all hash buckets and copies each key using `ft_strdup()`
 * into the provided `keys` array.
 *
 * @param ht Pointer to the hash table.
 * @param keys Pre-allocated array where copied keys will be stored.
 */
static int	fill_keys_from_hash(t_hash_tbl *ht, char **keys)
{
	t_mshell_var	*var;
	int				i;
	int				key_index;
	char			*dup;

	i = 0;
	key_index = 0;
	while (i < HASH_SIZE)
	{
		var = ht->buckets[i];
		while (var)
		{
			dup = ft_strdup(var->key);
			if (!dup)
				return (print_error("-minishell: fill_keys ft_strdup fail\n"),
					EXIT_FAILURE);
			free(keys[key_index]);
			keys[key_index] = dup;
			key_index++;
			var = var->next;
		}
		i++;
	}
	return (EXIT_SUCCESS);
}

/**
 * @brief Collects all environment variable keys from the hash table.
 *
 * Combines allocation and filling into a single entry point. Used by
 * `handle_sorted_env()` to obtain a complete, dynamic array of keys
 * for sorting and display.
 *
 * @param ht Pointer to the hash table.
 * @param keys Output parameter: pointer to the array of collected keys.
 * @param count Output parameter: number of collected keys.
 */
int	collect_keys(t_hash_tbl *ht, char ***keys, int *count)
{
	int	i;

	if (!allocate_keys_array(ht, keys, count))
		return (EXIT_FAILURE);
	if (fill_keys_from_hash(ht, *keys) != EXIT_SUCCESS)
	{
		i = 0;
		while (i < *count)
		{
			free((*keys)[i]);
			i++;
		}
		free(*keys);
		*keys = NULL;
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}
