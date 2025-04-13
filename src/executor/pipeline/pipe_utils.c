#include "minishell.h"

bool	is_minishell_executable(t_cmd *cmd)
{
	return (ft_strcmp(cmd->argv[0], "./minishell") == 0);
}

void	handle_empty_command(t_cmd *cmd)
{
	if (!cmd->redirs)
	{
		print_error("Command '' not found\n");
		_exit(127);
	}
	_exit(EXIT_SUCCESS);
}

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
	set_variable(cmd->minishell, "SHLVL", new_shlvl, 1);
	free(new_shlvl);
	update_env(cmd->minishell);
	return (EXIT_SUCCESS);
}

uint8_t	validate_dots(t_cmd *cmd)
{
	if (ft_strcmp(cmd->argv[0], ".") == 0 && !cmd->argv[1])
	{
		print_error("minishell: .: filename argument required\n");
		print_error(".: usage: . filename [arguments]\n");
		return (2);
	}
	if (ft_strcmp(cmd->argv[0], "..") == 0 && !cmd->argv[1])
	{
		print_error("..: command not found\n");
		return (127);
	}
	return (EXIT_SUCCESS);
}
