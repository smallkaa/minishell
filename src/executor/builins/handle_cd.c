/**
 * @file handle_cd.c
 * @brief Implementation of the `cd` built-in command in Minishell.
 */
#include "minishell.h"

/**
 * @brief Handles the case where `cd` is called with no arguments.
 *
 * This function attempts to change to the directory specified by `$HOME`.
 * If `$HOME` is unset or `chdir()` fails, an error is printed.
 *
 * @param cmd Pointer to the command structure.
 * @return `EXIT_SUCCESS` if successful, `EXIT_FAILURE` otherwise.
 */
static uint8_t	cd_no_args(t_cmd *cmd)
{
	char	*home_path;
	uint8_t	exit_status;

	exit_status = EXIT_FAILURE;
	home_path = getenv("HOME");
	if (!home_path)
	{
		print_error("minishell: cd: HOME not set\n");
		return (exit_status);
	}
	if (chdir(home_path) != 0)
	{
		cmd_error_handler(cmd);
		return (exit_status);
	}
	return (EXIT_SUCCESS);
}

/**
 * @brief Handles the case where `cd` is called with more than one argument.
 *
 * This function prints an error indicating that too many
 * arguments were supplied.
 *
 * @return Always `EXIT_FAILURE`.
 */
static uint8_t	cd_too_many_args(void)
{
	print_error("minishell: cd: too many arguments\n");
	return (EXIT_FAILURE);
}

/**
 * @brief Handles the case where `cd` is called with a single argument.
 *
 * Attempts to change the current working directory to the path specified
 * by `cmd->argv[1]`. If `chdir()` fails, it prints an error.
 *
 * @param cmd Pointer to the command structure.
 * @return `EXIT_SUCCESS` if directory change is successful, otherwise
 *         `EXIT_FAILURE`.
 */
static uint8_t	cd_one_arg(t_cmd *cmd)
{
	uint8_t	exit_status;

	exit_status = EXIT_FAILURE;
	if (chdir(cmd->argv[1]) != 0)
	{
		cmd_error_handler(cmd);
		return (exit_status);
	}
	return (EXIT_SUCCESS);
}

/**
 * @brief Main function to handle the `cd` built-in command.
 *
 * Depending on the number of arguments, it calls one of the helper functions:
 * - `cd_no_args()` if no arguments are provided.
 * - `cd_too_many_args()` if more than one argument is provided.
 * - `cd_one_arg()` if exactly one argument is provided.
 *
 * @param cmd Pointer to the command structure.
 * @return An exit status indicating success or failure.
 */
uint8_t	handle_cd(t_cmd *cmd)
{
	uint8_t	exit_status;

	exit_status = EXIT_FAILURE;
	if (!cmd)
	{
		print_error("Error: no *cmd\n");
		if (cmd->in_pipe)
			exit(exit_status);
		return exit_status;
	}
	if (!cmd->argv[1])
		exit_status = cd_no_args(cmd);
	else if (cmd->argv[2])
		exit_status = cd_too_many_args();
	else
		exit_status = cd_one_arg(cmd);
	cmd->minishell->exit_status = exit_status;
	if (cmd->in_pipe)
		exit(exit_status);
	return (exit_status);
}
