/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_helpers_tilde.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pvershin <pvershin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 15:10:00 by pvershin          #+#    #+#             */
/*   Updated: 2025/04/23 11:54:45 by pvershin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "tokenizer.h"

/*
 * Handle tilde expansion or literal '~'.
 */
void	handle_tilde(t_exp_ctx *ctx)
{
	char	*home;

	if (ctx->single_q || ctx->double_q)
	{
		append_char(ctx, '~');
		(*ctx->i)++;
	}
	else
	{
		home = expand_tilde(ctx->input, ctx->i, ctx->mshell, ctx->quote_style);
		if (home)
		{
			*ctx->result = append_to_result(*ctx->result, home);
			free(home);
		}
	}
}
