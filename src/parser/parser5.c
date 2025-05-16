/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser5.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pvershin <pvershin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 13:11:18 by pvershin          #+#    #+#             */
/*   Updated: 2025/05/16 10:18:35 by pvershin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

/**
 * @brief Tokenizes the input string into a token array.
 *
 * @param input User input string.
 * @return Pointer to a new t_TokenArray, or NULL on error.
 */
static void	handle_token_add(t_TokenArray *tokens, t_Token *current_token,
	t_mshell *mshell)
{
	token_array_add(tokens, *current_token, mshell);
	if (mshell->allocation_error)
	{
		if (current_token->value)
		{
			free(current_token->value);
			current_token->value = NULL;
		}
	}
}

static void	process_token_loop(t_Tokenizer *tokenizer, t_TokenArray *tokens,
	t_mshell *mshell)
{
	t_Token	current_token;

	while (1)
	{
		current_token = get_next_token(tokenizer, mshell);
		if (mshell->allocation_error || current_token.type == TOKEN_EOF)
		{
			free_token(&current_token);
			break ;
		}
		handle_token_add(tokens, &current_token, mshell);
		if (mshell->allocation_error)
			break ;
	}
}

static t_TokenArray	*tokenize_input(char *input, t_mshell *mshell)
{
	t_Tokenizer		*tokenizer;
	t_TokenArray	*tokens;

	tokenizer = tokenizer_create(input);
	if (!tokenizer)
		return (mshell->allocation_error = true, NULL);
	tokens = token_array_init();
	if (!tokens)
	{
		tokenizer_destroy(tokenizer);
		return (mshell->allocation_error = true, NULL);
	}
	process_token_loop(tokenizer, tokens, mshell);
	tokenizer_destroy(tokenizer);
	return (tokens);
}

/**
 * @brief Prints token debug information to stdout.
 *
 * @param tokens Token array to print.
 */
static void	debug_print_tokens(t_TokenArray *tokens)
{
	int	i;

	debug_printf("Found %d token(s):\n", tokens->count);
	i = 0;
	while (i < tokens->count)
	{
		debug_printf("\nToken %d:\n", i);
		print_token(tokens->tokens[i]);
		explain_token(tokens->tokens[i]);
		i++;
	}
}

/**
 * @brief Main parser entry point: tokenizes input and builds command list.
 *
 * @param minishell Minishell context.
 * @param input Raw user input.
 * @return Parsed command list, or NULL on error.
 */
t_cmd	*run_parser(t_mshell *minishell, char *input)
{
	t_TokenArray	*tokens;
	t_cmd			*cmd;

	debug_printf("\nTokenizing: %s\n\n", input);
	tokens = tokenize_input(input, minishell);
	if (minishell->allocation_error)
		return (token_array_free(tokens));
	expand_tokens(tokens, minishell);
	if (minishell->allocation_error)
		return (token_array_free(tokens));
	if (check_for_unsupported_syntax(minishell, input) != EXIT_SUCCESS)
		return (token_array_free(tokens));
	group_word_tokens(tokens, minishell);
	if (minishell->allocation_error)
		return (token_array_free(tokens));
	strip_words(tokens, minishell);
	if (minishell->allocation_error)
		return (token_array_free(tokens));
	cmd = create_command_from_tokens(minishell, tokens);
	debug_print_tokens(tokens);
	debug_print_parsed_commands(cmd);
	token_array_free(tokens);
	return (cmd);
}
