/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   write_heredoc_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Pavel Vershinin <pvershin@student.hive.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 14:00:09 by pvershin          #+#    #+#             */
/*   Updated: 2025/05/15 10:57:47 by Pavel Versh      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Handles SIGINT in heredoc (child process).
 */
void	heredoc_sigint_handler(int sig)
{
	(void)sig;
	g_signal_flag = 1;
	close(STDIN_FILENO);
}

static int	handle_expanded_line(int pipe_fd, char *line, t_mshell *mshell)
{
	char	*expanded_line;
	int		status;

	expanded_line = expand_env_variables(line, mshell, 0);
	if (!expanded_line)
	{
		perror_return("heredoc expansion failed", WRITE_HERED_ERR);
		expanded_line = line;
	}
	else
		free(line);
	status = write_heredoc_line(pipe_fd, expanded_line);
	free(expanded_line);
	if (status == WRITE_HERED_ERR)
		return (WRITE_HERED_ERR);
	return (EXIT_SUCCESS);
}

static int	process_heredoc_line(t_heredoc_ctx *ctx, char **line)
{
	int	status;

	if (!read_next_heredoc_line(line, ctx->delim))
		return (1);
	*ctx->total += ft_strlen(*line) + 1;
	if (heredoc_exceeds_limit(*ctx->total))
	{
		free(*line);
		return (error_return("heredoc: large input\n", WRITE_HERED_ERR));
	}
	if (ctx->expand)
		status = handle_expanded_line(ctx->pipe_fd, *line, ctx->mshell);
	else
	{
		status = write_heredoc_line(ctx->pipe_fd, *line);
		free(*line);
	}
	*line = NULL;
	if (status == WRITE_HERED_ERR)
		return (WRITE_HERED_ERR);
	return (EXIT_SUCCESS);
}

/**
 * @brief Child: Reads user input and writes heredoc content to pipe.
 */
int	run_heredoc_child(int pipe_fd, const char *delim, t_mshell *mshell,
	int expand_flag)
{
	t_heredoc_ctx	ctx;
	char			*line;
	size_t			total;
	int				status;

	setup_heredoc_signals();
	disable_echoctl();
	total = 0;
	line = NULL;
	status = EXIT_SUCCESS;
	ctx = (t_heredoc_ctx){pipe_fd, delim, mshell, expand_flag, &total};
	while (!g_signal_flag)
	{
		status = process_heredoc_line(&ctx, &line);
		if (status != 0)
			break ;
	}
	if (line)
		free(line);
	if (g_signal_flag)
		return (HEREDOC_INTERRUPTED);
	return (status);
}
