/**
 * @file setup_builtin.c
 * @brief Built-in command setup for Minishell.
 */
#include "minishell.h"

/**
 * @brief Initializes the list of built-in shell commands.
 *
 * This function allocates memory for an array of built-in command names
 * and populates it with supported shell commands. The last element is set to NULL.
 *
 * The built-in commands include:
 * - `echo`
 * - `cd`
 * - `pwd`
 * - `export`
 * - `unset`
 * - `env`
 * - `exit`
 *
 * @return A dynamically allocated array of built-in command names, or NULL on failure.
 */
char	**setup_builtin(void)
{
	char	**builtin;

	builtin = malloc(8 * sizeof(char *));
	if (!builtin)
	{
		print_error("-minishell: setup_builtin: builtin malloc failed\n");
		return (NULL);
	}
	builtin[0] = ft_strdup("echo");
	builtin[1] = ft_strdup("cd");
	builtin[2] = ft_strdup("pwd");
	builtin[3] = ft_strdup("export");
	builtin[4] = ft_strdup("unset");
	builtin[5] = ft_strdup("env");
	builtin[6] = ft_strdup("exit");
	builtin[7] = NULL;
	return (builtin);
}
