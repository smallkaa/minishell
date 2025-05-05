/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   apply_heredoc_utilc.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ilia Munaev <ilyamunaev@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 14:47:15 by Ilia Munaev       #+#    #+#             */
/*   Updated: 2025/05/05 19:31:42 by Ilia Munaev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file apply_heredoc_utils.c
 * @brief Helper functions for heredoc redirection handling in Minishell.
 */
#include "minishell.h"

/**
 * @brief Safely closes a file descriptor and resets it to -1.
 *
 * Prevents closing an invalid FD and ensures the FD is not reused accidentally.
 *
 * @param fd Pointer to the file descriptor to close.
 */
void	safe_close(int *fd)
{
	if (fd && *fd >= 0)
	{
		if (close(*fd) == -1)
			return ;
		*fd = -1;
	}
}

/**
 * @brief Checks if the total heredoc input exceeds the allowed limit.
 *
 * Used to prevent excessively large heredoc input that could cause
 * memory or performance issues.
 *
 * @param total_written The current size of heredoc input in bytes.
 * @return true if the limit is exceeded, false otherwise.
 */
bool	heredoc_exceeds_limit(size_t total_written)
{
	return (total_written >= HEREDOC_MAX_SIZE);
}

/**
 * @brief Checks whether a redirection is of type heredoc (`<<`).
 *
 * @param redirection Pointer to the redirection.
 * @return true if the redirection is a heredoc, false otherwise.
 */
bool	is_heredoc(t_redir *redirection)
{
	return (redirection->type == R_HEREDOC);
}

/**
 * @brief Closes all heredoc file descriptors in a list of commands.
 *
 * Iterates through all commands and redirections, closing any active
 * heredoc FDs to avoid leaks.
 *
 * @param cmd_list The head of the command list.
 */
void	close_all_heredoc_fds(t_cmd *cmd_list)
{
	t_list	*rlist;
	t_redir	*redirection;

	if (!cmd_list)
		return ;
	while (cmd_list)
	{
		rlist = cmd_list->redirs;
		while (rlist)
		{
			redirection = rlist->content;
			if (is_heredoc(redirection))
				safe_close(&redirection->fd);
			rlist = rlist->next;
		}
		cmd_list = cmd_list->next;
	}
}

int	handle_heredoc_redirection(t_redir *r)
{
	if (r->fd == -1)
		return (EXIT_SUCCESS);
	if (dup2(r->fd, STDIN_FILENO) == -1)
		return (perror_return("heredoc dup2", REDIR_ERR));
	safe_close(&r->fd);
	return (EXIT_SUCCESS);
}
