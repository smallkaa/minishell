/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_helpers2.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pvershin <pvershin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 14:00:16 by pvershin          #+#    #+#             */
/*   Updated: 2025/04/22 14:19:21 by pvershin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "tokenizer.h"

void	free_cmd_list(t_list **cmd_list)
{
	t_list	*tmp;
	t_cmd	*cmd;

	tmp = *cmd_list;
	while (tmp)
	{
		cmd = (t_cmd *)tmp->content;
		free_cmd(cmd);
		tmp = tmp->next;
	}
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
