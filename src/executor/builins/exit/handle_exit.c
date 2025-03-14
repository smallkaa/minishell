/**
 * @file handle_exit.c
 * @brief Implementation of the `exit` built-in command in Minishell.
 */
#include "minishell.h"
/*
EXIT STATUS

https://pubs.opengroup.org/onlinepubs/9699919799/utilities/V3_chap02.html#exit

The exit utility shall cause the shell to exit from
its current execution environment with the exit status
specified by the unsigned decimal integer n.
If the current execution environment is a subshell environment,
the shell shall exit from the subshell environment with
the specified exit status and continue in the environment from
which that subshell environment was invoked; otherwise,
the shell utility shall terminate with the specified exit status.
If n is specified, but its value is not between 0 and 255 inclusively,
the exit status is undefined.

A trap on EXIT shall be executed before the shell terminates,
except when the exit utility is invoked in that trap itself,
in which case the shell shall exit immediately.

The exit status shall be n, if specified, except that the behavior
is unspecified if n is not an unsigned decimal integer or
is greater than 255. Otherwise, the value shall be the exit value of
the last command executed, or zero if no command was executed.
When exit is executed in a trap action, the last command is
considered to be the command that executed immediately
preceding the trap action.
*/
#include "minishell.h"

/**
 * @brief Skips leading whitespace characters in a string.
 *
 * @param str The input string.
 * @return A pointer to the first non-whitespace character.
 */
static const char	*skip_whitespace(const char *str)
{
	while (ft_isspace(*str))
		str++;
	return (str);
}

/**
 * @brief Parses the sign of a number and updates the pointer accordingly.
 *
 * @param str The input string.
 * @param sign A pointer to store the sign (1 for positive, -1 for negative).
 * @return A pointer to the first numeric character in the string.
 */
static const char	*parse_sign(const char *str, int *sign)
{
	*sign = 1;
	if (*str == '-' || *str == '+')
	{
		if (*str == '-')
			*sign = -1;
		str++;
	}
	return (str);
}

/**
 * @brief Converts a string to a long long integer while checking for overflow.
 *
 * @param str The input string containing the number.
 * @param sign The sign of the number (1 or -1).
 * @param overflow Pointer to a boolean flag indicating overflow.
 * @return The converted long long integer or LLONG_MAX/LLONG_MIN in
 *         case of overflow.
 */
static long long	convert_to_ll(const char *str, int sign, bool *overflow)
{
	long long	res;
	int			digit;

	*overflow = false;
	res = 0;
	while (ft_isdigit(*str))
	{
		digit = *str - '0';
		if (sign == 1 && res > (LLONG_MAX - digit) / 10)
		{
			*overflow = true;
			return (LLONG_MAX);
		}
		if (sign == -1 && res > -(LLONG_MIN + digit) / 10)
		{
			*overflow = true;
			return (LLONG_MIN);
		}
		res = res * 10 + digit;
		str++;
	}
	return (res * sign);
}

/**
 * @brief Converts a string to a long long integer, handling errors and overflow.
 *
 * @param str The input string.
 * @param overflow Pointer to a boolean flag indicating overflow.
 * @return The converted long long integer or 0 in case of error.
 */
long long	ft_atoll_exit(const char *str, bool *overflow)
{
	int		sign;

	str = skip_whitespace(str);
	str = parse_sign(str, &sign);
	if (!ft_isdigit(*str))
	{
		*overflow = true;
		return (0);
	}
	return (convert_to_ll(str, sign, overflow));
}

/**
 * @brief Checks if an exit argument is numeric.
 *
 * @param arg The argument string.
 * @return true if numeric, false otherwise.
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
 */
static void	handle_too_many_args(void)
{
	ft_putendl_fd("minishell: exit: too many arguments", STDERR_FILENO);
	exit(1);
}

/**
 * @brief Parses and retrieves the exit status from an argument.
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
		exit_numeric_error(arg);
	return ((uint8_t)(exit_status & 0xFF));
}

/**
 * @brief Handles the `exit` built-in command.
 *
 * @param cmd The command structure containing execution context.
 * @return EXIT_SUCCESS or EXIT_FAILURE (not actually used since `exit`
 *         terminates).
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
		exit(cmd->minishell->exit_status);
	if (!is_valid_numeric_exit_arg(cmd->argv[1]))
		exit_numeric_error(cmd->argv[1]);
	if (cmd->argv[2])
		handle_too_many_args();
	exit(get_exit_status(cmd->argv[1]));
}

// static long long	ft_atoll_exit(const char *str, bool *overflow)
// {
// 	int			sign;
// 	long long	res;
// 	int			digit;

// 	sign = 1;
// 	res = 0;
// 	*overflow = false;
// 	while (ft_isspace(*str))
// 		str++;
// 	if (*str == '-' || *str == '+')
// 	{
// 		if (*str == '-')
// 			sign = -1;
// 		str++;
// 	}
// 	if (!ft_isdigit(*str))
// 	{
// 		*overflow = true;
// 		return (0);
// 	}
// 	while (ft_isdigit(*str))
// 	{
// 		digit = *str - '0';
// 		if (sign == 1 && res > (LLONG_MAX - digit) / 10)
// 		{
// 			*overflow = true;
// 			return (LLONG_MAX);
// 		}
// 		if (sign == -1 && res > -(LLONG_MIN + digit) / 10)
// 		{
// 			*overflow = true;
// 			return (LLONG_MIN);
// 		}
// 		res = res * 10 + digit;
// 		str++;
// 	}
// 	return (res * sign);
// }

// uint8_t	handle_exit(t_cmd *cmd)
// {
// 	long long	exit_status;
// 	bool		overflow;

// 	if (!cmd)
// 	{
// 		print_error("minishell: exit: no *cmd instance\n");
// 		return (EXIT_FAILURE);
// 	}
// 	printf("exit\n");
// 	if (!cmd->argv[1] || cmd->argv[1][0] == '\0')
// 		exit(cmd->minishell->exit_status);
// 	if (!cmd->argv[1])
// 		exit(cmd->minishell->exit_status);
// 	if (!is_numeric_arg(cmd->argv[1]))
// 	{
// 		exit_numeric_error(cmd->argv[1]);
// 		exit(2);
// 	}
// 	if (cmd->argv[2])
// 	{
// 		if (!is_numeric_arg(cmd->argv[1]))
// 		{
// 			exit_numeric_error(cmd->argv[1]);
// 			exit(2);
// 		}
// 		else
// 		{
// 			ft_putendl_fd("minishell: exit: too many arguments", STDERR_FILENO);
// 			exit(1);
// 		}
// 	}
// 	exit_status = ft_atoll_exit(cmd->argv[1], &overflow);
// 	if (overflow)
// 	{
// 		exit_numeric_error(cmd->argv[1]);
// 		exit(2);
// 	}
// 	exit((uint8_t)(exit_status & 0xFF));
// }
