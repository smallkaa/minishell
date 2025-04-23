/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_helpers_escape.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pvershin <pvershin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 15:05:00 by pvershin          #+#    #+#             */
/*   Updated: 2025/04/23 12:01:59 by pvershin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "tokenizer.h"

/*
 * Handle backslash escapes in expansion.
 */
void	handle_backslash(t_exp_ctx *ctx)
{
	char	next;

	if (!ctx->input[*ctx->i + 1])
	{
		append_char(ctx, '\\');
		(*ctx->i)++;
		return ;
	}
	(*ctx->i)++;
	next = ctx->input[*ctx->i];
	if (ctx->single_q || (next != '$' && next != '\\'
			&& next != '"' && next != '\''))
	{
		append_char(ctx, '\\');
		append_char(ctx, next);
	}
	else
		append_char(ctx, next);
	(*ctx->i)++;
}
