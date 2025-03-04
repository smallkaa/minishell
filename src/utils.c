#include "minishell.h"

void	print_error_exit(char *cmd, int exit_status)
{
	ft_putstr_fd(cmd, STDERR_FILENO);
	perror(": ");
	exit(exit_status);
}

/**
 * Prints an error message to `stderr` when a command is not found
 * and exits with the last recorded exit status of the shell.
 *
 * - Uses the `write()` system call for direct output.
 * - Displays the command name inside single quotes.
 * - Exits with the shell's last exit status (`cmd->minishell->exit_stat`).
 *
 * @param cmd  The command structure containing the attempted command.
 *             - `cmd->argv[0]` holds the name of the missing command.
 *             - `cmd->minishell->exit_stat` stores the exit status.
 *
 * @return     This function does not return (calls `exit()`).
 */
void	command_not_found_handle(t_cmd *cmd)
{
	write(STDERR_FILENO, "Command '", 9);
	write(STDERR_FILENO, cmd->argv[0], ft_strlen(cmd->argv[0]));
	write(STDERR_FILENO, "' not found.", 12);
	exit(cmd->minishell->exit_stat);
}
void	print_error(char *cmd)
{
	ft_putstr_fd(cmd, STDERR_FILENO);
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
