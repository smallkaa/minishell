#include "minishell.h"

/**
 * Reads user input for heredoc and writes it to a temporary file.
 *
 * - Reads input line by line using `readline()`.
 * - Stops reading when the delimiter is encountered.
 * - Writes the input to the temporary file.
 * - Handles Ctrl+D (EOF).
 * - Closes the file descriptor after writing.
 *
 * @param tmp_fd	The file descriptor of the temporary heredoc file.
 * @param cmd		The command containing heredoc metadata (e.g., delimiter).
 */
static void	write_heredoc_input(int tmp_fd, t_cmd *cmd)
{
	char	*line;
	char	*input;

	while (1)
	{
		line = readline("> ");
		if (!line)
			break ;
		if (ft_strcmp(line, cmd->in_redir->filename) == 0)
		{
			free(line);
			break ;
		}
		input = ft_strdup(line);
		if (write(tmp_fd, input, ft_strlen(input)) == -1)
			print_error_exit("write", EXIT_FAILURE);
		if (write(tmp_fd, "\n", 1) == -1)
			print_error_exit("write", EXIT_FAILURE);
		free(input);
		free(line);
	}
	if (close(tmp_fd) == -1)
		print_error_exit("close", EXIT_FAILURE);
}

/**
 * Handles heredoc input redirection by writing user input to a temporary file.
 *
 * - Creates a temporary file to store heredoc content.
 * - Calls `write_heredoc_input()` to capture user input.
 * - Ensures the file is properly created before writing.
 *
 * @param cmd The command containing heredoc information.
 */
void	handle_heredoc(t_cmd *cmd, char **envp)
{
	int	tmp_fd;

	if (!cmd->in_redir || cmd->in_redir->type != R_HEREDOC)
		return ;
	tmp_fd = open(HEREDOC_TFILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (tmp_fd < 0)
		print_error_exit("heredoc", EXIT_FAILURE);
	write_heredoc_input(tmp_fd, cmd);
}
