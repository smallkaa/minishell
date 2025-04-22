/**
 * @file builtin_utils.c
 * @brief Environment variable lookup function for Minishell.
 *
 * Provides a hash table-based implementation of `getenv()` to retrieve
 * environment variable values efficiently. Used internally by built-ins
 * and environment-related operations.
 */
#include "minishell.h"

/**
 * @brief Retrieves the value of an environment variable from the shell's
 * hash table.
 *
 * Computes the hash index for the given key and searches the corresponding
 * bucket's linked list for a matching key. If the key is found, its associated
 * value is returned.
 *
 * @param mshell Pointer to the Minishell structure that contains the hash table.
 * @param key The name of the environment variable to retrieve.
 * @return Pointer to the variable's value if found, or `NULL` if not found.
 *
 * @note All environment variables are stored in a hash table for fast lookup,
 *       efficient `export` handling, and `unset` support.
 */
char	*ms_getenv(t_mshell *mshell, char *key)
{
	unsigned int	index;
	t_mshell_var	*curr;

	index = hash_function(key);
	curr = mshell->hash_table->buckets[index];
	while (curr)
	{
		if (ft_strcmp(curr->key, key) == 0)
			return (curr->value);
		curr = curr->next;
	}
	return (NULL);
}
