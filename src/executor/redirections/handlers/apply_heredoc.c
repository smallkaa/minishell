#include "minishell.h"

uint8_t	apply_heredoc(t_cmd *cmd)
{

	int		pipefd[2];
	char	*line;

	if (pre_exec_validation(cmd, R_HEREDOC) == EXIT_FAILURE)
		return (EXIT_FAILURE);
	if (pipe(pipefd) == -1)
		perror_return("-minishell: pipe", EXIT_FAILURE);
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
		if (write(pipefd[1], line, ft_strlen(line)) == -1)
		{
			perror("-minishell: write");
			free(line);
			if (close(pipefd[1]) == -1)
				perror_return("-minishell: close", EXIT_FAILURE);
			if (close(pipefd[0]) == -1)
				perror_return("-minishell: close", EXIT_FAILURE);
		}
		if (write(pipefd[1], "\n", 1) == -1)
		{
			perror("-minishell: write");
			free(line);
			if (close(pipefd[1]) == -1)
				perror_return("-minishell: close", EXIT_FAILURE);
			if (close(pipefd[0]) == -1)
				perror_return("-minishell: close", EXIT_FAILURE);
		}
		free(line);
	}
	if (close(pipefd[1]) == -1)
		perror_return("-minishell: close", EXIT_FAILURE);
	if (dup2(pipefd[0], STDIN_FILENO) == -1)
	{
		perror("dup2");
		if (close(pipefd[0]) == -1)
			perror_return("-minishell: close", EXIT_FAILURE);
		perror_return("-minishell: dup2", EXIT_FAILURE);
	}
	if (close(pipefd[0]) == -1)
		perror_return("-minishell: close", EXIT_FAILURE);
	return (EXIT_SUCCESS);
}




		/*
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
		*/
		// Write the line (and a newline) to the write end of the pipe.
