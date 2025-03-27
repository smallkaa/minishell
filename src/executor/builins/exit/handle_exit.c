/**
 * @file handle_exit.c
 * @brief Functions for handling the `exit` built-in command in Minishell.
 */
#include "minishell.h"

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
static uint8_t	exit_numeric_error(char *arg)
{
	print_error("minishell: exit: ");
	print_error(arg);
	print_error(": numeric argument required\n");
	return (2);
}

/**
 * @brief Checks if an exit argument is numeric.
 *
 * The function ensures that the argument consists only of digits,
 * with an optional leading `+` or `-` sign.
 *
 * @param arg The argument string.
 * @return `true` if numeric, `false` otherwise.
 */
static bool	is_valid_numeric_exit_arg(const char *arg)
{
	if (!arg || *arg == '\0')
		return (false);
	if (*arg == '-' || *arg == '+')
		arg++;
	if (!ft_isdigit(*arg))
		return (false);
	while (*arg)
	{
		if (!ft_isdigit(*arg))
			return (false);
		arg++;
	}
	return (true);
}

/**
 * @brief Handles cases where too many arguments are passed to `exit`.
 *
 * The shell prints an error message and returns an error code.
 *
 * @return `1` (error exit status).
 */
static uint8_t	handle_too_many_args(void)
{
	ft_putendl_fd("minishell: exit: too many arguments", STDERR_FILENO);
	return (1);
}

/**
 * @brief Parses and retrieves the exit status from an argument.
 *
 * The exit status is taken modulo `256` (`& 0xFF`) to ensure it falls
 * within the valid range (0-255).
 *
 * @param arg The argument string.
 * @return The parsed exit status.
 */
static uint8_t	get_exit_status(char *arg)
{
	long long	exit_status;
	bool		overflow;

	exit_status = ft_atoll_exit(arg, &overflow);
	if (overflow)
		return (exit_numeric_error(arg));
	return ((uint8_t)(exit_status & 0xFF));
}

/**
 * @brief Handles the `exit` built-in command in Minishell.
 *
 * Behavior:
 * - If no arguments are provided, exits with the last command's status.
 * - If one argument is provided, checks if it is numeric:
 *   - If valid, exits with the specified status.
 *   - If invalid, prints an error and returns `EXIT_FAILURE`.
 * - If multiple arguments are provided, prints an error and does not exit.
 *
 * @param cmd The command structure containing execution context.
 * @return `EXIT_SUCCESS` (0) or `EXIT_FAILURE` (1) (not actually used,
 *         as `exit` terminates).
 */
uint8_t	handle_exit(t_cmd *cmd)
{
	if (!cmd)
	{
		print_error("minishell: exit: no *cmd instance\n");
		return (EXIT_FAILURE);
	}
	printf("exit\n");
	if (!cmd->argv[1] || cmd->argv[1][0] == '\0')
		return (cmd->minishell->exit_status);
	if (!is_valid_numeric_exit_arg(cmd->argv[1]))
		return (exit_numeric_error(cmd->argv[1]));
	if (cmd->argv[2])
		return (handle_too_many_args());
	return (get_exit_status(cmd->argv[1]));
}
