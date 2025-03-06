#include "minishell.h"

t_minishell	*init_minishell(char **envp)
{
	t_minishell	*minishell;

	minishell = malloc(sizeof(t_minishell));
	if (!minishell)
	{
		ft_putstr_fd("Error: init_minishell() malloc fail\n", STDOUT_FILENO);
		return (NULL);
	}
	minishell->envp = envp;
	minishell->exp_vars = NULL;
	minishell->exit_stat = 0;
	return (minishell);
}
