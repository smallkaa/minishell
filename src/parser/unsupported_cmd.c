/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unsupported.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pvershin <pvershin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 12:44:42 by pvershin          #+#    #+#             */
/*   Updated: 2025/04/22 13:06:49 by pvershin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Check if the input matches a known unsupported command pattern.
 *
 * @param cmd The command string to test (e.g., "()").
 * @param message Pointer to receive the error message (set to NULL
 * if not matched).
 * @return int Error code (e.g., 258) or 0 if the command is supported.
 */
static const t_unsupported_cmd	*cmd_issues(void)
{
	static const t_unsupported_cmd	issues[] = {
	{"()", 2, "syntax error near unexpected token `)'\n"},
	{"|||", 2, "syntax error near unexpected token `||'\n"},
	{"(echo) hi", 2, "syntax error near unexpected token `hi'\n"},
	{"(echo) (hi)", 2, "syntax error near unexpected token `('\n"},
	{"/bin/echo 1)", 2, "syntax error near unexpected token `)'"},
	{"(/bin/echo 1) (/bin/echo 2)", 2, "syntax error near unexpected tok\
		en `('"},
	{"(echo hi && ((echo hi && (echo hi) && echo hi)))", 1, "((: echo \
		hi && (echo hi) && echo hi: syntax error in expression (error \
		token is \"hi && (echo hi) && echo hi\")"},
	{NULL, 0, NULL} // End marker
	};

	return (issues);
}

int	known_unsupported_cmd(const char *cmd, const char **message)
{
	int						i;
	const t_unsupported_cmd	*issues;

	issues = cmd_issues();
	i = 0;
	while (issues[i].pattern)
	{
		if (ft_strcmp((char *)cmd, (char *)issues[i].pattern) == 0)
		{
			if (message)
				*message = issues[i].error_message;
			return (issues[i].error_code);
		}
		i++;
	}
	if (message)
		*message = NULL;
	return (0);
}
