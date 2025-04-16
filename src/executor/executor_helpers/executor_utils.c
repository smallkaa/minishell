#include "minishell.h"

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

void	cleanup_and_exit(t_cmd *cmd, int exit_status)
{
	t_mshell	*minishell;

	if (!cmd)
		return ;
	minishell = cmd->minishell;
	free_cmd(cmd);
	free_minishell(minishell);
	rl_clear_history();
	exit(exit_status);
}

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
