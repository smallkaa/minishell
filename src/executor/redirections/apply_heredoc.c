#include "minishell.h"

uint8_t	apply_heredoc(t_cmd *cmd)
{

	int		pipefd[2];
	char	*line;
	char	*temp;
	t_cmd *temp_cmd;

	if (!cmd->in_redir || !cmd->in_redir->filename)
    {
        print_error("Error: Heredoc redirection not properly initialized.\n");
        return (EXIT_FAILURE);
    }


	// Create a pipe to hold the heredoc content.
	if (pipe(pipefd) == -1)
	{
		perror("pipe");
		return EXIT_FAILURE;
	}
	line = NULL;
	while (1)
	{
		line = readline("> ");
		if (line == NULL)
			break ;
		if (ft_strcmp(line, cmd->in_redir->filename) == 0)
		{
			free(line);
			break ;
		}
		if (cmd->in_redir->expand == true)
		{
			temp_cmd = malloc (sizeof(t_cmd));
			if (!temp_cmd)
			{
				free (line);
				return (EXIT_FAILURE);
			}
			temp = ft_strjoin("foo", line);
			if (!temp)
			{
				free(line);
				free_cmd(temp_cmd);
				return(EXIT_FAILURE);
			}
			free(line);
			temp_cmd = run_parser(cmd->minishell, temp);
			line = temp_cmd->argv[1];
			free_cmd(temp_cmd);
		}
		// Write the line (and a newline) to the write end of the pipe.

		if (write(pipefd[1], line, ft_strlen(line)) == -1)
		{
			perror("write");
			free(line);
			close(pipefd[1]);
			close(pipefd[0]);
			return (EXIT_FAILURE);
		}
		if (write(pipefd[1], "\n", 1) == -1)
		{
			perror("write");
			free(line);
			close(pipefd[1]);
			close(pipefd[0]);

		}
		free(line);
	}
	// Close the write end so that the reading process sees an EOF.
	close(pipefd[1]);

	// Redirect the read end of the pipe to standard input (FD 0).
	if (dup2(pipefd[0], STDIN_FILENO) == -1)
	{
		perror("dup2");
		close(pipefd[0]);
		return EXIT_FAILURE;
	}
	close(pipefd[0]);
	// printf("---R_HERDOC applied\n"); // test

	return (EXIT_SUCCESS);
}
