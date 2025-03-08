#include "minishell.h"

t_mshell	*init_mshell(char **envp)
{
	t_mshell		*minishell;
	char			**env;
	t_hash_table	*hash_table;

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
	// printf("DEBUG START print env\n");
	// int j = 0;
	// while(minishell->env[j])
	// {
	// 	printf("%s\n", minishell->env[j]);
	// 	j++;
	// }
	// printf("DEBUG END\n");

	hash_table = setup_hash_table(minishell);
	if (!hash_table)
	{
		free_minishell(minishell);
		return (NULL);
	}
	minishell->hash_table = hash_table;
	// printf("DEBUG START print hash table\n");
	// print_all_variables(minishell->hash_table);
	// printf("DEBUG END\n");
	// // exit (2);


	minishell->exit_stat = 0;
	return (minishell);
}
