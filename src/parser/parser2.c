/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pvershin <pvershin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 13:10:07 by pvershin          #+#    #+#             */
/*   Updated: 2025/04/23 16:47:26 by pvershin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

bool	is_skippable_token(t_Token *tok)
{
	return (tok->type == TOKEN_EMPTY
		|| (tok->type == TOKEN_WORD && tok->value == NULL));
}

void	process_non_word(t_TokenArray *new_tokens, int *j, t_Token *tok)
{
	new_tokens->tokens[(*j)++] = *tok;
}

static void	init_wordinfo(t_wordinfo *info, t_Token *tok)
{
	info->grouped = ft_strdup(tok->value);
	info->single_q = tok->in_single_quotes;
	info->double_q = tok->in_double_quotes;
	info->qstyle = tok->quote_style;
}

static void	join_word_tokens(t_wordinfo *info, t_TokenArray *tokens, int *i)
{
	while (*i < tokens->count
		&& tokens->tokens[*i].type == TOKEN_WORD
		&& tokens->tokens[*i].type != TOKEN_EMPTY
		&& tokens->tokens[*i].needs_join == 0)
	{
		if (tokens->tokens[*i].type == TOKEN_EMPTY)
		{
			(*i)++;
			continue ;
		}
		info->tmp = ft_strjoin(info->grouped, tokens->tokens[*i].value);
		free(info->grouped);
		info->grouped = info->tmp;
		if (tokens->tokens[*i].in_single_quotes)
			info->single_q = true;
		if (tokens->tokens[*i].in_double_quotes)
			info->double_q = true;
		(*i)++;
	}
}

void	process_word(t_TokenArray *new_tokens,
	t_TokenArray *old_tokens, int *i, int *j)
{
	t_wordinfo	info;
	int			k;

	init_wordinfo(&info, &old_tokens->tokens[*i]);
	(*i)++;
	join_word_tokens(&info, old_tokens, i);
	k = 0;
	while (k < *i)
	{
		if (old_tokens->tokens[k].type == TOKEN_WORD)
		{
			free(old_tokens->tokens[k].value);
			old_tokens->tokens[k].value = NULL;
		}
		k++;
	}
	info.new_token = (t_Token){0};
	info.new_token.type = TOKEN_WORD;
	info.new_token.value = info.grouped;
	info.new_token.in_single_quotes = info.single_q;
	info.new_token.in_double_quotes = info.double_q;
	info.new_token.quote_style = info.qstyle;
	new_tokens->tokens[(*j)++] = info.new_token;
}
