/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_helpers4.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pvershin <pvershin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 14:20:00 by pvershin          #+#    #+#             */
/*   Updated: 2025/04/22 14:23:57 by pvershin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "tokenizer.h"

/**
 * @brief Initialize a new command node and set its first argument.
 *
 * @return 0 on success, -1 on failure.
 */
static int	init_word_command(t_mshell *shell, t_list **cmd_list,
	t_cmd **current, char *value)
{
	*current = ft_calloc(1, sizeof(t_cmd));
	if (!*current)
		return (-1);
	(*current)->argv = ft_calloc(MAX_ARGS + 1, sizeof(char *));
	if (!(*current)->argv)
	{
		free(*current);
		*current = NULL;
		return (-1);
	}
	(*current)->argv[0] = ft_strdup(value);
	(*current)->minishell = shell;
	ft_lstadd_back(cmd_list, ft_lstnew(*current));
	return (0);
}

/**
 * @brief Append an argument to an existing command.
 */
static void	append_word_argument(t_cmd *current, char *value)
{
	int	i;

	if (!current || !current->argv)
		return ;
	i = 0;
	while (current->argv[i])
		++i;
	if (i < MAX_ARGS)
	{
		current->argv[i] = ft_strdup(value);
		current->argv[i + 1] = NULL;
	}
}

/**
 * @brief Handle a word token by creating or appending to a command.
 */
void	handle_word_token(t_mshell *shell, t_list **cmd_list,
	t_cmd **current, char *value)
{
	if (!value)
		return ;
	if (!*current)
	{
		if (init_word_command(shell, cmd_list, current, value) < 0)
			return ;
	}
	else
	{
		append_word_argument(*current, value);
	}
}
