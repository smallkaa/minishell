#include "minishell.h"

static char	*get_last_meaningful_arg(t_cmd *cmd)
{
	int	i;

	i = 0;
	while (cmd->argv[i])
		i++;
	if (i == 0)
		return (cmd->argv[0]);
	if ((ft_strcmp(cmd->argv[0], "export") == 0)
		&& ft_strchr(cmd->argv[i - 1], '='))
	{
		return (cmd->argv[0]);
	}
	return (cmd->argv[i - 1]);
}

void	update_underscore(t_cmd *cmd, char *binary_path)
{
	char *last_arg;

	if (!cmd || !cmd->argv || !cmd->argv[0])
		return ;
	if (is_builtin(cmd))
	{
		last_arg = get_last_meaningful_arg(cmd);
		if (last_arg)
			set_variable(cmd->minishell, "_", last_arg, 1);
	}
	else
	{
		if (binary_path)
			set_variable(cmd->minishell, "_", binary_path, 1);
		else
			set_variable(cmd->minishell, "_", cmd->argv[0], 1);
	}
	update_env(cmd->minishell);
}
