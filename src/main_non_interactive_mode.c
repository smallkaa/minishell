/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_non_interactive_mode.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ilia Munaev <ilyamunaev@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/10 19:39:59 by Ilia Munaev       #+#    #+#             */
/*   Updated: 2025/05/10 19:41:12 by Ilia Munaev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static uint8_t	run_command_mode(t_mshell *mshell, char *input)
{
	t_cmd	*cmd;
	uint8_t	exit_status;

	cmd = run_parser(mshell, input);
	if (!cmd)
		return (EXIT_FAILURE);
	exit_status = run_executor(cmd);
	free_cmd(&cmd);
	return (exit_status);
}

uint8_t	run_non_interactive_mode(t_mshell *mshell)
{
	char	*line;
	char	*trimmed_line;
	uint8_t	exit_status;

	line = get_next_line(STDIN_FILENO);
	if (!line)
	{
		return (EXIT_FAILURE);
	}
	trimmed_line = ft_strtrim(line, "\n");
	free(line);
	if (!trimmed_line)
	{
		print_error("ft_strtrim memory allocation failed");
		return (EXIT_FAILURE);
	}
	exit_status = run_command_mode(mshell, trimmed_line);
	free(trimmed_line);
	return (exit_status);
}
