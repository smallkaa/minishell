/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser4.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Pavel Vershinin <pvershin@student.hive.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 13:11:07 by pvershin          #+#    #+#             */
/*   Updated: 2025/05/08 14:19:06 by Pavel Versh      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"
/**
 * @brief Processes a redirection token and applies it to the current command.
 *
 * @param ctx Redirection context containing all required pointers.
 * @return 0 on success, -1 on error.
 */
static int	process_redir_token(t_redir_ctx *ctx)
{
	t_TokenType	type;

	type = ctx->tokens->tokens[*ctx->i].type;
	if (!is_valid_redir_target(ctx->tokens, *ctx->i))
	{
		print_error("syntax error near unexpected token\n");
		free_cmd_list(ctx->cmd_list);
		return (ERROR_UNEXPECTED_TOKEN);
	}
	if (!*ctx->current)
	{
		if (init_redir_command_if_needed(ctx) < 0)
			return (-1);
	}
	if (apply_redirection(ctx, type) < 0)
		return (-1);
	*ctx->i += 2;
	return (0);
}

int	handle_redir(t_parse_ctx *ctx)
{
	int	status;

	status = process_redir_token(&ctx->redir);
	if (status == ERROR_UNEXPECTED_TOKEN)
		return (ERROR_UNEXPECTED_TOKEN);
	else if (status < 0)
		return (-1);
	return (0);
}
