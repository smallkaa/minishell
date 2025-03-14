#include "minishell.h"

/**
 * @brief Logs a fatal system error and terminates the program.
 *
 * This function is used for **critical failures in the parent process**.
 * It logs a system error message prefixed with `"minishell sys error: "`
 * and then terminates the shell using `exit(exit_status)`.
 *
 * **Behavior:**
 * - If `cmd` is provided, it is included in the error message.
 * - If `cmd` is `NULL` or empty, a generic `"Error"` message is used.
 * - Calls `perror(cmd)` to print a system-generated error message.
 * - Calls `exit(exit_status);` to **terminate the parent process**.
 *
 * - If called inside a child process, use `fatal_error_child()` instead
 *   to avoid unintended effects (like flushing shared streams).
 *
 * **Why `exit()` instead of `_exit()`?**
 * - `exit()` properly flushes stdio buffers and calls `atexit()` handlers.
 * - `_exit()` should only be used in child processes to prevent unintended
 *   parent-side buffer flushing.
 *
 * @param cmd The name of the command or operation that failed (can be `NULL`).
 * @param exit_status The exit code to return upon termination.
 */
void	fatal_error(char *cmd, int exit_status)
{
	ft_putstr_fd("minishell, sys error, parent: ", STDERR_FILENO);
	if (cmd && *cmd)
		perror(cmd);
	else
		perror("Error");
	exit(exit_status);
}

/**
 * @brief Handles execution failures in child processes and exits with the correct status.
 *
 * This function prints an error message based on the command that failed
 * and exits with the appropriate exit status:
 * - `126` if the command exists but is not executable (Permission denied).
 * - `127` if the command does not exist (No such file or directory).
 * - `1` for other execution failures.
 *
 * @param cmd The command structure containing execution details.
 * @param error_code The errno value from a failed execve() call.
 */
void fatal_error_child(t_cmd *cmd, int error_code)
{
	if (!cmd || !cmd->binary)
	{
		ft_putstr_fd("minishell: Unknown command error\n", STDERR_FILENO);
		_exit(126);
	}

	ft_putstr_fd("minishell: ", STDERR_FILENO);
	perror(cmd->binary);

	if (error_code == EACCES)
		_exit(126);
	else if (error_code == ENOENT)
		_exit(127);
	else
		_exit(1);
}

/**
 * exit_numeric_error - Prints an error message for an invalid numeric
 *                      argument in `exit`.
 *
 * Format:
 *   minishell: exit: <arg>: numeric argument required
 *
 * Behavior:
 * - Prints a formatted error message to `STDERR_FILENO`.
 * - Used when the `exit` command receives an invalid numeric argument.
 * - Ensures clear and consistent error messaging for non-numeric exit codes.
 *
 * @param arg The invalid argument that caused the error.
 */
void	exit_numeric_error(char *arg)
{
	ft_putstr_fd("minishell: exit: ", STDERR_FILENO);
	ft_putstr_fd(arg, STDERR_FILENO);
	ft_putendl_fd(": numeric argument required", STDERR_FILENO);
	exit (2);
}

/**
 * cmd_error_handler - Prints an error message for a failed command execution.
 *
 * Format:
 *   minishell: <command>: <arg> (if available): <error_message>
 *
 * Behavior:
 * - Extracts the error message from `strerror(errno)`.
 * - Handles cases where `cmd` or `cmd->argv[0]` is NULL.
 * - Uses a buffer (`ERROR_BUF_SIZE 256`) to construct the message safely.
 * - Ensures no buffer overflows using `ft_strlcpy()` and `ft_strlcat()`.
 * - Writes the formatted error message to `STDERR_FILENO` in a single
 *   `write()` call.
 * - If `write()` fails, prints a fallback error message.
 *
 * @param cmd Pointer to the `t_cmd` structure containing command arguments.
 */
int	cmd_error_handler(t_cmd *cmd, int exit_status)
{
	char	error_buf[ERROR_BUF_SIZE];
	int		err_num;

	if (!cmd || !cmd->argv || !cmd->argv[0])
	{
		write(STDERR_FILENO, "minishell: invalid command structure\n", 37);
		return (EXIT_FAILURE);
	}
	err_num = errno;
	ft_strlcpy(error_buf, "minishell: ", ERROR_BUF_SIZE);
	ft_strlcat(error_buf, cmd->argv[0], ERROR_BUF_SIZE);
	if (cmd->argv[1])
	{
		ft_strlcat(error_buf, ": ", ERROR_BUF_SIZE);
		ft_strlcat(error_buf, cmd->argv[1], ERROR_BUF_SIZE);
	}
	ft_strlcat(error_buf, ": ", ERROR_BUF_SIZE);
	ft_strlcat(error_buf, strerror(err_num), ERROR_BUF_SIZE);
	ft_strlcat(error_buf, "\n", ERROR_BUF_SIZE);
	if (write(STDERR_FILENO, error_buf, ft_strlen(error_buf)) < 0)
		write(STDERR_FILENO, "minishell: error: failed to print error\n", 40);
	return (exit_status);
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
	write(STDERR_FILENO, cmd->argv[0], ft_strlen(cmd->argv[0]));
	write(STDERR_FILENO, ": command not found\n", 20);
}
