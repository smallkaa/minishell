#include "minishell.h"

void	handle_var(t_cmd *cmd, char *key_value_pair)
{
	t_mshell_var	*mshell_var;

	mshell_var = split_key_value(key_value_pair);
	if (!ft_strchr(key_value_pair, '='))
		set_variable(cmd->minishell, mshell_var, 0);
	else
		set_variable(cmd->minishell, mshell_var, 1);
}

void print_all_variables(t_hash_table *hash_table)
{
	int i;
	t_mshell_var *current;

	for (i = 0; i < HASH_SIZE; i++)
	{
		current = hash_table->buckets[i];
		while (current)
		{
			printf("  %s=%s\n", current->key, current->value ? current->value : "(NULL)");
			current = current->next;
		}
	}
}
#include "minishell.h"

void	handle_export(t_cmd *cmd)
{
	int		i;
	char	**key_value_pairs;

	if (!cmd || !cmd->minishell)
	{
		print_error("minishell: handle_export, no cmd or minishell found\n");
		update_last_exit_status(cmd, EXIT_FAILURE);
		return ;
	}
	key_value_pairs = cmd->argv;
	// If user typed just 'export' with no arguments
	if (!key_value_pairs[1])
	{
		// Print directly from hash table:
		print_export_from_ht(cmd->minishell);

		update_last_exit_status(cmd, EXIT_SUCCESS);
		if (cmd->in_pipe)
			exit(EXIT_SUCCESS);
		return ;
	}

	// Otherwise, process each argument to set/export them
	i = 1;
	while (key_value_pairs[i])
	{
		if (!is_valid_varname(key_value_pairs[i]))
		{
			print_error("minishell: export: not a valid identifier\n");
			update_last_exit_status(cmd, EXIT_FAILURE);
			i++;
			continue ;
		}
		// Insert variable (exported or not) into the hash table
		handle_var(cmd, key_value_pairs[i]);

		update_last_exit_status(cmd, EXIT_SUCCESS);
		if (cmd->in_pipe)
			exit(EXIT_SUCCESS);
		i++;
	}
}
