/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   apply_heredocs.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Pavel Vershinin <pvershin@student.hive.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/08 12:23:28 by Pavel Versh       #+#    #+#             */
/*   Updated: 2025/05/15 20:39:55 by Pavel Versh      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	new_heredoc_fd(t_cmd *cmd,
		const char *delim,
		t_cmd *current,
		t_cmd *full_cmd_list)
{
	int					pipe_fd[2];
	pid_t				pid;
	t_heredoc_exec_ctx	ctx;

	if (pipe(pipe_fd) == -1)
		return (perror_return("new_heredoc_fd: pipe", WRITE_HERED_ERR));
	pid = fork();
	if (pid == -1)
	{
		safe_close(&pipe_fd[0]);
		safe_close(&pipe_fd[1]);
		return (perror_return("new_heredoc_fd: fork", WRITE_HERED_ERR));
	}
	ctx.cmd = cmd;
	ctx.delim = delim;
	ctx.current = current;
	ctx.full_cmd_list = full_cmd_list;
	ctx.pipe_fd = pipe_fd;
	if (pid == 0)
		handle_heredoc_child(ctx);
	return (handle_heredoc_parent(pid, pipe_fd, full_cmd_list));
}

static bool	assign_heredoc_fd(t_cmd *cmd,
		t_redir *redirection,
		t_cmd *current,
		t_cmd *full_cmd_list)
{
	redirection->fd = new_heredoc_fd(cmd,
			redirection->filename,
			current,
			full_cmd_list);
	if (redirection->fd == WRITE_HERED_ERR)
		return (false);
	if (redirection->fd == HEREDOC_INTERRUPTED)
	{
		if (isatty(STDOUT_FILENO) && isatty(STDIN_FILENO))
		{
			write(STDOUT_FILENO, "\n", 1);
			rl_replace_line("", 0);
			rl_on_new_line();
		}
		g_signal_flag = 1;
		redirection->fd = -1;
		return (true);
	}
	return (true);
}

static bool	handle_cmd_heredocs(t_cmd *cmd, t_cmd *full_cmd_list)
{
	t_list	*redir_list;
	t_redir	*redirection;

	redir_list = cmd->redirs;
	while (redir_list)
	{
		redirection = redir_list->content;
		if (is_heredoc(redirection))
		{
			if (!assign_heredoc_fd(cmd, redirection, cmd, full_cmd_list))
				return (false);
		}
		redir_list = redir_list->next;
	}
	return (true);
}

uint8_t	apply_heredocs(t_cmd *cmd)
{
	t_cmd	*initial_cmd_list;

	initial_cmd_list = cmd;
	if (!cmd)
		return (error_return("apply_heredocs: cmd not found\n", EXIT_FAILURE));
	while (cmd)
	{
		if (!handle_cmd_heredocs(cmd, initial_cmd_list))
		{
			close_all_heredoc_fds(initial_cmd_list);
			return (error_return("apply_heredocs: failed\n", EXIT_FAILURE));
		}
		cmd = cmd->next;
	}
	return (EXIT_SUCCESS);
}
