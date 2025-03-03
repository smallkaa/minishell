#include "minishell.h"

void	print_error_exit(char *cmd, int exit_status)
{
	ft_putstr_fd(cmd, STDERR_FILENO);
	perror(": ");
	exit(exit_status);
}
void	command_not_found_handle(t_cmd *cmd)
{
	write(STDERR_FILENO, "Command '", 9);
	write(STDERR_FILENO, cmd->argv[0], ft_strlen(cmd->argv[0]));
	write(STDERR_FILENO, "' not found.", 12);
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
