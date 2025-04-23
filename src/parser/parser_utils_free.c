/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils_free.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: auto <auto@student.hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 16:00:00 by auto              #+#    #+#             */
/*   Updated: 2025/04/22 16:00:00 by auto             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

/**
 * @brief Free a single redirection (t_redir).
 *
 * @param content Pointer to the t_redir structure.
 */
void	free_redir(void *content)
{
	t_redir	*redir;

	if (!content)
		return ;
	redir = (t_redir *)content;
	free(redir->filename);
	redir->filename = NULL;
	free(redir);
}

/**
 * @brief Free a single command (t_cmd), including its argv and redirs.
 *
 * @param cmd Pointer to the command to free.
 */
void	free_command(t_cmd *cmd)
{
	int	i;

	if (!cmd)
		return ;
	free(cmd->binary);
	cmd->binary = NULL;
	if (cmd->argv)
	{
		i = 0;
		while (cmd->argv[i])
		{
			free(cmd->argv[i]);
			cmd->argv[i] = NULL;
			i++;
		}
		free(cmd->argv);
		cmd->argv = NULL;
	}
	if (cmd->redirs)
		ft_lstclear(&cmd->redirs, free_redir);
	free(cmd);
}

/**
 * @brief Free the entire list of commands.
 *
 * @param head Pointer to the head of the command list.
 */
void	free_command_list(t_cmd **head)
{
	t_cmd	*current;
	t_cmd	*next;

	if (!head || !*head)
		return ;
	current = *head;
	while (current)
	{
		next = current->next;
		free_command(current);
		current = next;
	}
	*head = NULL;
}
