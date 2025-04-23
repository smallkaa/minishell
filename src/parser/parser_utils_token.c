/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils_token.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: auto <auto@student.hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 16:30:00 by auto              #+#    #+#             */
/*   Updated: 2025/04/22 16:30:00 by auto             ###   ########.fr       */
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
	[TOKEN_APPEND_OUT] = "APPEND_OUT", [TOKEN_BACKGROUND] = "BACKGROUND",
	[TOKEN_HEREDOC] = "HEREDOC", [TOKEN_EMPTY] = "EMPTY",
	[TOKEN_EOF] = "EOF"};

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
