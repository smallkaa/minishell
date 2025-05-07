/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Pavel Vershinin <pvershin@student.hive.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 13:13:59 by pvershin          #+#    #+#             */
/*   Updated: 2025/05/07 20:33:48 by Pavel Versh      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

volatile sig_atomic_t	g_signal_flag = 0;

extern void	rl_replace_line(const char *a, int b);

void	disable_echoctl(void)
{
	struct termios	term;

	if (tcgetattr(STDIN_FILENO, &term) == 0)
	{
		term.c_lflag &= ~ECHOCTL;
		tcsetattr(STDIN_FILENO, TCSANOW, &term);
	}
}

int readline_interrupt_hook(void) {
    if (g_signal_flag) {
        // g_signal_flag уже установлен обработчиком SIGINT
        // Здесь мы просто говорим readline завершиться.
        // rl_done = 1; // rl_done устанавливается в handle_sigint
        // Важно: rl_done может быть недостаточно, если readline "глубоко" в своем цикле.
        // Попробуем также rl_free_line_state() и rl_cleanup_after_signal()
        // перед тем как readline вернется в основной цикл.
        // Однако, главная задача хука - вернуть не 0, чтобы readline прекратил ожидание.

        // Этот вариант заставит readline немедленно вернуться.
        // rl_replace_line("", 0); // Очищаем строку
        // rl_done = 1; // Подтверждаем завершение
        // return 0; // Возвращаем 0, readline сам выйдет из-за rl_done

        // Более агрессивный вариант, если rl_done не всегда срабатывает мгновенно
        // для прерывания readline изнутри хука:
        rl_done = 1; // Убедимся, что он установлен
        return 1;    // Возврат ненулевого значения из event_hook прерывает readline
    }
    return 0; // Продолжаем нормальную работу readline
}

void handle_sigint(int sig)
{
	(void)sig;
	g_signal_flag = 1;

	if (isatty(STDOUT_FILENO)) {
		// 1. Переводим строку, чтобы следующее приглашение было на новой строке.
		write(STDOUT_FILENO, "\n", 1);

		// 2. Сообщаем readline, что курсор на новой строке (после нашего write \n).
		rl_on_new_line();

		// 3. Очищаем любой текст, который мог быть в буфере readline.
		rl_replace_line("", 0); // 0 -> не добавлять в историю

		// 4. Заставляем readline немедленно перерисовать приглашение и текущую (пустую) строку ввода.
		rl_redisplay();
	}
	// 5. Сигнализируем readline, что текущий ввод завершен.
	// Это важно, чтобы readline() в основном цикле корректно вышел,
	// если он все еще был активен в момент сигнала.
	rl_done = 1;

	debug_printf(">>> handle_sigint: All actions performed (\\n, on_new_line, replace_line, redisplay, rl_done=1) <<<\n");
}

// SIGQUIT (Ctrl-\) Handler - does nothing
void	handle_sigquit(int sig)
{
	(void)sig;
}

// Function to setup signal handling
void	setup_signal_handlers(void)
{
	struct sigaction	sa;

	disable_echoctl();
	sa.sa_handler = handle_sigint;
	sa.sa_flags = SA_RESTART;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGINT, &sa, NULL);
	signal(SIGQUIT, SIG_IGN);
}
