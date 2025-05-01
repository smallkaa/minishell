/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pvershin <pvershin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 08:10:49 by pvershin          #+#    #+#             */
/*   Updated: 2025/05/01 14:58:10 by pvershin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
static bool	is_input_interactive(void)
{
	return (isatty(STDIN_FILENO));
}

uint8_t	run_command_mode(t_mshell *mshell, char *input)
{
	t_cmd	*cmd;
	uint8_t	exit_status;

	cmd = run_parser(mshell, input);
	if (!cmd)
		return (EXIT_FAILURE);
	exit_status = run_executor(cmd);
	free_cmd(cmd);
	cmd = NULL;
	return (exit_status);
}

uint8_t	run_script_mode(t_mshell *mshell, const char *file)
{
	char	*input;
	uint8_t	exit_status;
	t_cmd	*cmd;
	int		in_fd;

	in_fd = open(file, O_RDONLY);
	if (in_fd < 0)
	{
		print_error("minishell: cannot open script file\n");
		return (EXIT_FAILURE);
	}
	input = NULL;
	while ((input = get_next_line(in_fd)) != NULL)
	{
		if (g_signal_flag)
		{
			mshell->exit_status = 130;
			g_signal_flag = 0;
		}
		cmd = run_parser(mshell, input);
		if (!cmd)
		{
			free(input);
			input = NULL;
			continue ;
		}
		exit_status = run_executor(cmd);
		free_cmd(cmd);
		cmd = NULL;
		free(input);
		input = NULL;
	}
	safe_close(&in_fd);
	return (exit_status);
}

// uint8_t	run_interactive_mode(t_mshell *mshell)
// {
// 	char	*input;
// 	uint8_t	exit_status;
// 	t_cmd	*cmd;

// 	while (1)
// 	{
// 		input = readline("minishell: ");
// 		if (!input)
// 			return (EXIT_FAILURE);
// 		if (*input)
// 			add_history(input);
// 		cmd = run_parser(mshell, input);
// 		if (!cmd)
// 		{
// 			free(input);
// 			if (g_signal_flag)
// 			{
// 				mshell->exit_status = 130;
// 				g_signal_flag = 0;
// 			}
// 			continue ;
// 		}
// 		if (g_signal_flag)
// 		{
// 			mshell->exit_status = 130;
// 			g_signal_flag = 0;
// 			free_cmd(cmd);
// 			free(input);
// 			continue;
// 		}
// 		exit_status = run_executor(cmd);
// 		free_cmd(cmd);
// 		free(input);
// 		input = NULL;
// 		if (g_signal_flag)
// 		{
// 			mshell->exit_status = 130;
// 			g_signal_flag = 0;
// 		}
// 	}
// 	return (exit_status);
// }

uint8_t	run_interactive_mode(t_mshell *mshell)
{
	char	*input;
	t_cmd	*cmd;
	uint8_t	exit_status;

	while (1)
	{
		// printf("\n-------------DEBUG: main() pid=%d\n", getpid());

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
		free_cmd(cmd);
		free(input);
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

	line = get_next_line(STDIN_FILENO); // Предполагается, что get_next_line есть
	if (!line)
	{
		// Можно добавить обработку ошибки или просто вернуть статус
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

	setup_signal_handlers(); // Set up signal handlers
	minishell = init_mshell(envp);
	if (!minishell)
		return (EXIT_FAILURE);
	// Handle -c option
	if (argc >= 3 && !ft_strncmp(argv[1], "-c", 3))
		exit_status = run_command_mode(minishell, argv[2]);
	// Handle script mode
	else if (argc == 2)
		exit_status = run_script_mode(minishell, argv[1]);
	// Handle interactive mode
// Handle interactive mode
else // argc == 1 (нет аргументов командной строки)
	{
		// Если BIGTEST=1 и ввод НЕ интерактивный (из pipe/файла)
		if (BIGTEST == 1 && !is_input_interactive())
			exit_status = run_non_interactive_command(minishell);
		else // Иначе (BIGTEST=0 ИЛИ ввод интерактивный)
			exit_status = run_interactive_mode(minishell);
	}
	free_minishell(minishell);
	rl_clear_history();
	return (exit_status);
}
