/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setup_hash_table.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imunaev- <imunaev-@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 14:50:27 by Ilia Munaev       #+#    #+#             */
/*   Updated: 2025/05/12 13:59:11 by imunaev-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	set_pwd_and_shlvl(t_mshell *mshell, char *working_dir)
{
	if (set_variable(mshell, "PWD", working_dir, 1) != EXIT_SUCCESS)
	{
		print_error("-minishell: set PWD failed\n");
		return (EXIT_FAILURE);
	}
	if (update_shlvl_setup_no_env(mshell) != EXIT_SUCCESS)
	{
		print_error("-minishell: update_shlvl_setup_no_env failed\n");
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}

static int	add_oldpwd_from_home(t_mshell *mshell)
{
	char	*home;
	char	working_dir[MS_PATHMAX];

	if (!mshell)
		return (EXIT_FAILURE);
	home = ms_getenv(mshell, "HOME");
	if (!home)
	{
		print_error("-minishell: env: No such file or directory\n");
		home = getcwd(working_dir, MS_PATHMAX);
		if (!home)
		{
			print_error("-minishell: getcwd, get working_dir failed\n");
			return (EXIT_FAILURE);
		}
		return (set_pwd_and_shlvl(mshell, home));
	}
	if (set_variable(mshell, "OLDPWD", home, 1) != EXIT_SUCCESS)
	{
		print_error("-minishell: add_oldpwd_from_home, set OLDPWD failed\n");
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

	ht = malloc(sizeof(t_hash_tbl));
	if (!ht)
	{
		print_error("-minishell: hash_table malloc failed\n");
		return (NULL);
	}
	ft_memset(ht, 0, sizeof(t_hash_tbl));
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
	{
		free_hash_table(mshell->hash_table);
		mshell->hash_table = NULL;
		return (EXIT_FAILURE);
	}
	if (update_env(mshell) != EXIT_SUCCESS)
		return (EXIT_FAILURE);
	return (EXIT_SUCCESS);
}
