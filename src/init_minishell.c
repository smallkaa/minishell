#include "minishell.h"

int	count_envp_len(char **envp)
{
	int len;

	len = 0;
	while(*envp)
	{
		len++;
		envp++;
	}
	return (len);
}
void	setup_envp(char **envp, t_minishell *minishell)
{
	int	i;

	i = 0;
	while(envp[i])
	{
		minishell->env[i] = envp[i];
		i++;
	}
	minishell->env[i] = NULL;
}

t_minishell	*init_minishell(char **envp)
{
	t_minishell	*minishell;
	int			envp_len;

	if (!envp)
	{
		print_error("Error: init_minishell, no environment found\n");
		return (NULL);
	}
	minishell = malloc(sizeof(t_minishell));
	if (!minishell)
	{
		print_error("Error: init_minishell, malloc failed\n");
		return (NULL);
	}
	envp_len = count_envp_len(envp);
	minishell->env = malloc((envp_len + 1) * sizeof(char *));
	if (!minishell->env)
	{
		print_error("Error: init_minishell, minishell->envp malloc failed\n");
		return (NULL);
	}
	setup_envp(envp, minishell);
	minishell->local_var = malloc(sizeof(char *));
	if (!minishell->local_var)
	{
		print_error("Error: init_minishell, minishell->local_var malloc failed\n");
		return (NULL);
	}
	minishell->local_var = NULL;
	minishell->exit_stat = 0;
	return (minishell);
}
