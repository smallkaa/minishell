#include "minishell.h"

bool heredoc_exceeds_limit(size_t total_written)
{
	return (total_written >= HEREDOC_MAX_SIZE);
}

bool	is_heredoc(t_redir *redirection)
{
	return (redirection->type == R_HEREDOC);
}

static uint8_t	write_heredoc_to_pipe(int pipe_fd, const char *delimiter)
{
	char	*line;
	size_t	total_written;

	total_written = 0;
	line = NULL;
	while (1)
	{
		line = readline("> ");
		if (line == NULL || ft_strcmp(line, (char *)delimiter) == 0)
		{
			free(line);
			break ;
		}
		total_written += ft_strlen(line) + 1;
		if (heredoc_exceeds_limit(total_written))
		{
			free(line);
			return (error_return("-minishell: heredoc: input too large\n", EXIT_FAILURE));
		}
		if (write(pipe_fd, line, ft_strlen(line)) == -1
			|| write(pipe_fd, "\n", 1) == -1)
		{
			free(line);
			return (perror_return("write_heredoc_to_pipe: write", EXIT_FAILURE));
		}
		free(line);
	}
	return (EXIT_SUCCESS);
}

static int	new_heredoc_fd(const char *delim)
{
	int	pipe_fd[2];

	if (pipe(pipe_fd) == -1)
		return (perror_return("new_heredoc_fd: pipe", -1));
	if (write_heredoc_to_pipe(pipe_fd[1], delim) != EXIT_SUCCESS)
		return (perror_return("new_heredoc_fd: write", -1));
	if (close(pipe_fd[1]) == -1)
		return (perror_return("new_heredoc_fd: close", -1));
	return (pipe_fd[0]);
}

uint8_t	apply_heredocs(t_cmd *cmd)
{
	t_cmd	*initial_cmd_list;
	t_redir	*redirection;
	t_list	*redir_list;

	if (!cmd)
		return (error_return("apply_heredocs: cmd not found\n", EXIT_FAILURE));
	initial_cmd_list = cmd;
	while (cmd)
	{
		redir_list = cmd->redirs;
		while (redir_list)
		{
			redirection = redir_list->content;
			if (is_heredoc(redirection))
			{
				redirection->fd = new_heredoc_fd(redirection->filename);
				if (redirection->fd == -1)
				{
					close_all_heredoc_fds(initial_cmd_list);
					return (EXIT_FAILURE);
				}
			}
			redir_list = redir_list->next;
		}
		cmd = cmd->next;
	}
	return (EXIT_SUCCESS);
}
