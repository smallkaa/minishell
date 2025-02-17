/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   find_binary.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imunaev- <imunaev-@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/16 22:48:14 by imunaev-          #+#    #+#             */
/*   Updated: 2025/02/17 10:52:06 by imunaev-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Assigns a binary path to a command.
 *
 * Joins the given path with the command name to check if the
 * binary exists. If found, assigns it to the command structure.
 *
 * @param path The directory path to check.
 * @param cmd The command structure.
 * @return EXIT_SUCCESS if the binary is assigned,
 * otherwise exit with coresponding status.
 */
static int	assign_binary(char *path, t_command *cmd)
{
	char	*temp_path;
	char	*binary;

	temp_path = ft_strjoin(path, "/");
	binary = ft_strjoin(temp_path, cmd->argv[0]);
	free(temp_path);
	if (access(binary, F_OK) == 0)
	{
		cmd->binary = binary;
		return (EXIT_SUCCESS);
	}
	if (access(binary, F_OK) != 0)
	{
		free(binary);
		exit(126);
	}
	free(binary);
	exit(127);
}

/**
 * @brief Finds the binary for a given command.
 *
 * Searches for the command's binary in the system PATH directories.
 * If found, assigns the binary path to the command structure.
 *
 * @param cmd The command structure.
 * @return EXIT_SUCCESS if the binary is found, otherwise EXIT_FAILURE.
 */
int	find_binary(t_command *cmd)
{
	int		i;
	char	**paths;

	paths = ft_split(getenv("PATH"), ':');
	if (!paths)
	{
		ft_putstr_fd("Error: find_binary() getenv failed.\n", STDERR_FILENO);
		return (EXIT_FAILURE);
	}
	i = 0;
	while (paths[i])
	{
		if (assign_binary(paths[i], cmd) == EXIT_SUCCESS)
		{
			ft_free_arrstrs(paths);
			return (EXIT_SUCCESS);
		}
		free(cmd->binary);
		i++;
	}
	ft_putstr_fd("Error: find_binary() no binary found.\n", STDERR_FILENO);
	ft_free_arrstrs(paths);
	return (EXIT_FAILURE);
}
