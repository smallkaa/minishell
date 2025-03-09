#include "minishell.h"

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

void	handle_heredoc(t_cmd *cmd)
{
	int	tmp_fd;

	if (!cmd->in_redir || cmd->in_redir->type != R_HEREDOC)
		return ;
	tmp_fd = open(HEREDOC_TFILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (tmp_fd < 0)
		print_error_exit("heredoc", EXIT_FAILURE);
	write_heredoc_input(tmp_fd, cmd);
}
