#include "minishell.h"

void print_error(char *msg)
{
	ft_putstr_fd(msg, STDERR_FILENO);
}

void export_error(t_mshell_var *pair)
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

void unset_error(char *str)
{
	print_error("-minishell: unset: ");
	write(STDERR_FILENO, str, 2);
	print_error(": invalid option\n");
	print_error("unset: usage: unset [name ...]\n");
}

void	child_execve_error(t_cmd *cmd)
{
	struct stat	st;

	if (stat(cmd->binary, &st) == 0 && S_ISDIR(st.st_mode))
	{
		write(STDERR_FILENO, "-minishell: ", 12);
		write(STDERR_FILENO, cmd->binary, ft_strlen(cmd->binary));
		write(STDERR_FILENO, ": Is a directory\n", 17);
		free_minishell(cmd->minishell);
		_exit(126);
	}
	if (errno == ENOENT)
	{
		write(STDERR_FILENO, "-minishell: ", 12);
		write(STDERR_FILENO, cmd->argv[0], ft_strlen(cmd->argv[0]));
	
		if (strchr(cmd->argv[0], '/') || !ms_getenv(cmd->minishell, "PATH") || ms_getenv(cmd->minishell, "PATH")[0] == '\0')
			write(STDERR_FILENO, ": No such file or directory\n", 28);
		else
			write(STDERR_FILENO, ": command not found\n", 20);
	
		_exit(127);
	}
	else if (errno == EACCES)
	{
		write(STDERR_FILENO, "-minishell: ", 12);
		write(STDERR_FILENO, cmd->binary, ft_strlen(cmd->binary));
		write(STDERR_FILENO, ": Permission denied\n", 20);
		free_minishell(cmd->minishell);
		_exit(126);
	}
	else if (errno == ENOEXEC)
	{
		write(STDERR_FILENO, "-minishell: ", 12);
		write(STDERR_FILENO, cmd->binary, ft_strlen(cmd->binary));
		write(STDERR_FILENO, ": Exec format error\n", 20);
		free_minishell(cmd->minishell);
		_exit(126);
	}
	else
	{
		write(STDERR_FILENO, "-minishell: execve: ", 20);
		write(STDERR_FILENO, strerror(errno), ft_strlen(strerror(errno)));
		write(STDERR_FILENO, "\n", 1);
		free_minishell(cmd->minishell);
		_exit(1);
	}
}

void	cmd_missing_command_error(t_cmd *cmd)
{
	const char	*path;

	if (!cmd || !cmd->argv || !cmd->argv[0])
	{
		write(STDERR_FILENO, "-minishell: invalid command structure\n", 37);
		_exit(127);
	}

	
	write(STDERR_FILENO, "-minishell: ", 12);
	write(STDERR_FILENO, cmd->argv[0], ft_strlen(cmd->argv[0]));
	
	path = ms_getenv(cmd->minishell, "PATH");
	if (ft_strchr(cmd->argv[0], '/') || !path || path[0] == '\0')
		write(STDERR_FILENO, ": No such file or directory\n", 28);
	else
		write(STDERR_FILENO, ": command not found\n", 20);

	free_minishell(cmd->minishell);
	_exit(127);
}

uint8_t perror_return(char *msg, u_int8_t exit_status)
{
	if (msg)
		perror(msg);
	return (exit_status);
}
