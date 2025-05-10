/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   write_heredoc_to_pipe.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ilia Munaev <ilyamunaev@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 14:47:38 by Ilia Munaev       #+#    #+#             */
/*   Updated: 2025/05/10 16:08:01 by Ilia Munaev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file write_heredoc_to_pipe.c
 * @brief Functions for collecting heredoc input and writing it into a pipe.
 */
#include "minishell.h"

static int	handle_heredoc_status(int status)
{
	if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
	{
		g_signal_flag = 1;
		return (HEREDOC_INTERRUPTED);
	}
	if (WIFEXITED(status))
	{
		if (WEXITSTATUS(status) == HEREDOC_INTERRUPTED)
		{
			g_signal_flag = 1;
			return (HEREDOC_INTERRUPTED);
		}
		if (WEXITSTATUS(status) == WRITE_HERED_ERR)
		{
			return (WRITE_HERED_ERR);
		}
		if (WEXITSTATUS(status) == EXIT_SUCCESS)
		{
			return (EXIT_SUCCESS);
		}
	}
	return (EXIT_SUCCESS);
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
 * 		return (WRITE_HERED_ERR);
	return (EXIT_SUCCESS);

 */
static int	get_expand_flag(t_cmd *cmd, const char *delim)
{
	t_redir	*redir;

	redir = find_redir_by_delim(cmd->redirs, delim);
	if (redir)
		return (redir->expand_in_heredoc);
	print_error("Internal error: heredoc delimiter not found.\n");
	return (0);
}

static int	heredoc_parent_process(pid_t pid, int pipe_fd,
	t_mshell **ms)
{
	int	status;

	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	safe_close(&pipe_fd);
	free_minishell(ms);
	waitpid(pid, &status, 0);
	signal(SIGINT, handle_sigint);
	signal(SIGQUIT, handle_sigquit);
	return (handle_heredoc_status(status));
}

int	write_heredoc_to_pipe(t_cmd *cmd, int pipe_fd, const char *delim)
{
	pid_t	pid;
	int		ret;
	int		expand_flag;
	t_cmd	*head;

	expand_flag = get_expand_flag(cmd, delim);
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
	return (heredoc_parent_process(pid, pipe_fd, &cmd->minishell));
}
