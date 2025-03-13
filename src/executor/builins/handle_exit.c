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

static bool	is_numeric_arg(const char *arg)
{
	int	i = 0;

	if (arg[i] == '-' || arg[i] == '+')
		i++;
	if (!arg[i])
		return (false);
	while (arg[i])
	{
		if (!ft_isdigit(arg[i]))
			return (false);
		i++;
	}
	return (true);
}

static long long	ft_atoll_exit(const char *str, bool *overflow)
{
	int			sign;
	long long	res;
	int			digit;

	sign = 1;
	res = 0;
	*overflow = false;
	while (ft_isspace(*str))
		str++;
	if (*str == '-' || *str == '+')
	{
		if (*str == '-')
			sign = -1;
		str++;
	}
	if (!ft_isdigit(*str))
	{
		*overflow = true;
		return (0);
	}
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


uint8_t	handle_exit(t_cmd *cmd)
{
	long long	exit_status;
	bool		overflow;

	if (!cmd)
	{
		print_error("minishell: exit: no *cmd instance\n");
		return (EXIT_FAILURE);
	}
	printf("exit\n");
	if (!cmd->argv[1])
		exit(cmd->minishell->exit_status);
	if (!is_numeric_arg(cmd->argv[1]))
	{
		exit_numeric_error(cmd->argv[1]);
		exit(2);
	}
	if (cmd->argv[2])
	{
		if (!is_numeric_arg(cmd->argv[1]))
		{
			exit_numeric_error(cmd->argv[1]);
			exit(2);
		}
		else
		{
			ft_putendl_fd("minishell: exit: too many arguments", STDERR_FILENO);
			return (EXIT_FAILURE);
		}
	}
	exit_status = ft_atoll_exit(cmd->argv[1], &overflow);
	if (overflow)
	{
		exit_numeric_error(cmd->argv[1]);
		exit(2);
	}
	exit((uint8_t)(exit_status & 0xFF));
}
