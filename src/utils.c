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

bool is_debug_mode()
{
	return (getenv("MINISHELL_DEBUG") != NULL);
}

void debug_printf(const char *format, ...) 
{
	if(is_debug_mode())
	{	
		va_list args;
		va_start(args, format);
		vprintf(format, args);
		va_end(args);
	}
}