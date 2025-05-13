/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser1.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Pavel Vershinin <pvershin@student.hive.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 13:09:26 by pvershin          #+#    #+#             */
/*   Updated: 2025/05/13 11:37:13 by Pavel Versh      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

// Initialize a token array
t_TokenArray	*token_array_init(void)
{
	t_TokenArray	*array;

	array = (t_TokenArray *)malloc(sizeof(t_TokenArray)); //PROTECTION = CHECKED
	if (!array)
	{
		print_error("Failed to allocate token array\n");
		return NULL;
	}
	array->capacity = 16;
	array->tokens = (t_Token *)malloc(sizeof(t_Token) * array->capacity); //PROTECTION = CHECKED
	if (!array->tokens)
	{
		print_error("Failed to allocate tokens buffer\n");
		free(array);
		return NULL;
	}
	array->count = 0;
	return (array);
}

// Add a token to the array
void	token_array_add(t_TokenArray *array, t_Token token, t_mshell *minishell)
{
	int		old_capacity;
	size_t	old_size;
	int		new_capacity;
	size_t	new_size;
	t_Token	*new_tokens_ptr;

	if (array->count >= array->capacity)
	{
		old_capacity = array->capacity;
		old_size = (size_t)old_capacity * sizeof(t_Token);
		new_capacity = old_capacity * 2;
		if (new_capacity < old_capacity)
			new_capacity = INT_MAX;
		new_size = (size_t)new_capacity * sizeof(t_Token);
		(void) new_size;
		(void) old_size;
		new_tokens_ptr = (t_Token *)ft_realloc(array->tokens, old_size, // PROTECTION=CHEKCED
				new_size);
		if(!new_tokens_ptr)
		{
			minishell->allocation_error = 1;
			return ;
		}
		array->tokens = new_tokens_ptr;
		array->capacity = new_capacity;
	}
	array->tokens[array->count] = token;
	array->count++;
}

// Free token array resources
void	token_array_free(t_TokenArray *array)
{
	int	i;

	if (!array)
	{
		return ;
	}
	i = 0;
	while (i < array->count)
	{
		free_token(&array->tokens[i]);
		i++;
	}
	free(array->tokens);
	free(array);
}

static void	skip_joined_words(t_TokenArray *tokens, int *i)
{
	while (*i < tokens->count
		&& tokens->tokens[*i].type == TOKEN_WORD
		&& tokens->tokens[*i].needs_join == 0)
	{
		(*i)++;
	}
}

int	count_new_tokens(t_TokenArray *tokens)
{
	int	i;
	int	count;

	i = 0;
	count = 0;
	while (i < tokens->count)
	{
		if (tokens->tokens[i].type == TOKEN_EMPTY)
		{
			i++;
			continue ;
		}
		count++;
		if (tokens->tokens[i].type != TOKEN_WORD)
		{
			i++;
			continue ;
		}
		i++;
		skip_joined_words(tokens, &i);
	}
	return (count);
}
