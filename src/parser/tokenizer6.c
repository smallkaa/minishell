/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer6.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pvershin <pvershin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 13:19:42 by pvershin          #+#    #+#             */
/*   Updated: 2025/05/16 10:53:30 by pvershin         ###   ########.fr       */
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

static t_Token	init_word_token(t_Tokenizer *tokenizer, size_t *index)
{
	t_Token	token;

	ft_bzero(&token, sizeof(t_Token));
	token.type = TOKEN_WORD;
	if (!tokenizer || !tokenizer->buffer)
		return ((t_Token){0});
	*index = fill_buffer_with_word(tokenizer, tokenizer->buffer,
			tokenizer->buffer_size);
	return (token);
}

static t_Token	finalize_word_token(t_Tokenizer *tokenizer, size_t index,
		int saw_space, t_mshell *ms)
{
	t_Token	token;

	token = (t_Token){0};
	token.type = TOKEN_WORD;
	tokenizer->buffer[index] = '\0';
	token.value = ft_strdup(tokenizer->buffer);
	if (!token.value)
	{
		ms->allocation_error = true;
		return ((t_Token){0});
	}
	token.quote_style = 0;
	token.in_single_quotes = 0;
	token.in_double_quotes = 0;
	token.needs_join = saw_space;
	return (token);
}

/**
 * @brief Parses an unquoted word token until special or whitespace.
 *
 * @param tokenizer Pointer to the tokenizer.
 * @param saw_space Indicates if token should be joined to previous.
 * @return t_Token Parsed word token.
 */
t_Token	tokenizer_parse_word(t_Tokenizer *tokenizer, int saw_space,
		t_mshell *minishell)
{
	t_Token	token;
	t_Token	next;
	size_t	index;

	token = init_word_token(tokenizer, &index);
	if (index == 0 && is_quote_char(*tokenizer->input))
	{
		tokenizer->buffer[0] = '\0';
		next = get_next_token(tokenizer, minishell);
		return (next);
	}
	return (finalize_word_token(tokenizer, index, saw_space, minishell));
}
