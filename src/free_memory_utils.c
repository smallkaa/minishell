#include "minishell.h"

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
}

void	free_hash_table(t_hash_table *hash_table)
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
			free(temp->key);
			free(temp->value);
			free(temp);
		}
		i++;
	}
	free(hash_table);
}

void	free_minishell(t_mshell *minishell)
{
	if (!minishell)
		return ;
	free_env(minishell->env);
	free_hash_table(minishell->hash_table);
	free(minishell);
}
