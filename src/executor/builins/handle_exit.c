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

/**
 * @brief Checks if a string represents a numeric value.
 *
 * Iterates through the given string and verifies whether all characters
 * are digits (`0-9`). If any non-numeric character is found, the function
 * returns `false`.
 *
 * @param num The string to check.
 * @return `true` if the string is numeric, `false` otherwise.
 */
static bool	is_numeric(char *num)
{
	while (*num)
	{
		if (!ft_isdigit(*num))
			return (false);
		num++;
	}
	return (true);
}

/**
 * @brief Handles the `exit` built-in command.
 *
 * - If an argument is provided:
 *   - If it is numeric, the shell exits with the specified status (mod 256).
 *   - If it is non-numeric, an error is printed, and the shell exits with status `255`.
 * - If no argument is provided, the shell exits using the last command's exit status.
 *
 * Before terminating, this function frees all allocated memory using `free_minishell()`.
 *
 * @param cmd Pointer to the command structure.
 * @note This function calls `exit()`, which terminates the shell process.
 */
uint8_t	handle_exit(t_cmd *cmd)
{
	uint8_t	exit_status;
	if (!cmd)
	{
		print_error("Err: exit, no cmd\n");
		////////
	}


	if (cmd->argv[1])
	{
		if (!is_numeric(cmd->argv[1]))
		{
			print_error("minishell: exit: numeric argument required");
			exit_status = 255;
		}
		else
			exit_status = ft_atoi(cmd->argv[1]) % 256;
	}
	else
		exit_status = cmd->minishell->exit_status;
	cmd->minishell->exit_status = exit_status;
	free_minishell(cmd->minishell);
	exit(exit_status);
}
