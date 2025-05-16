/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser3.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pvershin <pvershin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 13:10:38 by pvershin          #+#    #+#             */
/*   Updated: 2025/05/16 10:12:00 by pvershin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

void	replace_old_tokens(t_TokenArray *old, t_TokenArray *new_tokens_array)
{
	int	k;

	k = 0;
	while (k < old->count)
		free_token(&old->tokens[k++]);
	free(old->tokens);
	old->tokens = new_tokens_array->tokens;
	old->count = new_tokens_array->count;
	old->capacity = new_tokens_array->capacity;
}

// parser3.c (group_word_tokens)
int	group_word_tokens(t_TokenArray *tokens, t_mshell *msh)
{
	t_TokenArray	new_tokens_array;
	int				new_count;

	if (!tokens || tokens->count <= 1)
		return (0);
	new_count = count_new_tokens(tokens);
	new_tokens_array.tokens = malloc(sizeof(t_Token) * new_count);
	if (!new_tokens_array.tokens)
		return (handle_allocation_error(msh));
	new_tokens_array.capacity = new_count;
	new_tokens_array.count = 0;
	if (fill_new_tokens(&new_tokens_array, tokens))
		return (cleanup_and_return_error(&new_tokens_array, msh));
	replace_old_tokens(tokens, &new_tokens_array);
	return (0);
}

/**
 * @brief Handles a pipe token by resetting the current command.
 *
 * This function is called when a pipe (`|`) token is encountered,
 * indicating the start of a new command in a pipeline.
 *
 * @param current Pointer to the current command pointer.
 * @param i Pointer to the current token index, which is incremented.
 */
void	process_pipe_token(t_cmd **current, int *i)
{
	*current = NULL;
	(*i)++;
}

int	init_redir_command_if_needed(t_redir_ctx *ctx)
{
	t_list	*r_ptr;

	*ctx->current = create_empty_command(ctx->shell);
	if (!*ctx->current)
	{
		free_cmd_list(ctx->cmd_list);
		return (-1);
	}
	r_ptr = ft_lstnew(*ctx->current);
	if (!r_ptr)
	{
		ctx->shell->allocation_error = 1;
		free_cmd(ctx->current);
		return (-1);
	}
	ft_lstadd_back(ctx->cmd_list, r_ptr);
	return (0);
}

int	apply_redirection(t_redir_ctx *ctx, t_TokenType type)
{
	t_Token	*token;

	token = &ctx->tokens->tokens[*ctx->i];
	if (is_input_redir(type))
	{
		if (handle_input_redir(ctx->shell, ctx->cmd_list, ctx->current,
				token) < 0)
			return (-1);
	}
	else
	{
		if (handle_output_redir(ctx->shell, ctx->cmd_list, ctx->current,
				token) < 0)
			return (-1);
	}
	return (0);
}
