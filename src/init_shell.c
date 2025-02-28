#include "../include/minishell.h"

t_shell	*init_shell(void)
{
	t_shell	*shell;

	shell = malloc(sizeof(t_shell));
	if (!shell)
	{
		ft_putstr_fd("Error: init_shell() malloc fail\n", STDOUT_FILENO);
		return (NULL);
	}
	shell->l_exit_stat = 0;
	return (shell);
}
