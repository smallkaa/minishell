/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imunaev- <imunaev-@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 14:46:30 by Ilia Munaev       #+#    #+#             */
/*   Updated: 2025/04/24 17:52:06 by imunaev-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file executor_utils.c
 * @brief Utility functions for managing shell state and command validation.
 *
 * This file includes helper functions used throughout Minishell for:
 * - Validating environment variable names.
 * - Cleaning up and exiting the shell properly.
 * - Detecting if the last command was an `exit` command.
 */
#include "minishell.h"

/**
 * @brief Validates whether a given key is a valid environment variable name.
 *
 * A valid name must:
 * - Start with a letter or underscore.
 * - Contain only letters, digits, or underscores.
 * - Stop checking at the first `=` character, if present.
 *
 * Used for validating `export` or `unset` keys.
 *
 * @param key The string to validate.
 * @return `true` if the name is valid, `false` otherwise.
 */
bool	is_valid_varname(const char *key)
{
	int	i;

	if (!key || !(ft_isalpha(key[0]) || key[0] == '_'))
		return (false);
	i = 1;
	while (key[i] && key[i] != '=')
	{
		if (!(ft_isalnum(key[i]) || key[i] == '_'))
			return (false);
		i++;
	}
	return (true);
}

/**
 * @brief Frees all allocated memory and exits the shell.
 *
 * Performs proper cleanup:
 * - Frees the command and shell structures.
 * - Clears readline history.
 * - Exits the process with the specified status code.
 *
 * Used after executing an `exit` command or fatal error.
 *
 * @param cmd Pointer to the command structure.
 * @param exit_status Exit code to return from the shell.
 */
void	cleanup_and_exit(t_cmd *cmd, int exit_status)
{
	t_mshell	*minishell;

	if (!cmd)
		return ;
	if (isatty(STDIN_FILENO))
		ft_putendl_fd("exit", STDERR_FILENO);
	minishell = cmd->minishell;
	free_cmd(cmd);
	free_minishell(minishell);
	rl_clear_history();
	exit(exit_status);
}

/**
 * @brief Checks if the current command is an `exit` command.
 *
 * This is used in pipelines where only the last command should trigger
 * an actual shell exit.
 *
 * @param cmd Pointer to the command structure.
 * @return `true` if the last token is `"exit"`, `false` otherwise.
 */
bool	is_exit_command(t_cmd *cmd)
{
	int	i;

	if (!cmd)
		return (false);
	i = 0;
	if (!cmd->argv[0])
		return (false);
	while (cmd->argv[i])
		i++;
	return (ft_strcmp(cmd->argv[i - 1], "exit") == 0);
}
