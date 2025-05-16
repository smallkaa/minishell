/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils_token.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pvershin <pvershin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 12:46:17 by pvershin          #+#    #+#             */
/*   Updated: 2025/05/16 09:40:18 by pvershin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

/**
 * @brief Print token information in debug mode.
 *
 * @param token The token to print.
 */
void	print_token(t_Token token)
{
	const char	*token_type_names[] = {[TOKEN_WORD] = "WORD",
	[TOKEN_PIPE] = "PIPE", [TOKEN_REDIRECT_IN] = "REDIRECT_IN",
	[TOKEN_REDIRECT_OUT] = "REDIRECT_OUT",
	[TOKEN_APPEND_OUT] = "APPEND_OUT",
	[TOKEN_BACKGROUND] = "BACKGROUND", [TOKEN_HEREDOC] = "HEREDOC",
	[TOKEN_EMPTY] = "EMPTY", [TOKEN_EOF] = "EOF"};

	debug_printf("Token: { type: %s", token_type_names[token.type]);
	if (token.type == TOKEN_WORD)
	{
		if (token.value)
			debug_printf(", value: %s", token.value);
		else
			debug_printf(", value: (NULL)");
	}
	debug_printf(" }\n");
}
