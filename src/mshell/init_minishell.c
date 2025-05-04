/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_minishell.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ilia Munaev <ilyamunaev@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 14:50:46 by Ilia Munaev       #+#    #+#             */
/*   Updated: 2025/05/04 23:35:15 by Ilia Munaev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file init_minishell.c
 * @brief Initialization functions for the Minishell environment.
 *
 * This file contains functions responsible for allocating memory and setting up
 * the main `t_mshell` structure, including environment duplication and
 * hash table initialization.
 */
#include "minishell.h"

/**
 * @brief Allocates memory for the Minishell structure.
 *
 * This function dynamically allocates memory for a `t_mshell` structure.
 * If allocation fails, an error message is printed to `STDERR_FILENO`.
 *
 * @return A pointer to the allocated `t_mshell` structure, or NULL on failure.
 */
static t_mshell *allocate_minishell(void)
{
	t_mshell *mshell;

	mshell = malloc(sizeof(t_mshell));
	if (!mshell)
		print_error("Error (allocate_minishell): minishell malloc failed\n");
	return (mshell);
}

/**
 * @brief Initializes the environment variables.
 *
 * This function duplicates the provided environment (`envp`) array.
 * If `envp` is NULL or memory allocation fails during duplication, an error
 * is printed.
 *
 * @param envp The array of environment variables from the parent process.
 * @return A new environment array (heap-allocated), or NULL on failure.
 */
static char **init_env(char **envp)
{
	char **env;

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
 * @brief Sets up the shell's environment and hash table.
 *
 * This function:
 * - Duplicates the system environment variables.
 * - Initializes the shell's hash table from the copied environment.
 *
 * @param mshell Pointer to the `t_mshell` structure to initialize.
 * @param envp The environment array from the system.
 * @return `EXIT_SUCCESS` if setup completes, `EXIT_FAILURE` on error.
 */
static int setup_environment(t_mshell *mshell, char **envp)
{
	mshell->env = init_env(envp);
	if (!mshell->env)
		return (EXIT_FAILURE);
	if (setup_hash_table(mshell) == EXIT_FAILURE)
		return (EXIT_FAILURE);
	return (EXIT_SUCCESS);
}

/**
 * @brief Initializes the core `t_mshell` structure.
 *
 * This is the main initialization entry point for Minishell:
 * - Allocates the shell structure.
 * - Sets up environment variables and hash table.
 * - Initializes status values.
 *
 * If any of the steps fail, all allocated memory is freed and NULL is returned.
 *
 * @param envp Environment variables passed from the system.
 * @return Pointer to the initialized `t_mshell`, or NULL on failure.
 */
t_mshell *init_mshell(char **envp)
{
	t_mshell *mshell;

	mshell = allocate_minishell();
	if (!mshell)
		return (NULL);
	if (setup_environment(mshell, envp) == EXIT_FAILURE)
	{
		free_minishell(&mshell);
		return (NULL);
	}
	mshell->exit_status = 0;
	mshell->syntax_exit_status = 0;
	return (mshell);
}
