/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_helpers1.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ilia Munaev <ilyamunaev@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 13:56:20 by pvershin          #+#    #+#             */
/*   Updated: 2025/05/05 19:25:04 by Ilia Munaev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "tokenizer.h"

// Создаёт пустую команду с argv[0] = ""
t_cmd	*create_empty_command(t_mshell *shell)
{
	t_cmd	*cmd;

	cmd = ft_calloc(sizeof (t_cmd), 1);
	if (!cmd)
		return (NULL);
	cmd->argv = ft_calloc(MAX_ARGS + 1, sizeof(char *));
	// cmd->argv = calloc(MAX_ARGS + 1, sizeof(char *));
	if (!cmd->argv)
	{
		free(cmd);
		return (NULL);
	}
	cmd->minishell = shell;
	cmd->next = NULL;
	cmd->redirs = NULL;
	return (cmd);
}

int	is_input_redir(t_TokenType type)
{
	return (type == TOKEN_REDIRECT_IN || type == TOKEN_HEREDOC);
}

int	is_output_redir(t_TokenType type)
{
	return (type == TOKEN_REDIRECT_OUT || type == TOKEN_APPEND_OUT);
}

void	free_list_nodes_only(t_list **lst)
{
	t_list	*tmp;
	t_list	*next;

	tmp = *lst;
	while (tmp)
	{
		next = tmp->next;
		free(tmp);
		tmp = next;
	}
	*lst = NULL;
}

t_cmd	*finalize_cmd_list(t_list **cmd_list)
{
	t_cmd	*head;
	t_cmd	*prev;
	t_list	*iter;

	head = NULL;
	prev = NULL;
	iter = *cmd_list;
	while (iter)
	{
		if (!head)
			head = (t_cmd *)iter->content;
		if (prev)
			prev->next = (t_cmd *)iter->content;
		prev = (t_cmd *)iter->content;
		iter = iter->next;
	}
	free_list_nodes_only(cmd_list);
	return (head);
}
