/**
 * @file handle_cd.c
 * @brief Functions for handling the `cd` built-in command in Minishell.
 */
#include "minishell.h"

/**
 * @brief Prints an error message when too many arguments are provided to `cd`.
 *
 * The `cd` command in Minishell only accepts zero or one argument.
 * If more than one argument is provided, this function prints an error message.
 *
 * @return `EXIT_FAILURE` (1) since having too many arguments is an error.
 */
static uint8_t	cd_too_many_args(void)
{
	print_error("minishell: cd: too many arguments\n");
	return (EXIT_FAILURE);
}

/**
 * @brief Updates the `PWD` and `OLDPWD` environment variables after `cd`.
 *
 * After a successful `cd` operation, this function updates:
 * - `OLDPWD` to store the previous working directory.
 * - `PWD` to store the new working directory.
 * - Calls `update_env()` to reflect changes in the environment.
 *
 * @param cmd Pointer to the command structure containing shell information.
 * @param old_cwd The previous working directory before executing `cd`.
 */
static void	update_pwd_variables(t_cmd *cmd, char *old_cwd)
{
	char	new_cwd[MS_PATHMAX];

	ft_bzero(new_cwd, MS_PATHMAX);
	if (!get_directory(new_cwd, cmd))
		ft_strlcpy(new_cwd, "", MS_PATHMAX);
	set_variable(cmd->minishell, "OLDPWD", old_cwd, 1);
	set_variable(cmd->minishell, "PWD", new_cwd, 1);
	update_env(cmd->minishell);
}

/**
 * @brief Handles the `cd` command when no arguments are provided.
 *
 * - If `cd` is called with no arguments, it attempts to change to the `HOME`
 * directory.
 * - If `HOME` is not set, an error message is printed.
 * - On success, `PWD` and `OLDPWD` are updated.
 *
 * @param cmd Pointer to the command structure.
 * @return `EXIT_SUCCESS` (0) if the directory changes successfully.
 *         `EXIT_FAILURE` (1) if `HOME` is not set or an error occurs.
 */
static uint8_t	cd_no_args(t_cmd *cmd)
{
	char	old_cwd[MS_PATHMAX];
	char	*home_path;

	if (!cmd || !cmd->minishell)
		return (EXIT_FAILURE);
	ft_bzero(old_cwd, MS_PATHMAX);
	home_path = ms_getenv(cmd->minishell, "HOME");
	if (!home_path)
	{
		print_error("minishell: cd: HOME not set\n");
		return (EXIT_FAILURE);
	}
	(void)get_directory(old_cwd, cmd);
	if (chdir(home_path) != 0)
	{
		print_error("-minishell: ");
		perror(cmd->argv[0]);
		return (EXIT_FAILURE);
	}
		// return (cmd_error_handler(cmd, EXIT_FAILURE));
	update_pwd_variables(cmd, old_cwd);
	return (EXIT_SUCCESS);
}

/**
 * @brief Changes the working directory and updates `PWD` and `OLDPWD`.
 *
 * This function:
 * - Changes the directory to the specified path.
 * - Updates the shell environment with the new directory.
 * - If the path is invalid, it returns an error.
 *
 * @param cmd Pointer to the command structure.
 * @return `EXIT_SUCCESS` (0) if the directory changes successfully.
 *         `EXIT_FAILURE` (1) if an error occurs.
 */
static uint8_t	change_and_update_pwd(t_cmd *cmd)
{
	char	old_cwd[MS_PATHMAX];

	if (!cmd || !cmd->argv[1] || !cmd->minishell)
		return (EXIT_FAILURE);
	ft_bzero(old_cwd, MS_PATHMAX);
	(void)get_directory(old_cwd, cmd);
	if (chdir(cmd->argv[1]) != 0)
	{
		print_error("-minishell: ");
		perror(cmd->argv[1]);
		return (EXIT_FAILURE);
	}
		// return (cmd_error_handler(cmd, EXIT_FAILURE));
	update_pwd_variables(cmd, old_cwd);
	return (EXIT_SUCCESS);
}

/**
 * @brief Handles the execution of the `cd` command.
 *
 * Implements different behaviors based on input:
 * - `cd` with no arguments → Moves to the `HOME` directory.
 * - `cd ~` → Also moves to the `HOME` directory.
 * - `cd <path>` → Moves to the specified directory.
 * - `cd ..` → Moves up one directory.
 * - `cd -` → Moves to the previous working directory (`OLDPWD`).
 * - If multiple arguments are provided, an error message is displayed.
 *
 * @param cmd Pointer to the command structure containing arguments and
 *            shell state.
 * @return `EXIT_SUCCESS` (0) if the command executes successfully.
 *         `EXIT_FAILURE` (1) if an error occurs.
 */
uint8_t	handle_cd(t_cmd *cmd)
{
	uint8_t	status;

	if (!cmd || !cmd->minishell)
	{
		print_error("minishell: cd: invalid command structure\n");
		return (EXIT_FAILURE);
	}
	if (!cmd->argv[1] || (cmd->argv[1][0] == '~' && !cmd->argv[1][1]))
		status = cd_no_args(cmd);
	else if (cmd->argv[2])
		status = cd_too_many_args();
	else if (!is_valid_value(cmd))
		status = cd_too_many_args();
	else
		status = change_and_update_pwd(cmd);
	cmd->minishell->exit_status = status;
	return (status);
}
