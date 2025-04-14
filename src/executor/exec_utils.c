/**
 * @file exec_utils.c
 * @brief Utility functions for managing variables, pipeline limits, 
 * and exit statuses in Minishell.
 */
#include "minishell.h"

/**
 * @brief Splits a `KEY=VALUE` string into a `t_mshell_var` structure.
 *
 * - If an equal sign (`=`) is found, the string is split into a key and a value.
 * - If no equal sign is present, the value is set to `NULL`.
 *
 * @param kv_pair The string to split.
 * @return A pointer to a newly allocated `t_mshell_var` structure, 
 * or NULL on failure.
 */
t_mshell_var	*split_key_value(char *kv_pair)
{
	t_mshell_var	*mshell_var;
	char			*equal_sign;

	mshell_var = malloc(sizeof(t_mshell_var));
	if (!mshell_var)
	{
		print_error("Error (split_key_value): mshell_var malloc failed\n");
		return (NULL);
	}
	equal_sign = ft_strchr(kv_pair, '=');
	if (equal_sign)
	{
		mshell_var->key = ft_substr(kv_pair, 0, equal_sign - kv_pair);
		mshell_var->value = ft_strdup(equal_sign + 1);
	}
	else
	{
		mshell_var->key = ft_strdup(kv_pair);
		mshell_var->value = NULL;
	}
	return (mshell_var);
}
