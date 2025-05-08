/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   write_heredoc_utils1.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Pavel Vershinin <pvershin@student.hive.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/08 11:33:12 by Pavel Versh       #+#    #+#             */
/*   Updated: 2025/05/08 12:01:43 by Pavel Versh      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Searches for a heredoc redirection by its delimiter.
 *
 * Iterates through a linked list of redirection structures (`t_redir`)
 * and returns the one matching the given heredoc delimiter.
 *
 * @param redirs A pointer to the head of the redirection list (`t_list` 
 * of `t_redir`).
 * @param delim The delimiter string to match against the heredoc's filename.
 * @return A pointer to the matching `t_redir` if found; otherwise, NULL.
 */
t_redir	*find_redir_by_delim(t_list *redirs, const char *delim)
{
	t_list	*node;
	t_redir	*redir;

	node = redirs;
	while (node)
	{
		redir = (t_redir *)node->content;
		if (redir->type == R_HEREDOC && redir->filename && delim
			&& ft_strcmp(redir->filename, delim) == 0)
		{
			return (redir);
		}
		node = node->next;
	}
	return (NULL);
}

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
int	write_heredoc_line(int pipe_fd, const char *line)
{
	if (write(pipe_fd, line, ft_strlen(line)) == -1 || write(pipe_fd, "\n",
			1) == -1)
	{
		return (perror_return("write_heredoc_line: write", WRITE_HERED_ERR));
	}
	return (EXIT_SUCCESS);
}

int	read_next_heredoc_line(char **line, const char *delimiter)
{
	char	*line2;

	if (isatty(fileno(stdin)))
		*line = readline("> ");
	else
	{
		line2 = get_next_line(fileno(stdin));
		if (!line2)
			return (free_ptr((void **)line), 0);
		*line = ft_strtrim(line2, "\n");
		free(line2);
		if (!*line)
			return (0);
	}
	if (!*line || ft_strcmp(*line, delimiter) == 0)
		return (free_ptr((void **)line), 0);
	return (1);
}

/**
 * @brief Setup signal handlers inside heredoc child process.
 */
void	setup_heredoc_signals(void)
{
	struct sigaction	sa;

	ft_memset(&sa, 0, sizeof(sa));
	sa.sa_handler = heredoc_sigint_handler;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	if (sigaction(SIGINT, &sa, NULL) == -1)
		perror("sigaction(SIGINT) error in heredoc child");
	signal(SIGQUIT, SIG_IGN);
}
