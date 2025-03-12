#include "minishell.h"

/**
 * @brief Logs an error message and terminates the program.
 *
 * This function prints an error message to `stderr` using `perror()`
 * and then exits the program with the specified exit status.
 *
 * - If `cmd` is provided, it is included in the error message.
 * - If `cmd` is NULL or empty, a generic "Error" message is used.
 * - Calls `exit(exit_status)` to terminate the process.
 *
 * @param cmd The name of the function or operation that failed.
 * @param exit_status The exit code to return upon termination.
 */
void	fatal_error(char *cmd, int exit_status)
{
	if (cmd && *cmd)
		perror(cmd);
	else
		perror("Error");
	exit(exit_status);
}
