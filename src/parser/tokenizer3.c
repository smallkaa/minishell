/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer3.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pvershin <pvershin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 13:19:06 by pvershin          #+#    #+#             */
/*   Updated: 2025/05/16 10:42:52 by pvershin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file tokenizer3.c
 * @brief Parsing quoted and dollar-quoted tokens.
 */

#include "../include/minishell.h"
#include <stdlib.h>
#include <string.h>

/**
 * @brief Parses a dollar-quoted string token.
 *
 * Handles tokens starting with $\"...\".
 *
 * @param tokenizer Pointer to the tokenizer.
 * @param saw_space Indicates if token should be joined to previous.
 * @return t_Token Parsed token.
 */
t_Token	tokenizer_parse_special_dollar_quote(t_Tokenizer *tokenizer,
		int saw_space, t_mshell *mshell)
{
	t_Token	token;
	size_t	index;

	ft_bzero(&token, sizeof(t_Token));
	token.type = TOKEN_WORD;
	if (*tokenizer->input == '$' && *(tokenizer->input + 1) == '"')
	{
		index = 0;
		token.in_double_quotes = 1;
		tokenizer->buffer[index++] = *tokenizer->input++;
		tokenizer->buffer[index++] = *tokenizer->input++;
		while (*tokenizer->input && *tokenizer->input != '"')
			tokenizer->buffer[index++] = *tokenizer->input++;
		if (*tokenizer->input == '"')
			tokenizer->buffer[index++] = *tokenizer->input++;
		tokenizer->buffer[index] = '\0';
		token.value = ft_strdup(tokenizer->buffer);
		if (!token.value)
			mshell->allocation_error = true;
		token.needs_join = saw_space;
	}
	return (token);
}

/**
 * @brief Parses a quoted string token (single or double).
 *
 * @param tokenizer Pointer to the tokenizer.
 * @param saw_space Indicates if token should be joined to previous.
 * @return t_Token Parsed token.
 */
static void	init_quoted_token(t_Token *token, t_Tokenizer *tokenizer,
	char quote)
{
	ft_bzero(token, sizeof(t_Token));
	token->type = TOKEN_WORD;
	quote = *tokenizer->input++;
	if (quote == '"')
		token->in_double_quotes = 1;
	else
		token->in_single_quotes = 1;
	token->quote_style = (quote == '"') * 2 + (quote == '\'');
}

static void	fill_quoted_buffer(t_Tokenizer *tokenizer, char *buffer,
	size_t *index, char quote)
{
	while (*tokenizer->input && *tokenizer->input != quote)
	{
		if (*index < tokenizer->buffer_size - 1)
			buffer[(*index)++] = *tokenizer->input;
		tokenizer->input++;
	}
	if (*tokenizer->input == quote)
		tokenizer->input++;
	buffer[*index] = '\0';
}

t_Token	tokenizer_parse_quoted(t_Tokenizer *tokenizer, int saw_space,
	t_mshell *minishell)
{
	t_Token	token;
	size_t	index;
	char	quote;

	index = 0;
	quote = *tokenizer->input;
	init_quoted_token(&token, tokenizer, quote);
	fill_quoted_buffer(tokenizer, tokenizer->buffer, &index, quote);
	token.value = ft_strdup(tokenizer->buffer);
	if (!token.value)
		minishell->allocation_error = true;
	token.needs_join = saw_space;
	return (token);
}
