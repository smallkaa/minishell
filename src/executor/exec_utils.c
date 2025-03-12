/**
 * @file exec_utils.c
 * @brief Utility functions for managing variables, pipeline limits, and exit statuses in Minishell.
 */
#include "minishell.h"

// void	cleanup_heredoc(t_cmd *cmd)
// {
// 	if (cmd->in_redir && cmd->in_redir->type == R_HEREDOC)
// 	{
// 		if (unlink(HEREDOC_TFILE) == -1)
// 			fatal_error("unlink", EXIT_FAILURE);
// 	}
// }



/**
 * @brief Validates whether a given string is a valid variable name.
 *
 * - A valid variable name starts with an alphabetic character or an underscore (`_`).
 * - Subsequent characters can be alphanumeric or an underscore.
 * - If an equal sign (`=`) is present, only the portion before it is checked.
 *
 * @param key_value_pair The string to validate.
 * @return `true` if the string is a valid variable name, `false` otherwise.
 */
bool	is_valid_varname(const char *key_value_pair)
{
	// int	i;

	if (!key_value_pair)
		return (false);
	if (!(ft_isalpha(key_value_pair[0]) || key_value_pair[0] == '_'))
		return (false);
	// i = 1;
	// while (key_value_pair[i] && key_value_pair[i] != '=')
	// {
	// 	// if (!(ft_isalnum(key_value_pair[i]) || key_value_pair[i] == '_'))
	// 	if (!(ft_isalnum(key_value_pair[i])))
	// 		return (false);
	// 	i++;
	// }
	return (true);
}

/**
 * @brief Splits a `KEY=VALUE` string into a `t_mshell_var` structure.
 *
 * - If an equal sign (`=`) is found, the string is split into a key and a value.
 * - If no equal sign is present, the value is set to `NULL`.
 *
 * @param key_value_pair The string to split.
 * @return A pointer to a newly allocated `t_mshell_var` structure, or NULL on failure.
 */
t_mshell_var	*split_key_value(char *key_value_pair)
{
	t_mshell_var	*mshell_var;
	char			*equal_sign;

	mshell_var = malloc(sizeof(t_mshell_var));
	if (!mshell_var)
	{
		print_error("Error (split_key_value): mshell_var malloc failed\n");
		return (NULL);
	}
	equal_sign = ft_strchr(key_value_pair, '=');
	if (equal_sign)
	{
		mshell_var->key = ft_substr(key_value_pair, 0, equal_sign - key_value_pair);
		mshell_var->value = ft_strdup(equal_sign + 1);
	}
	else
	{
		mshell_var->key = ft_strdup(key_value_pair);
		mshell_var->value = NULL;
	}
	return (mshell_var);
}

/**
 * @brief Checks if the number of piped commands exceeds the allowed limit.
 *
 * This function increments the command count and checks if it exceeds
 * `MAX_CMDS`. If the limit is reached, an error message is printed.
 *
 * @param cmd_count Pointer to the command counter.
 * @return `true` if the pipeline limit is exceeded, `false` otherwise.
 */
bool	is_pipeline_limit(int *cmd_count)
{
	if (++(*cmd_count) > MAX_CMDS)
	{
		print_error("Error: pipeline limit exceeded\n");
		return (true);
	}
	return (false);
}
