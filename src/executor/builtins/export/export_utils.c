/**
 * @file handle_export_utils.c
 * @brief Helper functions for handle_export()
 */
#include "minishell.h"

/**
 * @brief Counts the total number of environment variable keys in the
 *        hash table.
 *
 * This function iterates through all buckets of the hash table and counts the
 * number of stored keys.
 *
 * @param ht Pointer to the hash table storing environment variables.
 * @return The total number of keys in the hash table.
 */
static int	count_total_keys(t_hash_table *ht)
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
 * @brief Collects all environment variable keys from the hash table into
 *        an array.
 *
 * This function allocates memory for an array of strings, each holding a
 * key from the environment. The total number of collected keys is stored
 * in `count`.
 *
 * @param ht Pointer to the hash table storing environment variables.
 * @param keys Pointer to a dynamically allocated array to store environment
 *             variable keys.
 * @param count Pointer to an integer that will store the number of collected
 *              keys.
 */
static void	collect_keys(t_hash_table *ht, char ***keys, int *count)
{
	int				total_keys;
	t_mshell_var	*var;
	int				i;
	int				key_index;

	if (!ht || !keys || !count)
		return ;
	total_keys = count_total_keys(ht);
	*count = total_keys;
	*keys = malloc(sizeof(char *) * total_keys);
	if (!(*keys))
	{
		print_error("minishell: export: key memory allocation failed\n");
		return ;
	}
	key_index = 0;
	i = 0;
	while (i < HASH_SIZE)
	{
		var = ht->buckets[i];
		while (var)
		{
			(*keys)[key_index] = ft_strdup(var->key);
			key_index++;
			var = var->next;
		}
		i++;
	}
}

/**
 * @brief Sorts an array of environment variable keys in alphabetical order.
 *
 * This function implements the **bubble sort** algorithm to arrange
 * environment variable keys in lexicographic order.
 *
 * @param keys Array of environment variable keys.
 * @param count Number of keys in the array.
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
 * @brief Prints sorted environment variables in the `declare -x key="value"`
 *        format.
 *
 * Iterates over the sorted keys, retrieves their values, and prints them in
 * the format used by `export` in Bash. If a variable has no value, it is
 * printed as `declare -x key`.
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
 * @brief Handles sorting and printing of environment variables in
 *        alphabetical order.
 *
 * This function:
 * - Collects environment variable keys.
 * - Sorts them using **bubble sort**.
 * - Prints them in the `declare -x key="value"` format.
 * - Frees allocated memory after processing.
 *
 * If the Minishell environment is invalid (e.g., `mshell->hash_table` is NULL),
 * an error message is printed.
 *
 * @param mshell Pointer to the Minishell structure.
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
