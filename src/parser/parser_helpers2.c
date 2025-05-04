/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_helpers2.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ilia Munaev <ilyamunaev@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 14:00:16 by pvershin          #+#    #+#             */
/*   Updated: 2025/05/04 23:33:20 by Ilia Munaev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "tokenizer.h"

void	free_cmd_list(t_list **cmd_list)
{
	t_list	*current;
	t_list	*next;
	t_cmd	*cmd;

	if (!cmd_list || !*cmd_list)
		return ;
	current = *cmd_list;
	while (current)
	{
		next = current->next;
		cmd = (t_cmd *)current->content;
		if (cmd)
		{
			free_cmd(&cmd);
		}
		free(current);
		current = next;
	}
	*cmd_list = NULL;
}

// parser_helpers.c
bool	is_valid_redir_target(t_TokenArray *tokens, int i)
{
	if (!tokens || i + 1 >= tokens->count)
		return (false);
	if (tokens->tokens[i + 1].type != TOKEN_WORD)
		return (false);
	if (!tokens->tokens[i + 1].value || tokens->tokens[i + 1].value[0] == '\0')
		return (false);
	return (true);
}
