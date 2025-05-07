/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error_utils_1.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ilia Munaev <ilyamunaev@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 14:43:42 by Ilia Munaev       #+#    #+#             */
/*   Updated: 2025/05/07 18:54:21 by Ilia Munaev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file error_utils_1.c
 * @brief Error reporting utilities for built-in and execution errors
 * in Minishell.
 *
 * Contains helper functions for printing error messages to stderr in
 * a consistent format.
 * These are used across built-in commands and execve-related failures.
 */
#include "minishell.h"

/**
 * @brief Prints an error message for an invalid `unset` option.
 *
 * This function is called when an illegal option
 * (e.g., `-x`) is passed to `unset`.
 * The message format is:
 * ```
 * minishell: unset: <str>: invalid option
 * unset: usage: unset [name ...]
 * ```
 *
 * If writing to `STDERR` fails, a fallback message is printed instead.
 *
 * @param str The invalid option string (e.g., "-x").
 * @return Always returns `2`, indicating invalid usage.
 */
uint8_t	unset_error(const char *str)
{
	char				error_buf[ERROR_BUF_SIZE];
	static const char	*fallback_msg;

	fallback_msg = "minishell: error: failed to print error\n";
	error_buf[0] = '\0';
	ft_strlcpy(error_buf, "minishell: unset: ", ERROR_BUF_SIZE);
	if (str)
	{
		ft_strlcat(error_buf, str, ERROR_BUF_SIZE);
		ft_strlcat(error_buf, ": invalid option\n", ERROR_BUF_SIZE);
	}
	ft_strlcat(error_buf, "unset: usage: unset [name ...]\n", ERROR_BUF_SIZE);
	if (write(STDERR_FILENO, error_buf, ft_strlen(error_buf)) < 0)
		write(STDERR_FILENO, fallback_msg, ft_strlen(fallback_msg));
	return (2);
}

/**
 * @brief Writes an error buffer to STDERR or falls back on failure.
 *
 * This function attempts to write the provided error buffer to `STDERR_FILENO`.
 * If the write fails, it prints a fallback error message.
 *
 * @param buf The error message buffer to write.
 */
static void	write_error_or_fallback(const char *buf)
{
	static const char	*fallback_msg;

	fallback_msg = "minishell: error: failed to print error\n";
	if (write(STDERR_FILENO, buf, ft_strlen(buf)) < 0)
		write(STDERR_FILENO, fallback_msg, ft_strlen(fallback_msg));
}

/**
 * @brief Constructs an error message for a missing or unknown command.
 *
 * Builds an error message like:
 * ```
 * minishell: <cmd>: command not found
 * ```
 * or, if the command contains a `/` or `PATH` is unset/empty:
 * ```
 * minishell: <cmd>: No such file or directory
 * ```
 *
 * @param buf The buffer to populate with the error message.
 * @param cmd The command structure containing `argv[0]`.
 */
static void	build_missing_command_error(char *buf, t_cmd *cmd)
{
	const char	*path;

	buf[0] = '\0';
	ft_strlcpy(buf, "minishell: ", ERROR_BUF_SIZE);
	ft_strlcat(buf, cmd->argv[0], ERROR_BUF_SIZE);
	path = ms_getenv(cmd->minishell, "PATH");
	if (ft_strchr(cmd->argv[0], '/') || !path || path[0] == '\0')
		ft_strlcat(buf, ": No such file or directory\n", ERROR_BUF_SIZE);
	else
		ft_strlcat(buf, ": command not found\n", ERROR_BUF_SIZE);
}

/**
 * @brief Handles and prints an error for a missing or invalid command.
 *
 * This function checks if the command structure or its arguments are invalid.
 * If so, it prints an appropriate error message and
 * exits the process with status `127`.
 *
 * - If `cmd` or `cmd->argv[0]` is NULL, prints "minishell: invalid cmd".
 * - Otherwise, checks for missing PATH, slashes, etc., and prints either
 *   "No such file or directory" or "command not found".
 *
 * Frees the shell context and command list before exiting.
 *
 * @param cmd The command structure to inspect.
 */
void	cmd_missing_command_error(t_cmd *cmd)
{
	char	error_buf[ERROR_BUF_SIZE];
	t_cmd	*head;

	head = get_cmd_head(cmd);
	if (!cmd || !cmd->argv || !cmd->argv[0])
	{
		ft_strlcpy(error_buf, "minishell: invalid cmd\n", ERROR_BUF_SIZE);
		write_error_or_fallback(error_buf);
		free_minishell(&cmd->minishell);
		free_cmd(&head);
		_exit(127);
	}
	build_missing_command_error(error_buf, cmd);
	write_error_or_fallback(error_buf);
	free_minishell(&cmd->minishell);
	free_cmd(&head);
	_exit(127);
}

/**
 * @brief Prints an error message and returns the given exit status.
 *
 * Constructs an error in the format:
 * ```
 * minishell: <msg>
 * ```
 * and prints it to `STDERR_FILENO`. If the write fails,
 * prints a fallback error message.
 *
 * @param msg The error message to print (may be NULL).
 * @param exit_status The exit status to return.
 * @return The same `exit_status` passed in.
 */
int	error_return(const char *msg, int exit_status)
{
	char				error_buf[ERROR_BUF_SIZE];
	static const char	*fallback_msg;

	fallback_msg = "minishell: error: failed to print error\n";
	error_buf[0] = '\0';
	ft_strlcpy(error_buf, "minishell: ", ERROR_BUF_SIZE);
	if (msg)
		ft_strlcat(error_buf, msg, ERROR_BUF_SIZE);
	if (write(STDERR_FILENO, error_buf, ft_strlen(error_buf)) < 0)
		write(STDERR_FILENO, fallback_msg, ft_strlen(fallback_msg));
	return (exit_status);
}
