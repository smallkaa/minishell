#include "minishell.h"

static char	*handle_direct_path(t_cmd *cmd)
{
	char	*binary;

	binary = ft_strdup(cmd->argv[0]);
	return (binary);
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
	ft_free_arrstrs(paths);
	return (binary);
}

char	*find_binary(t_cmd *cmd)
{
	char	*binary;
	char	*path_env;

	if (!cmd || !cmd->argv || !cmd->argv[0] || ft_strlen(cmd->argv[0]) == 0)
	{
		cmd->minishell->exit_status = 127;
		return (NULL);
	}
	if (is_builtin(cmd))
		return (NULL);
	if (cmd->argv[0][0] == '/' || cmd->argv[0][0] == '.')
		return (handle_direct_path(cmd));
	path_env = ms_getenv(cmd->minishell, "PATH");
	if (!path_env || path_env[0] == '\0')
	{
		if (access(cmd->argv[0], F_OK) == 0)
			return (ft_strdup(cmd->argv[0]));
		else
			return (NULL);
	}
	binary = handle_path_search(cmd);
	return (binary);
}
