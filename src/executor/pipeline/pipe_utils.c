/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ilia Munaev <ilyamunaev@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 14:47:09 by Ilia Munaev       #+#    #+#             */
/*   Updated: 2025/05/10 02:27:32 by Ilia Munaev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file pipe_utils.c
 * @brief Special-case handling utilities for Minishell command execution.
 *
 * This file contains helper functions used during command execution, such as:
 * - Detecting if Minishell is recursively called.
 * - Handling empty commands and syntax edge cases.
 * - Managing the `SHLVL` environment variable.
 * - Handling built-in error messages for `.` and `..`.
 */
#include "minishell.h"

t_cmd	*get_cmd_head(t_cmd *cmd)
{
	if (!cmd)
		return (NULL);
	if (cmd->origin_head)
		return (cmd->origin_head);
	return (cmd);
}

/**
 * @brief Checks whether the command is a recursive call to `./minishell`.
 *
 * Used to update the `SHLVL` environment variable when Minishell
 * is executed from itself.
 *
 * @param cmd Pointer to the command structure.
 * @return `true` if the command is `./minishell`, otherwise `false`.
 */
bool	is_minishell_executable(t_cmd *cmd)
{
	return (ft_strcmp(cmd->argv[0], "./minishell") == 0);
}

/**
 * @brief Handles an empty command (e.g., when only redirections are given).
 *
 * - If no redirections exist, prints an error and exits with status `127`.
 * - If redirections exist, exits silently with status `0`.
 *
 * @param cmd Pointer to the command structure.
 */
void	handle_empty_command(t_cmd *cmd)
{
	t_cmd	*head;

	head = get_cmd_head(cmd);
	if (!cmd->redirs)
	{
		print_error("Command '' not found\n");
		free_minishell(&cmd->minishell);
		free_cmd(&head);
		_exit(127);
	}
	free_minishell(&cmd->minishell);
	free_cmd(&head);
	_exit(EXIT_SUCCESS);
}

/**
 * @brief Increments the `SHLVL` environment variable.
 *
 * Called when Minishell is executed from within itself. Updates the internal
 * hash table and exported environment.
 *
 * @param cmd Pointer to the command structure.
 * @return `EXIT_SUCCESS` (0) on success, or `EXIT_FAILURE` (1)
 * if allocation fails.
 */
uint8_t	update_shlvl(t_cmd *cmd)
{
	char	*str_shlvl;
	int		shlvl;
	char	*new_shlvl;

	str_shlvl = ms_getenv(cmd->minishell, "SHLVL");
	if (str_shlvl != NULL)
		shlvl = ft_atoi(str_shlvl) + 1;
	else
		shlvl = 1;
	new_shlvl = ft_itoa(shlvl);
	if (!new_shlvl)
		return (EXIT_FAILURE);
	if (set_variable(cmd->minishell, "SHLVL", new_shlvl, 1) != EXIT_SUCCESS)
	{
		print_error("-minishell: update_shlvl, set_variable failed\n");
		return (EXIT_FAILURE);
	}
	free(new_shlvl);
	if (update_env(cmd->minishell) != EXIT_SUCCESS)
		perror_return("update_shlvl, update_env failed\n", EXIT_FAILURE);
	return (EXIT_SUCCESS);
}

/**
 * @brief Handles special cases for dot (`.`) and dot-dot (`..`) commands.
 *
 * - If the command is just `.` with no arguments, prints a usage error.
 * - If the command is `..` with no arguments, reports "command not found".
 *
 * @param cmd Pointer to the command structure.
 * @return Exit code: `2` for dot usage error, `127` for `..` not found,
 *         or `EXIT_SUCCESS` (0) otherwise.
 */
uint8_t	validate_dots(t_cmd *cmd)
{
	char	error_buf[ERROR_BUF_SIZE];

	if (ft_strcmp(cmd->argv[0], ".") == 0 && !cmd->argv[1])
	{
		error_buf[0] = '\0';
		ft_strlcpy(error_buf, "minishell: .: \
			filename argument required\n", ERROR_BUF_SIZE);
		ft_strlcat(error_buf, ".: usage: . \
			filename [arguments]\n", ERROR_BUF_SIZE);
		if (write(STDERR_FILENO, error_buf, ft_strlen(error_buf)) < 0)
			write(STDERR_FILENO, "minishell: error: \
				failed to print error\n", 40);
		return (2);
	}
	if (ft_strcmp(cmd->argv[0], "..") == 0 && !cmd->argv[1])
	{
		error_buf[0] = '\0';
		ft_strlcpy(error_buf, "..: command not found\n", ERROR_BUF_SIZE);
		if (write(STDERR_FILENO, error_buf, ft_strlen(error_buf)) < 0)
			write(STDERR_FILENO, "minishell: \
				error: failed to print error\n", 40);
		return (127);
	}
	return (EXIT_SUCCESS);
}
