/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   apply_redirections.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imunaev- <imunaev-@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 14:49:19 by Ilia Munaev       #+#    #+#             */
/*   Updated: 2025/04/24 17:47:56 by imunaev-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file apply_redirections.c
 * @brief Applies redirections for input/output and heredocs in Minishell.
 */
#include "minishell.h"

/**
 * @brief Applies heredoc (`<<`) redirection by duplicating its file
 * descriptor to stdin.
 *
 * This redirection uses a file descriptor already populated during
 * heredoc setup.
 * If the FD is invalid or `dup2` fails, an error is printed and
 * a redirection error is returned.
 *
 * @param redir Pointer to the heredoc redirection.
 * @return EXIT_SUCCESS on success, REDIR_ERR on failure.
 */
static int	handle_heredoc_redirection(t_redir *redir)
{
	if (redir->fd == -1)
	{
		return (EXIT_SUCCESS);
	}
	if (dup2(redir->fd, STDIN_FILENO) == -1)
		return (perror_return("herdoc: dup2", REDIR_ERR));
	safe_close(&redir->fd);
	return (EXIT_SUCCESS);
}

/**
 * @brief Applies input redirection (`<`) from a file.
 *
 * Opens the file for reading, duplicates it to stdin, and closes the FD.
 * Errors are handled and reported if the file can't be opened or redirected.
 *
 * @param redir Pointer to the input redirection.
 * @return EXIT_SUCCESS on success, REDIR_ERR on failure.
 */
static int	handle_file_input_redirection(t_redir *redir)
{
	int	fd;

	fd = open(redir->filename, O_RDONLY);
	if (fd == -1)
		return (perror_return(redir->filename, REDIR_ERR));
	if (dup2(fd, STDIN_FILENO) == -1)
	{
		safe_close(&fd);
		return (perror_return("in_redir: dup2 input", REDIR_ERR));
	}
	safe_close(&fd);
	return (EXIT_SUCCESS);
}

/**
 * @brief Dispatcher for input redirections.
 *
 * Handles both heredoc and file input types and routes to the correct handler.
 *
 * @param redir The redirection to apply.
 * @return EXIT_SUCCESS on success, REDIR_ERR on failure.
 */
static int	apply_input_redirection(t_redir *redir)
{
	if (redir->type == R_HEREDOC)
		return (handle_heredoc_redirection(redir));
	else if (redir->type == R_INPUT)
		return (handle_file_input_redirection(redir));
	return (EXIT_SUCCESS);
}

/**
 * @brief Applies output redirection (`>` or `>>`) to a file.
 *
 * Opens the file with the appropriate flags (truncate or append),
 * redirects stdout to the file, and closes the FD.
 *
 * @param redir Pointer to the output redirection.
 * @return EXIT_SUCCESS on success, REDIR_ERR on failure.
 */
static int	apply_output_redirection(t_redir *redir)
{
	int	fd;
	int	flags;

	flags = O_WRONLY | O_CREAT;
	if (redir->type == R_APPEND)
		flags |= O_APPEND;
	else
		flags |= O_TRUNC;
	fd = open(redir->filename, flags, 0644);
	if (fd == -1)
		return (perror_return(redir->filename, REDIR_ERR));
	if (dup2(fd, STDOUT_FILENO) == -1)
	{
		safe_close(&fd);
		return (perror_return("dup2 output", REDIR_ERR));
	}
	safe_close(&fd);
	return (EXIT_SUCCESS);
}

/**
 * @brief Applies all redirections for a given command.
 *
 * Iterates through the redirection list (`cmd->redirs`) and applies:
 * - Input redirection: `<` or `<<`
 * - Output redirection: `>` or `>>`
 * If any redirection fails, the function returns EXIT_FAILURE.
 *
 * @param cmd The command whose redirections should be applied.
 * @return EXIT_SUCCESS if all redirections succeed, otherwise EXIT_FAILURE.
 */
uint8_t	apply_redirections(t_cmd *cmd)
{
	t_list	*redir_list;
	t_redir	*redir;
	int		exit_status;

	redir_list = cmd->redirs;
	exit_status = EXIT_SUCCESS;
	while (redir_list)
	{
		redir = redir_list->content;
		if (redir->type == R_INPUT || redir->type == R_HEREDOC)
			exit_status = apply_input_redirection(redir);
		else if (redir->type == R_OUTPUT || redir->type == R_APPEND)
			exit_status = apply_output_redirection(redir);
		if (exit_status == REDIR_ERR)
			return (EXIT_FAILURE);
		redir_list = redir_list->next;
	}
	return ((uint8_t)exit_status);
}
