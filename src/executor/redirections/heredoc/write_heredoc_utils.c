/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   write_heredoc_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pvershin <pvershin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 14:00:09 by pvershin          #+#    #+#             */
/*   Updated: 2025/04/25 14:05:12 by pvershin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Handles SIGINT in heredoc (child process).
 */
void	heredoc_sigint_handler(int sig)
{
	(void)sig;
	write(STDOUT_FILENO, "\n", 1);
	exit(1);
}

/**
 * @brief Setup signal handlers inside heredoc child process.
 */
void	setup_heredoc_signals(void)
{
	signal(SIGINT, heredoc_sigint_handler);
	signal(SIGQUIT, SIG_IGN);
}

/**
 * @brief Child: Reads user input and writes heredoc content to pipe.
 */
int	run_heredoc_child(int pipe_fd, const char *delim)
{
	char	*line;
	size_t	total_written;

	setup_heredoc_signals();
	disable_echoctl();
	line = NULL;
	total_written = 0;
	while (read_next_heredoc_line(&line, delim))
	{
		total_written += ft_strlen(line) + 1;
		if (heredoc_exceeds_limit(total_written))
		{
			free(line);
			return (error_return("heredoc: large input\n", WRITE_HERED_ERR));
		}
		if (write_heredoc_line(pipe_fd, line) == WRITE_HERED_ERR)
		{
			free(line);
			return (WRITE_HERED_ERR);
		}
		free(line);
	}
	return (EXIT_SUCCESS);
}
