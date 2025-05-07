/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Pavel Vershinin <pvershin@student.hive.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 08:10:49 by pvershin          #+#    #+#             */
/*   Updated: 2025/05/07 20:42:57 by Pavel Versh      ###   ########.fr       */
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

    debug_printf("--- Starting interactive mode loop ---\n");
    exit_status = mshell->exit_status;

    // Устанавливаем rl_event_hook перед циклом
    rl_event_hook = readline_interrupt_hook;

	while (1)
	{
		debug_printf("--- Top of while(1) loop --- g_signal_flag: %d\n", g_signal_flag);

		// Проверка флага g_signal_flag ПЕРЕД вызовом readline()
		// Это случай, когда сигнал пришел, пока readline() не был активен
		// (например, во время выполнения предыдущей команды).
		if (g_signal_flag && !isatty(STDIN_FILENO)) // Добавил !isatty для случая, когда сигнал пришел не во время readline
		{
			debug_printf(">>> In if(g_signal_flag) at top of loop (non-readline context). status=%d\n", mshell->exit_status);
			mshell->exit_status = 130;
			g_signal_flag = 0;
			// Здесь не нужно очищать readline, т.к. он не был активен.
			// Приглашение будет выведено следующим вызовом read_user_input -> readline()
			continue;
		}

		input = read_user_input(); // Внутри read_user_input() вызывается readline()

		// Проверка флага g_signal_flag ПОСЛЕ вызова readline()
		// Это случай, когда Ctrl-C был нажат во время ожидания ввода в readline()
		if (g_signal_flag)
		{
			debug_printf(">>> In if(g_signal_flag) AFTER readline. Current input: [%s]\n", input ? input : "NULL");
			mshell->exit_status = 130;
			g_signal_flag = 0;

			if (input) { // Если readline что-то вернул (маловероятно при Ctrl-C + rl_done=1, но для безопасности)
				free(input);
				input = NULL;
			}
			// rl_cleanup_after_signal(); // Эта функция может быть полезна, но rl_done в обработchике + следующие строки должны сработать
			// rl_replace_line("", 0);    // Очищаем буфер readline
			// // rl_on_new_line(); // Уже вызван в handle_sigint через write \n
			// rl_redisplay();         // Перерисовываем приглашение немедленно
			debug_printf("DEBUG: SIGINT during/after readline. Input ignored. status=%d, flag=%d\n", mshell->exit_status, g_signal_flag);
			continue; // Переходим к следующей итерации цикла, чтобы снова вызвать read_user_input()
		}		

		debug_printf("Read input: [%s], g_signal_flag before parse: %d\n", input ? input : "NULL", g_signal_flag);

		if (!input) // EOF (Ctrl+D)
		{
			if (isatty(fileno(stdin))) // Только в интерактивном режиме выводить "exit"
				printf("exit\n");
			exit_status = mshell->exit_status; // Сохраняем последний статус перед выходом
			break; // Выход из цикла while(1)
		}

		// Если input не пустой и не было сигнала, продолжаем обработку
		if (ft_strlen(input) > 0) // Добавим проверку, что строка не пустая после Ctrl-D, если get_next_line вернул ""
		{
			cmd = run_parser(mshell, input);
			if (!cmd)
			{
				// handle_null_command освобождает input, если он не NULL
				if (handle_null_command(mshell, input)) 
				{
					input = NULL; // Убедимся, что input NULL после handle_null_command
					continue;
				}
			}
			// Освобождаем input здесь, если он не был освобожден в handle_null_command
			// и если cmd был успешно создан. Если cmd NULL, input уже должен быть free.
			// Однако, run_parser не должен освобождать input.
			// handle_signal_after_parse и free_cmd позаботятся об остальном.
			
			if (handle_signal_after_parse(mshell, cmd, input)) // Эта функция также освобождает input и cmd
			{
				input = NULL; // Убедимся, что input NULL
				cmd = NULL;   // И cmd NULL
				continue;
			}
			
			exit_status = run_executor(cmd);
			mshell->exit_status = exit_status; // Обновляем статус оболочки
			free_cmd(&cmd);
		}
		
		if (input) // Освобождаем input, если он еще не освобожден
		{
			free(input);
			input = NULL;
		}

		handle_signal_after_exec(mshell); // Обработка сигналов после выполнения

		// Логика для вывода новой строки после SIGINT/SIGQUIT в дочернем процессе
		if (mshell->exit_status == (128 + SIGINT)) {
			if (isatty(STDOUT_FILENO)) {
				// write(STDOUT_FILENO, "\n", 1); // Уже обрабатывается в wait_for_children или handle_sigint
				rl_on_new_line(); 
			}
		} else if (mshell->exit_status == (128 + SIGQUIT)) {
			if (isatty(STDOUT_FILENO)) {
				rl_on_new_line();
			}
		}
	}
    rl_event_hook = NULL; // Сбрасываем хук при выходе
	return (exit_status); // Возвращаем финальный статус выхода
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
