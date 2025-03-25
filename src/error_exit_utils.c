#include "minishell.h"


void	print_error(char *msg)
{
	ft_putstr_fd(msg, STDERR_FILENO);
}

// void	child_execve_error(t_cmd *cmd)
// {
// 	struct stat	st;

// 	if (stat(cmd->binary, &st) == 0 && S_ISDIR(st.st_mode))
// 	{
// 		// Match Bash: "Is a directory"
// 		print_error("-minishell: ");
// 		print_error(cmd->binary);
// 		print_error(": Is a directory\n");
// 		_exit(126);
// 	}
// 	if (errno == ENOENT)
// 	{
// 		print_error(cmd->binary);
// 		if (ft_strchr(cmd->argv[0], '/'))
// 		{
// 			print_error("-minishell: ");
// 			print_error(": No such file or directory\n");
// 		}
// 		else
// 			print_error(": command not found\n");
// 		_exit(127);
// 	}
// 	else if (errno == EACCES)
// 	{
// 		print_error("-minishell: ");
// 		print_error(cmd->binary);
// 		print_error(": Permission denied\n");
// 		_exit(126);
// 	}
// 	else
// 	{
// 		print_error("-minishell: ");
// 		perror("execve");
// 		_exit(1);
// 	}
// }

void	child_execve_error(t_cmd *cmd)
{
	char		error_buf[ERROR_BUF_SIZE];
	struct stat	st;

	ft_bzero(error_buf, ERROR_BUF_SIZE);
	if (stat(cmd->binary, &st) == 0 && S_ISDIR(st.st_mode))
	{
		ft_strlcat(error_buf, "-minishell: ", ERROR_BUF_SIZE);
		ft_strlcat(error_buf, cmd->binary, ERROR_BUF_SIZE);
		ft_strlcat(error_buf, ": Is a directory\n", ERROR_BUF_SIZE);
		if (write(STDERR_FILENO, error_buf, ft_strlen(error_buf)) < 0)
			write(STDERR_FILENO, "-minishell: failed to print error\n", 34);
		_exit(126);
	}
	if (errno == ENOENT)
	{
		ft_strlcat(error_buf, "-minishell: ", ERROR_BUF_SIZE);
		ft_strlcat(error_buf, cmd->binary, ERROR_BUF_SIZE);
		if (ft_strchr(cmd->argv[0], '/'))
			ft_strlcat(error_buf, ": No such file or directory\n", ERROR_BUF_SIZE);
		else
			ft_strlcat(error_buf, ": command not found\n", ERROR_BUF_SIZE);
		if (write(STDERR_FILENO, error_buf, ft_strlen(error_buf)) < 0)
			write(STDERR_FILENO, "-minishell: failed to print error\n", 34);
		_exit(127);
	}
	else if (errno == EACCES)
	{
		ft_strlcat(error_buf, "-minishell: ", ERROR_BUF_SIZE);
		ft_strlcat(error_buf, cmd->binary, ERROR_BUF_SIZE);
		ft_strlcat(error_buf, ": Permission denied\n", ERROR_BUF_SIZE);

		if (write(STDERR_FILENO, error_buf, ft_strlen(error_buf)) < 0)
			write(STDERR_FILENO, "-minishell: failed to print error\n", 34);
		_exit(126);
	}
	else
	{
		ft_strlcat(error_buf, "-minishell: execve: ", ERROR_BUF_SIZE);
		ft_strlcat(error_buf, strerror(errno), ERROR_BUF_SIZE);
		ft_strlcat(error_buf, "\n", ERROR_BUF_SIZE);

		if (write(STDERR_FILENO, error_buf, ft_strlen(error_buf)) < 0)
			write(STDERR_FILENO, "-minishell: failed to print error\n", 34);
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
	_exit(127);
}


uint8_t	perror_return(char *msg, u_int8_t exit_status)
{
	if (msg)
		perror(msg);
	return (exit_status);

}
