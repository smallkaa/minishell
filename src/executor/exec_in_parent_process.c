#include "minishell.h"


uint8_t exec_in_parent_process(t_cmd *cmd)
{
	int		saved_stdin;
	uint8_t	exit_status;

	saved_stdin = dup(STDIN_FILENO);
	if (saved_stdin == -1)
	{
		perror("dup");
		return (EXIT_FAILURE);
	}

	exit_status = apply_redirections(cmd);
	if (exit_status == (EXIT_FAILURE))
	{
		close(saved_stdin);
		return (EXIT_FAILURE);
	}
	exit_status = exec_builtin(cmd);

	// Restore the original STDIN.
	if (dup2(saved_stdin, STDIN_FILENO) == -1) {
		perror("dup2");
		close(saved_stdin);
		return (EXIT_FAILURE);
	}
	close(saved_stdin);

	return (exit_status);
}
