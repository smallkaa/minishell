/**
 * @file handle_cd.c
 * @brief Implementation of the `cd` built-in command in Minishell.
 */
#include "minishell.h"

/**
 * @brief Handles the case where `cd` is called without arguments.
 *
 * If no arguments are given, this function attempts to change the working
 * directory to `$HOME`. If `$HOME` is not set or `chdir()` fails, an
 * appropriate error message is printed. After a successful directory change,
 * it updates the `PWD` variable.
 *
 * @param cmd Pointer to the command structure.
 * @return `EXIT_SUCCESS` if successful, `EXIT_FAILURE` otherwise.
 */
static uint8_t	cd_no_args(t_cmd *cmd)
{
	char	*home_path;
	char	*new_home;
	char	buf[PATH_MAX];
	uint8_t	exit_status;

	exit_status = EXIT_FAILURE;
	home_path = ms_getenv(cmd->minishell, "HOME");
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
	new_home = getcwd(buf, PATH_MAX);
	if (!new_home)
	{
		print_error("minishell: cd: failed to retrieve current directory\n");
		return (EXIT_FAILURE);
	}
	set_variable(cmd->minishell, "PWD", new_home, 1);
	return (EXIT_SUCCESS);
}

/**
 * @brief Handles the case where `cd` is called with more than one argument.
 *
 * The `cd` command only supports zero or one argument. If more than one
 * argument is supplied, this function prints an error message and returns
 * `EXIT_FAILURE`.
 *
 * @return Always `EXIT_FAILURE`.
 */
static uint8_t	cd_too_many_args(void)
{
	print_error("minishell: cd: too many arguments\n");
	return (EXIT_FAILURE);
}

/**
 * @brief Changes the current directory to the specified path and updates `PWD`.
 *
 * This function attempts to change the directory to `cmd->argv[1]`. If `chdir()`
 * fails, an error message is printed. If successful, it updates the `PWD`
 * environment variable using `set_variable()`.
 *
 * @param cmd Pointer to the command structure.
 * @return `EXIT_SUCCESS` if successful, `EXIT_FAILURE` otherwise.
 */
static uint8_t	change_and_update_pwd(t_cmd *cmd)
{
	char	buf[PATH_MAX];
	char	*new_pwd;

	if (chdir(cmd->argv[1]) != 0)
	{
		cmd_error_handler(cmd);
		return (EXIT_FAILURE);
	}
	new_pwd = getcwd(buf, PATH_MAX);
	if (!new_pwd)
	{
		print_error("minishell: cd: failed to retrieve current directory\n");
		return (EXIT_FAILURE);
	}
	set_variable(cmd->minishell, "PWD", new_pwd, 1);
	return (EXIT_SUCCESS);
}

/**
 * @brief Handles the case where `cd` is called with a single argument.
 *
 * Calls `change_and_update_pwd()` to change the directory and update `PWD`.
 * If the directory change is successful, the shell's environment variables
 * are synchronized using `update_env()`.
 *
 * @param cmd Pointer to the command structure.
 * @return `EXIT_SUCCESS` if successful, otherwise `EXIT_FAILURE`.
 */
static uint8_t	cd_one_arg(t_cmd *cmd)
{
	if (!cmd || !cmd->minishell)
	{
		print_error("minishell: cd: invalid command structure\n");
		return (EXIT_FAILURE);
	}
	if (change_and_update_pwd(cmd) == EXIT_FAILURE)
		return (EXIT_FAILURE);
	update_env(cmd->minishell);
	return (EXIT_SUCCESS);
}

/**
 * @brief Main function to handle the `cd` built-in command.
 *
 * Determines the number of arguments provided and calls the corresponding
 * helper function:
 * - `cd_no_args()` if no arguments are provided.
 * - `cd_too_many_args()` if more than one argument is provided.
 * - `cd_one_arg()` if exactly one argument is provided.
 *
 * The exit status of the command is stored in `cmd->minishell->exit_status`.
 * If `cd` is executed within a pipeline, the function terminates the process
 * using `exit()`.
 *
 * @param cmd Pointer to the command structure.
 * @return An exit status indicating success (`EXIT_SUCCESS`) or
 *         failure (`EXIT_FAILURE`).
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
		return (exit_status);
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
