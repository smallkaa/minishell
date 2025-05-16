/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_helpers4.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pvershin <pvershin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 14:20:00 by pvershin          #+#    #+#             */
/*   Updated: 2025/05/16 10:18:57 by pvershin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "tokenizer.h"

static int	process_one_token(t_TokenArray *new_tokens,
	t_TokenArray *old_tokens, int *i, int *j)
{
	if (is_skippable_token(&old_tokens->tokens[*i]))
	{
		(*i)++;
		return (0);
	}
	if (old_tokens->tokens[*i].type != TOKEN_WORD)
	{
		if (process_non_word(new_tokens, j, &old_tokens->tokens[*i]))
			return (1);
		(*i)++;
	}
	else
	{
		if (process_word(new_tokens, old_tokens, i, j))
			return (1);
	}
	return (0);
}

int	fill_new_tokens(t_TokenArray *new_tokens, t_TokenArray *old_tokens)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
	while (i < old_tokens->count)
	{
		if (process_one_token(new_tokens, old_tokens, &i, &j))
		{
			new_tokens->count = j;
			return (1);
		}
	}
	new_tokens->count = j;
	return (0);
}

int	handle_allocation_error(t_mshell *msh)
{
	msh->allocation_error = 1;
	return (-1);
}

int	cleanup_and_return_error(t_TokenArray *new_tokens_array, t_mshell *msh)
{
	int	k;

	msh->allocation_error = 1;
	k = 0;
	while (k < new_tokens_array->count)
		free_token(&new_tokens_array->tokens[k++]);
	free(new_tokens_array->tokens);
	return (-1);
}

/**
 * @brief Checks if the input string matches any unsupported syntax pattern.
 *
 * @param shell The minishell instance.
 * @param input The original input string.
 * @return 1 if unsupported, 0 otherwise.
 */
int	check_for_unsupported_syntax(t_mshell *shell, char *input)
{
	const char	*err_msg;
	int			code;

	if (!input || ft_strlen(input) == 0)
		return (0);
	code = known_unsupported_cmd(input, &err_msg);
	shell->exit_status = code;
	if (code)
	{
		if (err_msg)
			print_error((char *)err_msg);
		else
			print_error("minishell: unsupported syntax\n");
	}
	return (code);
}
