#include "minishell.h"

static char	*join_path_with_command(const char *path, const char *cmd_name)
{
	char	*temp;
	char	*full_path;

	temp = ft_strjoin(path, "/");
	if (!temp)
	{
		print_error("Error (assign_binary): ft_strjoin failed\n");
		return (NULL);
	}
	full_path = ft_strjoin(temp, cmd_name);
	free(temp);
	if (!full_path)
		print_error("Error (assign_binary): ft_strjoin failed\n");
	return (full_path);
}

static uint8_t	check_binary_access(char *binary, t_cmd *cmd)
{
	(void)cmd;
	if (access(binary, F_OK) != 0)
	{
		free(binary);
		cmd->minishell->exit_status = 127;
		return (EXIT_FAILURE);
	}
	if (access(binary, X_OK) != 0)
	{
		free(binary);
		cmd->minishell->exit_status = 126;
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}

static char	*assign_binary(char *path, t_cmd *cmd)
{
	char	*binary;

	binary = join_path_with_command(path, cmd->argv[0]);
	if (!binary)
	{
		cmd->minishell->exit_status = 127;
		return (NULL);
	}
	if (check_binary_access(binary, cmd) != EXIT_SUCCESS)
		return (NULL);
	if (cmd->binary)
		free(cmd->binary);
	cmd->minishell->exit_status = EXIT_SUCCESS;
	return (binary);
}

char	*search_paths(char **paths, t_cmd *cmd)
{
	int		i;
	char	*binary;

	i = 0;
	while (paths[i])
	{
		binary = assign_binary(paths[i], cmd);
		if (binary)
			return (binary);
		i++;
	}
	return (NULL);
}
