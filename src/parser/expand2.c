/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pvershin <pvershin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 11:52:18 by pvershin          #+#    #+#             */
/*   Updated: 2025/04/23 11:53:20 by pvershin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file expand2.c
 * @brief Expand environment variables, handle quotes, escapes, and tilde.
 */

#include "libft.h"
#include "minishell.h"
#include "tokenizer.h"

/**
 * @brief Handle escape sequences and return the escaped string.
 *
 * Supports: \n, \t, \\, \", \', \$, or a literal pass‐through. Advances `*i`.
 *
 * @param input    Input string containing the backslash.
 * @param i        Pointer to current index in input; will be incremented.

 * @param single_q Non‐zero if inside single quotes (only take literal \
    backslash+char).
 * @return Allocated string of the escaped character(s), or NULL on failure.
 */
char	*handle_escape(const char *input, size_t *i, int single_q)
{
	char	single_char[2];

	(*i)++;
	if (single_q)
	{
		single_char[0] = '\\';
		single_char[1] = input[*i];
		return (ft_strdup(single_char));
	}
	if (input[*i] == 'n')
		return (ft_strdup("\n"));
	if (input[*i] == 't')
		return (ft_strdup("\t"));
	if (input[*i] == '\\')
		return (ft_strdup("\\"));
	if (input[*i] == '"')
		return (ft_strdup("\""));
	if (input[*i] == '\'')
		return (ft_strdup("\'"));
	if (input[*i] == '$')
		return (ft_strdup("$"));
	single_char[0] = input[*i];
	single_char[1] = '\0';
	return (ft_strdup(single_char));
}

/**
 * @brief Process a single character or sequence for expansion.
 *
 * Determines the type of the current character (quote, escape, dollar, tilde,
	or default)
 * and dispatches to the appropriate handler.
 *
 * @param input The input string.
 * @param i Pointer to current index in input; may be advanced by handlers.
 * @param result Pointer to the result string being built.
 * @param ms Shell context for environment and status.
 * @param single_q Pointer to single-quote mode flag.
 * @param double_q Pointer to double-quote mode flag.
 */
static void	process_input_char(t_exp_ctx *ctx)
{
	char	c;

	c = ctx->input[*ctx->i];
	if (c == '\'')
		handle_single_quote(ctx);
	else if (c == '"')
		handle_double_quote(ctx);
	else if (c == '\\')
		handle_backslash(ctx);
	else if (c == '$')
		handle_dollar(ctx);
	else if (c == '~')
		handle_tilde(ctx);
	else
	{
		append_char(ctx, c);
		(*ctx->i)++;
	}
}

/**
 * @brief Core loop for expanding environment variables, quotes, escapes,
	and tilde.
 *
 * Walks through the input string and builds the expanded result.
 *
 * @param input The input string to expand.
 * @param ms Shell context for environment and status.
 * @param quote_style Quote style: 0 (none), 1 (single), 2 (double).
 * @return Allocated expanded string, or NULL on failure.
 */
static char	*expand_env_variables_loop(const char *input, t_mshell *ms,
		int quote_style)
{
	char		*result;
	size_t		i;
	t_exp_ctx	ctx;

	if (!input)
		return (NULL);
	result = ft_strdup("");
	if (!result)
		return (NULL);
	i = 0;
	ctx.input = input;
	ctx.i = &i;
	ctx.result = &result;
	ctx.mshell = ms;
	ctx.quote_style = quote_style;
	ctx.single_q = (quote_style == 1);
	ctx.double_q = (quote_style == 2);
	while (input[i])
		process_input_char(&ctx);
	return (result);
}

/**
 * @brief Expand environment variables in an input string.
 *
 * Public function that dispatches to the core loop implementation.
 *
 * @param input The input string possibly containing variables.
 * @param ms Shell context with environment and exit status.
 * @param quote_style Quote style: 0 (none), 1 (single), 2 (double).
 * @return Allocated expanded string, or NULL on failure.
 */
char	*expand_env_variables(const char *input, t_mshell *ms, int quote_style)
{
	return (expand_env_variables_loop(input, ms, quote_style));
}
