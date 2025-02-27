#include "minishell.h"

void	print_error_exit(char *cmd, int exit_status)
{
	ft_putstr_fd(cmd, STDERR_FILENO);
	perror(": ");
	exit(exit_status);
}

void	print_error(char *cmd)
{
	ft_putstr_fd(cmd, STDERR_FILENO);
	perror(": ");
}