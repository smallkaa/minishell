#include "minishell.h"

void	print_and_exit(t_exit_info info)
{
	if (info.prefix)
		print_error(info.prefix);
	if (info.arg)
		print_error(info.arg);
	if (info.msg)
		print_error(info.msg);
	if (info.mshell)
		free_minishell(info.mshell);
	_exit(info.code);
}

void	child_execve_error(t_cmd *cmd)
{
	handle_is_directory(cmd);
	handle_not_found_or_command(cmd);
	handle_permission_denied(cmd);
	handle_exec_format_error(cmd);
	handle_generic_execve_error(cmd);
}
