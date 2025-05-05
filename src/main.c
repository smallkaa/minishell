/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ilia Munaev <ilyamunaev@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 08:10:49 by pvershin          #+#    #+#             */
/*   Updated: 2025/05/05 23:20:01 by Ilia Munaev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

uint8_t	run_command_mode(t_mshell *mshell, char *input)
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

uint8_t	run_interactive_mode(t_mshell *mshell)
{
	char	*input;
	t_cmd	*cmd;
	uint8_t	exit_status;

	while (1)
	{
		input = read_user_input();
		if (!input)
			return (EXIT_FAILURE);
		cmd = run_parser(mshell, input);
		if (!cmd)
		{
			if (handle_null_command(mshell, input))
				continue ;
		}
		if (handle_signal_after_parse(mshell, cmd, input))
			continue ;
		exit_status = run_executor(cmd);
		free_cmd(&cmd);
		free(input);
		input = NULL;
		handle_signal_after_exec(mshell);
	}
	return (exit_status);
}

// Читает одну строку из stdin, выполняет ее и возвращает статус.
// Используется, когда BIGTEST=1 и ввод неинтерактивный.
static uint8_t	run_non_interactive_command(t_mshell *mshell)
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
		perror_return("ft_strtrim memory allocation failed", EXIT_FAILURE);
		return (EXIT_FAILURE);
	}
	exit_status = run_command_mode(mshell, trimmed_line);
	free(trimmed_line);
	return (exit_status);
}

int	main(int argc, char **argv, char **envp)
{
	t_mshell	*minishell;
	uint8_t		exit_status;

	setup_signal_handlers();
	minishell = init_mshell(envp);
	if (!minishell)
		return (EXIT_FAILURE);
	if (argc >= 3 && !ft_strncmp(argv[1], "-c", 3))
		exit_status = run_command_mode(minishell, argv[2]);
	else if (argc == 2)
		exit_status = run_script_mode(minishell, argv[1]);
	else
	{
		if (BIGTEST == 1 && !is_input_interactive())
			exit_status = run_non_interactive_command(minishell);
		else
			exit_status = run_interactive_mode(minishell);
	}
	free_minishell(&minishell);
	rl_clear_history();
	return (exit_status);
}
