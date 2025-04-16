/**
 * @file exec_builtin.c
 * @brief Function for handle built-in commands in Minishell.
 */
#include "minishell.h"

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
