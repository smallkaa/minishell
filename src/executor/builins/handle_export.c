#include "minishell.h"

/**
 * Returns the number of environment variables.
 */
int	env_size(char **envp)
{
	int	len = 0;

	while (envp[len])
		len++;
	return (len);
}

/**
 * Prints all exported variables in `declare -x VAR="value"` format.
 */
static void	print_export_exp_vars(char **exp_vars)
{
	int		i;
	char	*equal_sign;

	if (!exp_vars)
		return ;
	i = 0;
	while (exp_vars[i])
	{
		equal_sign = ft_strchr(exp_vars[i], '=');
		if (equal_sign)
			printf("declare -x %.*s=\"%s\"\n", (int)(equal_sign - exp_vars[i]),
				exp_vars[i], equal_sign + 1);
		else
			printf("declare -x %s\n", exp_vars[i]);
		i++;
	}
}

/**
 * Finds an exported variable by name.
 */
int	find_exp_var(char **exp_var, char *var)
{
	int		i;
	size_t	var_len;

	var_len = ft_strlen(var);
	i = 0;
	while (exp_var[i])
	{
		if (ft_strncmp(exp_var[i], var, var_len) == 0 && exp_var[i][var_len] == '\0')
			return (i);
		i++;
	}
	return (-1);
}

/**
 * Finds an environment variable by name.
 */
int	find_env_var(char **envp, char *var)
{
	int		i;
	size_t	var_len;

	var_len = ft_strlen(var);
	i = 0;
	while (envp[i])
	{
		if (ft_strncmp(envp[i], var, var_len) == 0 && envp[i][var_len] == '=')
			return (i);
		i++;
	}
	return (-1);
}

/**
 * Updates an exported variable list.
 */
void	update_exp_var(t_cmd *cmd, char *var)
{
	int		index;
	int		exp_var_len;
	char	**new_exp_var;

	if (!cmd->minishell->exp_vars)
		return ;

	index = find_exp_var(cmd->minishell->exp_vars, var);
	if (index >= 0)
	{
		free(cmd->minishell->exp_vars[index]);
		cmd->minishell->exp_vars[index] = ft_strdup(var);
		return ;
	}
	exp_var_len = env_size(cmd->minishell->exp_vars);
	new_exp_var = malloc((exp_var_len + 2) * sizeof(char *));
	if (!new_exp_var)
		return ;

	for (int i = 0; i < exp_var_len; i++)
		new_exp_var[i] = cmd->minishell->exp_vars[i];
	new_exp_var[exp_var_len] = ft_strdup(var);
	new_exp_var[exp_var_len + 1] = NULL;

	free(cmd->minishell->exp_vars);
	cmd->minishell->exp_vars = new_exp_var;
}

/**
 * Updates the environment variable list.
 */
void	update_env(t_cmd *cmd, char *var)
{
	int		index;
	int		envp_len;
	char	**new_envp;

	index = find_env_var(cmd->minishell->envp, var);
	if (index >= 0)
	{
		free(cmd->minishell->envp[index]);
		cmd->minishell->envp[index] = ft_strdup(var);
		return ;
	}

	envp_len = env_size(cmd->minishell->envp);
	new_envp = malloc((envp_len + 2) * sizeof(char *));
	if (!new_envp)
		return ;

	for (int i = 0; i < envp_len; i++)
		new_envp[i] = cmd->minishell->envp[i];
	new_envp[envp_len] = ft_strdup(var);
	new_envp[envp_len + 1] = NULL;

	free(cmd->minishell->envp);
	cmd->minishell->envp = new_envp;
}

/**
 * Adds or updates an environment variable.
 */
static void	add_or_update_env(t_cmd *cmd, char *var)
{
	if (ft_strchr(var, '='))
		update_env(cmd, var);
	else
		update_exp_var(cmd, var);
}

/**
 * Handles the `export` built-in command.
 */
void	handle_export(t_cmd *cmd)
{
	int i;

	// if (!cmd->minishell->envp)
	// {
	// 	update_last_exit_status(cmd, EXIT_FAILURE);
	// 	print_error("Error: handle_export, no environment variables found\n");
	// 	if (cmd->in_pipe)
	// 		exit(EXIT_FAILURE);
	// 	return ;
	// }

	if (!cmd->argv[1])
	{
		print_export_exp_vars(cmd->minishell->exp_vars);
		update_last_exit_status(cmd, EXIT_SUCCESS);
		if (cmd->in_pipe)
			exit(EXIT_SUCCESS);
		return ;
	}

	i = 1;
	while (cmd->argv[i])
	{
		add_or_update_env(cmd, cmd->argv[i]);
		i++;
	}
}
