/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error_utils_2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ilia Munaev <ilyamunaev@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 14:43:50 by Ilia Munaev       #+#    #+#             */
/*   Updated: 2025/05/10 16:07:16 by Ilia Munaev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file error_utils_2.c
 * @brief Utility functions for printing and handling errors with `perror()`
 * in Minishell.
 *
 * Provides helper functions to print system error messages using `perror()`
 * with custom prefixes,
 * and to exit or return with appropriate statuses in error conditions.
 */
#include "minishell.h"

/**
 * @brief Prints a formatted error message for an invalid `export` argument.
 *
 * This function constructs and prints a message like:
 * `minishell: export: \`KEY=VALUE\`: not a valid identifier`
 * using the provided key-value pair. If `value` is NULL, only the key is shown.
 *
 * On write failure, a fallback message is printed instead.
 *
 * @param pair A `t_mshell_var` structure containing the invalid export variable.
 */
void	export_error(t_mshell_var *pair)
{
	char				error_buf[ERROR_BUF_SIZE];
	static const char	*fallback_msg;

	fallback_msg = "minishell: error: failed to print error\n";
	error_buf[0] = '\0';
	ft_strlcpy(error_buf, "minishell: export: `", ERROR_BUF_SIZE);
	if (pair->key)
		ft_strlcat(error_buf, pair->key, ERROR_BUF_SIZE);
	if (pair->value)
	{
		ft_strlcat(error_buf, "=", ERROR_BUF_SIZE);
		ft_strlcat(error_buf, pair->value, ERROR_BUF_SIZE);
	}
	ft_strlcat(error_buf, "': not a valid identifier\n", ERROR_BUF_SIZE);
	if (write(STDERR_FILENO, error_buf, ft_strlen(error_buf)) < 0)
		write(STDERR_FILENO, fallback_msg, ft_strlen(fallback_msg));
}

/**
 * @brief Prints a string to STDERR.
 *
 * Uses `ft_putstr_fd` to write the given message directly to `STDERR_FILENO`.
 *
 * @param msg The string to print.
 */
void	print_error(const char *msg)
{
	ft_putstr_fd(msg, STDERR_FILENO);
}

/**
 * @brief Prints a system error message and returns a given exit status.
 *
 * This function builds a message like:
 * `minishell: <msg>: <strerror(errno)>`
 * and writes it to STDERR using a buffer. If writing fails,
 * a fallback message is printed instead.
 *
 * @param msg Optional context message (e.g., command or file name).
 * @param exit_status The exit status to return.
 * @return The same `exit_status` passed in.
 */
int	perror_return(const char *msg, int exit_status)
{
	char				error_buf[ERROR_BUF_SIZE];
	static const char	*fallback_msg;

	fallback_msg = "minishell: error: failed to print error\n";
	error_buf[0] = '\0';
	ft_strlcpy(error_buf, "-minishell: ", ERROR_BUF_SIZE);
	if (msg)
	{
		ft_strlcat(error_buf, msg, ERROR_BUF_SIZE);
		ft_strlcat(error_buf, ": ", ERROR_BUF_SIZE);
		ft_strlcat(error_buf, strerror(errno), ERROR_BUF_SIZE);
	}
	else
		ft_strlcat(error_buf, strerror(errno), ERROR_BUF_SIZE);
	ft_strlcat(error_buf, "\n", ERROR_BUF_SIZE);
	if (write(STDERR_FILENO, error_buf, ft_strlen(error_buf)) < 0)
		write(STDERR_FILENO, fallback_msg, ft_strlen(fallback_msg));
	return (exit_status);
}

/**
 * @brief Prints a system error message and exits the child process.
 *
 * This function is used in a child process to handle fatal errors.
 * It prints an error like:
 * `minishell: <msg>: <strerror(errno)>`
 * then frees the shell state and exits the process.
 *
 * @param cmd The current command context.
 * @param msg The context to print before the system error string.
 * @param exit_status The exit status to use for `_exit()`.
 * @return This function does not return.
 */
uint8_t	perror_exit_child(t_cmd *cmd, const char *msg, uint8_t exit_status)
{
	char				error_buf[ERROR_BUF_SIZE];
	t_cmd				*head;
	static const char	*fallback_msg;

	fallback_msg = "minishell: error: failed to print error\n";
	error_buf[0] = '\0';
	if (msg)
	{
		ft_strlcpy(error_buf, "minishell: ", ERROR_BUF_SIZE);
		ft_strlcat(error_buf, msg, ERROR_BUF_SIZE);
		ft_strlcat(error_buf, ": ", ERROR_BUF_SIZE);
		ft_strlcat(error_buf, strerror(errno), ERROR_BUF_SIZE);
		ft_strlcat(error_buf, "\n", ERROR_BUF_SIZE);
		if (write(STDERR_FILENO, error_buf, ft_strlen(error_buf)) < 0)
			write(STDERR_FILENO, fallback_msg, ft_strlen(fallback_msg));
	}
	if (cmd->minishell)
		free_minishell(&cmd->minishell);
	head = get_cmd_head(cmd);
	free_cmd(&head);
	_exit(exit_status);
}

/**
 * @brief Prints an error for a missing or invalid command structure.
 *
 * Typically used when `t_cmd` is NULL or lacks arguments (`argv[0]`).
 * Constructs and writes a message like:
 * `minishell: <msg>: no cmd or argv found`
 * then returns failure.
 *
 * @param msg Optional string to provide command context.
 * @return Always returns `EXIT_FAILURE` (1).
 */
uint8_t	no_cmd_error(const char *msg)
{
	char				error_buf[ERROR_BUF_SIZE];
	static const char	*fallback_msg;

	fallback_msg = "minishell: error: failed to print error\n";
	error_buf[0] = '\0';
	ft_strlcpy(error_buf, "minishell: ", ERROR_BUF_SIZE);
	if (msg)
		ft_strlcat(error_buf, msg, ERROR_BUF_SIZE);
	ft_strlcat(error_buf, ": no cmd or argv found\n", ERROR_BUF_SIZE);
	if (write(STDERR_FILENO, error_buf, ft_strlen(error_buf)) < 0)
		write(STDERR_FILENO, fallback_msg, ft_strlen(fallback_msg));
	return (EXIT_FAILURE);
}
