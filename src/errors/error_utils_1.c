#include "minishell.h"

void	print_error(const char *msg)
{
	ft_putstr_fd(msg, STDERR_FILENO);
}

void	export_error(t_mshell_var *pair)
{
	print_error("-minishell: export: `");
	print_error(pair->key);
	if (pair->value)
	{
		print_error("=");
		print_error(pair->value);
	}
	print_error("': not a valid identifier\n");
}

void	unset_error(char *str)
{
	print_error("-minishell: unset: ");
	(void)write(STDERR_FILENO, str, 2);
	print_error(": invalid option\n");
	print_error("unset: usage: unset [name ...]\n");
}

void	cmd_missing_command_error(t_cmd *cmd)
{
	const char	*path;

	if (!cmd || !cmd->argv || !cmd->argv[0])
	{
		print_error("-minishell: invalid cmd structure\n");
		_exit(127);
	}
	print_error("-minishell: ");
	print_error(cmd->argv[0]);
	path = ms_getenv(cmd->minishell, "PATH");
	if (ft_strchr(cmd->argv[0], '/') || !path || path[0] == '\0')
	{
		print_error(": No such file or directory\n");
	}
	else
	{
		print_error(": command not found\n");
	}
	free_minishell(cmd->minishell);
	_exit(127);
}

int	error_return(char *msg, int exit_status)
{
	ft_putstr_fd("-minishell: ", STDERR_FILENO);
	if (msg)
		print_error(msg);
	return (exit_status);
}
