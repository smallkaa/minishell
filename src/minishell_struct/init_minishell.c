#include "minishell.h"

t_mshell	*init_mshell(char **envp)
{
	t_mshell		*minishell;
	char			**env;
	t_hash_table	*hash_table;
	char			**builtin;

	if (!envp)
	{
		print_error("Error (init_mshell): no environ found\n");
		return (NULL);
	}
	minishell = malloc(sizeof(t_mshell));
	if (!minishell)
	{
		print_error("Error (init_mshell): minishell malloc failed\n");
		return (NULL);
	}
	env = setup_env(envp);
	if (!env)
	{
		free_minishell(minishell);
		return (NULL);
	}
	minishell->env = env;
	hash_table = setup_hash_table(minishell);
	if (!hash_table)
	{
		free_minishell(minishell);
		return (NULL);
	}
	minishell->hash_table = hash_table;
	builtin = setup_builtin();
	if (!builtin)
	{
		free_minishell(minishell);
		return (NULL);
	}
	minishell->builtin = builtin;
	minishell->exit_status = 0;
	return (minishell);
}
