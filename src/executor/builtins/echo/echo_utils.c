/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ilia Munaev <ilyamunaev@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 14:44:47 by Ilia Munaev       #+#    #+#             */
/*   Updated: 2025/04/23 14:44:48 by Ilia Munaev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file echo_utils.c
 * @brief Utility functions for parsing and validating arguments in
 * the `echo` built-in.
 *
 * Contains helper functions to identify properly formatted `-n` flags and detect
 * whether strings are wrapped in single or double quotes.
 */
#include "minishell.h"

/**
 * @brief Checks if a string is wrapped in matching single or double quotes.
 *
 * This function returns true if the first and last characters of the input
 * string are either both single quotes `'` or both double quotes `"`.
 *
 * @param s The string to check.
 * @return `1` if the string is quote-wrapped, `0` otherwise.
 *
 * @note Strings shorter than 2 characters are never considered wrapped.
 */
int	is_wrapped_in_quotes(char *s)
{
	size_t	len;

	if (!s)
		return (0);
	len = ft_strlen(s);
	if (len < 2)
		return (0);
	return ((s[0] == '\'' && s[len - 1] == '\'')
		|| (s[0] == '"' && s[len - 1] == '"'));
}

/**
 * @brief Validates whether a string is a valid `-n` flag for the `echo` command.
 *
 * This function checks if the input string starts with `-n` and contains only
 * the character `n` after the dash (e.g., `-n`, `-nnn`, etc.). Quoted flags
 * like `"-n"` are explicitly rejected.
 *
 * @param arg The argument string to validate.
 * @return `1` if it is a valid unquoted `-n` flag, `0` otherwise.
 */
int	is_echo_flag(char *arg)
{
	int	i;

	if (!arg || arg[0] != '-' || arg[1] != 'n')
		return (0);
	if (ft_strchr(arg, '\'') || ft_strchr(arg, '"'))
		return (0);
	i = 1;
	while (arg[++i])
	{
		if (arg[i] != 'n')
			return (0);
	}
	return (1);
}
