#include "minishell.h"

uint8_t	apply_heredoc(t_cmd *cmd)
{
	int		pipefd[2];
	char	*line;
	t_list	*current_redir;
	t_redir	*redir;

	if (!cmd->redirs)
		return (EXIT_SUCCESS);

	current_redir = cmd->redirs;
	while (current_redir)
	{
		redir = (t_redir *)current_redir->content;

		if (redir->type == R_HEREDOC)
		{
			if (pipe(pipefd) == -1)
				perror_return("-minishell: pipe", EXIT_FAILURE);

			line = NULL;
			while (1)
			{
				line = readline("> ");
				if (line == NULL || ft_strcmp(line, redir->filename) == 0)
				{
					free(line);
					break ;
				}

				// Writing to the pipe
				if (write(pipefd[1], line, ft_strlen(line)) == -1 || write(pipefd[1], "\n", 1) == -1)
				{
					perror("-minishell: write");
					free(line);
					close(pipefd[1]);
					close(pipefd[0]);
					return (EXIT_FAILURE);
				}
				free(line);
			}

			if (close(pipefd[1]) == -1)
				perror_return("-minishell: close", EXIT_FAILURE);

			if (dup2(pipefd[0], STDIN_FILENO) == -1)
			{
				perror("dup2");
				close(pipefd[0]);
				return (EXIT_FAILURE);
			}
			if (close(pipefd[0]) == -1)
				perror_return("-minishell: close", EXIT_FAILURE);
		}

		current_redir = current_redir->next;
	}
	return (EXIT_SUCCESS);
}
