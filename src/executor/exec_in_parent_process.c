#include "minishell.h"

uint8_t exec_in_parent_process(t_cmd *cmd)
{
	int		saved_stdout;
	int		saved_stdin;;
	uint8_t	exit_status;

	saved_stdin = dup(STDIN_FILENO);
	if (saved_stdin == -1)
		perror_return("dup #1: exec_in_parent_process", EXIT_FAILURE);
	saved_stdout = dup(STDOUT_FILENO);
	if (saved_stdout == -1)
		perror_return("dup #2: exec_in_parent_process", EXIT_FAILURE);
	exit_status = apply_redirections(cmd);
	if (exit_status == EXIT_FAILURE)
	{
		if (close(saved_stdout) == -1)
			perror_return("close #1: exec_in_parent_process", EXIT_FAILURE);
		return (EXIT_FAILURE);
	}
	exit_status = exec_builtin(cmd);

	// Restore STDOUT
	if (dup2(saved_stdout, STDOUT_FILENO) == -1)
	{
		if (close(saved_stdout) == -1)
			perror_return("close #2: exec_in_parent_process", EXIT_FAILURE);
		perror_return("dup #3: exec_in_parent_process", EXIT_FAILURE);
	}
	if (close(saved_stdout) == -1)
		perror_return("close: exec_in_parent_process", EXIT_FAILURE);

	// Restore STDIN
	if (dup2(saved_stdin, STDIN_FILENO) == -1)
	{
		if (close(saved_stdin) == -1)
			perror_return("close #3: exec_in_parent_process", EXIT_FAILURE);
		perror_return("dup #4: exec_in_parent_process", EXIT_FAILURE);

	}
	if (close(saved_stdin) == -1)
		perror_return("close #4: exec_in_parent_process", EXIT_FAILURE);
	return (exit_status);
}

