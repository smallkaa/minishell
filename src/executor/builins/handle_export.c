#include "minishell.h"

int	find_local_var(char **local_var, char *key_value_pair)
{
	int		i;
	size_t	key_value_pair_len;

	key_value_pair_len = ft_strlen(key_value_pair);
	i = 0;
	while (local_var && local_var[i])
	{
		if (ft_strncmp(local_var[i], key_value_pair, key_value_pair_len) == 0
			&& local_var[i][key_value_pair_len] == '\0')
			{
				return (i);
			}
		i++;
	}
	return (-1);
}

void	update_local_var(t_cmd *cmd, char *key_value_pair)
{
	int		index;
	int		i;
	int		local_var_len;
	char	**new_local_var;

	if (!cmd || !cmd->minishell)
	{
		print_error("minishell: update_local_var, no cmd or minishell found\n");
		update_last_exit_status(cmd, EXIT_FAILURE);
		return ;
	}
	local_var_len = arr_size(cmd->minishell->local_var);
	index = find_local_var(cmd->minishell->local_var, key_value_pair);
	if (index >= 0)
	{
		free(cmd->minishell->local_var[index]);
		cmd->minishell->local_var[index] = ft_strdup(key_value_pair);
		return;
	}
	new_local_var = malloc((local_var_len + 2) * sizeof(char *));
	if (!new_local_var)
	{
		print_error("minishell: update_local_var, new_local_var malloc failed\n");
		update_last_exit_status(cmd, EXIT_FAILURE);
		return ;
	}
	i = 0;
	while (i < local_var_len)
	{
		new_local_var[i] = cmd->minishell->local_var[i];
		i++;
	}
	new_local_var[i] = ft_strdup(key_value_pair);
	new_local_var[i + 1] = NULL;
	free(cmd->minishell->local_var);
	cmd->minishell->local_var = new_local_var;
}

int	find_env_var(char **env, char *key_value_pair)
{
	int		i;
	size_t	var_len;

	var_len = ft_strlen(key_value_pair);
	i = 0;
	while (env && env[i])
	{
		if (ft_strncmp(env[i], key_value_pair, var_len) == 0 && env[i][var_len] == '=')
			return (i);
		i++;
	}
	return (-1);
}

void	update_env(t_cmd *cmd, char *key_value_pair)
{
	int		index;
	int		i;
	int		env_len;
	char	**new_env;

	if (!cmd || !cmd->minishell)
	{
		print_error("minishell: update_env, no cmd or minishell found\n");
		update_last_exit_status(cmd, EXIT_FAILURE);
		return ;
	}
	env_len = arr_size(cmd->minishell->env);
	index = find_env_var(cmd->minishell->env, key_value_pair);
	if (index >= 0)
	{
		free(cmd->minishell->env[index]);
		cmd->minishell->env[index] = ft_strdup(key_value_pair);
		return ;
	}
	new_env = malloc((env_len + 2) * sizeof(char *));
	if (!new_env)
	{
		print_error("minishell: update_env, new_env malloc failed\n");
		update_last_exit_status(cmd, EXIT_FAILURE);
		return ;
	}
	i = 0;
	while (i < env_len)
	{
		new_env[i] = cmd->minishell->env[i];
		i++;
	}
	new_env[i] = ft_strdup(key_value_pair);
	printf("[TEST]: key_value_pair: %s, index: %d\n", new_env[i], i);
	new_env[i + 1] = NULL;
	free(cmd->minishell->env);
	cmd->minishell->env = new_env;
	printf("[TEST]: env: %s\n", new_env[47]);

}

void print_all(t_cmd *cmd)
{
	print_export_env(cmd);
	print_local_var(cmd);
}

void	handle_var(t_cmd *cmd, char *key_value_pair)
{
	if (!ft_strchr(key_value_pair, '='))
	{
		printf("Test: it's a local var\n");
		update_local_var(cmd, key_value_pair);
	}
	else
	{
		printf("Test: it's a env var\n");
		update_env(cmd, key_value_pair);
	}
}

void	handle_export(t_cmd *cmd)
{
	int		i;
	char	**key_value_pairs;

	if (!cmd || !cmd->minishell)
	{
		print_error("minishell: handle_export, no cmd or minishell found\n");
		update_last_exit_status(cmd, EXIT_FAILURE);
		return ;
	}
	key_value_pairs = cmd->argv;
	if (!key_value_pairs[1])
	{
		print_all(cmd);
		update_last_exit_status(cmd, EXIT_SUCCESS);
		if (cmd->in_pipe)
			exit(EXIT_SUCCESS);
		return ;
	}
	i = 1;
	while (key_value_pairs[i])
	{
		if (!is_valid_varname(key_value_pairs[i]))
		{
			// error_handler(cmd);
			print_error("minishell: export, not a valid identifier\n"); // fix
			update_last_exit_status(cmd, EXIT_FAILURE);
			i++;
			continue ;
		}
		handle_var(cmd, key_value_pairs[i]);
		update_last_exit_status(cmd, EXIT_SUCCESS);
		if (cmd->in_pipe)
			exit(EXIT_SUCCESS);
		i++;
	}
}
