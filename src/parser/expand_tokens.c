/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_tokens.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pvershin <pvershin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 11:54:54 by pvershin          #+#    #+#             */
/*   Updated: 2025/04/23 12:27:34 by pvershin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	is_single_quote_literal(t_Token *token)
{
	size_t	len;

	if (token->quote_style != 1)
		return (0);
	len = ft_strlen(token->value);
	if (len >= 2 && token->value[0] == '\'' && token->value[len - 1] == '\'')
		return (1);
	return (0);
}

static int	handle_special_dollar_quote(t_Token *token)
{
	size_t	len;
	char	*content;

	if (ft_strncmp(token->value, "$\"", 2) != 0)
		return (0);
	len = ft_strlen(token->value);
	if (len < 3 || token->value[len - 1] != '"')
		return (0);
	content = ft_substr(token->value, 2, len - 3);
	if (!content)
		return (0);
	free(token->value);
	token->value = content;
	return (1);
}

static int	needs_expansion(char *value)
{
	if (!ft_strchr(value, '$') && !ft_strchr(value, '~') && \
	!ft_strchr(value, '\\'))
		return (0);
	return (1);
}

static void	expand_token_value(t_Token *token, t_mshell *ms)
{
	char	*expanded;

	if (!token || !token->value || is_single_quote_literal(token))
		return ;
	if (handle_special_dollar_quote(token))
		return ;
	if (!needs_expansion(token->value))
		return ;
	expanded = expand_env_variables(token->value, ms, token->quote_style);
	if (!expanded)
		return ;
	if (expanded[0] == '\0' && token->quote_style == 0)
	{
		free(token->value);
		token->value = NULL;
		token->type = TOKEN_EMPTY;
		free(expanded);
		return ;
	}
	free(token->value);
	token->value = expanded;
	classify_token_if_needed(token, expanded);
}

void	expand_tokens(t_TokenArray *tokens, t_mshell *ms)
{
	int	i;

	i = 0;
	while (i < tokens->count)
	{
		if (tokens->tokens[i].type == TOKEN_WORD)
		{
			expand_token_value(&tokens->tokens[i], ms);
		}
		i++;
	}
}
