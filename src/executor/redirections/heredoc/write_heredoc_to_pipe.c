/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   write_heredoc_to_pipe.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Pavel Vershinin <pvershin@student.hive.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 14:47:38 by Ilia Munaev       #+#    #+#             */
/*   Updated: 2025/05/07 11:19:22 by Pavel Versh      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file write_heredoc_to_pipe.c
 * @brief Functions for collecting heredoc input and writing it into a pipe.
 */
#include "minishell.h"

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
int write_heredoc_line(int pipe_fd, const char *line)
{
	if (write(pipe_fd, line, ft_strlen(line)) == -1 || write(pipe_fd, "\n", 1) == -1)
	{
		return (perror_return("write_heredoc_line: write", WRITE_HERED_ERR));
	}
	return (EXIT_SUCCESS);
}

int read_next_heredoc_line(char **line, const char *delimiter)
{
	if (isatty(fileno(stdin)))
	{
		*line = readline("> ");
	}
	else
	{
		char *line2 = get_next_line(fileno(stdin));
		if (!line2)
		{
			*line = NULL;
			return (0);
		}
		*line = ft_strtrim(line2, "\n");
		free(line2);
		if (!*line)
			return (0);
	}

	if (*line == NULL || ft_strcmp(*line, delimiter) == 0)
	{
		free(*line);
		*line = NULL;
		return (0);
	}
	return (1);
}

// static int handle_heredoc_status(int status)
// {
// 	if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
// 	{
// 		g_signal_flag = 1;
// 		return (HEREDOC_INTERRUPTED);
// 	}
// 	if (WIFEXITED(status) && WEXITSTATUS(status) == WRITE_HERED_ERR)
// 		return (WRITE_HERED_ERR);
// 	return (EXIT_SUCCESS);
// }

static int handle_heredoc_status(int status) // status от Child 2 (run_heredoc_child)
{
    if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
    {
        g_signal_flag = 1; // В контексте Child 1
        return (HEREDOC_INTERRUPTED);
    }
    if (WIFEXITED(status))
    {
        if (WEXITSTATUS(status) == HEREDOC_INTERRUPTED) { // Child 2 вышел с этим кодом
            g_signal_flag = 1; // В контексте Child 1
            return (HEREDOC_INTERRUPTED);
        }
        if (WEXITSTATUS(status) == WRITE_HERED_ERR) {
            return (WRITE_HERED_ERR);
        }
        if (WEXITSTATUS(status) == EXIT_SUCCESS) {
             return (EXIT_SUCCESS);
        }
        // Если Child 2 вышел с каким-то другим кодом, можно его пробросить или вернуть общую ошибку
        // return WEXITSTATUS(status); // или
        // return SOME_OTHER_ERROR;
    }
    return (EXIT_SUCCESS); // По умолчанию
}

// Вспомогательная функция для поиска t_redir по разделителю
static t_redir	*find_redir_by_delim(t_list *redirs, const char *delim)
{
	t_list *node;
	t_redir *redir;

	node = redirs;
	while (node)
	{
		redir = (t_redir *)node->content;
		// Сравниваем filename (который используется как delim для heredoc)
		// Добавим проверку на NULL для надежности
		if (redir->type == R_HEREDOC && redir->filename && delim && ft_strcmp(redir->filename, delim) == 0)
		{
			return (redir);
		}
		node = node->next;
	}
	return (NULL); // Не найден
}

/**
 * @brief Reads user input and writes heredoc content to a pipe.
 *
 * Loops over input lines using readline(), checking against the given
 * delimiter. Writes each line into the write-end of the pipe, with
 * a newline. Prevents overly large input using heredoc size limits.
 *
 * @param pipe_fd The write-end of the pipe.
 * @param delim The heredoc delimiter.
 * @return EXIT_SUCCESS on success, WRITE_HERED_ERR on error.
 * 		return (WRITE_HERED_ERR); // ошибка внутри heredoc (write, malloc)
	return (EXIT_SUCCESS); // heredoc завершён нормально

 */
int write_heredoc_to_pipe(t_cmd *cmd, int pipe_fd, const char *delim)
{
	pid_t pid;
	int status;
	int ret;
	int expand_flag;
	t_redir *heredoc_redir;
	t_cmd	*head;

	// --- Находим нужный редирект и его флаг ---
	heredoc_redir = find_redir_by_delim(cmd->redirs, delim);
	if (heredoc_redir)
	{
		expand_flag = heredoc_redir->expand_in_heredoc;
	}
	else
	{
		// Эта ситуация не должна возникать при корректном парсинге
		print_error("Internal error: heredoc delimiter not found.\n");
		expand_flag = 0; // По умолчанию не раскрываем
	}
	head = get_cmd_head(cmd);
	pid = fork();
	if (pid == -1)
		return (perror_return("fork", WRITE_HERED_ERR));
	if (pid == 0)
	{
		ret = run_heredoc_child(pipe_fd, delim, cmd->minishell, expand_flag);
		safe_close(&pipe_fd);
		free_minishell(&cmd->minishell);
		free_cmd(&head);
		_exit(ret);
	}
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	safe_close(&pipe_fd);
	free_minishell(&cmd->minishell);
	waitpid(pid, &status, 0);
	signal(SIGINT, handle_sigint);
	signal(SIGQUIT, handle_sigquit);
	return (handle_heredoc_status(status));
}
