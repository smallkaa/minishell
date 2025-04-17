#include "minishell.h"

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
		set_variable(cmd->minishell, "_", val, 1);
	if (to_free && val)
		free(val);
	update_env(cmd->minishell);
}
