/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   write_heredoc_to_pipe.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ilia Munaev <ilyamunaev@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 14:47:38 by Ilia Munaev       #+#    #+#             */
/*   Updated: 2025/04/23 15:00:19 by Ilia Munaev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file write_heredoc_to_pipe.c
 * @brief Functions for collecting heredoc input and writing it into a pipe.
 */
#include "minishell.h"
// #include "signals.h"
// #include <signal.h>
// #include <sys/wait.h>
// #include <unistd.h>
// #include <stdlib.h>

/**
 * @brief Writes a single line to the heredoc pipe with a trailing newline.
 *
 * This function appends a newline character after writing the given line
 * and handles any `write()` errors.
 *
 * @param pipe_fd Write end of the pipe.
 * @param line The line to write.
 * @return EXIT_SUCCESS on success, WRITE_HERED_ERR on failure.
 */
static int	write_heredoc_line(int pipe_fd, const char *line)
{
	if (write(pipe_fd, line, ft_strlen(line)) == -1
		|| write(pipe_fd, "\n", 1) == -1)
	{
		return (perror_return("write_heredoc_line: write", WRITE_HERED_ERR));
	}
	return (EXIT_SUCCESS);
}

/**
 * @brief Reads the next line from the user for heredoc input.
 *
 * Uses `readline("> ")` to prompt the user.
 * If the user input matches the delimiter, input stops.
 *
 * @param line Pointer to store the allocated line.
 * @param delimiter The heredoc end marker.
 * @return 1 if a valid line was read, 0 if EOF or delimiter matched.
 */
static int	read_next_heredoc_line(char **line, const char *delimiter)
{
	*line = readline("> ");
	if (*line == NULL || ft_strcmp(*line, delimiter) == 0)
	{
		if (line)
			free(*line);
		return (0);
	}
	return (1);
}

/**
 * @brief Handles SIGINT in heredoc (child process).
 */
static void	heredoc_sigint_handler(int sig)
{
	(void)sig;
	write(STDOUT_FILENO, "\n", 1);
	exit(1);
}

/**
 * @brief Setup signal handlers inside heredoc child process.
 */
static void	setup_heredoc_signals(void)
{
	signal(SIGINT, heredoc_sigint_handler);
	signal(SIGQUIT, SIG_IGN);
}

/**
 * @brief Child: Reads user input and writes heredoc content to pipe.
 */
static int	run_heredoc_child(int pipe_fd, const char *delim)
{
	char	*line;
	size_t	total_written;

	setup_heredoc_signals();
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

/**
 * @brief Reads user input and writes heredoc content to a pipe.
 *
 * Loops over input lines using `readline()`, checking against the given
 * delimiter. Writes each line into the write-end of the pipe, with
 * a newline. Prevents overly large input using heredoc size limits.
 *
 * @param pipe_fd The write-end of the pipe.
 * @param delim The heredoc delimiter.
 * @return EXIT_SUCCESS on success, WRITE_HERED_ERR on error.
 */
int	write_heredoc_to_pipe(int pipe_fd, const char *delim)
{
	pid_t	pid;
	int		status;

	pid = fork();
	if (pid == -1)
		return (perror_return("fork", WRITE_HERED_ERR));
	if (pid == 0)
	{
		int ret = run_heredoc_child(pipe_fd, delim);
		close(pipe_fd);
		exit(ret == EXIT_SUCCESS ? 0 : 1);
	}
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	waitpid(pid, &status, 0);
	signal(SIGINT, handle_sigint);
	signal(SIGQUIT, handle_sigquit);
	if (WIFEXITED(status) && WEXITSTATUS(status) == 1)
	{
		g_signal_flag = 1;
		return (1);
	}
	return (EXIT_SUCCESS);
}
