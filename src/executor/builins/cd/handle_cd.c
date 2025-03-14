/**
 * @file handle_cd.c
 * @brief Implementation of the `cd` built-in command in Minishell.
 */
#include "minishell.h"

/**
 * @brief Prints an error message when too many arguments are provided to `cd`.
 *
 * This function is called when the `cd` command receives more than one argument,
 * which is not allowed in Minishell.
 *
 * @return `EXIT_FAILURE` always, since having too many arguments is an error.
 */
static uint8_t	cd_too_many_args(void)
{
	print_error("minishell: cd: too many arguments\n");
	return (EXIT_FAILURE);
}

/**
 * @brief Updates the `PWD` and `OLDPWD` environment variables.
 *
 * After a successful `cd` operation, this function updates the shell's
 * environment variables to reflect the new and old working directories.
 *
 * @param cmd Pointer to the command structure containing shell information.
 * @param old_cwd The previous working directory before the `cd`
 *                command was executed.
 */
static void	update_pwd_variables(t_cmd *cmd, char *old_cwd)
{
	char	new_cwd[MS_PATHMAX];

	if (!get_directory(new_cwd, cmd))
		ft_strlcpy(new_cwd, "", MS_PATHMAX);
	(void)set_variable(cmd->minishell, "OLDPWD", old_cwd, 1);
	(void)set_variable(cmd->minishell, "PWD", new_cwd, 1);
	(void)update_env(cmd->minishell);
}

/**
 * @brief Handles the `cd` command when no arguments are provided.
 *
 * If no arguments are passed to `cd`, the function attempts to change the
 * directory to the `HOME` environment variable. If `HOME` is not set, an
 * error is printed.
 *
 * @param cmd Pointer to the command structure.
 * @return `EXIT_SUCCESS` if the directory is changed successfully,
 *         `EXIT_FAILURE` otherwise.
 */
static uint8_t	cd_no_args(t_cmd *cmd)
{
	char	old_cwd[MS_PATHMAX];
	char	*home_path;

	if (!cmd || !cmd->minishell)
		return (EXIT_FAILURE);
	home_path = ms_getenv(cmd->minishell, "HOME");
	if (!home_path)
	{
		print_error("minishell: cd: HOME not set\n");
		return (EXIT_FAILURE);
	}
	(void)get_directory(old_cwd, cmd);
	if (chdir(home_path) != 0)
		return (cmd_error_handler(cmd, EXIT_FAILURE));
	(void)update_pwd_variables(cmd, old_cwd);
	return (EXIT_SUCCESS);
}

/**
 * @brief Handles the `cd` command when a directory argument is provided.
 *
 * This function attempts to change the working directory to the specified
 * path and updates `PWD` and `OLDPWD` in the environment.
 *
 * @param cmd Pointer to the command structure.
 * @return `EXIT_SUCCESS` if the directory is changed successfully,
 *         `EXIT_FAILURE` if an error occurs.
 */
static uint8_t	change_and_update_pwd(t_cmd *cmd)
{
	char	old_cwd[MS_PATHMAX];

	if (!cmd || !cmd->argv[1] || !cmd->minishell)
		return (EXIT_FAILURE);
	(void)get_directory(old_cwd, cmd);
	if (chdir(cmd->argv[1]) != 0)
		return (cmd_error_handler(cmd, EXIT_FAILURE));
	(void)update_pwd_variables(cmd, old_cwd);
	return (EXIT_SUCCESS);
}

/**
 * @brief Handles the execution of the `cd` command.
 *
 * This function processes the `cd` command in Minishell, handling different
 * cases such as changing to `HOME`, handling errors for invalid arguments,
 * and updating environment variables accordingly.
 *
 * @param cmd Pointer to the command structure containing
 *            arguments and shell state.
 * @return `EXIT_SUCCESS` if the command executes successfully,
 *         `EXIT_FAILURE` otherwise.
 */
uint8_t	handle_cd(t_cmd *cmd)
{
	uint8_t	status;

	if (!cmd || !cmd->minishell)
	{
		print_error("minishell: cd: invalid command structure\n");
		if (cmd && cmd->in_pipe)
			exit(EXIT_FAILURE);
		return (EXIT_FAILURE);
	}
	if (!cmd->argv[1])
		status = cd_no_args(cmd);
	else if (cmd->argv[2])
		status = cd_too_many_args();
	else if (!is_valid_value(cmd))
		status = cd_too_many_args();
	else
		status = change_and_update_pwd(cmd);
	cmd->minishell->exit_status = status;
	if (cmd->in_pipe)
		exit(status);
	return (status);
}

// static bool is_valid_value(t_cmd *cmd)
// {
// 	if (ft_strchr(cmd->argv[1], ' '))
// 		return (false);
// 	return (true);
// }

// static uint8_t	cd_too_many_args(void)
// {
// 	print_error("minishell: cd: too many arguments\n");
// 	return (EXIT_FAILURE);
// }

// static uint8_t cd_no_args(t_cmd *cmd)
// {
// 	char	old_cwd[MS_PATHMAX];
// 	char	new_cwd[MS_PATHMAX];
// 	char	*home_path;
// 	char	*temp_pwd;

// 	if (!cmd || !cmd->minishell)
// 		return (EXIT_FAILURE);
// 	home_path = ms_getenv(cmd->minishell, "HOME");
// 	if (!home_path)
// 	{
// 		print_error("minishell: cd: HOME not set\n");
// 		return (EXIT_FAILURE);
// 	}
// 	if (!getcwd(old_cwd, MS_PATHMAX))
// 	{
// 		temp_pwd = ms_getenv(cmd->minishell, "PWD");
// 		if (temp_pwd)
// 		{
// 			ft_strlcpy(old_cwd, temp_pwd, MS_PATHMAX);
// 			temp_pwd = NULL;
// 		}
// 		else
// 			ft_strlcpy(old_cwd, "", MS_PATHMAX);
// 	}
// 	if (chdir(home_path) != 0)
// 		return(cmd_error_handler(cmd, EXIT_FAILURE));
// 	if (!getcwd(new_cwd, MS_PATHMAX))
// 		ft_strlcpy(new_cwd, home_path, MS_PATHMAX);
// 	set_variable(cmd->minishell, "OLDPWD", old_cwd, 1);
// 	set_variable(cmd->minishell, "PWD", new_cwd, 1);
// 	update_env(cmd->minishell);
// 	return (EXIT_SUCCESS);
// }

// static uint8_t	change_and_update_pwd(t_cmd *cmd)
// {
// 	char	old_cwd[MS_PATHMAX];
// 	char	new_cwd[MS_PATHMAX];
// 	char	*temp_pwd;

// 	if (!cmd || !cmd->argv[1] || !cmd->minishell)
// 		return (EXIT_FAILURE);
// 	if (!getcwd(old_cwd, MS_PATHMAX))
// 	{
// 		temp_pwd = ms_getenv(cmd->minishell, "PWD");
// 		if (temp_pwd)
// 		{
// 			ft_strlcpy(old_cwd, temp_pwd, MS_PATHMAX);
// 			temp_pwd = NULL;
// 		}
// 		else
// 			ft_strlcpy(old_cwd, "", MS_PATHMAX);
// 	}
// 	if (chdir(cmd->argv[1]) != 0)
// 		return(cmd_error_handler(cmd, EXIT_FAILURE));
// 	if (!getcwd(new_cwd, MS_PATHMAX))
// 	{
// 		temp_pwd = ms_getenv(cmd->minishell, "PWD");
// 		if (temp_pwd)
// 		{
// 			ft_strlcpy(new_cwd, temp_pwd, MS_PATHMAX);
// 			temp_pwd = NULL;
// 		}
// 		else
// 			ft_strlcpy(new_cwd, "", MS_PATHMAX);
// 	}
// 	set_variable(cmd->minishell, "OLDPWD", old_cwd, 1);
// 	set_variable(cmd->minishell, "PWD", new_cwd, 1);
// 	update_env(cmd->minishell);
// 	return (EXIT_SUCCESS);
// }

// uint8_t	handle_cd(t_cmd *cmd)
// {
// 	uint8_t	status;

// 	if (!cmd || !cmd->minishell)
// 	{
// 		print_error("minishell: cd: invalid command structure\n");
// 		if (cmd && cmd->in_pipe)
// 			exit(EXIT_FAILURE);
// 		return (EXIT_FAILURE);
// 	}
// 	if (!cmd->argv[1])
// 		status = cd_no_args(cmd);
// 	else if (cmd->argv[2])
// 		status = cd_too_many_args();
// 	else if (!is_valid_value(cmd))
// 		status = cd_too_many_args();
// 	else
// 		status = change_and_update_pwd(cmd);
// 	cmd->minishell->exit_status = status;
// 	if (cmd->in_pipe)
// 		exit(status);
// 	return (status);
// }
