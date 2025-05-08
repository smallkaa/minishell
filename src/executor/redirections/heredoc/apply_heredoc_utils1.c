/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   apply_heredoc_utils1.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Pavel Vershinin <pvershin@student.hive.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/08 12:37:10 by Pavel Versh       #+#    #+#             */
/*   Updated: 2025/05/08 12:42:44 by Pavel Versh      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	pre_heredoc_setup(t_cmd *current, t_cmd *full_cmd_list,
		int *pipe_fd)
{
	signal(SIGINT, heredoc_sigint_handler);
	close_unused_heredocs_child(current, full_cmd_list);
	safe_close(&pipe_fd[0]);
	close_all_heredoc_fds(current);
}

static void	post_heredoc_cleanup(t_cmd *cmd, t_cmd *full_cmd_list, int *pipe_fd,
		int write_result)
{
	t_cmd	*head;

	close_all_heredoc_fds(full_cmd_list);
	safe_close(&pipe_fd[1]);
	head = get_cmd_head(cmd);
	free_minishell(&cmd->minishell);
	free_cmd(&head);
	if (write_result == WRITE_HERED_ERR)
		_exit(EXIT_FAILURE);
	else if (write_result == HEREDOC_INTERRUPTED)
		_exit(HEREDOC_INTERRUPTED);
	_exit(EXIT_SUCCESS);
}

void	handle_heredoc_child(t_heredoc_exec_ctx ctx)
{
	int	write_result;

	pre_heredoc_setup(ctx.current, ctx.full_cmd_list, ctx.pipe_fd);
	write_result = write_heredoc_to_pipe(ctx.cmd, ctx.pipe_fd[1], ctx.delim);
	post_heredoc_cleanup(ctx.cmd, ctx.full_cmd_list, ctx.pipe_fd, write_result);
}

static int	handle_heredoc_status(int status, int *pipe_fd,
		t_cmd *full_cmd_list)
{
	if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
	{
		safe_close(&pipe_fd[0]);
		close_all_heredoc_fds(full_cmd_list);
		g_signal_flag = 1;
		return (HEREDOC_INTERRUPTED);
	}
	if (WIFEXITED(status))
	{
		if (WEXITSTATUS(status) == HEREDOC_INTERRUPTED)
		{
			safe_close(&pipe_fd[0]);
			close_all_heredoc_fds(full_cmd_list);
			g_signal_flag = 1;
			return (HEREDOC_INTERRUPTED);
		}
		if (WEXITSTATUS(status) != EXIT_SUCCESS)
		{
			safe_close(&pipe_fd[0]);
			close_all_heredoc_fds(full_cmd_list);
			return (WRITE_HERED_ERR);
		}
	}
	return (pipe_fd[0]);
}

int	handle_heredoc_parent(pid_t pid, int *pipe_fd, t_cmd *full_cmd_list)
{
	int	status;

	safe_close(&pipe_fd[1]);
	signal(SIGINT, SIG_IGN);
	waitpid(pid, &status, 0);
	signal(SIGINT, handle_sigint);
	if (g_signal_flag)
	{
		safe_close(&pipe_fd[0]);
		return (HEREDOC_INTERRUPTED);
	}
	return (handle_heredoc_status(status, pipe_fd, full_cmd_list));
}
