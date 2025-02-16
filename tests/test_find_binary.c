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
}	t_command;

int	assign_binary(char *path, t_command *cmd)
{
	char	*temp_path;
	char	*binary;

	temp_path = ft_strjoin(path, "/");
	binary = ft_strjoin(temp_path, cmd->argv[0]);
	free(temp_path);
	if (access(binary, F_OK) == 0)
	{
		cmd->binary = binary;
		return (EXIT_SUCCESS);
	}
	free(binary);
	return (EXIT_FAILURE);
}

int	find_binary(t_command *cmd)
{
	int		i;
	char 	**paths;

	paths = ft_split(getenv("PATH"), ':');
	if (!paths)
	{
		ft_putstr_fd("Error: find_binary() getenv failed.\n", STDERR_FILENO);
		return (EXIT_FAILURE);
	}
	i = 0;
	while (paths[i])
	{
		if(assign_binary(paths[i], cmd) == EXIT_SUCCESS)
		{
			ft_free_arrstrs(paths);
			return (EXIT_SUCCESS);
		}
		free(cmd->binary);
		i++;
	}
	ft_putstr_fd("Error: find_binary() no binary found.\n", STDERR_FILENO);
	ft_free_arrstrs(paths);
	return (EXIT_FAILURE);
}


void	test_find_binary(void)
{
	t_command	cmd;

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
	printf("Test 2 passed: 'wc' founf in PATH.\n");
	free(cmd.binary);

	// Test 3: Non-existing command
	char *argv3[] = {"nonexistentcommand", NULL};
	cmd.argv = argv3;
	cmd.binary = NULL;
	assert(find_binary(&cmd) == EXIT_FAILURE);
	printf("Test 3 passed: 'nonexistentcommand' not found in PATH.\n");

	// Test 4: Empty PATH variable
	setenv("PATH", "", 1);
	char *argv4[] = {"ls", NULL};
	cmd.argv = argv4;
	cmd.binary = NULL;
	assert(find_binary(&cmd) == EXIT_FAILURE);
	printf("Test 4 passed: Empty PATH leads to failure.\n");

	// Test 5: No PATH environment variable
	unsetenv("PATH");
	char *argv5[] = {"ls", NULL};
	cmd.argv = argv5;
	cmd.binary = NULL;
	assert(find_binary(&cmd) == EXIT_FAILURE);
	printf("Test 5 passed: Missing PATH environment variable handled correctly.\n");

	// Restore original PATH
	setenv("PATH", "/usr/bin:/bin:/usr/local/bin", 1);
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
