/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   apply_heredocs.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Pavel Vershinin <pvershin@student.hive.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 11:58:28 by Ilia Munaev       #+#    #+#             */
/*   Updated: 2025/05/07 11:26:35 by Pavel Versh      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file apply_heredocs.c
 * @brief Handles heredoc creation and FD assignment in Minishell.
 */
#include "minishell.h"

static void	close_old_heredocs(t_cmd *cmd_list, int current_fd)
{
	t_list	*redir_list;
	t_redir	*redir;

	if (!cmd_list)
		return ;
	while (cmd_list)
	{
		redir_list = cmd_list->redirs;
		while (redir_list)
		{
			redir = redir_list->content;
			if (redir->type == R_HEREDOC
				&& redir->fd >= 0
				&& redir->fd != current_fd)
				safe_close(&redir->fd);
			redir_list = redir_list->next;
		}
		cmd_list = cmd_list->next;
	}
}

static int	new_heredoc_fd(t_cmd *cmd,
		const char *delim,
		t_cmd *current,
		t_cmd *full_cmd_list)
{
	int		pipe_fd[2];
	pid_t	pid;
	int		status;
	t_cmd	*head;

	head = get_cmd_head(cmd);
	if (pipe(pipe_fd) == -1)
		return (perror_return("new_heredoc_fd: pipe", WRITE_HERED_ERR));
	pid = fork();
	if (pid == -1)
	{
		safe_close(&pipe_fd[0]);
		safe_close(&pipe_fd[1]);
		return (perror_return("new_heredoc_fd: fork", WRITE_HERED_ERR));
	}
	else if (pid == 0)
	{
		int write_pipe_result;
		signal(SIGINT, heredoc_sigint_handler);
		close_old_heredocs(full_cmd_list, pipe_fd[0]);
		safe_close(&pipe_fd[0]);
		close_all_heredoc_fds(current);
		write_pipe_result = write_heredoc_to_pipe(cmd, pipe_fd[1], delim);
		close_all_heredoc_fds(full_cmd_list);
		safe_close(&pipe_fd[1]);
		free_minishell(&cmd->minishell);
		free_cmd(&head);

		if (write_pipe_result == WRITE_HERED_ERR)
		{
			_exit(EXIT_FAILURE); // Или какой-то специфичный код для ошибки записи
		}
		else if (write_pipe_result == HEREDOC_INTERRUPTED)
		{
			// Child 1 должен выйти с кодом HEREDOC_INTERRUPTED, чтобы родитель (new_heredoc_fd) это увидел
			_exit(HEREDOC_INTERRUPTED);
		}
		// Иначе, если write_pipe_result == EXIT_SUCCESS (или что-то еще)
		_exit(EXIT_SUCCESS);	
	}
	else
	{
		safe_close(&pipe_fd[1]);
		signal(SIGINT, SIG_IGN);
		waitpid(pid, &status, 0);
		signal(SIGINT, handle_sigint);
		if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT) // Если Child 1 был убит SIGINT
		{
			safe_close(&pipe_fd[0]);
			close_all_heredoc_fds(full_cmd_list);
			g_signal_flag = 1;
			return (HEREDOC_INTERRUPTED);
		}
		else if (WIFEXITED(status)) // Если Child 1 завершился нормально
		{
			if (WEXITSTATUS(status) == HEREDOC_INTERRUPTED) // Child 1 вышел с кодом HEREDOC_INTERRUPTED
			{
				safe_close(&pipe_fd[0]);
				close_all_heredoc_fds(full_cmd_list);
				g_signal_flag = 1; // Устанавливаем флаг в главном процессе
				return (HEREDOC_INTERRUPTED); // Возвращаем внутренний статус прерывания
			}
			else if (WEXITSTATUS(status) != EXIT_SUCCESS) // Другие ошибки от Child 1
			{
				safe_close(&pipe_fd[0]);
				close_all_heredoc_fds(full_cmd_list);
				// Здесь можно проверить, был ли WEXITSTATUS(status) равен EXIT_FAILURE (если Child 1 так вышел при WRITE_HERED_ERR)
				// и вернуть WRITE_HERED_ERR
				return (WRITE_HERED_ERR); // Или более специфичный код ошибки, если WEXITSTATUS(status) это позволяет
			}
			// Если WEXITSTATUS(status) == EXIT_SUCCESS, то все прошло хорошо
		}
	}
	if (g_signal_flag)
	{
		safe_close(&pipe_fd[0]);
		return (HEREDOC_INTERRUPTED);
	}
	return (pipe_fd[0]);
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
