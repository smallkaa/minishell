#include "minishell.h"

static int	backup_fd(int fd)
{
	int	backup;

	backup = dup(fd);
	if (backup == -1)
		return (perror_return("backup_fd: dup", -1));
	return (backup);
}

static uint8_t	restore_fd(int saved_fd, int target_fd)
{
	if (dup2(saved_fd, target_fd) == -1)
	{
		safe_close(&saved_fd);
		return (perror_return("restore_fd: dup2", EXIT_FAILURE));
	}
	safe_close(&saved_fd);
	return (EXIT_SUCCESS);
}

static uint8_t	redirect_and_execute(t_cmd *cmd)
{
	uint8_t	exit_status;

	exit_status = apply_redirections(cmd);
	if (exit_status != EXIT_SUCCESS)
		return (exit_status);
	return (exec_builtins(cmd));
}

uint8_t	exec_in_current_process(t_cmd *cmd)
{
	int			saved_stdout;
	int			saved_stdin;
	uint8_t		exit_status;

	saved_stdin = backup_fd(STDIN_FILENO);
	saved_stdout = backup_fd(STDOUT_FILENO);
	if (saved_stdin == -1 || saved_stdout == -1)
		return (EXIT_FAILURE);
	exit_status = redirect_and_execute(cmd);
	if (restore_fd(saved_stdout, STDOUT_FILENO) != EXIT_SUCCESS
		|| restore_fd(saved_stdin, STDIN_FILENO) != EXIT_SUCCESS)
		return (EXIT_FAILURE);
	return (exit_status);
}
