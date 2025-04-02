#include "minishell.h"

uint8_t	write_heredoc_to_pipe(int pipefd, char *delimiter)
{
	char	*line;

	line = NULL;
	while (1)
	{
		line = readline("> ");
		if (line == NULL || ft_strcmp(line, delimiter) == 0)
		{
			free(line);
			break ;
		}
		if (write(pipefd, line, ft_strlen(line)) == -1 || write(pipefd, "\n", 1) == -1)
		{
			perror("-minishell: write");
			free(line);
			return (EXIT_FAILURE);
		}
		free(line);
	}
	return (EXIT_SUCCESS);
}

uint8_t	setup_heredoc_pipe(t_redir *redir)
{
	int	pipefd[2];

	if (pipe(pipefd) == -1)
		return (perror_return("-minishell: pipe", EXIT_FAILURE));
	if (write_heredoc_to_pipe(pipefd[1], redir->filename) != EXIT_SUCCESS)
	{
		close(pipefd[1]);
		close(pipefd[0]);
		return (EXIT_FAILURE);
	}
	if (close(pipefd[1]) == -1)
		return (perror_return("-minishell: close", EXIT_FAILURE));
	if (dup2(pipefd[0], STDIN_FILENO) == -1)
	{
		perror("dup2");
		close(pipefd[0]);
		return (EXIT_FAILURE);
	}
	if (close(pipefd[0]) == -1)
		return (perror_return("-minishell: close", EXIT_FAILURE));
	return (EXIT_SUCCESS);
}

uint8_t	apply_heredoc(t_cmd *cmd)
{
	t_list	*current_redir;
	t_redir	*redir;
	uint8_t	result = EXIT_SUCCESS;

	if (!cmd->redirs)
		return (EXIT_SUCCESS);
	current_redir = cmd->redirs;
	while (current_redir)
	{
		redir = (t_redir *)current_redir->content;
		if (redir->type == R_HEREDOC)
		{
			result = setup_heredoc_pipe(redir);
			if (result != EXIT_SUCCESS)
				return (result);
		}
		current_redir = current_redir->next;
	}
	return (result);
}
