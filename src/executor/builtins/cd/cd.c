/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imunaev- <imunaev-@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 14:44:42 by Ilia Munaev       #+#    #+#             */
/*   Updated: 2025/05/12 14:09:12 by imunaev-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file cd.c
 * @brief Functions for handling the `cd` built-in command in Minishell.
 */
#include "minishell.h"

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
 * @brief Handles `cd -` (switch to previous directory).
 *
 * Attempts to change to the directory stored in `OLDPWD`.
 * On success, prints the new directory path and updates environment.
 *
 * @param cmd Pointer to the command structure.
 * @return `EXIT_SUCCESS` on success, `EXIT_FAILURE` on error.
 */
static uint8_t	handle_cd_dash(t_cmd *cmd)
{
	char	*old_pwd;
	char	*old_env;

	old_env = ms_getenv(cmd->minishell, "OLDPWD");
	if (!old_env)
	{
		print_error("-minishell: cd, OLDPWD not set\n");
		return (EXIT_FAILURE);
	}
	old_pwd = ft_strdup(old_env);
	if (!old_pwd)
	{
		print_error("-minishell: cd, strdup failed\n");
		return (EXIT_FAILURE);
	}
	if (chdir(old_pwd) != 0)
	{
		print_error("-minishell: cd, chdir failed\n");
		free(old_pwd);
		return (EXIT_FAILURE);
	}
	ft_putendl_fd(old_pwd, STDOUT_FILENO);
	free(old_pwd);
	return (EXIT_SUCCESS);
}

/**
 * @brief Handles `cd <path>` by attempting to change directory.
 *
 * @param path The target directory path.
 * @return `EXIT_SUCCESS` on success, `EXIT_FAILURE` on error.
 */
static uint8_t	handle_cd_path(const char *path)
{
	if (chdir(path) != 0)
	{
		print_error("-minishell: cd: ");
		perror(path);
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}

/**
 * @brief Main logic for changing directory and updating PWD/OLDPWD.
 *
 * Handles both `cd -` and `cd <path>` logic and updates the environment
 * accordingly after a successful change.
 *
 * @param cmd Pointer to the command structure.
 * @return `EXIT_SUCCESS` on success, `EXIT_FAILURE` on error.
 */
static uint8_t	change_and_update_pwd(t_cmd *cmd)
{
	char	old_cwd[MS_PATHMAX];
	uint8_t	status;

	if (!cmd || !cmd->argv[1] || !cmd->minishell)
		return (EXIT_FAILURE);
	ft_bzero(old_cwd, MS_PATHMAX);
	(void)get_directory(old_cwd, cmd);
	if (ft_strcmp(cmd->argv[1], "-") == 0)
		status = handle_cd_dash(cmd);
	else
		status = handle_cd_path(cmd->argv[1]);
	if (status == EXIT_FAILURE)
		return (EXIT_FAILURE);
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
	if (cmd->argv[1] && ft_strcmp(cmd->argv[1], "--") == 0)
	{
		if (cmd->argv[2])
			exit_status = cd_too_many_args();
		else
			exit_status = cd_no_args(cmd);
	}
	else if (!cmd->argv[1] || (cmd->argv[1][0] == '~' && !cmd->argv[1][1]))
		exit_status = cd_no_args(cmd);
	else if (cmd->argv[2])
		exit_status = cd_too_many_args();
	else if (ft_strlen(cmd->argv[1]) == 0)
		return (EXIT_SUCCESS);
	else
		exit_status = change_and_update_pwd(cmd);
	return (exit_status);
}
