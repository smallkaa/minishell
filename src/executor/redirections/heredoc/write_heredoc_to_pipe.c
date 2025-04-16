#include "minishell.h"

static int	write_heredoc_line(int pipe_fd, const char *line)
{
	if (write(pipe_fd, line, ft_strlen(line)) == -1
		|| write(pipe_fd, "\n", 1) == -1)
	{
		return (perror_return("write_heredoc_line: write", WRITE_HERED_ERR));
	}
	return (EXIT_SUCCESS);
}

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

int	write_heredoc_to_pipe(int pipe_fd, const char *delimiter)
{
	char	*line;
	size_t	total_written;

	line = NULL;
	total_written = 0;
	while (read_next_heredoc_line(&line, delimiter))
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
