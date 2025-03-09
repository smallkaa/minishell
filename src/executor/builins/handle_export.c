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

static void	print_export_from_ht(t_mshell *mshell)
{
	t_hash_table	*ht;
	t_mshell_var	*var;
	int				i;

	ht = mshell->hash_table;
	i = 0;
	while (i < HASH_SIZE)
	{
		var = ht->buckets[i];
		while (var)
		{
			if (var->exported)
			{
				if (var->value)
					printf("declare -x %s=\"%s\"\n", var->key, var->value);
				else
					printf("declare -x %s\n", var->key);
			}
			var = var->next;
		}
		i++;
	}
}

static uint8_t	process_export_arguments(t_cmd *cmd, char **key_value_pairs)
{
	int		i;
	uint8_t	exit_status;

	exit_status = EXIT_SUCCESS;
	i = 1;
	while (key_value_pairs[i])
	{
		if (!is_valid_varname(key_value_pairs[i]))
		{
			print_error("minishell: not a valid identifier\n");
			exit_status = EXIT_FAILURE;
			i++;
			continue ;
		}
		handle_var(cmd, key_value_pairs[i]);
		if (cmd->in_pipe)
			exit(exit_status);
		i++;
	}
	return (exit_status);
}

static uint8_t	print_exported_variables(t_cmd *cmd)
{
	print_export_from_ht(cmd->minishell);
	cmd->minishell->exit_status = EXIT_SUCCESS;
	if (cmd->in_pipe)
		exit(EXIT_SUCCESS);
	return (EXIT_SUCCESS);
}

uint8_t	handle_export(t_cmd *cmd)
{
	if (!cmd->argv[1])
	{
		return (print_exported_variables(cmd));
	}
	return (process_export_arguments(cmd, cmd->argv));
}
