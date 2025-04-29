/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_in_current_process.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ilia Munaev <ilyamunaev@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 14:49:23 by Ilia Munaev       #+#    #+#             */
/*   Updated: 2025/04/29 23:25:09 by Ilia Munaev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file exec_in_current_process.c
 * @brief Executes built-in commands in the current process context.
 */
#include "minishell.h"

/**
 * @brief Creates a backup of the specified file descriptor.
 *
 * Duplicates the given file descriptor using `dup()`
 * to allow restoring it later.
 *
 * @param fd The file descriptor to back up.
 * @return A duplicate of the file descriptor, or -1 on failure.
 */
static int	backup_fd(int fd)
{
	int	backup;

	backup = dup(fd);
	if (backup == -1)
		return (perror_return("backup_fd: dup", -1));
	return (backup);
}

/**
 * @brief Restores a file descriptor from a saved backup.
 *
 * Uses `dup2()` to copy the saved FD back to the original location.
 * Also closes the backup FD afterward.
 *
 * @param saved_fd The backup file descriptor.
 * @param target_fd The FD to restore.
 * @return EXIT_SUCCESS on success, EXIT_FAILURE on failure.
 */
static uint8_t	restore_fd(int saved_fd, int target_fd)
{
	if (dup2(saved_fd, target_fd) == -1)
	{
		safe_close(&saved_fd);
		return (perror_return("restore_fd: dup2", EXIT_FAILURE));
	}
	safe_close(&saved_fd);
	return (EXIT_SUCCESS);
}

/**
 * @brief Applies redirections and executes a built-in command.
 *
 * This function first applies any redirections from the command.
 * If successful, it executes the command as a built-in.
 *
 * @param cmd The command structure.
 * @return The exit status of the redirection or built-in execution.
 */
static uint8_t	redirect_and_execute(t_cmd *cmd)
{
	// uint8_t	exit_status;

	apply_redirections(cmd);
	// exit_status = apply_redirections(cmd);
	// if (exit_status != EXIT_SUCCESS)
	// 	return (exit_status);
	return (exec_builtins(cmd));
}

/**
 * @brief Executes a built-in command in the current (parent) process.
 *
 * Saves `stdin` and `stdout` FDs, applies redirections,
 * runs the built-in, and then restores the original FDs.
 *
 * Used for commands like `cd`, `export`, and `unset` that affect shell state.
 *
 * @param cmd The command to execute.
 * @return The exit status of the executed command.
 */
uint8_t	exec_in_current_process(t_cmd *cmd)
{
	int			saved_stdout;
	int			saved_stdin;
	uint8_t		exit_status;

	saved_stdin = backup_fd(STDIN_FILENO);
	saved_stdout = backup_fd(STDOUT_FILENO);
	if (saved_stdin == -1 || saved_stdout == -1)
		return (EXIT_FAILURE);
	exit_status = redirect_and_execute(cmd);
	if (restore_fd(saved_stdout, STDOUT_FILENO) != EXIT_SUCCESS
		|| restore_fd(saved_stdin, STDIN_FILENO) != EXIT_SUCCESS)
		return (EXIT_FAILURE);
	return (exit_status);
}
