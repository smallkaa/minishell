#include "minishell.h"

int	perror_return(char *msg, int exit_status)
{
	ft_putstr_fd("-minishell: ", STDERR_FILENO);
	if (msg)
		perror(msg);
	return (exit_status);
}

uint8_t	perror_exit_child(char *msg, u_int8_t exit_status)
{
	if (msg)
		perror(msg);
	_exit(exit_status);
}
