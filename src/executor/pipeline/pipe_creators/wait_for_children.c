/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wait_for_children.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Pavel Vershinin <pvershin@student.hive.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 14:46:47 by Ilia Munaev       #+#    #+#             */
/*   Updated: 2025/05/07 15:18:23 by Pavel Versh      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file wait_for_children.c
 * @brief Waits for all child processes in a pipeline and returns final status.
 *
 * This file implements logic to wait for multiple forked child processes,
 * typically part of a pipeline in Minishell. The exit status of the last
 * process in the pipeline is used as the final return value.
 */
#include "minishell.h"

static void	print_signal_message(int sig)
{
	const char	*msg;

	msg = NULL;
	if (sig == SIGSEGV)
		msg = "Segmentation fault (core dumped)";
	else if (sig == SIGQUIT)
		msg = "Quit: 3 (core dumped)";
	else if (sig == SIGBUS)
		msg = "Bus error";
	else if (sig == SIGABRT)
		msg = "Aborted";
	else if (sig == SIGFPE)
		msg = "Floating point exception";
	else if (sig == SIGILL)
		msg = "Illegal instruction";
	else if (sig == SIGTERM)
		msg = "Terminated";
	if (msg)
	{
		write(STDERR_FILENO, msg, ft_strlen(msg));
		write(STDERR_FILENO, "\n", 1);
	}
}

/**
 * @brief Interprets the status of the last child process.
 *
 * Handles normal exits, signals (like SIGQUIT), and sets the correct
 * shell-compliant exit status. Also prints a message for signals other
 * than SIGINT.
 *
 * @param status The status returned by waitpid().
 * @return The computed exit status.
 */
static uint8_t	process_child_status(int status)
{
	int	term_sig;

	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	else if (WIFSIGNALED(status))
	{
		term_sig = WTERMSIG(status);
        if (term_sig == SIGINT) {
            if (isatty(STDERR_FILENO)) { // Проверка, что вывод идет на терминал
                write(STDERR_FILENO, "^C", 2); // Вывод ^C без новой строки
            }
        } else if (term_sig != 0) { // Для других сигналов, например SIGQUIT
            print_signal_message(term_sig); // Эта функция уже выводит сообщение и \n 
        }
        return (128 + term_sig);
	}
	return (EXIT_FAILURE);
}

/**
 * @brief Waits for all child processes and returns the last one’s status.
 *
 * Iterates through the list of PIDs using a while loop and waits for each.
 * Only the last child’s status is used to determine the return value.
 *
 * @param pids Array of child process IDs.
 * @param count Number of child processes.
 * @return The exit status of the last child process.
 */
uint8_t	wait_for_children(pid_t *pids, int count)
{
	int		i;
	int		status;
	uint8_t	exit_status;

	i = 0;
	exit_status = EXIT_SUCCESS;
	while (i < count)
	{
		waitpid(pids[i], &status, 0);
		if (i == count - 1)
			exit_status = process_child_status(status);
		i++;
	}
	return (exit_status);
}
