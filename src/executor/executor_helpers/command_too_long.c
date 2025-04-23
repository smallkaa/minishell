/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_too_long.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ilia Munaev <ilyamunaev@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 14:46:25 by Ilia Munaev       #+#    #+#             */
/*   Updated: 2025/04/23 14:46:26 by Ilia Munaev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file command_too_long.c
 * @brief Command length validation for Minishell.
 *
 * Contains logic to verify that a given command does not exceed
 * the maximum allowed buffer size (`CMD_MAX_SIZE`).
 * Helps prevent buffer overflows or input issues with excessively long commands.
 */
#include "minishell.h"

/**
 * @brief Checks whether a command's total length exceeds the shell limit.
 *
 * Calculates the cumulative length of all arguments in `argv`, including spaces,
 * and compares it against a predefined maximum command size (`CMD_MAX_SIZE`).
 *
 * @param argv The array of command arguments (null-terminated).
 * @return `true` if the total command length is too long, `false` otherwise.
 */
bool	command_too_long(char **argv)
{
	size_t	total_len;
	int		i;

	total_len = 0;
	i = 0;
	while (argv[i])
	{
		total_len += ft_strlen(argv[i]) + 1;
		if (total_len >= CMD_MAX_SIZE)
			return (true);
		i++;
	}
	return (false);
}
