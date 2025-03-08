#include "minishell.h"

void	print_export_env(t_cmd *cmd)
{
	int		i;
	char	*equal_sign;
	char	**env;

	printf("[TEST]: Going to print env vars\n");


	env = cmd->minishell->env;
	if (!env)
		return ;
	i = 0;
	while (env[i])
	{
		equal_sign = ft_strchr(env[i], '=');
		if (equal_sign)
			printf("declare -x %.*s=\"%s\"\n", (int)(equal_sign - env[i]),
			env[i], equal_sign + 1);
		else
			printf("declare -x %s\n", env[i]);
		i++;
	}
}

void	print_local_var(t_cmd *cmd)
{
	int		i;
	char	**local_var;
	printf("[TEST]: Going to print local vars\n");

	local_var = cmd->minishell->local_var;
	if (!local_var)
		return ;
	i = 0;
	while (local_var[i])
	{
		printf("declare -x %s\n", local_var[i]);
		i++;
	}
}

bool	is_valid_varname(const char *key_value_pair)
{
	int	i;

	if (!key_value_pair)
		return (false);

	if (!(ft_isalpha(key_value_pair[0]) || key_value_pair[0] == '_'))
		return (false);
	i = 1;
	while (key_value_pair[i] && key_value_pair[i] != '=')
	{
		if (!(ft_isalnum(key_value_pair[i]) || key_value_pair[i] == '_'))
			return (false);
		i++;
	}
	return (true);
}
