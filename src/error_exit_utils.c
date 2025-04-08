#include "minishell.h"


void	print_error(char *msg)
{
	ft_putstr_fd(msg, STDERR_FILENO);
}

void	print_export_error(t_mshell_var *pair)
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


void	print_unset_error(char *str)
{
		print_error("-minishell: unset: ");
		write(STDERR_FILENO, str, 2);
		print_error(": invalid option\n");
		print_error("unset: usage: unset [name ...]\n");
}

//BUFFER APPROACH WORKS AS WELL
// void	child_execve_error(t_cmd *cmd)
// {
// 	char		error_buf[ERROR_BUF_SIZE];
// 	struct stat	st;

// 	ft_bzero(error_buf, ERROR_BUF_SIZE);
// 	if (stat(cmd->binary, &st) == 0 && S_ISDIR(st.st_mode))
// 	{
// 		ft_strlcat(error_buf, "-minishell: ", ERROR_BUF_SIZE);
// 		ft_strlcat(error_buf, cmd->binary, ERROR_BUF_SIZE);
// 		ft_strlcat(error_buf, ": Is a directory\n", ERROR_BUF_SIZE);
// 		if (write(STDERR_FILENO, error_buf, ft_strlen(error_buf)) < 0)
// 			write(STDERR_FILENO, "-minishell: failed to print error\n", 34);
// 		free_minishell(cmd->minishell);
// 		_exit(126);
// 	}
// 	if (errno == ENOENT)
// 	{
// 		ft_strlcat(error_buf, "-minishell: ", ERROR_BUF_SIZE);
// 		ft_strlcat(error_buf, cmd->binary, ERROR_BUF_SIZE);
// 		if (ft_strchr(cmd->argv[0], '/'))
// 			ft_strlcat(error_buf, ": No such file or directory\n", ERROR_BUF_SIZE);
// 		else
// 			ft_strlcat(error_buf, ": command not found\n", ERROR_BUF_SIZE);
// 		if (write(STDERR_FILENO, error_buf, ft_strlen(error_buf)) < 0)
// 			write(STDERR_FILENO, "-minishell: failed to print error\n", 34);
// 		free_minishell(cmd->minishell);
// 		_exit(127);
// 	}
// 	else if (errno == EACCES)
// 	{
// 		ft_strlcat(error_buf, "-minishell: ", ERROR_BUF_SIZE);
// 		ft_strlcat(error_buf, cmd->binary, ERROR_BUF_SIZE);
// 		ft_strlcat(error_buf, ": Permission denied\n", ERROR_BUF_SIZE);

// 		if (write(STDERR_FILENO, error_buf, ft_strlen(error_buf)) < 0)
// 			write(STDERR_FILENO, "-minishell: failed to print error\n", 34);
// 		free_minishell(cmd->minishell);
// 		_exit(126);
// 	}
// 	else
// 	{
// 		ft_strlcat(error_buf, "-minishell: execve: ", ERROR_BUF_SIZE);
// 		ft_strlcat(error_buf, strerror(errno), ERROR_BUF_SIZE);
// 		ft_strlcat(error_buf, "\n", ERROR_BUF_SIZE);

// 		if (write(STDERR_FILENO, error_buf, ft_strlen(error_buf)) < 0)
// 			write(STDERR_FILENO, "-minishell: failed to print error\n", 34);
// 		free_minishell(cmd->minishell);
// 		_exit(1);
// 	}
// }

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
		write(STDERR_FILENO, cmd->binary, ft_strlen(cmd->binary));
		if (ft_strchr(cmd->argv[0], '/'))
			write(STDERR_FILENO, ": No such file or directory\n", 28);
		else
			write(STDERR_FILENO, ": command not found\n", 20);
		free_minishell(cmd->minishell);
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
	char error_buf[ERROR_BUF_SIZE];

	if (!cmd || !cmd->argv || !cmd->argv[0])
	{
		write(STDERR_FILENO, "-minishell: invalid command structure\n", 37);
		_exit(127);
	}
	ft_bzero(error_buf, ERROR_BUF_SIZE);
	ft_strlcat(error_buf, cmd->argv[0], ERROR_BUF_SIZE);
	if (strchr(cmd->argv[0], '/'))
		ft_strlcat(error_buf, ": No such file or directory\n", ERROR_BUF_SIZE);
	else
		ft_strlcat(error_buf, ": command not found\n", ERROR_BUF_SIZE);
	if (write(STDERR_FILENO, error_buf, ft_strlen(error_buf)) < 0)
		write(STDERR_FILENO, "-minishell: error: failed to print error\n", 40);
	free_minishell(cmd->minishell);
	_exit(127);
}


uint8_t	perror_return(char *msg, u_int8_t exit_status)
{
	if (msg)
		perror(msg);
	return (exit_status);

}
