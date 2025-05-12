/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer6.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Pavel Vershinin <pvershin@student.hive.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 13:19:42 by pvershin          #+#    #+#             */
/*   Updated: 2025/05/12 20:45:27 by Pavel Versh      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file tokenizer6.c
 * @brief Parsing word tokens and buffer helper functions.
 */

#include "../include/minishell.h"
#include <stdlib.h>
#include <string.h>

/**
 * @brief Checks if a character is a quote.
 *
 * @param c Character to check.
 * @return true if single or double quote, false otherwise.
 */
static bool	is_quote_char(char c)
{
	return (c == '\'' || c == '"');
}

/**
 * @brief Checks if a character is a special shell operator.
 *
 * @param c Character to test.
 * @return true if operator (|, <, >, &), false otherwise.
 */
bool	ft_is_special_char(char c)
{
	return (c == '|' || c == '<' || c == '>' || c == '&');
}

/**
 * @brief Determines if parsing should stop on this character.
 *
 * @param c Character to test.
 * @return true if character ends the word.
 */
static bool	should_break_char(char c)
{
	return (c == ' ' || c == '\t' || ft_is_special_char(c) || is_quote_char(c));
}

/**
 * @brief Reads the next word into the buffer.
 *
 * @param tokenizer Pointer to tokenizer.
 * @param buffer Destination buffer.
 * @param bufsize Size of buffer.
 * @return Number of characters copied.
 */
static size_t	fill_buffer_with_word(t_Tokenizer *tokenizer, char *buffer,
		size_t bufsize)
{
	size_t	index;
	char	c;

	index = 0;
	while (*tokenizer->input)
	{
		c = *tokenizer->input;
		if (should_break_char(c))
			break ;
		if (index < bufsize - 1)
			buffer[index++] = c;
		tokenizer->input++;
	}
	return (index);
}

/**
 * @brief Parses an unquoted word token until special or whitespace.
 *
 * @param tokenizer Pointer to the tokenizer.
 * @param saw_space Indicates if token should be joined to previous.
 * @return t_Token Parsed word token.
 */
t_Token	tokenizer_parse_word(t_Tokenizer *tokenizer, int saw_space, t_mshell *minishell)
{
	t_Token	token;
	size_t	index;
	t_Token	next;

	ft_bzero(&token, sizeof(t_Token));
	if (!tokenizer || !tokenizer->buffer)
		return ((t_Token){0});
	token.type = TOKEN_WORD;
	index = fill_buffer_with_word(tokenizer, tokenizer->buffer,
			tokenizer->buffer_size);
	if (index == 0 && is_quote_char(*tokenizer->input))
	{
		tokenizer->buffer[0] = '\0';
		next = get_next_token(tokenizer, minishell);
		return (next);
	}
	tokenizer->buffer[index] = '\0';
	token.value = ft_strdup(tokenizer->buffer);
	if (!token.value)
		return ((t_Token){0});
	token.quote_style = 0;
	token.in_single_quotes = 0;
	token.in_double_quotes = 0;
	token.needs_join = saw_space;
	return (token);
}
