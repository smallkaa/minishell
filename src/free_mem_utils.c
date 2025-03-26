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
void	free_env(char **env)
{
	int	i;

	if (!env)
		return ;
	i = 0;
	while (env[i])
	{
		free(env[i]);
		i++;
	}
	free(env);
	env = NULL;
}

void free_mshell_var(t_mshell_var *var)
{
	free(var->key);
	free(var->value);
	free(var);
	var = NULL;
}

/**
 * @brief Frees the hash table storing environment variables.
 *
 * Iterates through each bucket in the hash table, freeing all stored variables,
 * and then deallocates the hash table itself.
 *
 * @param hash_table The hash table to free.
 */
void	free_hash_table(t_hash_table *hash_table)
{
	int				i;	t_mshell_var	*current;
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
	hash_table = NULL;
}

/**
 * @brief Frees the built-in command list.
 *
 * Iterates through the `builtin` array, freeing each allocated string,
 * and then frees the array itself.
 *
 * @param builtin The array of built-in command names to free.
 */
void	free_builtin(char **builtin)
{
	int	i;

	if (!builtin)
		return ;
	i = 0;
	while (builtin[i])
	{
		free(builtin[i]);
		i++;
	}
	free(builtin);
	builtin = NULL;
}


void	free_cmd(t_cmd *cmd)
{
	int	i;

	if (!cmd)
		return;
	if (cmd->argv)
	{
		i = 0;
		while (cmd->argv[i])
		free(cmd->argv[i++]);
		free(cmd->argv);
	}
	if (cmd->binary)
		free(cmd->binary);
	if (cmd->in_redir)
		free(cmd->in_redir);
	if (cmd->out_redir)
		free(cmd->out_redir);
	if (cmd->next)
		free_cmd(cmd->next);
	cmd->minishell = NULL;
	free(cmd);
	cmd = NULL;
}
void	free_minishell(t_mshell *minishell)
{
	if (!minishell)
		return ;
	free_env(minishell->env);
	free_hash_table(minishell->hash_table);
	free_builtin(minishell->builtin);
	minishell = NULL;
}
