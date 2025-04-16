/**
 * @file free_mem_utils.c
 * @brief Functions to free allocated memory in Minishell.
 */
#include "minishell.h"

/**
 * @brief Frees the environment variable array.
 *
 * Iterates through the `env` array, freeing each allocated string,
 * and then frees the array itself.
 *
 * @param env The array of environment variables to free.
 */
void free_env(char **env)
{
	int i;

	if (!env)
		return;
	i = 0;
	while (env[i])
	{
		free(env[i]);
		i++;
	}
	free(env);
}

void free_mshell_var(t_mshell_var *var)
{
	if (!var)
		return;
	free(var->key);
	free(var->value);
	free(var);
}

/**
 * @brief Frees the hash table storing environment variables.
 *
 * Iterates through each bucket in the hash table, freeing all stored variables,
 * and then deallocates the hash table itself.
 *
 * @param hash_table The hash table to free.
 */
void free_hash_table(t_hash_tbl *hash_table)
{
	int i;
	t_mshell_var *current;
	t_mshell_var *temp;

	if (!hash_table)
		return;
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

void free_cmd(t_cmd *cmd)
{
	t_cmd *next;
	int i;
	if (!cmd)
		return ;
	while (cmd)
	{
		i = 0;
		if (cmd->argv)
		{
			while (cmd->argv[i])
				free(cmd->argv[i++]);
			free(cmd->argv);
		}
		if (cmd->binary)
			free(cmd->binary);
		next = cmd->next;
		free(cmd);
		cmd = next;
	}
}

void free_minishell(t_mshell *minishell)
{
	if (!minishell)
		return ;
	if (minishell->env)
	{
		free_env(minishell->env);
		minishell->env = NULL;
	}
	if (minishell->hash_table)
	{
		free_hash_table(minishell->hash_table);
		minishell->hash_table = NULL;
	}
	free(minishell);
	minishell = NULL;
}
