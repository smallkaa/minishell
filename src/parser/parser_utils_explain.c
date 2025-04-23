/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils_explain.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: auto <auto@student.hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 16:00:00 by auto              #+#    #+#             */
/*   Updated: 2025/04/22 16:00:00 by auto             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

/* Local helper to toggle and print single quote */
static void	handle_explain_single_quote(int *in_single_quote)
{
	*in_single_quote = !*in_single_quote;
	if (*in_single_quote)
		debug_printf("\033[33m'%s", "\033[33m");
	else
		debug_printf("\033[0m'%s", "\033[0m");
}

/* Local helper to toggle and print double quote */
static void	handle_explain_double_quote(int *in_double_quote)
{
	*in_double_quote = !*in_double_quote;
	if (*in_double_quote)
		debug_printf("\033[36m\"%s", "\033[36m");
	else
		debug_printf("\033[0m\"%s", "\033[0m");
}

/* Local helper to print default character with color */
static void	handle_explain_default(char *color_code, int in_single_quote,
		int in_double_quote, char c)
{
	if (in_single_quote)
		ft_strcpy(color_code, "\033[33m");
	else if (in_double_quote)
		ft_strcpy(color_code, "\033[36m");
	debug_printf("%s%c\033[0m", color_code, c);
}

static void	handle_explain_loop(const char *str)
{
	size_t			i;
	int				in_single_quote;
	int				in_double_quote;
	char			color_code[10];

	ft_strcpy(color_code, "\033[0m");
	in_single_quote = 0;
	in_double_quote = 0;
	i = 0;
	while (i < ft_strlen(str))
	{
		if (str[i] == '\'' && !in_double_quote)
			handle_explain_single_quote(&in_single_quote);
		else if (str[i] == '"' && !in_single_quote)
			handle_explain_double_quote(&in_double_quote);
		else if (str[i] == '$' && (in_double_quote || !in_single_quote))
			debug_printf("\033[32m$\033[0m");
		else
			handle_explain_default(color_code,
				in_single_quote, in_double_quote, str[i]);
		i++;
	}
}

void	explain_token(t_Token token)
{
	if (token.type != TOKEN_WORD || !token.value)
		return ;
	debug_printf("  Analysis:\n");
	debug_printf("    ");
	handle_explain_loop(token.value);
	debug_printf("\n");
	debug_printf("\033[0m");
}
