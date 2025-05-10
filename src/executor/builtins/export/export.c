/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ilia Munaev <ilyamunaev@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 14:45:42 by Ilia Munaev       #+#    #+#             */
/*   Updated: 2025/05/10 03:26:18 by Ilia Munaev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file handle_export.c
 * @brief Implementation of the `export` built-in command in Minishell.
 *
 * This file defines the logic for handling the `export` command, which allows
 * users to create, update, or list environment variables. The implementation
 * validates variable names, parses key-value pairs, and supports
 * Bash-compatible behavior, including error handling and formatted output.
 */
#include "minishell.h"

/**
 * @brief Validates and prepares a variable pair for export.
 *
 * Verifies the input and splits it into key-value. Also validates
 * that the key conforms to shell naming rules.
 *
 * @param arg The export argument string.
 * @return Pointer to t_mshell_var on success, NULL on error.
 */
static t_mshell_var	*validate_export_pair(char *arg)
{
	t_mshell_var	*pair;

	if (!arg)
	{
		print_error("-minishell: process_export_arg: no arg");
		return (NULL);
	}
	pair = split_key_value(arg);
	if (!pair)
	{
		print_error("-minishell: export: memory allocation failed\n");
		return (NULL);
	}
	if (!is_valid_varname(pair->key))
	{
		export_error(pair);
		free(pair->key);
		free(pair->value);
		free(pair);
		return (NULL);
	}
	return (pair);
}

/**
 * @brief Processes a single argument for the `export` command.
 *
 * Sets the validated variable in the shell environment, determining
 * whether it is assigned or declared. Handles memory cleanup.
 *
 * @param cmd Pointer to the command structure.
 * @param arg Argument string containing the variable.
 * @return EXIT_SUCCESS or EXIT_FAILURE.
 */
static uint8_t	process_export_arg(t_cmd *cmd, char *arg)
{
	t_mshell_var	*pair;
	int				assigned;

	pair = validate_export_pair(arg);
	if (!pair)
		return (EXIT_FAILURE);
	assigned = (ft_strchr(arg, '=') != NULL || !pair->value);
	if (set_variable(cmd->minishell, pair->key,
			pair->value, assigned) != EXIT_SUCCESS)
	{
		free(pair->key);
		free(pair->value);
		free(pair);
		return (error_return("export: set_variable failed", EXIT_FAILURE));
	}
	free(pair->key);
	free(pair->value);
	free(pair);
	return (EXIT_SUCCESS);
}

/**
 * @brief Prints a usage error message for invalid export options.
 *
 * Mimics the format of Bash's error messages when invalid flags are passed
 * to `export`. This function is triggered when the argument begins with `-`,
 * which is not supported in this implementation.
 *
 * @param arg The invalid option string passed to `export`.
 */
static void	print_export_usage(char *arg)
{
	print_error("minishell: export: ");
	print_error(arg);
	print_error(": invalid option\n"\
"export: usage: export [name[=value] ...]\n");
}

/**
 * @brief Checks if the given argument is an invalid option for `export`.
 *
 * Minishell's `export` does not support any command-line options
 * (e.g., `-f`, `-p`).
 * Therefore, any argument starting with `-` (and not empty) is treated as
 * invalid.
 *
 * @param arg The argument string to evaluate.
 * @return `true` if the argument is an invalid option, `false` otherwise.
 */
static bool	is_invalid_option(const char *arg)
{
	if (!arg)
		return (false);
	if (arg[0] == '-' && arg[1] != '\0')
		return (true);
	return (false);
}

/**
 * @brief Handles the execution of the `export` built-in command.
 *
 * Behavior:
 * - If called with no arguments, prints all exported variables in sorted order.
 * - If any argument starts with `-`, prints usage and exits with status `2`.
 * - If an argument contains an invalid variable name, prints an error and
 *   returns `1`, but continues processing other arguments.
 * - If all variables are valid and assigned, updates the environment.
 *
 * @param cmd Pointer to the command structure.
 * @return `0` if all arguments are valid, `1` if any invalid identifier
 * was encountered, `2` if an invalid option was used.
 */
uint8_t	handle_export(t_cmd *cmd)
{
	uint8_t	exit_status;
	int		i;

	if (!cmd)
		return (no_cmd_error("export"));
	exit_status = EXIT_SUCCESS;
	if (!cmd->argv[1])
		return (handle_sorted_env(cmd->minishell));
	i = 1;
	while (cmd->argv[i])
	{
		if (is_invalid_option(cmd->argv[i]))
		{
			print_export_usage(cmd->argv[i]);
			return (2);
		}
		exit_status = process_export_arg(cmd, cmd->argv[i]);
		i++;
	}
	if (update_env(cmd->minishell) != EXIT_SUCCESS)
		perror_return("handle_export, update_env failed\n", EXIT_FAILURE);
	return (exit_status);
}
