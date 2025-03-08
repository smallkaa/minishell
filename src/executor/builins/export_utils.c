#include "minishell.h"


void	print_export_from_ht(t_mshell *mshell)
{
	t_hash_table	*ht;
	t_mshell_var	*var;
	int				i;

	ht = mshell->hash_table;
	// printf("[TEST]: Printing export variables from hash table\n");

	for (i = 0; i < HASH_SIZE; i++)
	{
		var = ht->buckets[i];
		while (var)
		{
			// Only print if it's marked exported
			if (var->exported)
			{
				if (var->value)
					printf("declare -x %s=\"%s\"\n", var->key, var->value);
				else
					printf("declare -x %s\n", var->key);
			}
			var = var->next;
		}
	}
	// printf("[TEST]: END Printing export variables from hash table\n");

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
