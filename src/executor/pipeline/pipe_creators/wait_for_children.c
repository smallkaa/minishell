/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wait_for_children.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pvershin <pvershin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 14:46:47 by Ilia Munaev       #+#    #+#             */
/*   Updated: 2025/05/02 12:24:14 by pvershin         ###   ########.fr       */
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

/**
 * @brief Waits for all child processes in a pipeline and returns
 * the final exit status.
 *
 * This function:
 * - Waits for each process in the array of `pids`.
 * - Determines the final exit status from the **last** command in the pipeline.
 * - If the last command exited normally, returns its exit code.
 * - If it was terminated by a signal, returns `128 + signal_number`.
 * - If it neither exited nor was signaled, returns `EXIT_FAILURE`.
 *
 * @param pids Array of process IDs (one for each command in the pipeline).
 * @param count Number of child processes to wait for.
 * @return The normalized exit status of the **last** command in the pipeline.
 */

uint8_t	wait_for_children(pid_t *pids, int count)
{
	int		i;
	int		status;
	uint8_t	exit_status;
	int		term_sig;

	i = 0;
	exit_status = EXIT_SUCCESS;
	while (i < count)
	{
		waitpid(pids[i], &status, 0);
		if (i == count - 1)
		{
			if (WIFEXITED(status))
				exit_status = WEXITSTATUS(status);
			else if (WIFSIGNALED(status))
			{
				term_sig = WTERMSIG(status);
				if (term_sig == SIGQUIT)
				{
					write(STDERR_FILENO, "Quit: 3\n", 8);
				}
				exit_status = 128 + term_sig;
			}
			else
				exit_status = EXIT_FAILURE;
		}
		i++;
	}
	return (exit_status);
}
