#include "minishell.h"

void	free_old_env(char **env)
{
	int	i;

	if (!env)
		return;
	i = 0;
	while (env[i])
	{
		free(env[i]);
		i++;
	}
	free(env);
}

char	*create_env_entry(t_mshell_var *var)
{
	const char	*val;

	val = (var->value) ? var->value : "";
	return (malloc(ft_strlen(var->key) + ft_strlen(val) + 2));
}

void	populate_env_array(t_mshell *minishell, char **new_env)
{
	int				index;
	int				i;
	t_mshell_var	*current;

	index = 0;
	for (i = 0; i < HASH_SIZE; i++)
	{
		current = minishell->hash_table->buckets[i];
		while (current)
		{
			if (current->val_assigned)
			{
				new_env[index] = create_env_entry(current);
				if (!new_env[index])
				{
					print_error("minishell: update_environ, malloc failed\n");
					free_old_env(new_env);
					return;
				}
				sprintf(new_env[index], "%s=%s", current->key, (current->value) ? current->value : "");
				index++;
			}
			current = current->next;
		}
	}
	new_env[index] = NULL;
}

void	update_env(t_mshell *minishell)
{
	int		count;
	char	**new_env;

	count = count_exported_vars(minishell->hash_table);
	new_env = malloc((count + 1) * sizeof(char *));
	if (!new_env)
	{
		print_error("minishell: update_environ, malloc failed\n");
		return;
	}
	populate_env_array(minishell, new_env);
	free_old_env(minishell->env);
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

void	update_existing_variable(t_mshell_var *current, t_mshell_var *mshell_var, int val_assigned, t_mshell *minishell)
{
	if (mshell_var->value)
	{
		free(current->value);
		current->value = ft_strdup(mshell_var->value);
	}
	current->val_assigned = val_assigned;
	if (val_assigned)
		update_env(minishell);
}

t_mshell_var	*create_new_variable(t_mshell_var *mshell_var, int val_assigned)
{
	t_mshell_var	*new_var;

	new_var = malloc(sizeof(t_mshell_var));
	if (!new_var)
	{
		print_error("Error (set_variable): new_var malloc failed\n");
		return (NULL);
	}
	new_var->key = ft_strdup(mshell_var->key);
	if (mshell_var->value)
		new_var->value = ft_strdup(mshell_var->value);
	else
	new_var->value = NULL;
	new_var->val_assigned = val_assigned;
	new_var->next = NULL;
	return (new_var);
}

void	set_variable(t_mshell *minishell, t_mshell_var *mshell_var, int val_assigned)
{
	unsigned int	index;
	t_mshell_var	*current;
	t_hash_table	*hash_table;
	t_mshell_var	*new_var;

	index = hash_function(mshell_var->key);
	hash_table = minishell->hash_table;
	current = hash_table->buckets[index];
	while (current)
	{
		if (ft_strcmp(current->key, mshell_var->key) == 0)
		{
			update_existing_variable(current, mshell_var, val_assigned, minishell);
			return;
		}
		current = current->next;
	}
	new_var = create_new_variable(mshell_var, val_assigned);
	if (!new_var)
		return ;
	new_var->next = hash_table->buckets[index];
	hash_table->buckets[index] = new_var;
	if (val_assigned)
		update_env(minishell);
}

static void	fillup_hash_table(t_mshell *minishell)
{
	t_mshell_var	*mshell_var;
	t_mshell_var	*curr;
	int				i;

	i = 0;
	while (minishell->env[i])
	{
		mshell_var = split_key_value(minishell->env[i]);
		if (mshell_var)
		{
			set_variable(minishell, mshell_var, 0);
			free(mshell_var->key);
			free(mshell_var->value);
			free(mshell_var);
		}
		i++;
	}
	i = 0;
	while (i < HASH_SIZE)
	{
		curr = minishell->hash_table->buckets[i];
		while (curr)
		{
			curr->val_assigned = 1;
			curr = curr->next;
		}
		i++;
	}
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
	i = 0;
	while (i < HASH_SIZE)
	{
		hash_table->buckets[i] = NULL;
		i++;
	}
	minishell->hash_table = hash_table;
	fillup_hash_table(minishell);
	return (hash_table);
}
