#include "minishell.h"

static uint8_t	exec_pwd(t_cmd *cmd)
{
	char	*working_dir;
	char	buf[PATH_MAX];

	ft_bzero(buf, PATH_MAX);
	working_dir = getcwd(buf, PATH_MAX);
	if (!working_dir)
	{
		error_handler(cmd);
		return (EXIT_FAILURE);
	}
	printf("%s\n", working_dir);
	return (EXIT_SUCCESS);
}

uint8_t	handle_pwd(t_cmd *cmd)
{
	uint8_t	exit_status;

	if (cmd->argv[1])
	{
		print_error("minishell: pwd: incorrect usage\n");
		exit_status = 2;
	}
	else
	{
		exit_status = exec_pwd(cmd);
	}
	cmd->minishell->exit_status = exit_status;
	if (cmd->in_pipe)
		exit(exit_status);
	return (exit_status);
}
