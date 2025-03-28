#include "minishell.h"

static int	backup_fd(int fd)
{
	int	backup;

	backup = dup(fd);
	if (backup == -1)
		perror_return("-minishell: dup", -1);
	return (backup);
}

static uint8_t	restore_fd(int saved_fd, int target_fd)
{
	if (dup2(saved_fd, target_fd) == -1)
	{
		close(saved_fd);
		perror_return("-minishell: dup2", EXIT_FAILURE);
	}
	if (close(saved_fd) == -1)
		perror_return("-minishell: close", EXIT_FAILURE);
	return (EXIT_SUCCESS);
}

static uint8_t	redirect_and_execute(t_cmd *cmd)
{
	uint8_t	status;

	status = apply_redirections(cmd);
	if (status != EXIT_SUCCESS)
		return (status);
	return exec_builtins(cmd);
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

	if (restore_fd(saved_stdout, STDOUT_FILENO) != EXIT_SUCCESS ||
		restore_fd(saved_stdin, STDIN_FILENO) != EXIT_SUCCESS)
		return (EXIT_FAILURE);

	return (exit_status);
}


// old version
// uint8_t exec_in_current_process(t_cmd *cmd)
// {
// 	int		saved_stdout;
// 	int		saved_stdin;;
// 	uint8_t	exit_status;

// 	saved_stdin = dup(STDIN_FILENO);
// 	if (saved_stdin == -1)
// 		perror_return("-minishell: dup", EXIT_FAILURE);
// 	saved_stdout = dup(STDOUT_FILENO);
// 	if (saved_stdout == -1)
// 		perror_return("-minishell: dup", EXIT_FAILURE);
// 	exit_status = apply_redirections(cmd);
// 	exit_status = exec_builtin(cmd);
// 	if (dup2(saved_stdout, STDOUT_FILENO) == -1)
// 	{
// 		if (close(saved_stdout) == -1)
// 			perror_return("-minishell: close", EXIT_FAILURE);
// 		perror_return("-minishell: dup", EXIT_FAILURE);
// 	}
// 	if (close(saved_stdout) == -1)
// 		perror_return("-minishell: close", EXIT_FAILURE);
// 	if (dup2(saved_stdin, STDIN_FILENO) == -1)
// 	{
// 		if (close(saved_stdin) == -1)
// 			perror_return("-minishell: close", EXIT_FAILURE);
// 		perror_return("-minishell: dup", EXIT_FAILURE);
// 	}
// 	if (close(saved_stdin) == -1)
// 		perror_return("-minishell: close", EXIT_FAILURE);
// 	return (exit_status);
// }

