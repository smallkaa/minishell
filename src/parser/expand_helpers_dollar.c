/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_helpers_dollar.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Pavel Vershinin <pvershin@student.hive.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 15:15:00 by pvershin          #+#    #+#             */
/*   Updated: 2025/05/14 14:08:01 by Pavel Versh      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "tokenizer.h"

/*
 * Handle special dollar cases: empty and '?'.
 * Returns 1 if handled, 0 otherwise.
 */
static int	handle_dollar_special(t_exp_ctx *ctx)
{
	char	*exit_code;

	if (ctx->input[*ctx->i] == '\0')
	{
		append_char(ctx, '$');
		return (1);
	}
	if (ctx->input[*ctx->i] == '?')
	{
		exit_code = get_exit_code(ctx->mshell);
		if (exit_code)
		{
			*ctx->result = append_to_result(*ctx->result, exit_code, ctx->mshell);
			free(exit_code);
		}
		(*ctx->i)++;
		return (1);
	}
	return (0);
}

/*
 * Handle variable-name expansion after '$'.
 */
static void	handle_dollar_var(t_exp_ctx *ctx)
{
	size_t	start;
	char	*name;
	char	*val;

	start = *ctx->i;
	while (ctx->input[*ctx->i] && (ft_isalnum(ctx->input[*ctx->i])
			|| ctx->input[*ctx->i] == '_'))
		(*ctx->i)++;
	name = ft_substr(ctx->input, start, *ctx->i - start); //PROTECTION = CHECKED
	if (!name)
	{
		ctx->mshell->allocation_error = 1;
		return ;
	}
	val = get_env_value(name, ctx->mshell); //PROTECTION = CHECKED
	free(name);
	if (val)
	{
		*ctx->result = append_to_result(*ctx->result, val, ctx->mshell);
		free(val);
	}
	else
	{
		ctx->mshell->allocation_error = 1;
	}
}

/*
 * Handle single-digit positional-parameter expansion after '$'.
 */
static void	handle_dollar_digit(t_exp_ctx *ctx)
{
	char	name[2];
	char	*val;

	name[0] = ctx->input[*ctx->i];
	name[1] = '\0';
	val = get_env_value(name, ctx->mshell);
	if (val)
	{
		*ctx->result = append_to_result(*ctx->result, val, ctx->mshell);
		free(val);
	}
	(*ctx->i)++;
}

/*
 * Handle dollar-sign variable expansions.
 */
void	handle_dollar(t_exp_ctx *ctx)
{
	if (ctx->single_q)
	{
		append_char(ctx, '$');
		(*ctx->i)++;
		return ;
	}
	(*ctx->i)++;
	if (handle_dollar_special(ctx))
		return ;
	if (ft_isalpha(ctx->input[*ctx->i]) || ctx->input[*ctx->i] == '_')
	{
		handle_dollar_var(ctx);
		return ;
	}
	if (ft_isdigit(ctx->input[*ctx->i]))
	{
		handle_dollar_digit(ctx);
		return ;
	}
	append_char(ctx, '$');
}
