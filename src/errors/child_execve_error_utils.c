#include "minishell.h"

void	handle_is_directory(t_cmd *cmd)
{
	struct stat	st;

	if (stat(cmd->binary, &st) == 0 && S_ISDIR(st.st_mode))
	{
		print_and_exit((t_exit_info){
			.prefix = "-minishell: ",
			.arg = cmd->binary,
			.msg = ": Is a directory\n",
			.code = 126,
			.mshell = cmd->minishell
		});
	}
}

void	handle_not_found_or_command(t_cmd *cmd)
{
	char	*path;

	if (errno != ENOENT)
		return ;
	print_error("-minishell: ");
	print_error(cmd->argv[0]);
	path = ms_getenv(cmd->minishell, "PATH");
	if (ft_strchr(cmd->argv[0], '/') || !path || path[0] == '\0')
		print_error(": No such file or directory\n");
	else
		print_error(": command not found\n");
	_exit(127);
}

void	handle_permission_denied(t_cmd *cmd)
{
	if (errno == EACCES)
	{
		print_and_exit((t_exit_info){
			.prefix = "-minishell: ",
			.arg = cmd->binary,
			.msg = ": Permission denied\n",
			.code = 126,
			.mshell = cmd->minishell
		});
	}
}

void	handle_exec_format_error(t_cmd *cmd)
{
	if (errno == ENOEXEC)
	{
		print_and_exit((t_exit_info){
			.prefix = "-minishell: ",
			.arg = cmd->binary,
			.msg = ": Exec format error\n",
			.code = 126,
			.mshell = cmd->minishell
		});
	}
}

void	handle_generic_execve_error(t_cmd *cmd)
{
	print_error("-minishell: execve: ");
	print_error(strerror(errno));
	print_error("\n");
	free_minishell(cmd->minishell);
	_exit(EXIT_FAILURE);
}
