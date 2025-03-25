#include "minishell.h"




bool is_debug_mode()
{
	return (getenv("MINISHELL_DEBUG") != NULL);
}

void debug_printf(const char *format, ...)
{
	if(is_debug_mode())
	{
		va_list args;
		va_start(args, format);
		vprintf(format, args);
		va_end(args);
	}
}
int	ft_arr_size(char **arr)
{
	int	len;

	len = 0;
	while (arr && arr[len])
		len++;
	return (len);
}

/**
 * @brief Retrieves the value of an environment variable from the hash table.
 *
 * This function searches for an environment variable stored in the Minishell
 * hash table by computing its hash index and traversing the linked list of
 * stored variables.
 *
 * @param mshell Pointer to the Minishell structure containing the hash table.
 * @param key The name of the environment variable to retrieve.
 * @return Pointer to the value of the variable if found, or NULL if not found.
 *
 * @note All values stors in the hash table for faster search, export, and unset
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
