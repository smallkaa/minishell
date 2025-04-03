#include "minishell.h"

char	*handle_direct_path(t_cmd *cmd)
{
	char	*binary;

	binary = ft_strdup(cmd->argv[0]);
	if (!binary)
		return (NULL);

	if (access(binary, F_OK) != 0)
	{
		free(binary);
		cmd->minishell->exit_status = 127;
		return (NULL);
	}
	if (access(binary, X_OK) != 0)
	{
		free(binary);
		cmd->minishell->exit_status = 126;
		return (NULL);
	}
	return (binary);
}

char	*assign_binary(char *path, t_cmd *cmd)
{
	char	*binary;
	char	*temp;

	temp = ft_strjoin(path, "/");
	if (!temp)
	{
		print_error("Error (assign_binary): ft_strjoin failed\n");
		return (NULL);
	}
	binary = ft_strjoin(temp, cmd->argv[0]);
	free(temp);
	if (!binary)
	{
		cmd->minishell->exit_status = 127;
		return (NULL);
	}
	if (access(binary, F_OK) != 0)
	{
		free(binary);
		cmd->minishell->exit_status = 127;
		return (NULL);
	}
	if (access(binary, X_OK) != 0)
	{
		free(binary);
		cmd->minishell->exit_status = 126;
		return (NULL);
	}
	if (cmd->binary)
		free(cmd->binary);
	cmd->minishell->exit_status = EXIT_SUCCESS;
	return (binary);
}

static char	*search_paths(char **paths, t_cmd *cmd)
{
	int		i;
	char	*binary;

	i = 0;
	while (paths[i])
	{
		binary = assign_binary(paths[i], cmd);
		if (binary)
			return (binary);
		free(binary);
		i++;
	}
	return (NULL);
}

static char	*handle_path_search(t_cmd *cmd)
{
	char	*env;
	char	**paths;
	char	*binary;

	env = ms_getenv(cmd->minishell, "PATH");
	if (!env || !*env)
	{
		cmd->minishell->exit_status = 127;
		return (NULL);
	}
	paths = ft_split(env, ':');
	if (!paths)
	{
		cmd->minishell->exit_status = 127;
		return (NULL);
	}
	binary = search_paths(paths, cmd);
	ft_free_arrstrs(paths); // Free the array of paths
	return (binary);
}

char	*find_binary(t_cmd *cmd)
{
	char	*binary;

	if (!cmd || !cmd->argv || !cmd->argv[0] || ft_strlen(cmd->argv[0]) == 0)
	{
		cmd->minishell->exit_status = 127;
		return (NULL);
	}
	if (is_builtin(cmd))
		return (NULL);
	if (cmd->argv[0][0] == '/' || cmd->argv[0][0] == '.')
		binary = handle_direct_path(cmd);
	else
		binary = handle_path_search(cmd);
	return (binary);
}
