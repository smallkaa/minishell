#include "minishell.h"



// void print_all_variables(t_hash_table *hash_table)
// {
// 	int i;
// 	t_mshell_var *current;

// 	for (i = 0; i < HASH_SIZE; i++)
// 	{
// 		current = hash_table->buckets[i];
// 		while (current)
// 		{
// 			printf("  %s=%s\n", current->key, current->value ? current->value : "(NULL)");
// 			current = current->next;
// 		}
// 	}
// }

void	handle_var(t_cmd *cmd, char *key_value_pair)
{
	t_mshell_var	*mshell_var;

	mshell_var = split_key_value(key_value_pair);
	if (!ft_strchr(key_value_pair, '='))
		set_variable(cmd->minishell, mshell_var, 0);
	else
		set_variable(cmd->minishell, mshell_var, 1);
}

static bool	is_valid_varname(const char *key_value_pair)
{
	int	i;

	if (!key_value_pair)
		return (false);
	if (!(ft_isalpha(key_value_pair[0]) || key_value_pair[0] == '_'))
		return (false);
	i = 1;
	while (key_value_pair[i] && key_value_pair[i] != '=')
	{
		if (!(ft_isalnum(key_value_pair[i]) || key_value_pair[i] == '_'))
			return (false);
		i++;
	}
	return (true);
}

static void	print_export_from_ht(t_mshell *mshell)
{
	t_hash_table	*ht;
	t_mshell_var	*var;
	int				i;

	ht = mshell->hash_table;
	i = 0;
	while(i < HASH_SIZE)
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

uint8_t	handle_export(t_cmd *cmd)
{
	int		i;
	char	**key_value_pairs;
	uint8_t	exit_status;

	exit_status = EXIT_SUCCESS;
	key_value_pairs = cmd->argv;
	if (!key_value_pairs[1])
	{
		print_export_from_ht(cmd->minishell);
		cmd->minishell->exit_status = exit_status;
		if (cmd->in_pipe)
			exit(exit_status);
		return (exit_status);	}

	i = 1;
	while (key_value_pairs[i])
	{
		if (!is_valid_varname(key_value_pairs[i]))
		{
			error_handler(cmd);
			// print_error("minishell: export: not a valid identifier\n");
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
