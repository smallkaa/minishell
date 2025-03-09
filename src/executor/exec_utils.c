#include "minishell.h"

// void	cleanup_heredoc(t_cmd *cmd)
// {
// 	if (cmd->in_redir && cmd->in_redir->type == R_HEREDOC)
// 	{
// 		if (unlink(HEREDOC_TFILE) == -1)
// 			print_error_exit("unlink", EXIT_FAILURE);
// 	}
// }

void	update_last_exit_status(t_cmd *cmd, int status)
{
	cmd->minishell->exit_status = status;
}
bool	is_valid_varname(const char *key_value_pair)
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

int	count_exported_vars(t_hash_table *hash_table)
{
	int				i;
	int				count;
	t_mshell_var	*current;

	i = 0;
	count = 0;
	while (i < HASH_SIZE)
	{
		current = hash_table->buckets[i];
		while (current)
		{
			if (current->val_assigned)
				count++;
			current = current->next;
		}
		i++;
	}
	return (count);
}

t_mshell_var	*split_key_value(char *key_value_pair)
{
	t_mshell_var	*mshell_var;
	char			*equal_sign;

	mshell_var = malloc(sizeof(t_mshell_var));
	if (!mshell_var)
	{
		print_error("Error (split_key_value): mshell_var malloc failed\n");
		return (NULL);
	}
	equal_sign = ft_strchr(key_value_pair, '=');
	if (equal_sign)
	{
		mshell_var->key = ft_substr(key_value_pair, 0, equal_sign - key_value_pair);
		mshell_var->value = ft_strdup(equal_sign + 1);
	}
	else
	{
		mshell_var->key = ft_strdup(key_value_pair);
		mshell_var->value = NULL;
	}
	return (mshell_var);
}
