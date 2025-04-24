/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imunaev- <imunaev-@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 14:44:42 by Ilia Munaev       #+#    #+#             */
/*   Updated: 2025/04/24 17:28:38 by imunaev-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file cd.c
 * @brief Functions for handling the `cd` built-in command in Minishell.
 */
#include "minishell.h"

/**
 * @brief Reports an error when too many arguments are passed to `cd`.
 *
 * The `cd` built-in in Minishell supports only 0 or 1 arguments. If more than
 * one is detected (e.g., `cd dir1 dir2`), this function is called to print a
 * descriptive error message.
 *
 * @return Always returns `EXIT_FAILURE` (1) to signal argument misuse.
 */
static uint8_t	cd_too_many_args(void)
{
	return (error_return("cd: too many arguments\n", EXIT_FAILURE));
}

/**
 * @brief Updates the `PWD` and `OLDPWD` environment variables after a
 * directory change.
 *
 * This function:
 * - Retrieves the new working directory.
 * - Sets `OLDPWD` to the previous directory.
 * - Sets `PWD` to the current directory.
 * - Triggers a rebuild of the exported environment with `update_env()`.
 *
 * @param cmd      Pointer to the current command structure.
 * @param old_cwd  The previous working directory before the `cd` call.
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
 * @brief Handles `cd` with no arguments or `cd ~`.
 *
 * Changes directory to the `HOME` environment variable if it's set.
 * If `HOME` is not defined, it prints an error. On success, this updates
 * `PWD` and `OLDPWD` using the helper function.
 *
 * @param cmd Pointer to the current command structure.
 * @return `EXIT_SUCCESS` (0) on success,
 *         `EXIT_FAILURE` (1) on error (e.g., `HOME` not set or `chdir()` fails).
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
		return (error_return("cd: HOME not set\n", EXIT_FAILURE));
	(void)get_directory(old_cwd, cmd);
	if (chdir(home_path) != 0)
		return (perror_return(cmd->argv[0], EXIT_FAILURE));
	update_pwd_variables(cmd, old_cwd);
	return (EXIT_SUCCESS);
}

/**
 * @brief Changes to a specified directory path and updates environment
 * variables.
 *
 * Handles both standard path changes (`cd <path>`) and the special case `cd -`,
 * which switches to the previous directory stored in `OLDPWD`.
 *
 * If `chdir()` fails, prints an error and returns failure.
 *
 * @param cmd Pointer to the current command structure.
 * @return `EXIT_SUCCESS` (0) if the directory is changed successfully,
 *         `EXIT_FAILURE` (1) if an error occurs (invalid path or unset OLDPWD).
 */
static uint8_t	change_and_update_pwd(t_cmd *cmd)
{
	char	old_cwd[MS_PATHMAX];
	char	*old_pwd;

	if (!cmd || !cmd->argv[1] || !cmd->minishell)
		return (EXIT_FAILURE);
	ft_bzero(old_cwd, MS_PATHMAX);
	(void)get_directory(old_cwd, cmd);
	if (ft_strcmp(cmd->argv[1], "-") == 0)
	{
		old_pwd = ft_strdup(ms_getenv(cmd->minishell, "OLDPWD"));
		if (!old_pwd)
		{
			print_error("minishell: cd: OLDPWD not set\n");
			return (EXIT_FAILURE);
		}
		if (chdir(old_pwd) != 0)
		{
			print_error("minishell: cd: ");
			perror(old_pwd);
			free(old_pwd);
			return (EXIT_FAILURE);
		}
		ft_putendl_fd(old_pwd, STDOUT_FILENO);
		free(old_pwd);
	}
	else if (chdir(cmd->argv[1]) != 0)
	{
		print_error("minishell: cd: ");
		perror(cmd->argv[1]);
		return (EXIT_FAILURE);
	}
	update_pwd_variables(cmd, old_cwd);
	return (EXIT_SUCCESS);
}

/**
 * @brief Entry point for handling the `cd` built-in command in Minishell.
 *
 * This function implements the logic and argument validation for all
 * valid `cd` usages:
 * - `cd` / `cd ~` → go to `$HOME`.
 * - `cd -`        → switch to `$OLDPWD`.
 * - `cd <path>`   → change to a specific path.
 * - `cd ..`       → change to the parent directory.
 *
 * Handles edge cases:
 * - More than one argument → returns an error.
 * - Empty string → ignored as a no-op.
 *
 * @param cmd Pointer to the command structure containing user input and
 * shell state.
 * @return `EXIT_SUCCESS` (0) on success,
 *         `EXIT_FAILURE` (1) on failure or misuse.
 */
uint8_t	handle_cd(t_cmd *cmd)
{
	uint8_t	exit_status;

	if (!cmd)
		return (no_cmd_error("cd"));
	if (!cmd->argv[1] || (cmd->argv[1][0] == '~' && !cmd->argv[1][1]))
		exit_status = cd_no_args(cmd);
	else if (cmd->argv[2])
		exit_status = cd_too_many_args();
	else if (ft_strlen(cmd->argv[1]) == 0)
		return (EXIT_SUCCESS);
	else
		exit_status = change_and_update_pwd(cmd);
	return (exit_status);
}
