/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Pavel Vershinin <pvershin@student.hive.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 13:13:59 by pvershin          #+#    #+#             */
/*   Updated: 2025/05/07 21:01:33 by Pavel Versh      ###   ########.fr       */
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

int readline_interrupt_hook(void)
{
	if (g_signal_flag)
	{
		rl_done = 1;
		return 1;
	}
	return 0;
}

void handle_sigint(int sig)
{
	(void)sig;
	g_signal_flag = 1;
	rl_done = 1;
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
