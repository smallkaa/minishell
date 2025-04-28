/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   write_heredoc_to_pipe.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imunaev- <imunaev-@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 14:47:38 by Ilia Munaev       #+#    #+#             */
/*   Updated: 2025/04/28 14:25:54 by imunaev-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file write_heredoc_to_pipe.c
 * @brief Functions for collecting heredoc input and writing it into a pipe.
 */
#include "minishell.h"

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
int	write_heredoc_line(int pipe_fd, const char *line)
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
// int	read_next_heredoc_line(char **line, const char *delimiter)
// {
// 	*line = readline("> ");
// 	if (*line == NULL || ft_strcmp(*line, delimiter) == 0)
// 	{
// 		if (line)
// 			free(*line);
// 		return (0);
// 	}
// 	return (1);
// }
int	read_next_heredoc_line(char **line, const char *delimiter)
{
	//*line = readline("> ");
	if (isatty(fileno(stdin)))
		*line = readline("> ");
	else
	{
		char *line2;
		line2 = get_next_line(fileno(stdin));
		*line = ft_strtrim(line2, "\n");
		free(line2);
	}
	if (*line == NULL || ft_strcmp(*line, delimiter) == 0)
	{
		if (line)
			free(*line);
		return (0);
	}
	return (1);
}
static int	handle_heredoc_status(int status)
{
	if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
	{
		g_signal_flag = 1;
		return (HEREDOC_INTERRUPTED);
	}
	if (WIFEXITED(status) && WEXITSTATUS(status) == WRITE_HERED_ERR)
		return (WRITE_HERED_ERR);
	return (EXIT_SUCCESS);
}

/**
 * @brief Reads user input and writes heredoc content to a pipe.
 *
 * Loops over input lines using readline(), checking against the given
 * delimiter. Writes each line into the write-end of the pipe, with
 * a newline. Prevents overly large input using heredoc size limits.
 *
 * @param pipe_fd The write-end of the pipe.
 * @param delim The heredoc delimiter.
 * @return EXIT_SUCCESS on success, WRITE_HERED_ERR on error.
 * 		return (WRITE_HERED_ERR); // ошибка внутри heredoc (write, malloc)
	return (EXIT_SUCCESS); // heredoc завершён нормально

 */
int	write_heredoc_to_pipe(int pipe_fd, const char *delim)
{
	pid_t	pid;
	int		status;
	int		ret;

	pid = fork();
	if (pid == -1)
		return (perror_return("fork", WRITE_HERED_ERR));
	if (pid == 0)
	{
		ret = run_heredoc_child(pipe_fd, delim);
		close(pipe_fd);
		exit(ret);
	}
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	close(pipe_fd);
	waitpid(pid, &status, 0);
	signal(SIGINT, handle_sigint);
	signal(SIGQUIT, handle_sigquit);
	return (handle_heredoc_status(status));
}
