#include "minishell.h"
// exit
/*
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
 * Handles the `exit` command.
 * - If no command or no arguments are provided, 
 * 		exits with status `EXIT_FAILURE`.
 * - If more than one argument is provided, 
 * 		prints "too many arguments" and returns `EXIT_FAILURE` without exiting.
 * - If a single numeric argument is provided, 
 * 		converts it to an integer using `ft_atoi()`, else set to exit code to zero.
 * - Terminates the shell using `exit(status)`, 
 * 		where `status` is the provided exit code.
 *
 * @param cmd	The command structure containing arguments.
 * 				- cmd->argv[0] should contain "exit".
 * 				- cmd->argv[1] (optional) is the exit status argument.
 * 				- If cmd->argv[2] exists, the function returns an error.
 *
 * @return		Returns `EXIT_FAILURE (1)` if "too many arguments" is detected.
 * 				Does not return if `exit()` is called.
 */
int	exec_exit(t_cmd *cmd)
{
	int	status;

	if (!cmd || !cmd->argv[0])
	{
		ft_putstr_fd("minishell: Error exec_exit(), no args\n", STDERR_FILENO);
		return (EXIT_FAILURE);
	}
	if (cmd->argv[1] && cmd->argv[2])
	{
		ft_putstr_fd("minishell: exit: too many arguments\n", STDERR_FILENO);
		return (EXIT_FAILURE);
	}
	if (!cmd->argv[1])
		status = 0;
	else
		status = ft_atoi(cmd->argv[1]);
	exit(status);
}

// echo
// cd
// pwd
// export
// unset
// env
char	**setup_builtin(void)
{
	char	**builtin;

	builtin = malloc(8 * sizeof(char *));
	if (!builtin)
	{
		print_error("Error: is_builtin() malloc failed");
		return (NULL);
	}
	builtin[0] = ft_strdup("echo");
	builtin[1] = ft_strdup("cd");
	builtin[2] = ft_strdup("pwd");
	builtin[3] = ft_strdup("export");
	builtin[4] = ft_strdup("unset");
	builtin[5] = ft_strdup("env");
	builtin[6] = ft_strdup("exit");
	builtin[7] = NULL;
	return (builtin);
}

bool	is_builtin(t_cmd *cmd)
{
	char	**builtin;
	int		i;

	if (!cmd || !cmd->argv[0])
	{
		print_error("Error: is_builtin() no cmd provided");
		return (false);
	}
	builtin = setup_builtin();
	if (!builtin)
	{
		print_error("Error: setup_builtin() failed");
		return (false);
	}
	i = 0;
	while (builtin[i] && ft_strcmp(cmd->argv[0], builtin[i]) != 0)
		i++;
	ft_free_arrstrs(builtin);
	return (builtin[i] != NULL);
}

void	exec_builtin(t_cmd *cmd)
{
	// do
	(void) cmd;
	return ;
}
