/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_builtins.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ilia Munaev <ilyamunaev@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 14:46:07 by Ilia Munaev       #+#    #+#             */
/*   Updated: 2025/04/23 14:46:08 by Ilia Munaev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file exec_builtin.c
 * @brief Functions for identifying and executing built-in commands in Minishell.
 *
 * This file implements the built-in command dispatch system, including:
 * - Detecting whether a command is built-in.
 * - Providing a dispatch table of all supported built-ins.
 * - Executing the correct handler function for the built-in.
 */
#include "minishell.h"

/**
 * @brief Checks whether the command is a built-in.
 *
 * Compares the first argument of the command (`cmd->argv[0]`) with known
 * built-in command names stored in a dispatch table.
 *
 * @param cmd Pointer to the command structure.
 * @return `true` if the command is a recognized built-in, `false` otherwise.
 */
bool	is_builtin(t_cmd *cmd)
{
	const t_builtin_disp	*table;
	size_t					size;
	size_t					i;

	table = get_builtin_table(&size);
	i = 0;
	if (!cmd->argv[0])
		return (false);
	while (i < size)
	{
		if (ft_strcmp(cmd->argv[0], (char *)table[i].name) == 0)
			return (true);
		i++;
	}
	return (false);
}

/**
 * @brief Returns the built-in command dispatch table.
 *
 * The dispatch table is a static array containing built-in command names
 * and corresponding handler function pointers. Optionally sets `*size`
 * to the number of built-ins available.
 *
 * @param size Pointer to a size_t that will be filled with the table size.
 * @return Pointer to the internal static array of built-in command handlers.
 */
const t_builtin_disp	*get_builtin_table(size_t *size)
{
	static const t_builtin_disp	table[] = {
	{"cd", &handle_cd},
	{"echo", &handle_echo},
	{"env", &handle_env},
	{"exit", &handle_exit},
	{"export", &handle_export},
	{"pwd", &handle_pwd},
	{"unset", &handle_unset},
	};

	if (size)
		*size = sizeof(table) / sizeof(table[0]);
	return (table);
}

/**
 * @brief Executes a built-in command if it matches one from the dispatch table.
 *
 * Looks up the command by name in the built-in table and, if found,
 * calls the corresponding handler function.
 *
 * @param cmd Pointer to the command structure.
 * @return Return value of the built-in handler if matched,
 *         `EXIT_FAILURE` (1) if the command is not a built-in.
 */
uint8_t	exec_builtins(t_cmd *cmd)
{
	size_t					i;
	size_t					size;
	const t_builtin_disp	*table;

	if (!cmd->argv || !cmd->argv[0])
		return (EXIT_FAILURE);
	table = get_builtin_table(&size);
	i = 0;
	while (i < size)
	{
		if (ft_strcmp(cmd->argv[0], (char *)table[i].name) == 0)
			return (table[i].func(cmd));
		i++;
	}
	return (EXIT_FAILURE);
}
