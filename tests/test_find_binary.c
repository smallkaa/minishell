#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include "libft/libft.h"

typedef struct s_command
{
	char				**argv;
	char				*binary;
}	t_cmd;

static void	print_error(char *cmd, const char *msg)
{
	ft_putstr_fd(cmd, STDERR_FILENO);
	ft_putstr_fd((char *)msg, STDERR_FILENO);
}
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

static int	handle_direct_path(t_cmd *cmd)
{
	int	status;

	status = assign_binary("", cmd);
	if (status == EXIT_SUCCESS)
		return (status);
	if (status == 126)
	{
		print_error(cmd->argv[0], ": Permission denied\n");
		return (status);
	}
	print_error(cmd->argv[0], ": command not found\n");
	return (127);
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

static int	handle_path_search(t_cmd *cmd)
{
	int		status;
	char	**paths;

	paths = ft_split(getenv("PATH"), ':');
	if (!paths)
	{
		print_error(cmd->argv[0], ": command not found\n");
		return (127);
	}
	status = search_paths(paths, cmd);
	ft_free_arrstrs(paths);
	if (status == 126)
		print_error(cmd->argv[0], ": Permission denied\n");
	else if (status == 127)
		print_error(cmd->argv[0], ": command not found\n");
	return (status);
}

int	find_binary(t_cmd *cmd)
{
	if (!cmd->argv[0])
		return (127);
	if (cmd->argv[0][0] == '/' || cmd->argv[0][0] == '.')
		return (handle_direct_path(cmd));
	return (handle_path_search(cmd));
}

void	test_find_binary(void)
{
	t_cmd	cmd;

	// Test 1: Valid command
	char *argv1[] = {"ls", NULL};
	cmd.argv = argv1;
	cmd.binary = NULL;
	assert(find_binary(&cmd) == EXIT_SUCCESS);
	printf("Test 1 passed: 'ls' found in PATH.\n");
	free(cmd.binary);

	// Test 2: Valid command
	char *argv2[] = {"wc", NULL};
	cmd.argv = argv2;
	cmd.binary = NULL;
	assert(find_binary(&cmd) == EXIT_SUCCESS);
	printf("Test 2 passed: 'wc' found in PATH.\n");
	free(cmd.binary);

	// Test 3: Non-existing command (Triggers 127)
	char *argv3[] = {"nonexistentcommand", NULL};
	cmd.argv = argv3;
	cmd.binary = NULL;
	assert(find_binary(&cmd) == 127);
	printf("Test 3 passed: 'nonexistentcommand' correctly returns 127.\n");

	// Test 4: Empty PATH variable (Triggers 127)
	setenv("PATH", "", 1);
	char *argv4[] = {"ls", NULL};
	cmd.argv = argv4;
	cmd.binary = NULL;
	assert(find_binary(&cmd) == 127);
	printf("Test 4 passed: Empty PATH leads to failure (127).\n");

	// Test 5: No PATH environment variable (Triggers 127)
	unsetenv("PATH");
	char *argv5[] = {"ls", NULL};
	cmd.argv = argv5;
	cmd.binary = NULL;
	assert(find_binary(&cmd) == 127);
	printf("Test 5 passed: Missing PATH handled correctly (127).\n");

	// Restore original PATH
	setenv("PATH", "/usr/bin:/bin:/usr/local/bin", 1);

	// Test 6: Non-Executable File (Triggers 126)
	system("touch test_script.sh");   // Ensure file exists
	system("chmod 600 test_script.sh");

	char *argv6[] = {"./test_script.sh", NULL};
	cmd.argv = argv6;
	cmd.binary = NULL;

	assert(find_binary(&cmd) == 126);
	printf("Test 6 passed: Non-executable file returns 126.\n");

	// Cleanup
	system("rm test_script.sh");



}


// RUN
// cc -Wall -Wextra -Werror test_find_binary.c -Ilibft -Llibft -lft -o test_find_binary
// valgrind --leak-check=full ./test_find_binary
int	main(void)
{
	test_find_binary();
	printf("All tests done.\n");
	return (0);
}
