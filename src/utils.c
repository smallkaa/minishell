#include "minishell.h"

void	print_error_exit(char *cmd, int exit_status)
{
	if (cmd && *cmd)
		perror(cmd);
	else
		perror("Error");
	exit(exit_status);
}

/**
 * Prints an error message to `stderr` when a command is not found
 * and exits with the last recorded exit status of the shell.
 *
 * - Uses the `write()` system call for direct output.
 * - Displays the command name inside single quotes.
 * - Exits with the shell's last exit status (`cmd->minishell->exit_status`).
 *
 * @param cmd  The command structure containing the attempted command.
 *			 - `cmd->argv[0]` holds the name of the missing command.
 *			 - `cmd->minishell->exit_status` stores the exit status.
 *
 * @return	 This function does not return (calls `exit()`).
 */
void	command_not_found_handle(t_cmd *cmd)
{
	// write(STDERR_FILENO, "Command '", 9);
	write(STDERR_FILENO, cmd->argv[0], ft_strlen(cmd->argv[0]));
	write(STDERR_FILENO, ": command not found\n", 20);
}

/**
 * cmd_error_handler - Prints an error message for a failed command.
 *
 * Behavior:
 * - Prints the format: `minishell: <command>: <arg> (if available): <error_message>`
 * - Retrieves the error message using `strerror(errno)`.
 * - Writes the message to `STDERR_FILENO` using `write()`.
 *
 * @param cmd Pointer to the `t_cmd` structure containing command arguments.
 */
void	cmd_error_handler(t_cmd *cmd)
{
	char	*error_msg;
	int		err_num;

	err_num = errno;
	write(STDERR_FILENO, "minishell: ", 11);
	write(STDERR_FILENO, cmd->argv[0], ft_strlen(cmd->argv[0]));
	if (cmd->argv[1])
	{
		write(STDERR_FILENO, ": ", 2);
		write(STDERR_FILENO, cmd->argv[1], ft_strlen(cmd->argv[1]));
	}
	write(STDERR_FILENO, ": ", 2);
	error_msg = strerror(err_num);
	if (write(STDERR_FILENO, error_msg, ft_strlen(error_msg)) < 0)
		write(STDERR_FILENO, ": Error printing error message\n", 31);
	write(STDERR_FILENO, "\n", 1);
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
int	ft_arr_size(char **arr)
{
	int	len;

	len = 0;
	while (arr && arr[len])
		len++;
	return (len);
}

