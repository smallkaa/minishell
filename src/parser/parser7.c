/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser7.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pvershin <pvershin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 10:22:44 by pvershin          #+#    #+#             */
/*   Updated: 2025/05/16 10:53:11 by pvershin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

/**
 * @brief Creates a linked list of command structures from a token array.
 *
 * Parses tokens into separate t_cmd commands:
 * - Splits on pipes (`|`)
 * - Handles redirections (`<`, `>`, `<<`, `>>`)
 * - Builds argv from words
 *
 * @param shell Minishell context.
 * @param tokens Array of tokens representing the parsed input line.
 * @return Pointer to the first command in the constructed linked list,
 * or NULL on error.
 */
t_cmd	*create_command_from_tokens(t_mshell *shell, t_TokenArray *tokens)
{
	t_list		*cmd_list;
	t_cmd		*current;
	t_cmd		*head;
	t_parse_ctx	ctx;
	int			status;

	cmd_list = NULL;
	current = NULL;
	ctx.shell = shell;
	ctx.tokens = tokens;
	ctx.cmd_list = &cmd_list;
	ctx.current = &current;
	init_parse_context(&ctx);
	status = parse_tokens(&ctx);
	if (shell->allocation_error)
	{
		free_cmd_list(&cmd_list);
		return (NULL);
	}
	if (status == ERROR_UNEXPECTED_TOKEN)
		return (shell->exit_status = 2, NULL);
	if (status < 0)
		return (NULL);
	head = finalize_cmd_list(&cmd_list);
	return (finalize_commands(head));
}

/**
 * @brief Handle input redirection tokens.
 */
int	handle_input_redir(t_mshell *shell, t_list **cmd_list, t_cmd **current,
	t_Token *token)
{
	if (ensure_current_cmd(shell, cmd_list, current) < 0)
		return (-1);
	return (apply_input_redir(current, token));
}

/**
 * @brief Checks if a character is a quote.
 *
 * @param c Character to check.
 * @return true if single or double quote, false otherwise.
 */
bool	is_quote_char(char c)
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
