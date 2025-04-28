
/**
 * @file apply_heredocs.c
 * @brief Handles heredoc creation and FD assignment in Minishell.
 */
#include "minishell.h"

static void	heredoc_child(const char *delim, int w_fd,
	t_cmd *current, t_cmd *full_list)
{
	signal(SIGINT, heredoc_sigint_handler);
	close_all_heredoc_fds(current);
	if (write_heredoc_to_pipe(w_fd, delim) == WRITE_HERED_ERR)
	{
		close_all_heredoc_fds(full_list);
		safe_close(&w_fd);
		_exit(EXIT_FAILURE);
	}
	close_all_heredoc_fds(full_list);
	safe_close(&w_fd);
	_exit(EXIT_SUCCESS);
}

static int	handle_writer_status(int r_fd, int status, t_cmd *full_list)
{
	if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
	{
		safe_close(&r_fd);
		close_all_heredoc_fds(full_list);
		g_signal_flag = 1;
	return (HEREDOC_INTERRUPTED);
	}
	if (WIFEXITED(status) && WEXITSTATUS(status) != EXIT_SUCCESS)
	{
		safe_close(&r_fd);
		close_all_heredoc_fds(full_list);
		return (WRITE_HERED_ERR);
	}
	return (EXIT_SUCCESS);
}

static int	new_heredoc_fd(const char *delim, t_cmd *current, t_cmd *full_list)
{
	int		pipe_fd[2];
	pid_t	pid;
	pid_t	status;

	if (pipe(pipe_fd) == -1)
		return (perror_return("new_heredoc_fd: pipe", WRITE_HERED_ERR));
	pid = fork();
	if (pid == -1)
	{
		safe_close(&pipe_fd[0]);
		safe_close(&pipe_fd[1]);
		return (perror_return("new_heredoc_fd: fork", WRITE_HERED_ERR));
	}
	if (pid == 0)
		heredoc_child(delim, pipe_fd[1], current, full_list);
	safe_close(&pipe_fd[1]);
	waitpid(pid, &status, 0);
	if (handle_writer_status(pipe_fd[0], status, full_list) != EXIT_SUCCESS)
		return (HEREDOC_INTERRUPTED);
	if (g_signal_flag)
	{
		safe_close(&pipe_fd[0]);
		return (HEREDOC_INTERRUPTED);
	}
	return (pipe_fd[0]);
}

static bool	assign_heredoc_fd(t_redir *redirection, t_cmd *current, t_cmd *full_cmd_list)
{
	redirection->fd = new_heredoc_fd(redirection->filename, current, full_cmd_list);
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
			if (!assign_heredoc_fd(redirection, cmd, full_cmd_list))
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
