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
 * @brief Handles fatal system errors in a child process and exits immediately.
 *
 * This function is used exclusively in **child processes** when a critical
 * system error occurs (e.g., `fork()` failure, `execve()` failure, etc.).
 *
 * **Behavior:**
 * - Prints the error message prefixed with `"minishell sys error: "`.
 * - Uses `perror()` to display a system-generated error message.
 * - Calls `_exit(exit_status);` to terminate the child process immediately
 *   without flushing stdio buffers or calling atexit handlers.
 *
 * - `_exit()` prevents flushing of shared file streams, avoiding data
 *   corruption.
 * - It ensures **only the child process terminates**, without
 *   affecting the parent.
 *
 * @param cmd The name of the command or operation that failed (can be `NULL`).
 * @param exit_status The exit code to return (typically `EXIT_FAILURE` or `127`).
 */
void	fatal_error_child(char *cmd, int exit_status)
{
	ft_putstr_fd("minishell, sys error, child: ", STDERR_FILENO);
	if (cmd && *cmd)
		perror(cmd);
	else
		perror("Error");
	_exit(exit_status);
}
