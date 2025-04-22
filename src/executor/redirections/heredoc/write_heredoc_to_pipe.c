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
	char	*line;
	size_t	total_written;

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
