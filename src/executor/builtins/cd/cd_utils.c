/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd_utils.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ilia Munaev <ilyamunaev@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 14:44:00 by Ilia Munaev       #+#    #+#             */
/*   Updated: 2025/04/23 14:44:01 by Ilia Munaev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file cd_utils.c
 * @brief Utility functions used by the `cd` built-in implementation
 * in Minishell.
 *
 * This file provides helper functions that support the core logic in
 * `handle_cd.c`,
 * including directory resolution and environment fallbacks.
 */
#include "minishell.h"

/**
 * @brief Retrieves the current working directory with fallback support.
 *
 * Attempts to obtain the current working directory using `getcwd()`. If it fails
 * (e.g., due to permission errors or deleted working directory), the function
 * falls back to retrieving the `$PWD` environment variable from the shell's
 * internal environment.
 *
 * The result is stored in the provided buffer `cwd`, which must be at least
 * `MS_PATHMAX` bytes in size.
 *
 * @param cwd  Buffer to store the resolved current working directory path.
 * @param cmd  Pointer to the command structure, used to access shell state
 * and environment.
 * @return `true` if `getcwd()` succeeds, `false` if `$PWD` is used as
 * a fallback.
 *
 * @note The function does not allocate memory. Always zero or validate `cwd
 *  before use.
 */
bool	get_directory(char *cwd, t_cmd *cmd)
{
	char	*temp_pwd;

	if (getcwd(cwd, MS_PATHMAX))
		return (true);
	temp_pwd = ms_getenv(cmd->minishell, "PWD");
	if (temp_pwd)
		ft_strlcpy(cwd, temp_pwd, MS_PATHMAX);
	else
		ft_strlcpy(cwd, "", MS_PATHMAX);
	return (false);
}
