/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imunaev- <imunaev-@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 14:44:51 by Ilia Munaev       #+#    #+#             */
/*   Updated: 2025/05/05 16:44:23 by imunaev-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file echo.c
 * @brief Implementation of the `echo` built-in command for Minishell.
 *
 * This file provides the full logic for handling the `echo` command, including
 * support for the `-n` flag (no trailing newline), flag parsing, and
 * argument printing.
 * The implementation mimics the behavior of the Bash `echo` built-in.
 */
#include "minishell.h"

/**
 * @brief Writes a string to standard output using `write()`.
 *
 * A wrapper around `write()` that prints the given string to STDOUT
 * (file descriptor 1).
 * If writing fails, an error message is printed, and failure is returned.
 *
 * @param str The null-terminated string to print.
 * @return `EXIT_SUCCESS` (0) on success, `EXIT_FAILURE` (1) on failure.
 */
static int	ft_putstr_custom(t_cmd *cmd, char *str)
{
	ssize_t	written;
	t_cmd	*head;

	if (!str)
		return (EXIT_FAILURE);
	head = get_cmd_head(cmd);
	written = write(STDOUT_FILENO, str, ft_strlen(str));
	
	//fprintf(stderr, "[DEBUG] ft_putstr_custom written BEFORE: %ld errno=%d pid=%d\n", written, errno, getpid());

	if (written == -1 && errno == EPIPE)
	{
		//fprintf(stderr, "[DEBUG] ft_putstr_custom written: %ld\n", written);
		free_minishell(&cmd->minishell);
		free_cmd(&head);
		return (perror_return("ft_putstr_custom: write", EXIT_FAILURE));
	}
	return (EXIT_SUCCESS);
}

/**
 * @brief Prints the actual content (arguments) of the `echo` command.
 *
 * Iterates through the argument list and prints each token, separated by
 * a space.
 * If the `-n` flag was not set, a newline is printed at the end.
 * If no arguments are present after the command, a newline is printed by
 * default.
 *
 * @param cmd Pointer to the current command structure.
 * @param i Index in `cmd->argv[]` to start printing from.
 * @param newline_flag Pointer to an int indicating whether to print
 * a newline (1 = yes).
 * @return `EXIT_SUCCESS` on successful printing, `EXIT_FAILURE` on write error.
 */
static uint8_t	print_content(t_cmd *cmd, int i, int *newline_flag)
{
	if (!cmd->argv[1])
		return (ft_putstr_custom(cmd, "\n"));
	while (cmd->argv[i])
	{
		if (!cmd->argv[i])
			return (error_return("print_content: failed\n", EXIT_FAILURE));
		if (ft_putstr_custom(cmd, cmd->argv[i]) == EXIT_FAILURE)
			return (EXIT_FAILURE);
		if (cmd->argv[i + 1] && ft_putstr_custom(cmd, " ") == EXIT_FAILURE)
			return (EXIT_FAILURE);
		i++;
	}
	if (*newline_flag && ft_putstr_custom(cmd, "\n") == EXIT_FAILURE)
		return (EXIT_FAILURE);
	return (EXIT_SUCCESS);
}

/**
 * @brief Parses `-n` flags for the `echo` command.
 *
 * Scans and validates consecutive `-n` flags (e.g., `-n`, `-nnn`, etc.)
 * and disables
 * the newline if any are found. Flags wrapped in quotes (e.g., `"-n"`)
 * are not treated
 * as valid options.
 *
 * @param cmd Pointer to the current command structure.
 * @param newline_flag Pointer to an int that will be set to 0 if `-n` is found.
 * @return Index from which to start printing actual content.
 */
static int	handle_echo_flags(t_cmd *cmd, int *newline_flag)
{
	int	i;

	i = 1;
	while (cmd->argv[i]
		&& is_echo_flag(cmd->argv[i])
		&& !is_wrapped_in_quotes(cmd->argv[i]))
	{
		*newline_flag = 0;
		i++;
	}
	return (i);
}

/**
 * @brief Executes the `echo` built-in command.
 *
 * Handles parsing of options and printing of arguments for the `echo` command.
 * Supports the `-n` flag to suppress the trailing newline.
 * Delegates actual printing
 * and flag handling to helper functions.
 *
 * Behavior Summary:
 * - With `-n` flag: suppress newline.
 * - Without arguments: print a newline only.
 * - Otherwise: print arguments separated by spaces, ending with a newline.
 *
 * @param cmd Pointer to the command structure containing the input arguments.
 * @return `EXIT_SUCCESS` (0) on success, `EXIT_FAILURE` (1) on write failure.
 */
uint8_t	handle_echo(t_cmd *cmd)
{
	int		i;
	uint8_t	exit_status;
	int		newline_flag;

	if (!cmd)
		return (no_cmd_error("echo"));
	newline_flag = 1;
	i = handle_echo_flags(cmd, &newline_flag);
	exit_status = print_content(cmd, i, &newline_flag);
	return (exit_status);
}
