/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ilia Munaev <ilyamunaev@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 14:45:24 by Ilia Munaev       #+#    #+#             */
/*   Updated: 2025/05/10 03:44:37 by Ilia Munaev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file exit.c
 * @brief Implementation of the `exit` built-in command for Minishell.
 *
 * This file provides the full logic for handling the `exit` command,
 * including numeric argument validation, overflow detection, and error handling.
 * The behavior follows POSIX and Bash conventions for built-in exit behavior.
 */
#include "minishell.h"

/**
 * @brief Prints an error for a non-numeric exit argument.
 *
 * Formats the error as:
 *   minishell: exit: <arg>: numeric argument required
 *
 * @param arg The invalid argument passed to `exit`.
 * @return Always returns `2`, the standard Bash exit code for invalid
 * numeric input.
 */
static uint8_t	exit_numeric_error(char *arg)
{
	print_error("-minishell: exit: ");
	print_error(arg);
	print_error(": numeric argument required\n");
	return (2);
}

/**
 * @brief Validates whether an argument is a proper numeric exit code.
 *
 * Accepts strings that contain only digits, optionally prefixed with '+' or '-'.
 * Rejects any string with non-digit characters after the prefix.
 *
 * @param arg The argument string to validate.
 * @return `true` if the argument is numeric, `false` otherwise.
 */
bool	is_valid_numeric_exit_arg(const char *arg)
{
	if (!arg || *arg == '\0')
		return (false);
	if (*arg == '-' || *arg == '+')
		arg++;
	if (!ft_isdigit(*arg))
		return (false);
	while (*arg)
	{
		if (!ft_isdigit(*arg))
			return (false);
		arg++;
	}
	return (true);
}

/**
 * @brief Handles the case when `exit` is passed too many arguments.
 *
 * Mimics Bash behavior: prints an error and returns `1` without exiting
 * the shell.
 *
 * @return `EXIT_FAILURE` (1), indicating improper usage.
 */
static uint8_t	handle_too_many_args(void)
{
	return (error_return("exit: too many arguments\n", EXIT_FAILURE));
}

/**
 * @brief Parses and returns the final exit status from a valid numeric string.
 *
 * Uses a safe conversion function that detects overflow. If overflow or
 * out-of-range
 * values are detected, it prints an error and returns status `2`.
 *
 * The return value is masked to 8 bits to simulate real shell exit behavior.
 *
 * @param arg The numeric string representing the exit code.
 * @return Exit status in the range [0, 255], or `2` on invalid numeric
 * conversion.
 */
uint8_t	get_exit_status(char *arg)
{
	long long	exit_status;
	bool		overflow;

	exit_status = ft_atoll_exit(arg, &overflow);
	if (overflow || exit_status > LLONG_MAX || exit_status < LLONG_MIN)
		return (exit_numeric_error(arg));
	return ((uint8_t)(exit_status & 0xFF));
}

/**
 * @brief Handles the `exit` built-in command logic.
 *
 * Behavior:
 * - No arguments → exits with the last command's status.
 * - One argument → if numeric, exits with that status; if not, prints an error.
 * - Two or more arguments → prints a usage error and does not exit.
 *
 * This function does not call `exit()` directly here; instead, the caller is
 * expected to handle cleanup and exit with the returned status.
 *
 * @param cmd The command structure containing arguments and shell state.
 * @return Exit code to be passed to cleanup and termination logic.
 */
uint8_t	handle_exit(t_cmd *cmd)
{
	bool	overflow;

	overflow = false;
	if (!cmd)
		return (no_cmd_error("exit"));
	if (!cmd->argv[1])
		return (cmd->minishell->exit_status);
	if (cmd->argv[1][0] == '\0')
		return (exit_numeric_error(cmd->argv[1]));
	if (!is_valid_numeric_exit_arg(cmd->argv[1]))
		return (exit_numeric_error(cmd->argv[1]));
	ft_atoll_exit(cmd->argv[1], &overflow);
	if (cmd->argv[2] && overflow)
		return (exit_numeric_error(cmd->argv[1]));
	if (cmd->argv[2])
		return (handle_too_many_args());
	return (get_exit_status(cmd->argv[1]));
}
