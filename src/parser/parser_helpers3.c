/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_helpers3.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pvershin <pvershin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 14:05:00 by pvershin          #+#    #+#             */
/*   Updated: 2025/05/16 10:45:30 by pvershin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "tokenizer.h"

/**
 * @brief Ensure current command exists in list, add if missing.
 */
int	ensure_current_cmd(t_mshell *shell, t_list **cmd_list,
		t_cmd **current)
{
	t_list	*new;

	if (!*current)
	{
		*current = create_empty_command(shell);
		if (!*current)
			return (-1);
		new = ft_lstnew(*current);
		if (!new)
		{
			shell->allocation_error = 1;
			return (-1);
		}
		ft_lstadd_back(cmd_list, new);
	}
	return (0);
}

static int	free_and_minus_one(void *redir)
{
	free(redir);
	return (-1);
}

/**
 * @brief Create and append an input redirection node.
 */
int	apply_input_redir(t_cmd **current, t_Token *token)
{
	t_redir	*redir;
	t_list	*r_ptr;

	redir = malloc(sizeof(t_redir));
	if (!redir)
		return (-1);
	redir->fd = -1;
	if (token->type == TOKEN_REDIRECT_IN)
		redir->type = R_INPUT;
	else
		redir->type = R_HEREDOC;
	redir->filename = ft_strdup(token[1].value);
	if (!redir->filename)
		return (free_and_minus_one(redir));
	redir->expand_in_heredoc = (token[1].quote_style == 0);
	r_ptr = ft_lstnew(redir);
	if (!r_ptr)
	{
		free(redir->filename);
		return (free_and_minus_one(redir));
	}
	ft_lstadd_back(&(*current)->redirs, r_ptr);
	return (0);
}

/**
 * @brief Create and append an output redirection node.
 */
static int	apply_output_redir(t_cmd **current, t_Token *token)
{
	t_redir	*redir;
	t_list	*r_ptr;

	redir = malloc(sizeof(t_redir));
	if (!redir)
		return (-1);
	if (token->type == TOKEN_REDIRECT_OUT)
		redir->type = R_OUTPUT;
	else
		redir->type = R_APPEND;
	redir->filename = ft_strdup(token[1].value);
	if (!redir->filename)
	{
		free(redir);
		return (-1);
	}
	r_ptr = ft_lstnew(redir);
	if (!r_ptr)
	{
		free(redir->filename);
		free(redir);
		return (-1);
	}
	ft_lstadd_back(&(*current)->redirs, r_ptr);
	return (0);
}

/**
 * @brief Handle output redirection tokens.
 */
int	handle_output_redir(t_mshell *shell, t_list **cmd_list, t_cmd **current,
		t_Token *token)
{
	if (ensure_current_cmd(shell, cmd_list, current) < 0)
		return (-1);
	return (apply_output_redir(current, token));
}
