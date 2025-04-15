#include "minishell.h"

// char	*assign_binary(char *path, t_cmd *cmd)
// {
// 	char	*binary;
// 	char	*temp;

// 	temp = ft_strjoin(path, "/");
// 	if (!temp)
// 	{
// 		print_error("Error (assign_binary): ft_strjoin failed\n");
// 		return (NULL);
// 	}
// 	binary = ft_strjoin(temp, cmd->argv[0]);
// 	free(temp);
// 	if (!binary)
// 	{
// 		cmd->minishell->exit_status = 127;
// 		return (NULL);
// 	}
// 	if (access(binary, F_OK) != 0)
// 	{
// 		free(binary);
// 		cmd->minishell->exit_status = 127;
// 		return (NULL);
// 	}
// 	if (access(binary, X_OK) != 0)
// 	{
// 		free(binary);
// 		cmd->minishell->exit_status = 126;
// 		return (NULL);
// 	}
// 	if (cmd->binary)
// 		free(cmd->binary);
// 	cmd->minishell->exit_status = EXIT_SUCCESS;
// 	return (binary);
// }

static char	*handle_direct_path(t_cmd *cmd)
{

	// printf("DEBUG: handle_direct_path: start func\n");
	char	*binary;

	binary = ft_strdup(cmd->argv[0]);
	// if (!binary)
	// {
	// 	printf("DEBUG: handle_direct_path: !binary\n");

	// 	if (access(cmd->argv[0], F_OK) != 0)
	// 	{
	// 		cmd->minishell->exit_status = 127;
	// 		return (NULL);
	// 	}
	// 	if (access(cmd->argv[0], X_OK) != 0)
	// 	{
	// 		cmd->minishell->exit_status = 126;
	// 		return (NULL);
	// 	}
	// 	return (NULL);
	// }
	// printf("DEBUG: handle_direct_path: binary found\n");
	// printf("DEBUG: handle_direct_path: binary is: %s\n", binary);

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
