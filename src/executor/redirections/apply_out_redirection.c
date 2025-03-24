#include "minishell.h"

uint8_t	apply_out_redirection(t_cmd *cmd)
{
	int	out_fd;
	int	mode;

	if (pre_exec_validation(cmd, R_OUTPUT) == EXIT_FAILURE)
		return (EXIT_FAILURE);
	mode = O_WRONLY | O_CREAT;
	if (cmd->out_redir->type == R_OUTPUT)
		mode |= O_TRUNC;
	else if (cmd->out_redir->type == R_APPEND)
		mode |= O_APPEND;
	else
		perror_return("unknown redirection mode:", EXIT_FAILURE);

	out_fd = open(cmd->out_redir->filename, mode, 0644);
	if (out_fd < 0)
		perror_return("open in_redir", EXIT_FAILURE);

	if (dup2(out_fd, STDOUT_FILENO) == -1)
	{
		if (close(out_fd) == -1)
			perror_return("close out_redir after dup2", EXIT_FAILURE);
		perror_return("out_redir", EXIT_FAILURE);
	}
	if (close(out_fd) == -1)
			perror_return("close out_redir", EXIT_FAILURE);
	return (EXIT_SUCCESS);
}
