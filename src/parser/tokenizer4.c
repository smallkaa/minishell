/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer4.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pvershin <pvershin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 13:19:13 by pvershin          #+#    #+#             */
/*   Updated: 2025/05/16 09:39:12 by pvershin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file tokenizer4.c
 * @brief Token validation and stripping utility functions.
 */

#include "../include/minishell.h"
#include <stdlib.h>
#include <string.h>

/**
 * @brief Checks if a string is enclosed in single quotes.
 *
 * @param str The string to check.
 * @return true if enclosed in single quotes, false otherwise.
 */
bool	is_in_single_quotes(char *str)
{
	size_t	len;

	if (!str || !*str)
		return (false);
	len = ft_strlen(str);
	if (len < 2)
		return (false);
	return (str[0] == '\'' && str[len - 1] == '\'');
}

/**
 * @brief Checks if a string is enclosed in double quotes.
 *
 * @param str The string to check.
 * @return true if enclosed in double quotes, false otherwise.
 */
bool	is_in_double_quotes(char *str)
{
	size_t	len;

	if (!str || !*str)
		return (false);
	len = ft_strlen(str);
	if (len < 2)
		return (false);
	return (str[0] == '"' && str[len - 1] == '"');
}

/**
 * @brief Strips surrounding quotes from a string while preserving inner quotes.
 *
 * @param str The input string.
 * @param quote_style 1 for single quotes, 2 for double quotes.
 * @return Newly allocated string without outer quotes, or NULL on error.
 */
static char	*strip_quotes_simple(const char *str, int quote_style)
{
	size_t	len;

	if (!str)
		return (NULL);
	len = ft_strlen(str);
	if (quote_style == 1 && len >= 2 && str[0] == '\'' && str[len - 1] == '\'')
		return (ft_substr(str, 1, len - 2));
	if (quote_style == 2 && len >= 2 && str[0] == '"' && str[len - 1] == '"')
		return (ft_substr(str, 1, len - 2));
	return (ft_strdup(str));
}

/**
 * @brief Strips outer quotes from all tokens of type TOKEN_WORD.
 *
 * Iterates through tokens and replaces values enclosed in matching
 * quotes with unquoted strings.
 *
 * @param tokens Array of tokens to process.
 * @return 0 on success, -1 on error.
 */
int	strip_words(t_TokenArray *tokens, t_mshell *msh)
{
	int		i;
	char	*new_val;
	t_Token	*tok;

	if (!tokens || !tokens->tokens)
		return (-1);
	i = -1;
	while (++i < tokens->count)
	{
		tok = &tokens->tokens[i];
		if (tok->type == TOKEN_WORD && tok->value)
		{
			if (tok->in_single_quotes && tok->in_double_quotes)
				continue ;
			new_val = strip_quotes_simple(tok->value, tok->quote_style);
			if (!new_val)
			{
				msh->allocation_error = 1;
				return (-1);
			}
			free(tok->value);
			tok->value = new_val;
		}
	}
	return (0);
}
