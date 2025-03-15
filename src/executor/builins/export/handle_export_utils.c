/**
 * @file handle_export_utils.c
 * @brief Helper functions for handle_export()
 */
#include "minishell.h"

/**
 * @brief Frees the dynamically allocated memory for sorted environment
 *        variable keys.
 *
 * @param keys Array of allocated strings containing environment variable keys.
 * @param num_keys Number of keys in the array.
 */
void	free_keys(char **keys, int num_kyes)
{
	int	i;

	i = 0;
	while (i < num_kyes)
	{
		free(keys[i]);
		i++;
	}
	free(keys);
}
