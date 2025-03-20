#include "minishell.h"

static int	save_stdin(void)
{
	int saved_stdin;

	saved_stdin = dup(STDIN_FILENO);
	if (saved_stdin == -1)
		perror("dup");
	return (saved_stdin);
}

static char	**read_heredoc_input(const char *end_of_msg)
{
	char	*line;
	char	**heredoc_content = NULL;
	size_t	line_count;
	char	**new_heredoc;

	line_count = 0;
	while (1)
	{
		line = readline("> ");
		if (!line)
			break ;
		if (ft_strcmp(line, end_of_msg) == 0)
		{
			free(line);
			break ;
		}
		new_heredoc = ft_realloc(heredoc_content,
					line_count * sizeof(char *),
					(line_count + 2) * sizeof(char *));
		if (!new_heredoc)
		{
			print_error("Memory allocation failed\n");
			free(line);
			return (NULL);
		}
		heredoc_content = new_heredoc;
		heredoc_content[line_count] = ft_strdup(line);
		if (!heredoc_content[line_count])
		{
			print_error("ft_strdup failed\n");
			free(line);
			return NULL;
		}
		line_count++;
		heredoc_content[line_count] = NULL;
		free(line);
	}
	return (heredoc_content);
}

/**
 * @brief Redirects heredoc input to stdin using a memory stream (no pipe).
 */
static int heredoc_to_stdin(char **heredoc_content, size_t line_count)
{
	int fds[2];
	size_t i = 0;

	if (pipe(fds) == -1)
	{
		perror("pipe");
		return -1;
	}

	while (i < line_count)
	{
		write(fds[1], heredoc_content[i], ft_strlen(heredoc_content[i]));
		write(fds[1], "\n", 1);
		i++;
	}
	close(fds[1]); // Close write-end of the pipe

	return fds[0]; // Return read-end of the pipe
}

/**
 * @brief Executes a built-in command after handling heredoc input.
 */
static uint8_t execute_builtin_with_heredoc(t_cmd *cmd, char **heredoc_content, int saved_stdin)
{
	size_t	line_count;
	int		fd_read;
	uint8_t	exit_status;

	line_count = 0;
	while (heredoc_content[line_count] != NULL)
		line_count++;

	fd_read = heredoc_to_stdin(heredoc_content, line_count);
	if (fd_read == -1)
	{
		free(heredoc_content);
		close(saved_stdin);
		return (EXIT_FAILURE);
	}

	dup2(fd_read, STDIN_FILENO);
	close(fd_read);

	exit_status = run_executor(cmd); // Execute the built-in command
	dup2(saved_stdin, STDIN_FILENO);
	close(saved_stdin);

	return exit_status;
}

/**
 * @brief Handles heredoc for a single built-in command (no pipes).
 */
uint8_t	handle_heredoc_builtin(t_cmd *cmd)
{
	int		saved_stdin;
	char	**heredoc_content;
	uint8_t	exit_status;

	exit_status = EXIT_SUCCESS;
	if (!cmd->in_redir || !cmd->in_redir->filename || cmd->in_redir->type != R_HEREDOC)
		return (EXIT_FAILURE);

	saved_stdin = save_stdin();
	if (saved_stdin == -1)
		return (EXIT_FAILURE);

	heredoc_content = read_heredoc_input(cmd->in_redir->filename);
	if (!heredoc_content)
	{
		close(saved_stdin);
		return (EXIT_FAILURE);
	}
	exit_status = execute_builtin_with_heredoc(cmd, heredoc_content, saved_stdin);
	free(heredoc_content);
	cmd->in_redir->type = R_INPUT;
	free(cmd->in_redir->filename);
	return (exit_status);
}
