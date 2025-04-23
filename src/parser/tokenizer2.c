/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer2.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pvershin <pvershin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 13:18:10 by pvershin          #+#    #+#             */
/*   Updated: 2025/04/23 12:25:47 by pvershin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file tokenizer2.c
 * @brief Core tokenizer functions including token retrieval.
 */

#include "../include/minishell.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/**
 * @brief Determines if the tokenizer should end tokenization.
 *
 * Writes an error to stderr and exits if tokenizer or buffer is invalid.
 *
 * @param tokenizer Pointer to the tokenizer.
 * @param token Pointer to a token to set end-of-file.
 * @return true if end-of-input or error, false otherwise.
 */
static bool	tokenizer_should_end(t_Tokenizer *tokenizer, t_Token *token)
{
	if (!tokenizer || !tokenizer->buffer || tokenizer->buffer_size == 0)
	{
		write(2, "token_buffer not initialized\n", 30);
		exit(1);
	}
	if (!*tokenizer->input)
	{
		token->type = TOKEN_EOF;
		return (true);
	}
	return (false);
}

/**
 * @brief Skips whitespace characters in the input.
 *
 * Advances the tokenizer input pointer past spaces and tabs.
 *
 * @param tokenizer Pointer to the tokenizer.
 * @param saw_space Pointer to an int to be set if whitespace was found.
 */
static void	tokenizer_skip_whitespace(t_Tokenizer *tokenizer, int *saw_space)
{
	*saw_space = 0;
	while (*tokenizer->input == ' ' || *tokenizer->input == '\t')
	{
		*saw_space = 1;
		tokenizer->input++;
	}
}

/**
 * @brief Retrieves the next token from the tokenizer.
 *
 * @param tokenizer Pointer to the tokenizer.
 * @return t_Token The next token.
 */
t_Token	get_next_token(t_Tokenizer *tokenizer)
{
	t_Token	token;
	int		saw_space;

	ft_bzero(&token, sizeof(t_Token));
	token.type = TOKEN_WORD;
	if (tokenizer_should_end(tokenizer, &token))
		return (token);
	tokenizer_skip_whitespace(tokenizer, &saw_space);
	token.needs_join = saw_space;
	if (!*tokenizer->input)
	{
		token.type = TOKEN_EOF;
		return (token);
	}
	if (*tokenizer->input == '$' && *(tokenizer->input + 1) == '"')
		return (tokenizer_parse_special_dollar_quote(tokenizer, saw_space));
	if (*tokenizer->input == '"' || *tokenizer->input == '\'')
		return (tokenizer_parse_quoted(tokenizer, saw_space));
	if ((*tokenizer->input == '<' && *(tokenizer->input + 1) == '<')
		|| (*tokenizer->input == '>' && *(tokenizer->input + 1) == '>'))
		return (tokenizer_parse_redirection(tokenizer));
	if (ft_is_special_char(*tokenizer->input))
		return (tokenizer_parse_operator(tokenizer));
	return (tokenizer_parse_word(tokenizer, saw_space));
}
