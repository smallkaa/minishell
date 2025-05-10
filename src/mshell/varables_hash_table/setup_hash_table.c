/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setup_hash_table.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ilia Munaev <ilyamunaev@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 14:50:27 by Ilia Munaev       #+#    #+#             */
/*   Updated: 2025/05/10 03:11:02 by Ilia Munaev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Parses and inserts one env entry to hash table.
 *
 * @param mshell Shell instance.
 * @param entry Key=value environment string.
 */
static int	insert_env_var(t_mshell *mshell, char *entry)
{
	t_mshell_var	*tmp;

	tmp = split_key_value(entry);
	if (!tmp)
		return (EXIT_FAILURE);
	if (set_variable(mshell, tmp->key, tmp->value, 1) != EXIT_SUCCESS)
	{
		print_error("-minishell: failed to insert env var\n");
		return (EXIT_FAILURE);
	}
	free_pair_and_return_null(tmp);
	return (EXIT_SUCCESS);
}

/**
 * @brief Initializes SHLVL to 1 if not already present in the environment.
 *
 * @param minishell Shell instance.
 * @return EXIT_SUCCESS or EXIT_FAILURE.
 */
uint8_t	update_shlvl_setup_no_env(t_mshell *minishell)
{
	char	*str_shlvl;

	str_shlvl = ms_getenv(minishell, "SHLVL");
	if (str_shlvl == NULL)
	{
		if (set_variable(minishell, "SHLVL", "1", 1) != EXIT_SUCCESS)
		{
			print_error("-minishell: update_shlvl, set_variable failed\n");
			return (EXIT_FAILURE);
		}
	}
	return (EXIT_SUCCESS);
}

/**
 * @brief Ensures OLDPWD is initialized from HOME in the hash table.
 *
 * @param mshell Shell instance.
 * @return EXIT_SUCCESS or EXIT_FAILURE.
 */
static int	add_oldpwd_from_home(t_mshell *mshell)
{
	char	*home;
	char	working_dir[MS_PATHMAX];

	home = ms_getenv(mshell, "HOME");
	if (!home)
	{
		print_error("-minishell: env: No such file or directory\n");
		home = getcwd(working_dir, MS_PATHMAX);
		if (!home)
		{
			print_error("-minishell: getcwd, ger working_dir failed\n");
			return (EXIT_FAILURE);
		}
		set_variable(mshell, "PWD", home, 1);
		update_shlvl_setup_no_env(mshell);
		return (EXIT_SUCCESS);
	}
	if (set_variable(mshell, "OLDPWD", home, 1) != EXIT_SUCCESS)
	{
		print_error("-minishell: load_env_into_ht, set_variable failed\n");
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}

/**
 * @brief Loads the environment array into the hash table.
 *
 * Iterates over mshell->env and populates hash table.
 *
 * @param mshell Shell instance.
 * @return EXIT_SUCCESS or EXIT_FAILURE.
 */
static int	load_env_into_ht(t_mshell *mshell)
{
	int	i;

	i = 0;
	while (mshell->env[i])
	{
		if (insert_env_var(mshell, mshell->env[i]) != EXIT_SUCCESS)
			return (EXIT_FAILURE);
		i++;
	}
	return (add_oldpwd_from_home(mshell));
}

/**
 * @brief Initializes an empty hash table.
 *
 * Allocates and zeroes all bucket pointers.
 *
 * @return Pointer to hash table, or NULL on failure.
 */
static t_hash_tbl	*init_hash_tbl(void)
{
	t_hash_tbl	*ht;
	int			i;

	ht = malloc(sizeof(t_hash_tbl)); // tested
	if (!ht)
	{
		print_error("-minishell: hash_table malloc failed\n");
		return (NULL);
	}
	i = 0;
	while (i < HASH_SIZE)
		ht->buckets[i++] = NULL;
	return (ht);
}

/**
 * @brief Sets up the shell's hash table and populates it from env.
 *
 * @param mshell Shell instance.
 * @return EXIT_SUCCESS or EXIT_FAILURE.
 */
int	setup_hash_table(t_mshell *mshell)
{
	if (!mshell)
	{
		print_error("-minishell: no minishell structure\n");
		return (EXIT_FAILURE);
	}
	mshell->hash_table = init_hash_tbl();
	if (!mshell->hash_table)
		return (EXIT_FAILURE);
	if (load_env_into_ht(mshell) != EXIT_SUCCESS)
		return (EXIT_FAILURE);
	if (update_env(mshell) != EXIT_SUCCESS)
		return (EXIT_FAILURE);
	return (EXIT_SUCCESS);
}
