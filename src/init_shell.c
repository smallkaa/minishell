#include "../include/minishell.h"

t_minishell	*init_minishell(void)
{
	t_minishell	*shell;

	shell = malloc(sizeof(t_minishell));
	if (!shell)
	{
		ft_putstr_fd("Error: init_minishell() malloc fail\n", STDOUT_FILENO);
		return (NULL);
	}
	shell->l_exit_stat = 0;
	return (shell);
}
