#include "minishell.h"

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
			if (current->exported)
				count++;
			current = current->next;
		}
		i++;
	}
	return (count);
}
void	update_env(t_mshell *minishell)
{
	int				count;
	char			**new_env;
	int				index;
	int				i;
	t_mshell_var	*current;

	count = count_exported_vars(minishell->hash_table);
	new_env = malloc((count + 1) * sizeof(char *));
	if (!new_env)
	{
		print_error("minishell: update_environ, malloc failed\n");
		return ;
	}
	i = 0;
	index = 0;
	while (i < HASH_SIZE)
	{
		current = minishell->hash_table->buckets[i];
		while (current)
		{
			if (current->exported)
			{
				// Safely handle NULL value
				const char *val = (current->value == NULL) ? "" : current->value;

				new_env[index] = malloc(
					ft_strlen(current->key) + ft_strlen(val) + 2
				);
				if (!new_env[index])
				{
					print_error("minishell: update_environ, malloc failed\n");
					while (--index >= 0)
						free(new_env[index]);
					free(new_env);
					return ;
				}
				// If value is NULL, we print key= instead of key=(null)
				sprintf(new_env[index], "%s=%s", current->key, val);
				index++;
			}
			current = current->next;
		}
		i++;
	}
	new_env[index] = NULL;

	// Free the old environment
	if (minishell->env)
	{
		i = 0;
		while (minishell->env[i])
		{
			free(minishell->env[i]);
			i++;
		}
		free(minishell->env);
	}

	// Update minishell struct
	minishell->env = new_env;
}



unsigned int	hash_function(const char *key)
{
	unsigned long	hash;
	int	c;

	hash = 5381;
	while ((c = *key++))
		hash = ((hash << 5) + hash) + c;
	return (hash % HASH_SIZE);
}
void set_variable(t_mshell *minishell, t_mshell_var *mshell_var, int exported)
{
	unsigned int	index;
	t_mshell_var	*current;
	t_hash_table	*hash_table;
	t_mshell_var	*new_var;

	index = hash_function(mshell_var->key);
	hash_table = minishell->hash_table;
	current = hash_table->buckets[index];

	// 🔥 Debugging: Print hash table index assignment
	// printf("[DEBUG] Hashing key: %s -> Bucket: %d\n", mshell_var->key, index);

	// Check if the variable already exists
	while (current)
	{
		if (ft_strcmp(current->key, mshell_var->key) == 0)
		{
			free(current->value);
			if(mshell_var->value)
				current->value = ft_strdup(mshell_var->value);
			else
				current->value = NULL;
			current->exported = exported;

			printf("[DEBUG] Updated var: %s=%s\n", current->key, current->value);
			if (exported)
				update_env(minishell);
			return;
		}
		current = current->next;
	}

	// 🟢 Create a new variable if not found
	new_var = malloc(sizeof(t_mshell_var));
	if (!new_var)
	{
		print_error("Error (set_variable): new_var malloc failed\n");
		return;
	}
	new_var->key = ft_strdup(mshell_var->key);
	new_var->value = ft_strdup(mshell_var->value);
	new_var->exported = exported;
	new_var->next = hash_table->buckets[index]; // 🟢 Store in hash table
	hash_table->buckets[index] = new_var;

	// // 🔥 Debugging: Confirm new addition
	// printf("[DEBUG] Added var: %s=%s -> Bucket: %d\n",
	// 	new_var->key, new_var->value, index);

	if (exported)
		update_env(minishell);
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
		// 🟢 Split into key and value
		mshell_var->key = ft_substr(key_value_pair, 0, equal_sign - key_value_pair);
		mshell_var->value = ft_strdup(equal_sign + 1);
	}
	else
	{
		mshell_var->key = ft_strdup(key_value_pair);
		mshell_var->value = NULL;
		// mshell_var->value = ft_strdup("");
	}

	return (mshell_var);
}

// fillup_hash_table with a function like:
void fillup_hash_table(t_mshell *minishell)
{
    int i = 0;
	t_mshell_var *mshell_var;

    while (minishell->env[i])
    {
        mshell_var = split_key_value(minishell->env[i]);
        if (mshell_var)
        {
            // pass exported=0 for initial load, so update_env is not triggered
            set_variable(minishell, mshell_var, 0);
            free(mshell_var->key);
            free(mshell_var->value);
            free(mshell_var);
        }
        i++;
    }
	// 2) Mark them all exported
	for (i = 0; i < HASH_SIZE; i++)
	{
		t_mshell_var *curr = minishell->hash_table->buckets[i];
		while (curr)
		{
			curr->exported = 1;
			curr = curr->next;
		}
	}
    // Then just once at the end:
    update_env(minishell);
}



t_hash_table	*setup_hash_table(t_mshell *minishell)
{
	t_hash_table	*hash_table;
	int				i;

	hash_table = malloc(sizeof(t_hash_table));
	if (!hash_table)
	{
		print_error("Error (setup_hash_table): hash_table malloc failed\n");
		return (NULL);
	}

	// Initialize hash table buckets
	i = 0;
	while (i < HASH_SIZE)
	{
		hash_table->buckets[i] = NULL;
		i++;
	}

	// Assign hash table before filling it
	minishell->hash_table = hash_table;
	fillup_hash_table(minishell);

	return (hash_table);
}

