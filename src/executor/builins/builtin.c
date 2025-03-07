#include "minishell.h"
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

// cd
// export
// unset
// env
char	**setup_builtin(void)
{
	char	**builtin;

	builtin = malloc(8 * sizeof(char *));
	if (!builtin)
	{
		print_error("Error: is_builtin() malloc failed\n");
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
		print_error("Error: is_builtin() no cmd provided\n");
		return (false);
	}
	builtin = setup_builtin();
	if (!builtin)
	{
		print_error("Error: setup_builtin() failed\n");
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
	if(is_builtin(cmd))
	{
		if (ft_strcmp(cmd->argv[0], "exit") == 0)
			handle_exit(cmd);
		else if (ft_strcmp(cmd->argv[0], "echo") == 0)
			handle_echo(cmd);
		else if (ft_strcmp(cmd->argv[0], "pwd") == 0)
			handle_pwd(cmd);
		else if (ft_strcmp(cmd->argv[0], "cd") == 0)
			handle_cd(cmd);
		else if (ft_strcmp(cmd->argv[0], "env") == 0)
			handle_env(cmd);
		else if (ft_strcmp(cmd->argv[0], "export") == 0)
			handle_export(cmd);
	}
}
