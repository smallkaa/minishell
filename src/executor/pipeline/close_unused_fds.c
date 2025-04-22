/**
 * @file close_unused_fds.c
 * @brief File descriptor management utilities for Minishell.
 *
 * Provides helper functions to cleanly close file descriptors in parent
 * and child processes, especially when working with pipes.
 */
#include "minishell.h"

/**
 * @brief Closes unused file descriptors in a pipeline context.
 *
 * This function is used in the parent process after forking a child
 * and setting up redirections. It ensures that:
 * - The previous input file descriptor (`in_fd`) is closed if it's
 * not `STDIN_FILENO`.
 * - The read and write ends of the current pipe are closed if they were opened.
 *
 * Properly handles and reports close errors using `perror()`.
 *
 * @param in_fd The current input file descriptor to be closed if applicable.
 * @param pipe_fd Array of 2 integers representing the read (`pipe_fd[0]`)
 * and write (`pipe_fd[1]`) ends of a pipe.
 * @return `EXIT_SUCCESS` (0) if all closures succeed, or `EXIT_FAILURE`
 * (1) if any `close()` fails.
 */
uint8_t	close_unused_fds(int in_fd, int *pipe_fd)
{
	if (in_fd != STDIN_FILENO && close(in_fd) == -1)
	{
		perror("-close_unused_fds: close in_fd");
		return (EXIT_FAILURE);
	}
	if (pipe_fd[0] != -1 && close(pipe_fd[0]) == -1)
	{
		perror("-close_unused_fds: close pipe_fd[0]");
		return (EXIT_FAILURE);
	}
	if (pipe_fd[1] != -1 && close(pipe_fd[1]) == -1)
	{
		perror("-close_unused_fds: close pipe_fd[1]");
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}
