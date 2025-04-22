/**
 * @file find_binary_utils.c
 * @brief Resolves the path to a binary by searching directories from $PATH.
 *
 * This file contains helper functions for locating a binary executable
 * by scanning each directory in the `PATH` environment variable.
 * It constructs full paths, checks access permissions, and ensures proper
 * memory management.
 */
#include "minishell.h"

/**
 * @brief Joins a directory path with a command name using a `/` separator.
 *
 * Combines the provided `path` (e.g., `/usr/bin`) and `cmd_name` (e.g., `ls`)
 * into a full path like `/usr/bin/ls`. Returns a newly allocated string.
 *
 * @param path Directory to prepend.
 * @param cmd_name Name of the command to append.
 * @return Heap-allocated full path string, or NULL on failure.
 */
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

/**
 * @brief Checks whether the binary exists and is executable.
 *
 * Verifies that the binary path is valid and has execute permission.
 * Sets the appropriate `exit_status` in the command if the check fails:
 * - 127 if file not found.
 * - 126 if not executable.
 *
 * @param binary Full path to the binary.
 * @param cmd Command structure (used to set exit status).
 * @return `EXIT_SUCCESS` if accessible, `EXIT_FAILURE` otherwise.
 */
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

/**
 * @brief Creates the full binary path and checks its accessibility.
 *
 * - Calls `join_path_with_command()` to build full path.
 * - Uses `check_binary_access()` to validate existence and permissions.
 * - Frees the existing `cmd->binary` if already set.
 *
 * @param path A directory from the PATH array.
 * @param cmd The command structure.
 * @return The valid full path to the binary, or NULL on failure.
 */
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

/**
 * @brief Searches for a binary in an array of PATH directories.
 *
 * Iterates over the `paths[]`, constructing and validating a full binary path
 * for each directory. Returns the first accessible binary path.
 *
 * @param paths Array of directories to search (e.g., from `$PATH`).
 * @param cmd Command structure.
 * @return Full binary path on success, or NULL if not found.
 */
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
