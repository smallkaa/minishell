/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   update_underscore.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ilia Munaev <ilyamunaev@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 14:46:35 by Ilia Munaev       #+#    #+#             */
/*   Updated: 2025/05/07 22:08:39 by Ilia Munaev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file update_underscore.c
 * @brief Updates the `_` environment variable in Minishell.
 *
 * The special variable `_` is set after each command to the last argument
 * or the full binary path. This file provides logic to compute and update
 * that value in the environment hash table, mimicking Bash behavior.
 */
#include "minishell.h"

/**
 * @brief Joins a variable name and value with an `=` in between.
 *
 * Used when reconstructing an `export` variable from split arguments
 * (e.g., `export VAR VALUE` → `VAR=VALUE`).
 *
 * @param name The variable name (left-hand side).
 * @param value The variable value (right-hand side).
 * @return Newly allocated string in the form `name=value`, or `NULL` on failure.
 */
static char	*join_with_equal(char *name, char *value)
{
	char	*tmp;
	char	*res;

	tmp = ft_strjoin(name, "=");
	if (tmp == NULL)
		return (NULL);
	res = ft_strjoin(tmp, value);
	free(tmp);
	return (res);
}

/**
 * @brief Determines the value to assign to the `_` variable.
 *
 * Behavior:
 * - For general commands: returns the last argument.
 * - For `export`, handles special cases where the value may be split
 * across args.
 *
 * @param cmd Pointer to the command structure.
 * @param need_free Output flag: set to `1` if returned value needs to be freed.
 * @return Pointer to the argument or combined string to assign to `_`.
 */
static char	*get_last_arg(t_cmd *cmd, int *need_free)
{
	int	i;

	*need_free = 0;
	i = 0;
	while (cmd->argv[i])
		i++;
	if (i == 0)
		return (cmd->argv[0]);
	if (ft_strcmp(cmd->argv[0], "export") == 0)
	{
		if (ft_strchr(cmd->argv[i - 1], '='))
			return (cmd->argv[i - 1]);
		if (i >= 3)
		{
			*need_free = 1;
			return (join_with_equal(cmd->argv[i - 2], cmd->argv[i - 1]));
		}
	}
	return (cmd->argv[i - 1]);
}

/**
 * @brief Updates the special shell variable `_` with the last
 * command argument or path.
 *
 * Mimics Bash behavior by assigning `_` to:
 * - The last argument of the command.
 * - For built-ins like `export`, reconstructs `key=value` if necessary.
 * - If no argument exists, falls back to the binary path or command name.
 *
 * The variable is stored in the shell's hash table and `env` array.
 *
 * @param cmd Pointer to the command structure.
 * @param binary_path The resolved binary path of the command (if applicable).
 */
void	update_underscore(t_cmd *cmd, char *binary_path)
{
	char	*val;
	int		to_free;

	if (!cmd || !cmd->argv || !cmd->argv[0])
		return ;
	to_free = 0;
	if (is_builtin(cmd))
		val = get_last_arg(cmd, &to_free);
	else
	{
		if (cmd->argv[1])
			val = get_last_arg(cmd, &to_free);
		else
		{
			if (binary_path)
				val = binary_path;
			else
				val = cmd->argv[0];
		}
	}
	if (val)
		if(set_variable(cmd->minishell, "_", val, 1) != EXIT_SUCCESS)
		{
			print_error("-minishell: update_underscore, set_variable failed\n");
			return ;
		}
	if (to_free && val)
		free(val);
	if (update_env(cmd->minishell) != EXIT_SUCCESS)
	{
		print_error("-minishell: update_underscore, update_env failed\n");
		return ;
	}
}
