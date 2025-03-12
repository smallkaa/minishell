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
