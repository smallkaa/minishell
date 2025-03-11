#include "minishell.h"

/**
 * @brief Checks if the given command is an absolute or relative path.
 *
 * This function verifies whether the command is specified as
 * an absolute (`/bin/ls`) or relative (`./script.sh`) path.
 * 	It duplicates the command string and checks:
 *  - If the file exists (`F_OK`).
 *  - If the file is executable (`X_OK`).
 * If either check fails, it sets an appropriate exit status and returns NULL.
 *
 * @param cmd Pointer to the command structure.
 * @return Pointer to the valid binary path, or NULL if not found or
 *         not executable.
 */
static char	*handle_direct_path(t_cmd *cmd)
{
	char	*binary;

	binary = ft_strdup(cmd->argv[0]);
	if (!binary)
		return (NULL);
	if (access(binary, F_OK) != 0)
	{
		free(binary);
		cmd_error_handler(cmd);
		cmd->minishell->exit_status = 127;
		return (NULL);
	}
	if (access(binary, X_OK) != 0)
	{
		free(binary);
		cmd_error_handler(cmd);
		cmd->minishell->exit_status = 126;
		return (NULL);
	}
	return (binary);
}

/**
 * @brief Constructs a full executable path and checks its validity.
 *
 * Given a directory path, this function appends the command name to create a
 * full binary path (`/usr/bin/ls`), then checks:
 *  - If the file exists (`F_OK`).
 *  - If the file is executable (`X_OK`).
 * If the command is valid, it returns the binary path. Otherwise, it sets
 * an appropriate exit status and returns NULL.
 *
 * @param path Directory in which to search for the command.
 * @param cmd Pointer to the command structure.
 * @return Pointer to the valid binary path, or
 *         NULL if not found or not executable.
 */
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

/**
 * @brief Searches for an executable in the directories listed in PATH.
 *
 * Iterates over each directory in the PATH environment variable and attempts
 * to construct a valid executable path using `assign_binary()`. If a valid
 * path is found, it is returned; otherwise, an error is set.
 *
 * @param paths Array of directories from the PATH environment variable.
 * @param cmd Pointer to the command structure.
 * @return Pointer to the found binary path, or NULL if not found.
 */
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
		i++;
	}
	return (NULL);
}

/**
 * @brief Searches for a command in the system's PATH environment variable.
 *
 * Retrieves the PATH environment variable and splits it into directories.
 * Then, it attempts to find the given command in one of those directories.
 * If successful, it returns the full binary path; otherwise, it returns NULL.
 *
 * @param cmd Pointer to the command structure.
 * @return Pointer to the found binary path, or NULL if not found.
 */
static char	*handle_path_search(t_cmd *cmd)
{
	char	*env;
	char	**paths;
	char	*binary;

	env = getenv("PATH");
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
/**
 * @brief Determines the executable path for a given command.
 *
 * This function checks whether the command is:
 *  - A built-in command (no binary path needed).
 *  - An absolute or relative path (`/bin/ls`, `./script.sh`).
 *  - Located in the system's PATH.
 * If found, the command's binary path is stored in `cmd->binary`.
 *
 * @param cmd Pointer to the command structure.
 * @return Pointer to the found binary path, or NULL if not found.
 */
char	*find_binary(t_cmd *cmd)
{
	char	*binary;

	if (!cmd || !cmd->argv || !cmd->argv[0])
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
	return binary;
}
