/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   child_execve_error.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ilia Munaev <ilyamunaev@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 14:43:26 by Ilia Munaev       #+#    #+#             */
/*   Updated: 2025/05/06 08:51:52 by Ilia Munaev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
 * @file child_execve_error.c
 * @brief Utilities for handling execve-related errors in child processes.
 *
 * Provides structured error reporting and graceful exiting for failed execve
 * calls, including memory cleanup and exit status handling.
 */
#include "minishell.h"

/**
 * @brief Builds an error message string into a buffer.
 *
 * This function concatenates a prefix, argument, and message (if provided)
 * into a single buffer. Used for building a complete error
 * message before output.
 *
 * @param buf The target buffer to store the message.
 * @param prefix The prefix string (e.g., "minishell: ").
 * @param arg The argument string (e.g., the command or filename).
 * @param msg The message string (e.g., ": command not found\n").
 */
static void	build_error_message(char *buf,
	const char *prefix,
	const char *arg,
	const char *msg)
{
	buf[0] = '\0';
	if (prefix)
		ft_strlcpy(buf, prefix, ERROR_BUF_SIZE);
	if (arg)
		ft_strlcat(buf, arg, ERROR_BUF_SIZE);
	if (msg)
		ft_strlcat(buf, msg, ERROR_BUF_SIZE);
}

/**
 * @brief Writes an error message buffer to STDERR.
 *
 * Attempts to write a constructed error message to standard error. If the
 * write fails, prints a fallback internal error.
 *
 * @param buf The buffer containing the error message to print.
 */
static void	write_error_buffer(const char *buf)
{
	static const char	*msg;

	msg = "-minishell: failed to print error\n";
	if (buf[0] != '\0')
	{
		if (write(STDERR_FILENO, buf, ft_strlen(buf)) < 0)
			write(STDERR_FILENO, msg, ft_strlen(msg));
	}
}

/**
 * @brief Builds and prints an error message, frees resources, and exits.
 *
 * Constructs an error message from an optional prefix, argument, and message,
 * then writes it to STDERR. Frees the `minishell` context, command list, and
 * optionally the full command head before exiting the process.
 *
 * @param info A `t_exit_info` structure containing the message parts, resources
 *             to free, and the exit code.
 */
void	print_and_exit(t_exit_info info)
{
	char	error_buf[ERROR_BUF_SIZE];
	t_cmd	*head;

	build_error_message(error_buf, info.prefix, info.arg, info.msg);
	write_error_buffer(error_buf);
	if (info.mshell)
		free_minishell(&info.mshell);
	if (info.cmd)
		free_cmd(&info.cmd);
	if (info.origin_head)
	{
		head = get_cmd_head(info.cmd);
		if (head && head != info.cmd)
			free_cmd(&head);
	}
	_exit(info.code);
}

/**
 * @brief Handles execve-related errors in a child process.
 *
 * This function delegates to specific error handlers depending on the
 * nature of the `execve` failure, such as:
 * - Directory execution attempts
 * - Permission denied
 * - Command not found
 * - Format errors
 * - Other generic failures
 *
 * This function does not return; one of the handlers will call `_exit()`.
 *
 * @param cmd The command structure causing the error.
 */
void	child_execve_error(t_cmd *cmd)
{
	handle_is_directory(cmd);
	handle_not_found_or_command(cmd);
	handle_permission_denied(cmd);
	handle_exec_format_error(cmd);
	handle_generic_execve_error(cmd);
}
