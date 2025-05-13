/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser3.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Pavel Vershinin <pvershin@student.hive.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 13:10:38 by pvershin          #+#    #+#             */
/*   Updated: 2025/05/13 16:15:00 by Pavel Versh      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

int	fill_new_tokens(t_TokenArray *new_tokens, t_TokenArray *old_tokens)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
	while (i < old_tokens->count)
	{
		if (is_skippable_token(&old_tokens->tokens[i]))
		{
			i++;
			continue ;
		}
		if (old_tokens->tokens[i].type != TOKEN_WORD)
		{
			if (process_non_word(new_tokens, &j, &old_tokens->tokens[i]))
			{
				new_tokens->count = j; // Обновляем count перед выходом
				return 1; // Ошибка
			}
			i++;
		}
		else
		{
			if(process_word(new_tokens, old_tokens, &i, &j))
			{
				new_tokens->count = j;
				return 1;
			}
		}
	}
	new_tokens->count = j;
	return 0;
}

// parser3.c (group_word_tokens)
int	group_word_tokens(t_TokenArray *tokens, t_mshell * msh)
{
	t_TokenArray	new_tokens_array;
	int				new_count;
	int k;

	if (!tokens || tokens->count <= 1)
		return (0);
	new_count = count_new_tokens(tokens);
	new_tokens_array.tokens = malloc(sizeof(t_Token) * new_count); //PROTECTION = CHECKED
	if (!new_tokens_array.tokens)
	{
		msh->allocation_error = 1;
		return (-1);
	}
	new_tokens_array.capacity = new_count;
	new_tokens_array.count = 0;
	if (fill_new_tokens(&new_tokens_array, tokens))
	{
		msh->allocation_error = true;
		int k = 0;
		while(k < new_tokens_array.count)
		{
			free_token(&new_tokens_array.tokens[k]);
			k++;
		}
		free(new_tokens_array.tokens);
		return (-1);
	}
    k = 0;
    while(k < tokens->count)
    {
        free_token(&tokens->tokens[k]);
        k++;
    }
	free(tokens->tokens);
	tokens->tokens = new_tokens_array.tokens;
	tokens->count = new_tokens_array.count;
	tokens->capacity = new_tokens_array.capacity;
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
	*ctx->current = create_empty_command(ctx->shell);
	if (!*ctx->current)
	{
		free_cmd_list(ctx->cmd_list);
		return (-1);
	}
	ft_lstadd_back(ctx->cmd_list, ft_lstnew(*ctx->current));
	return (0);
}

int	apply_redirection(t_redir_ctx *ctx, t_TokenType type)
{
	t_Token	*token;

	token = &ctx->tokens->tokens[*ctx->i];
	if (is_input_redir(type))
	{
		if (handle_input_redir(ctx->shell, ctx->cmd_list,
				ctx->current, token) < 0)
			return (-1);
	}
	else
	{
		if (handle_output_redir(ctx->shell, ctx->cmd_list,
				ctx->current, token) < 0)
			return (-1);
	}
	return (0);
}
