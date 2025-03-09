#include "minishell.h"

t_mshell_var	*find_variable(t_hash_table *hash_table, char *key)
{
	unsigned int	index;
	t_mshell_var	*current;

	if (!hash_table || !key)
		return (NULL);
	index = hash_function(key);
	current = hash_table->buckets[index];
	while (current)
	{
		if (ft_strcmp(current->key, key) == 0)
			return (current);
		current = current->next;
	}
	return (NULL);
}

void	handle_var(t_cmd *cmd, char *key_value_pair)
{
	t_mshell_var	*mshell_var;
	t_mshell_var	*existing;
	char			*equal_sign;

	mshell_var = split_key_value(key_value_pair);
	equal_sign = ft_strchr(key_value_pair, '=');
	existing = find_variable(cmd->minishell->hash_table, mshell_var->key);
	if (equal_sign)
		set_variable(cmd->minishell, mshell_var, 1);
	else
	{
		if (existing)
			set_variable(cmd->minishell, mshell_var, 1);
		else
			set_variable(cmd->minishell, mshell_var, 0);
	}
	free(mshell_var->key);
	free(mshell_var->value);
	free(mshell_var);
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
			if (var->value)
				printf("declare -x %s=\"%s\"\n", var->key, var->value);
			else
				printf("declare -x %s\n", var->key);
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

uint8_t	handle_export(t_cmd *cmd)
{
	uint8_t	exit_status;

	if (!cmd->argv[1])
	{
		print_export_from_ht(cmd->minishell);
		cmd->minishell->exit_status = EXIT_SUCCESS;
		if (cmd->in_pipe)
			exit(EXIT_SUCCESS);
		return (EXIT_SUCCESS);
	}
	exit_status = process_export_arguments(cmd, cmd->argv);
	if (cmd->in_pipe)
		exit(exit_status);
	return (exit_status);
}
