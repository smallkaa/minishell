#include "minishell.h"

/**
 * @brief Assigns a valid binary path to a command.
 *
 * Constructs a full binary path by joining the given directory path
 * with the command name. Checks if the binary exists and is executable.
 * If valid, assigns it to the command structure.
 *
 * @param path The directory path where the binary might be located.
 * @param cmd Pointer to the command structure.
 * @return EXIT_SUCCESS if binary is found and executable,
 *         127 if not found,
 *         126 if found but not executable.
 */
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
		binary = ft_strdup(cmd->argv[0]);
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

/**
 * @brief Handles commands with direct paths.
 *
 * Checks if the command provided is an absolute or relative path
 * and verifies its existence and permissions.
 * Prints appropriate error messages if execution is not allowed.
 *
 * @param cmd Pointer to the command structure.
 * @return EXIT_SUCCESS if executable,
 *         126 if permission is denied,
 *         127 if the command is not found.
 */
static int	handle_direct_path(t_cmd *cmd)
{
	int	status;

	status = assign_binary("", cmd);
	if (status == EXIT_SUCCESS)
		return (status);
	if (status == 126)
	{
		print_error(cmd->argv[0]);
		return (status);
	}
	print_error(cmd->argv[0]);
	return (127);
}

/**
 * @brief Searches for a command binary in multiple paths.
 *
 * Iterates through the directories in the system PATH variable
 * and attempts to assign a valid binary for execution.
 *
 * @param paths Array of directory paths.
 * @param cmd Pointer to the command structure.
 * @return EXIT_SUCCESS if a valid binary is found,
 *         127 if not found.
 */
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

/**
 * @brief Handles binary search in system paths.
 *
 * Retrieves system PATH environment variable and searches for the
 * command in each directory. If not found, prints an error message.
 *
 * @param cmd Pointer to the command structure.
 * @return EXIT_SUCCESS if binary is found,
 *         126 if permission is denied,
 *         127 if command is not found.
 */
static int	handle_path_search(t_cmd *cmd)
{
	int		status;
	char	**paths;

	paths = ft_split(getenv("PATH"), ':');
	if (!paths)
	{
		print_error(cmd->argv[0]);
		return (127);
	}
	status = search_paths(paths, cmd);
	ft_free_arrstrs(paths);
	if (status == 126)
		print_error(cmd->argv[0]);
	else if (status == 127)
		print_error(cmd->argv[0]);
	return (status);
}

/**
 * @brief Finds and assigns the binary path for a given command.
 *
 * Determines whether the command is provided as a direct path or
 * needs to be searched within the system PATH directories.
 * Calls appropriate functions for handling each case.
 *
 * @param cmd Pointer to the command structure.
 * @return EXIT_SUCCESS if binary is found and executable,
 *         126 if permission is denied,
 *         127 if command is not found.
 */
void	find_binary(t_cmd *cmd)
{
	int	status;

	if (!is_builtin(cmd))
	{
		if (!cmd->argv[0])
		{
			print_error(cmd->argv[0]);
			status = 127;
			update_last_exit_status(cmd, status);
		}
		if (cmd->argv[0][0] == '/' || cmd->argv[0][0] == '.')
			status = handle_direct_path(cmd);
		else
			status = handle_path_search(cmd);
		update_last_exit_status(cmd, status);
	}
}
