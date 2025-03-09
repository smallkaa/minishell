#include "minishell.h"

static uint8_t	print_env_variables(t_hash_table *hash_table)
{
	t_mshell_var	*var;
	int				i;

	i = 0;
	while (i < HASH_SIZE)
	{
		var = hash_table->buckets[i];
		while (var)
		{
			if (var->val_assigned)
			{
				if (var->value)
					printf("%s=%s\n", var->key, var->value);
				else
					printf("%s=\n", var->key);
			}
			var = var->next;
		}
		i++;
	}
	return (EXIT_SUCCESS);
}

uint8_t	handle_env(t_cmd *cmd)
{
	t_hash_table	*hash_table;
	uint8_t			exit_status;

	hash_table = cmd->minishell->hash_table;
	exit_status = print_env_variables(hash_table);
	cmd->minishell->exit_status = exit_status;
	if (cmd->in_pipe)
		exit (exit_status);
	return (exit_status);
}
