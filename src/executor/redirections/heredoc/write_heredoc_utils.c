/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   write_heredoc_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Pavel Vershinin <pvershin@student.hive.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 14:00:09 by pvershin          #+#    #+#             */
/*   Updated: 2025/05/07 12:36:20 by Pavel Versh      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"


int check_heredoc_interrupt_hook(void) {
    if (g_signal_flag) {
        // Можно попробовать сначала это, если rl_done из обработчика не сработало
        rl_replace_line("", 0); // Очистить строку
        rl_done = 1;            // Завершить readline

        // Альтернативно или в дополнение, можно попытаться "протолкнуть" ввод,
        // чтобы readline вышел из ожидания. Это более "хакерский" способ.
        // rl_stuff_char('\n'); // Эмулировать нажатие Enter (может быть нежелательно)
    }
    return 0; // Возвращаем 0, чтобы readline продолжал нормально, если нет флага
}

/**
 * @brief Handles SIGINT in heredoc (child process).
 */
void	heredoc_sigint_handler(int sig)
{
	(void)sig;
	g_signal_flag = 1; // nothing else only setting global flag 
//	rl_replace_line("", 1); // Очищаем буфер readline
	rl_done = 1; 
	//write(STDOUT_FILENO, "\n", 1);
	//_exit(1);
	//write(STDOUT_FILENO, "\n", 1);
	rl_replace_line("", 0);
	rl_on_new_line();
	rl_redisplay();
}

/**
 * @brief Setup signal handlers inside heredoc child process.
 */
void	setup_heredoc_signals(void)
{
	//signal(SIGINT, heredoc_sigint_handler);
	//signal(SIGQUIT, SIG_IGN);
	struct sigaction sa;

	ft_memset(&sa, 0, sizeof(sa)); // Инициализация структуры нулями
	sa.sa_handler = heredoc_sigint_handler;
	sa.sa_flags = 0; // Определите нужные флаги (например, SA_RESTART)
	sigemptyset(&sa.sa_mask);
	if (sigaction(SIGINT, &sa, NULL) == -1)
		perror("sigaction(SIGINT) error in heredoc child"); // Обработка ошибки

	signal(SIGQUIT, SIG_IGN);
}

/**
 * @brief Child: Reads user input and writes heredoc content to pipe.
 */
int	run_heredoc_child(int pipe_fd, const char *delim, t_mshell *mshell, int expand_flag)
{
	char	*line;
	char	*expanded_line;
	size_t	total_written;
	int     write_status;

	setup_heredoc_signals();
	disable_echoctl();
	line = NULL;
	total_written = 0;
	write_status = EXIT_SUCCESS;
    rl_event_hook = check_heredoc_interrupt_hook; // Устанавливаем хук

	while (1)
	{
		// Проверяем флаг перед чтением строки
		if (g_signal_flag) {
			break;
		}

		if (!read_next_heredoc_line(&line, delim)) {
			// EOF (Ctrl+D) или найден разделитель.
			// readline вернет NULL при Ctrl+C, если обработчик сигнала не выходит,
			// что приведет к выходу из read_next_heredoc_line с возвратом 0.
			break;
		}

		// Еще одна проверка флага после readline, на случай если сигнал пришел во время его выполнения
		if (g_signal_flag) {
			if (line) free(line); // Освобождаем line, если readline успел его выделить
			line = NULL;
			break;
		}
		
		total_written += ft_strlen(line) + 1;
		if (heredoc_exceeds_limit(total_written))
		{
			free(line);
			return (error_return("heredoc: large input\n", WRITE_HERED_ERR));
		}
		if (expand_flag)
        {
            expanded_line = expand_env_variables(line, mshell, 0);
            if (!expanded_line)
			{
                perror_return("heredoc expansion failed", WRITE_HERED_ERR);
                expanded_line = line;
            }
			else
                free(line);
            line = NULL;
            
            write_status = write_heredoc_line(pipe_fd, expanded_line);
            free(expanded_line);
            if (write_status == WRITE_HERED_ERR)
                 return (WRITE_HERED_ERR);
        }
        else
        {
            write_status = write_heredoc_line(pipe_fd, line);
            free(line);
            line = NULL;
            if (write_status == WRITE_HERED_ERR)
                return (WRITE_HERED_ERR);
        }
	}
	rl_event_hook = NULL;
	if (line)
		free(line);
	if (g_signal_flag) {
		// Дочерний процесс завершается из-за сигнала.
		// Родительский процесс (new_heredoc_fd или write_heredoc_to_pipe)
		// должен корректно обработать этот код выхода.
		return (HEREDOC_INTERRUPTED);
	}
	return (EXIT_SUCCESS);
}
