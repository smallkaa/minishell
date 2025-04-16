#include "minishell.h"

static int	count_total_keys(t_hash_tbl *ht)
{
	int				i;
	int				total_keys;
	t_mshell_var	*var;

	i = 0;
	total_keys = 0;
	while (i < HASH_SIZE)
	{
		var = ht->buckets[i];
		while (var)
		{
			total_keys++;
			var = var->next;
		}
		i++;
	}
	return (total_keys);
}

static bool	allocate_keys_array(t_hash_tbl *ht, char ***keys, int *count)
{
	if (!ht || !keys || !count)
		return (false);
	*count = count_total_keys(ht);
	*keys = malloc(sizeof(char *) * (*count));
	if (!(*keys))
	{
		print_error("minishell: export: key memory allocation failed\n");
		return (false);
	}
	return (true);
}

static void	fill_keys_from_hash(t_hash_tbl *ht, char **keys)
{
	t_mshell_var	*var;
	int				i;
	int				key_index;

	i = 0;
	key_index = 0;
	while (i < HASH_SIZE)
	{
		var = ht->buckets[i];
		while (var)
		{
			keys[key_index++] = ft_strdup(var->key);
			var = var->next;
		}
		i++;
	}
}

void	collect_keys(t_hash_tbl *ht, char ***keys, int *count)
{
	if (!allocate_keys_array(ht, keys, count))
		return ;
	fill_keys_from_hash(ht, *keys);
}
