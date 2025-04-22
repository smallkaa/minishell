/**
 * @file apply_heredocs.c
 * @brief Handles heredoc creation and FD assignment in Minishell.
 */
#include "minishell.h"

/**
 * @brief Creates a new heredoc pipe and writes user input into it.
 *
 * Opens a pipe and calls `write_heredoc_to_pipe()` to write heredoc content
 * into the write-end of the pipe. Closes the write-end after writing and
 * returns the read-end FD.
 *
 * @param delim The heredoc delimiter.
 * @return Read-end of the pipe on success, or WRITE_HERED_ERR on failure.
 */
static int	new_heredoc_fd(const char *delim)
{
	int	pipe_fd[2];

	if (pipe(pipe_fd) == -1)
		return (perror_return("new_heredoc_fd: pipe", WRITE_HERED_ERR));
	if (write_heredoc_to_pipe(pipe_fd[1], delim) == WRITE_HERED_ERR)
	{
		safe_close(&pipe_fd[0]);
		safe_close(&pipe_fd[1]);
		return (WRITE_HERED_ERR);
	}
	safe_close(&pipe_fd[1]);
	return (pipe_fd[0]);
}

/**
 * @brief Assigns a heredoc FD to the given redirection.
 *
 * Creates the heredoc pipe and stores its read-end in `redirection->fd`.
 *
 * @param redirection Pointer to a heredoc-type redirection.
 * @return true on success, false on failure.
 */
static bool	assign_heredoc_fd(t_redir *redirection)
{
	redirection->fd = new_heredoc_fd(redirection->filename);
	if (redirection->fd == WRITE_HERED_ERR)
		return (false);
	return (true);
}

/**
 * @brief Processes all heredoc redirections in a single command.
 *
 * Iterates through the redirection list and assigns FDs for any heredocs found.
 *
 * @param cmd The command whose heredocs will be handled.
 * @return true if all heredocs were handled successfully, false otherwise.
 */
static bool	handle_cmd_heredocs(t_cmd *cmd)
{
	t_list	*redir_list;
	t_redir	*redirection;

	redir_list = cmd->redirs;
	while (redir_list)
	{
		redirection = redir_list->content;
		if (is_heredoc(redirection))
		{
			if (!assign_heredoc_fd(redirection))
				return (false);
		}
		redir_list = redir_list->next;
	}
	return (true);
}

/**
 * @brief Applies heredoc processing for a list of piped commands.
 *
 * For each command in the pipeline:
 * - If a heredoc is found, assigns a pipe FD with the heredoc contents.
 * - On failure, cleans up all opened heredoc FDs and returns failure.
 *
 * @param cmd The head of the command list.
 * @return EXIT_SUCCESS on success, EXIT_FAILURE on failure.
 */
uint8_t	apply_heredocs(t_cmd *cmd)
{
	t_cmd	*initial_cmd_list;

	initial_cmd_list = cmd;
	if (!cmd)
		return (error_return("apply_heredocs: cmd not found\n", EXIT_FAILURE));
	while (cmd)
	{
		if (!handle_cmd_heredocs(cmd))
		{
			close_all_heredoc_fds(initial_cmd_list);
			return (error_return("apply_heredocs: failed\n", EXIT_FAILURE));
		}
		cmd = cmd->next;
	}
	return (EXIT_SUCCESS);
}
