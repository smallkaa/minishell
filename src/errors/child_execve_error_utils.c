/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   child_execve_error_utils.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ilia Munaev <ilyamunaev@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 14:43:16 by Ilia Munaev       #+#    #+#             */
/*   Updated: 2025/05/05 22:41:42 by Ilia Munaev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file child_execve_error.c
 * @brief Error handlers for execve failures in child processes.
 *
 * Provides functions to handle various `execve`-related errors
 * such as permission denied, invalid format, or trying to execute a directory.
 */
#include "minishell.h"

/**
 * @brief Handles the case when a directory is executed instead of a file.
 *
 * Checks if `cmd->binary` refers to a directory using `stat()`.
 * If it is a directory,
 * prints an error message:
 * ```
 * minishell: <binary>: Is a directory
 * ```
 * then frees resources and exits with status code `126`.
 *
 * @param cmd The command structure containing the binary path and shell context.
 */
void	handle_is_directory(t_cmd *cmd)
{
	struct stat	st;

	if (stat(cmd->binary, &st) == 0 && S_ISDIR(st.st_mode))
	{
		print_and_exit((t_exit_info){
			.prefix = "-minishell: ",
			.arg = cmd->binary,
			.msg = ": Is a directory\n",
			.code = 126,
			.mshell = cmd->minishell,
			.origin_head = cmd->origin_head,
			.cmd = cmd});
	}
}

/**
 * @brief Handles "command not found" or "no such file or directory" scenarios.
 *
 * If `errno` is `ENOENT`, this function prints an error message based on whether
 * the command has a slash or whether `PATH` is defined.
 * It then frees resources and exits.
 *
 * Format:
 * - With `/` or missing PATH:
 *   ```
 *   minishell: <cmd>: No such file or directory
 *   ```
 * - Otherwise:
 *   ```
 *   minishell: <cmd>: command not found
 *   ```
 *
 * Exits with status `127` (command not found).
 *
 * @param cmd The command structure containing argv, shell context, and binary.
 */
void	handle_not_found_or_command(t_cmd *cmd)
{
	char				error_buf[ERROR_BUF_SIZE];
	char				*path;
	t_cmd				*head;
	static const char	*fallback_msg;

	fallback_msg = "minishell: failed to print error\n";
	if (errno != ENOENT)
		return ;
	ft_strlcpy(error_buf, "minishell: ", ERROR_BUF_SIZE);
	ft_strlcat(error_buf, cmd->argv[0], ERROR_BUF_SIZE);
	path = ms_getenv(cmd->minishell, "PATH");
	if (ft_strchr(cmd->argv[0], '/') || !path || path[0] == '\0')
		ft_strlcat(error_buf, ": No such file or directory\n", ERROR_BUF_SIZE);
	else
		ft_strlcat(error_buf, ": command not found\n", ERROR_BUF_SIZE);
	if (write(STDERR_FILENO, error_buf, ft_strlen(error_buf)) < 0)
		write(STDERR_FILENO, fallback_msg, ft_strlen(fallback_msg));
	free_minishell(&cmd->minishell);
	head = get_cmd_head(cmd);
	free_cmd(&head);
	_exit(127);
}

/**
 * @brief Handles execution permission errors (`EACCES`) from `execve()`.
 *
 * If `errno` is `EACCES`, this function prints a permission denied message:
 * ```
 * minishell: <binary>: Permission denied
 * ```
 * then frees all resources and exits with status `126`.
 *
 * @param cmd The command structure containing the binary and execution context.
 */
void	handle_permission_denied(t_cmd *cmd)
{
	if (errno == EACCES)
	{
		print_and_exit((t_exit_info){
			.prefix = "-minishell: ",
			.arg = cmd->binary,
			.msg = ": Permission denied\n",
			.code = 126,
			.mshell = cmd->minishell,
			.origin_head = cmd->origin_head,
			.cmd = cmd});
	}
}

/**
 * @brief Handles `ENOEXEC` errors when executing a binary.
 *
 * If `errno` is `ENOEXEC`, this function prints:
 * ```
 * minishell: <binary>: Exec format error
 * ```
 * then cleans up and exits with status code `126`.
 *
 * @param cmd The command that triggered the error.
 */
void	handle_exec_format_error(t_cmd *cmd)
{
	if (errno == ENOEXEC)
	{
		print_and_exit((t_exit_info){
			.prefix = "-minishell: ",
			.arg = cmd->binary,
			.msg = ": Exec format error\n",
			.code = 126,
			.mshell = cmd->minishell,
			.origin_head = cmd->origin_head,
			.cmd = cmd});
	}
}

/**
 * @brief Handles any `execve()` error not covered by specific handlers.
 *
 * Prints a general error using `strerror(errno)` and exits with `EXIT_FAILURE`.
 * Used as a final fallback when no other error condition matches.
 *
 * Format:
 * ```
 * minishell: execve: <strerror message>
 * ```
 *
 * Frees shell context and command list before exiting.
 *
 * @param cmd The command structure that caused the error.
 */
void	handle_generic_execve_error(t_cmd *cmd)
{
	char				error_buf[ERROR_BUF_SIZE];
	t_cmd				*head;
	static const char	*fallback_msg;

	fallback_msg = "minishell: failed to print error\n";
	error_buf[0] = '\0';
	ft_strlcpy(error_buf, "minishell: execve: ", ERROR_BUF_SIZE);
	ft_strlcat(error_buf, strerror(errno), ERROR_BUF_SIZE);
	ft_strlcat(error_buf, "\n", ERROR_BUF_SIZE);
	if (write(STDERR_FILENO, error_buf, ft_strlen(error_buf)) < 0)
		write(STDERR_FILENO, fallback_msg, ft_strlen(fallback_msg));
	free_minishell(&cmd->minishell);
	head = get_cmd_head(cmd);
	free_cmd(&head);
	_exit(EXIT_FAILURE);
}
