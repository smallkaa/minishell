/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_helpers_quotes.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pvershin <pvershin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 15:00:00 by pvershin          #+#    #+#             */
/*   Updated: 2025/04/23 11:54:17 by pvershin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "tokenizer.h"

/*
 * Append a single character to the expansion result.
 */
void	append_char(t_exp_ctx *ctx, char c)
{
	char	str[2];

	str[0] = c;
	str[1] = '\0';
	*ctx->result = append_to_result(*ctx->result, str);
}

/*
 * Handle single-quote characters in expansion.
 */
void	handle_single_quote(t_exp_ctx *ctx)
{
	if (ctx->double_q)
		append_char(ctx, ctx->input[*ctx->i]);
	else
		ctx->single_q = !ctx->single_q;
	(*ctx->i)++;
}

/*
 * Handle double-quote characters in expansion.
 */
void	handle_double_quote(t_exp_ctx *ctx)
{
	if (ctx->single_q)
		append_char(ctx, ctx->input[*ctx->i]);
	else
		ctx->double_q = !ctx->double_q;
	(*ctx->i)++;
}
