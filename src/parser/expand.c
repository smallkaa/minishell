/**
 * @file   expand.c
 * @brief  Core helper functions for variable/escape/tilde expansion.
 */

#include "libft.h"
#include "minishell.h"
#include <stdlib.h>

/**
 * @brief Get a string representation of the shell's exit status.
 *
 * @param minishell Shell context containing exit_status.
 * @return Allocated string of the exit status (ft_itoa), or NULL on failure.
 */
char	*get_exit_code(t_mshell *minishell)
{
	return (ft_itoa(minishell->exit_status));
}

/**
 * @brief Retrieve the value of an environment variable.
 *
 * If var == "?", returns the exit code string. Otherwise searches first in
 * the shell's hash‐table (ms_getenv), then falls back to the raw envp array.
 *
 * @param var        Name of the variable (NUL‐terminated).
 * @param minishell  Shell context with env and hash_table populated.
 * @return Allocated string: the variable's value, or "" if not found,
 *         or NULL if var == NULL or an allocation fails.
 */
char	*get_env_value(const char *var, t_mshell *minishell)
{
	int		i;
	size_t	var_len;
	char	*value;

	if (!var)
		return (NULL);
	if (ft_strcmp((char *)var, "?") == 0)
		return (get_exit_code(minishell));
	value = ms_getenv(minishell, (char *)var);
	if (value)
		return (ft_strdup(value));
	if (!minishell->env)
		return (ft_strdup(""));
	var_len = ft_strlen(var);
	i = 0;
	while (minishell->env[i])
	{
		if (ft_strncmp(minishell->env[i], var, var_len) == 0
			&& minishell->env[i][var_len] == '=')
			return (ft_strdup(minishell->env[i] + var_len + 1));
		i++;
	}
	return (ft_strdup(""));
}

/**
 * @brief Append a string to the current result buffer.
 *
 * Frees the old result on success.
 *
 * @param result Current allocated string.
 * @param append Null‐terminated string to append.
 * @return Newly allocated concatenated string, or NULL on failure.
 */
char	*append_to_result(char *result, const char *append)
{
	char	*new_result;

	if (!result || !append)
		return (NULL);
	new_result = ft_strjoin(result, append);
	free(result);
	return (new_result);
}

/**
 * @brief Check if tilde should be literal based on context.
 *
 * Advances index if literal. Returns 1 if literal, 0 otherwise.
 */
static int	is_literal_tilde(const char *input, size_t *i, int single_q,
		int double_q)
{
	if (single_q || double_q)
	{
		(*i)++;
		return (1);
	}
	if (*i > 0 && input[*i - 1] != ' ' && input[*i - 1] != '\t')
	{
		(*i)++;
		return (1);
	}
	if (input[*i + 1] && input[*i + 1] != ' ' && input[*i + 1] != '\t'
		&& input[*i + 1] != '/')
	{
		(*i)++;
		return (1);
	}
	return (0);
}

/**
 * @brief Expand a tilde (~) to the user's home directory if applicable.
 *
 * @param input Input string containing '~'.
 * @param i Pointer to current index; advanced past tilde or home.
 * @param mshell Shell context for environment access.
 * @param quote_style Quote style: 0 (none), 1 (single), 2 (double).
 * @return Allocated expansion string or "~" on failure.
 */
char	*expand_tilde(const char *input, size_t *i, t_mshell *mshell,
		int quote_style)
{
	int single_q = (quote_style == 1);
	int double_q = (quote_style == 2);
	char *home;

	if (is_literal_tilde(input, i, single_q, double_q))
		return (ft_strdup("~"));
	home = ms_getenv(mshell, "HOME");
	(*i)++;
	if (!home || !*home)
		return (ft_strdup("~"));
	return (ft_strdup(home));
}