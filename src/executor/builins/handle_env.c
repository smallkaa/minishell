#include "minishell.h"



// Prints "KEY=VALUE" for each exported variable in the hash table
void	print_env_variables(t_hash_table *ht)
{
	t_mshell_var	*var;
	int				i;

	if (!ht)
		return;
	for (i = 0; i < HASH_SIZE; i++)
	{
		var = ht->buckets[i];
		while (var)
		{
			// Typically 'env' only shows exported variables
			if (var->exported)
			{
				// If there's no value (var->value == NULL), some shells print "KEY="
				// while others skip. It's your choice which behavior to follow.
				if (var->value)
					printf("%s=%s\n", var->key, var->value);
				else
					printf("%s=\n", var->key);
			}
			var = var->next;
		}
	}
}

void	handle_env(t_cmd *cmd)
{
	char			**env;
	t_hash_table	*hash_table;

	env = cmd->minishell->env;
	if (!env)
	{
		update_last_exit_status(cmd, EXIT_FAILURE);
		print_error("Error: no environment variables found\n");
		if (cmd->in_pipe)
			exit (EXIT_FAILURE);
		return ;
	}
	hash_table = cmd->minishell->hash_table;
	print_env_variables(hash_table);
	update_last_exit_status(cmd, EXIT_SUCCESS);
	if (cmd->in_pipe)
		exit (EXIT_SUCCESS);
	return ;
}
