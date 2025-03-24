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
// void	fatal_error(char *cmd, int exit_status)
// {
// 	ft_putstr_fd("minishell, sys error, parent: ", STDERR_FILENO);
// 	if (cmd && *cmd)
// 		perror(cmd);
// 	else
// 		perror("Error");
// 	exit(exit_status);
// }

// void	exit_child(t_cmd *cmd, char *msg, int error_code)
// {
// 	char	err_buf[ERROR_BUF_SIZE];
// 	int		exit_code;
// 	char 	*errmsg;

// 	ft_bzero(err_buf, ERROR_BUF_SIZE);
// 	ft_strlcpy(err_buf, "minishell: ", ERROR_BUF_SIZE);
// 	if (cmd->binary != NULL)
// 		ft_strlcat(err_buf, cmd->binary, ERROR_BUF_SIZE);
// 	else
// 		ft_strlcat(err_buf, "unknown", ERROR_BUF_SIZE);
// 	ft_strlcat(err_buf, " - ", ERROR_BUF_SIZE);
// 	if (msg != NULL)
// 		ft_strlcat(err_buf, msg, ERROR_BUF_SIZE);
// 	else
// 		ft_strlcat(err_buf, "error", ERROR_BUF_SIZE);
// 	ft_strlcat(err_buf, ": ", ERROR_BUF_SIZE);
// 	errmsg = strerror(error_code);
// 	ft_strlcat(err_buf, errmsg, ERROR_BUF_SIZE);
// 	ft_strlcat(err_buf, "\n", ERROR_BUF_SIZE);

// 	if (write(STDERR_FILENO, err_buf, ft_strlen(err_buf)) < 0)
// 		write(STDERR_FILENO, "minishell: error: failed to print error\n", 40);
// 	if (error_code == ENOENT)
// 		exit_code = 127;
// 	else if (error_code == EACCES)
// 		exit_code = 126;
// 	else
// 		exit_code = 1;
// 	_exit(exit_code);
// }

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
uint8_t	exit_numeric_error(char *arg)
{
	ft_putstr_fd("minishell: exit: ", STDERR_FILENO);
	ft_putstr_fd(arg, STDERR_FILENO);
	ft_putendl_fd(": numeric argument required", STDERR_FILENO);
	return (2);
}

void child_execve_error(void)
{
	perror("execve");

	if (errno == ENOENT)
		_exit(127);
	else if (errno == EACCES)
		_exit(126);
	else
		_exit(1);
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
uint8_t	cmd_error_handler(t_cmd *cmd, uint8_t exit_status)
{
	char	error_buf[ERROR_BUF_SIZE];
	int		err_num;

	if (!cmd || !cmd->argv)
	{
		write(STDERR_FILENO, "minishell: invalid command structure\n", 37);
		return (EXIT_FAILURE);
	}
	ft_bzero(error_buf, ERROR_BUF_SIZE);
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

/**
 * @brief Prints an error message for invalid options in built-in commands.
 *
 * This function constructs an error message when a built-in command receives
 * an invalid option (e.g., `pwd -abc`). Only the first two characters of the
 * invalid option are displayed to match the behavior of standard shells.
 *
 * @param cmd_name The name of the command that received the invalid option.
 * @param option The invalid option provided by the user.
 * @return Returns an exit status of `2`, following standard shell behavior.
 */
uint8_t invalid_opt_exit(const char *cmd_name, const char *option)
{
	uint8_t	exit_status;
	char	error_buf[ERROR_BUF_SIZE];
	char	opt_buf[3];

	ft_strlcpy(opt_buf, option, 3);
	ft_strlcpy(error_buf, "minishell: ", ERROR_BUF_SIZE);
	ft_strlcat(error_buf, cmd_name, ERROR_BUF_SIZE);
	ft_strlcat(error_buf, ": ", ERROR_BUF_SIZE);
	ft_strlcat(error_buf, opt_buf, ERROR_BUF_SIZE);
	ft_strlcat(error_buf, ": invalid option\n", ERROR_BUF_SIZE);
	ft_strlcat(error_buf, cmd_name, ERROR_BUF_SIZE);
	ft_strlcat(error_buf, ": usage: ", ERROR_BUF_SIZE);
	ft_strlcat(error_buf, cmd_name, ERROR_BUF_SIZE);
	ft_strlcat(error_buf, "\n", ERROR_BUF_SIZE);
	if (write(STDERR_FILENO, error_buf, ft_strlen(error_buf)) < 0)
		write(STDERR_FILENO, "minishell: error: failed to print error\n", 40);
	exit_status = 2;
	return (exit_status);
}

uint8_t	perror_return(char *msg, u_int8_t exit_status)
{
	if (msg)
		ft_putstr_fd(msg, STDERR_FILENO);
	return (exit_status);

}
