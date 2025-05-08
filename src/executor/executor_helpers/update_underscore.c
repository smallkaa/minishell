/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   update_underscore.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ilia Munaev <ilyamunaev@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 14:46:35 by Ilia Munaev       #+#    #+#             */
/*   Updated: 2025/05/08 10:06:32 by Ilia Munaev      ###   ########.fr       */
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
 * @brief Determines the underscore variable value based on the command.
 *
 * Logic varies depending on whether the command is a built-in or external.
 * Falls back to binary path or command name if no argument is available.
 *
 * @param cmd Pointer to the command.
 * @param binary_path Optional binary path.
 * @param to_free Pointer to int for tracking ownership of returned value.
 * @return Resolved string for underscore variable.
 */
static char	*resolve_underscore_value(t_cmd *cmd, char *binary_path,
		int *to_free)
{
	if (is_builtin(cmd))
		return (get_last_arg(cmd, to_free));
	if (cmd->argv[1])
		return (get_last_arg(cmd, to_free));
	if (binary_path)
		return (binary_path);
	return (cmd->argv[0]);
}

/**
 * @brief Updates the special shell variable `_` with the last command argument.
 *
 * Mimics Bash behavior. Resolves what `_` should be set to and assigns it
 * to the environment. Also updates `env` array after setting the variable.
 *
 * @param cmd Pointer to the command structure.
 * @param binary_path The resolved binary path of the command.
 */
void	update_underscore(t_cmd *cmd, char *binary_path)
{
	char	*val;
	int		to_free;

	if (!cmd || !cmd->argv || !cmd->argv[0])
		return ;
	to_free = 0;
	val = resolve_underscore_value(cmd, binary_path, &to_free);
	if (val && set_variable(cmd->minishell, "_", val, 1) != EXIT_SUCCESS)
	{
		print_error("-minishell: update_underscore, set_variable failed");
		return ;
	}
	if (to_free && val)
		free(val);
	if (update_env(cmd->minishell) != EXIT_SUCCESS)
		print_error("-minishell: update_underscore, update_env failed");
}
