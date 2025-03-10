#include "minishell.h"

static int	assign_binary(char *path, t_cmd *cmd)
{
	char	*binary;
	char	*temp;
	if (path && *path)
	{
		temp = ft_strjoin(path, "/");
		binary = ft_strjoin(temp, cmd->argv[0]);
		free(temp);
	}
	else
	{
		binary = ft_strdup(cmd->argv[0]);
	}
	if (access(binary, F_OK))
	{
		free(binary);
		return (127);
	}
	if (access(binary, X_OK))
	{
		free(binary);
		return (126);
	}
	cmd->binary = binary;
	return (EXIT_SUCCESS);
}


static int	handle_direct_path(t_cmd *cmd)
{
	int	status;

	status = assign_binary("", cmd);
	if (status == EXIT_SUCCESS)
		return (status);
	else
	{
		cmd->binary = NULL;
		print_error(cmd->argv[0]);
		return (status);
	}
}

static int	search_paths(char **paths, t_cmd *cmd)
{
	int	i;
	int	status;

	i = 0;
	while (paths[i])
	{
		status = assign_binary(paths[i], cmd);
		if (status != 127)
			return (status);
		i++;
	}
	return (127);
}
char	*ft_getenv(t_cmd *cmd, char *env_name)
{
	int		i;
	size_t	name_len;

	name_len = ft_strlen(env_name);
	i = 0;
	while(cmd->minishell->env[i])
	{
		if (ft_strncmp(cmd->minishell->env[i], env_name, name_len) == 0 &&
			cmd->minishell->env[i][name_len] == '=')
		{
			return (cmd->minishell->env[i] + name_len + 1);
		}
		i++;
	}
	return (NULL);
}

static int	handle_path_search(t_cmd *cmd)
{
	int		status;
	char	*env;
	char	**paths;


	env = ft_getenv(cmd, "PATH");
	if (!env)
	{
		print_error("Error: handle_path_search, no PATH found\n");
		return(EXIT_FAILURE);
	}
	paths = ft_split(env, ':');
	if (!paths)
	{
		print_error(cmd->argv[0]);
		return (127);
	}
	status = search_paths(paths, cmd);

	ft_free_arrstrs(paths);
	return (status);
}

void	find_binary(t_cmd *cmd)
{
	int	status;

	if (!cmd)
	{
		print_error("Error (find_binary): no cmd found\n");
		return ;
	}
	if (!cmd->argv)
	{
		print_error("Error (find_binary): no cmd found\n");
		return ;
	}

	if (!cmd->argv[0])
	{
		cmd->binary = NULL;
		print_error("Error (find_binary): no cmd found\n");
		return ;
	}
	if (!is_builtin(cmd))
	{
		if (cmd->argv[0][0] == '/' || cmd->argv[0][0] == '.')
			status = handle_direct_path(cmd);
		else
			status = handle_path_search(cmd);

		update_last_exit_status(cmd, status);
	}
}
