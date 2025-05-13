/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Pavel Vershinin <pvershin@student.hive.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 13:10:07 by pvershin          #+#    #+#             */
/*   Updated: 2025/05/13 15:53:48 by Pavel Versh      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

bool	is_skippable_token(t_Token *tok)
{
	return (tok->type == TOKEN_EMPTY
		|| (tok->type == TOKEN_WORD && tok->value == NULL));
}

int	process_non_word(t_TokenArray *new_tokens, int *j, t_Token *old_tok)
{
	t_Token	new_tok_instance; // Используем локальный экземпляр, чтобы не менять old_tok

	new_tok_instance.type = old_tok->type;
	new_tok_instance.in_single_quotes = old_tok->in_single_quotes;
	new_tok_instance.in_double_quotes = old_tok->in_double_quotes;
	new_tok_instance.needs_join = old_tok->needs_join;
	new_tok_instance.quote_style = old_tok->quote_style;
	if (old_tok->value)
	{
		new_tok_instance.value = ft_strdup(old_tok->value); //PROTECTION = CHECKED
		if (!new_tok_instance.value)
			return 1;
	}
	else
		new_tok_instance.value = NULL;
	new_tokens->tokens[(*j)++] = new_tok_instance;
	return 0; // Успех
}
// returns 1 in case of allocation error
static int	init_wordinfo(t_wordinfo *info, t_Token *tok)
{
	info->grouped = ft_strdup(tok->value); //PROTECTION = CHECKED
	info->single_q = tok->in_single_quotes;
	info->double_q = tok->in_double_quotes;
	info->qstyle = tok->quote_style;
	if(!info->grouped)
		return 1;
	return 0;
}
// returns 1 in case of allocation error
static int	join_word_tokens(t_wordinfo *info, t_TokenArray *tokens, int *i)
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
		info->tmp = ft_strjoin(info->grouped, tokens->tokens[*i].value); //PROTECTION = CHECKED
		free(info->grouped);
		info->grouped = NULL;
		if(!info->tmp)
			return 1;
		info->grouped = info->tmp;
		if (tokens->tokens[*i].in_single_quotes)
			info->single_q = true;
		if (tokens->tokens[*i].in_double_quotes)
			info->double_q = true;
		(*i)++;
	}
	return 0;
}
//returns 1 in case of alloc error
int	process_word(t_TokenArray *new_tokens,
	t_TokenArray *old_tokens, int *i, int *j)
{
	t_wordinfo	info;

	if(init_wordinfo(&info, &old_tokens->tokens[*i]))
		return 1;
	(*i)++;
	if (join_word_tokens(&info, old_tokens, i))
		return 1;
	info.new_token = (t_Token){0};
	info.new_token.type = TOKEN_WORD;
	info.new_token.value = info.grouped;
	info.new_token.in_single_quotes = info.single_q;
	info.new_token.in_double_quotes = info.double_q;
	info.new_token.quote_style = info.qstyle;
	new_tokens->tokens[(*j)++] = info.new_token;
	return 0;
}
