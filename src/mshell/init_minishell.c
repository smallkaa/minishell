/**
 * @file init_minishell.c
 * @brief Initialization functions for the Minishell environment.
 */
#include "minishell.h"

/**
 * @brief Allocates memory for the Minishell structure.
 *
 * This function dynamically allocates memory for a `t_mshell` structure.
 * If the allocation fails, an error message is printed.
 *
 * @return A pointer to the allocated `t_mshell` structure, or NULL on failure.
 */
static t_mshell	*allocate_minishell(void)
{
	t_mshell	*mshell;

	mshell = malloc(sizeof(t_mshell));
	if (!mshell)
		print_error("Error (allocate_minishell): minishell malloc failed\n");
	return (mshell);
}

/**
 * @brief Initializes the environment variables.
 *
 * This function sets up the shell's environment by duplicating the
 * provided `envp` array. If no environment is provided, an error is printed.
 *
 * @param envp The array of environment variables from the system.
 * @return A newly allocated array of environment variables, or NULL on failure.
 */
static char	**init_env(char **envp)
{
	char	**env;

	if (!envp)
	{
		print_error("Error (init_env): no environ found\n");
		return (NULL);
	}
	env = setup_env(envp);
	if (!env)
		print_error("Error (init_env): failed to setup environment\n");
	return (env);
}

/**
 * @brief Sets up the shell environment.
 *
 * This function initializes the shell's environment variables and
 * sets up the hash table for storing environment variables.
 *
 * @param minishell Pointer to the `t_mshell` structure.
 * @param envp The array of environment variables from the system.
 * @return `EXIT_SUCCESS` on success, `EXIT_FAILURE` on failure.
 */
static int	setup_environment(t_mshell	*mshell, char **envp)
{
	mshell->env = init_env(envp);
	if (!mshell->env)
		return (EXIT_FAILURE);
	if (setup_hash_table(mshell) == EXIT_FAILURE)
		return (EXIT_FAILURE);
	return (EXIT_SUCCESS);
}

/**
 * @brief Initializes the Minishell structure.
 *
 * This function initializes the shell by:
 * - Allocating memory for the `t_mshell` structure.
 * - Setting up the environment variables.
 * - Initializing the hash table for variable storage.
 * - Loading built-in commands.
 *
 * If any initialization step fails, the allocated memory is
 * freed and NULL is returned.
 *
 * @param envp The array of environment variables from the system.
 * @return A pointer to the initialized `t_mshell` structure, or NULL on failure.
 */
t_mshell	*init_mshell(char **envp)
{
	t_mshell	*mshell;

	mshell = allocate_minishell();
	if (!mshell)
		return (NULL);
	if (setup_environment(mshell, envp) == EXIT_FAILURE)
	{
		free_minishell(mshell);
		return (NULL);
	}
	mshell->exit_status = 0;
	mshell->syntax_exit_status = 0;
	return (mshell);
}
