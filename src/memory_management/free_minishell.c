/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_minishell.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ilia Munaev <ilyamunaev@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 14:49:41 by Ilia Munaev       #+#    #+#             */
/*   Updated: 2025/05/10 22:06:16 by Ilia Munaev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file free_minishell.c
 * @brief Functions for freeing Minishell environment and memory resources.
 */
#include "minishell.h"

void	free_env(char ***env_ptr)
{
	char	**env;
	int		i;

	if (!env_ptr || !*env_ptr)
		return ;
	env = *env_ptr;
	i = 0;
	while (env[i])
	{
		free(env[i]);
		i++;
	}
	free(env);
	*env_ptr = NULL;
}

/**
 * @brief Frees a single `t_mshell_var` variable structure.
 *
 * Frees both the key and value strings and the structure itself.
 *
 * @param var The environment variable structure to free.
 */
void	free_mshell_var(t_mshell_var *var)
{

	if (!var)
	return ;
	if (var->key)
		free(var->key);
	if (var->value)
		free(var->value);
	free(var);
}

/**
 * @brief Frees all memory used by the shell’s hash table.
 *
 * Iterates over each bucket in the hash table and frees all
 * linked environment variable structures.
 *
 * @param hash_table Pointer to the hash table to free.
 */
void	free_hash_table(t_hash_tbl *hash_table)
{
	int				i;
	t_mshell_var	*current;
	t_mshell_var	*temp;

	if (!hash_table)
		return ;
	i = 0;
	while (i < HASH_SIZE)
	{
		current = hash_table->buckets[i];
		while (current)
		{
			temp = current;
			current = current->next;
			free_mshell_var(temp);
		}
		i++;
	}
	free(hash_table);
}

/**
 * @brief Frees the entire `t_mshell` structure and its resources.
 *
 * This includes:
 * - The environment variable array (`env`)
 * - The hash table (`hash_table`)
 * - The structure itself
 *
 * @param minishell The Minishell structure to free.
 */
void	free_minishell(t_mshell **minishell_ptr)
{
	t_mshell	*minishell;

	if (!minishell_ptr || !*minishell_ptr)
		return ;
	minishell = *minishell_ptr;
	if (minishell->env)
	{
		free_env(&minishell->env);
		minishell->env = NULL;
	}
	if (minishell->hash_table)
	{
		free_hash_table(minishell->hash_table);
		minishell->hash_table = NULL;
	}
	free(minishell);
	*minishell_ptr = NULL;
}
