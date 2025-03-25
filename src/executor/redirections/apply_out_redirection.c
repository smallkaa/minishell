#include "minishell.h"

static uint8_t	apply_mode(t_cmd *cmd, int *mode)
{
	*mode = O_WRONLY | O_CREAT;
	if (cmd->out_redir->type == R_OUTPUT)
		*mode |= O_TRUNC;
	else if (cmd->out_redir->type == R_APPEND)
		*mode |= O_APPEND;
	else
	{
		ft_putstr_fd("Error: unknown redirection mode\n", STDERR_FILENO);
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}

uint8_t	apply_out_redirection(t_cmd *cmd)
{
	int	out_fd;
	int	mode;

	if (pre_exec_validation(cmd, R_OUTPUT) == EXIT_FAILURE)
		return (EXIT_FAILURE);
	if (apply_mode(cmd, &mode) == EXIT_FAILURE)
		return (EXIT_FAILURE);
	out_fd = open(cmd->out_redir->filename, mode, 0644);
	if (out_fd < 0)
	{
		ft_putstr_fd("-minishell: ", STDERR_FILENO);
		perror_return(cmd->out_redir->filename, EXIT_FAILURE);
	}
	if (dup2(out_fd, STDOUT_FILENO) == -1)
	{
		if (close(out_fd) == -1)
			perror_return("-minishell: close", EXIT_FAILURE);
		perror_return("-minishell: dup2", EXIT_FAILURE);
	}
	if (close(out_fd) == -1)
		perror_return("-minishell: close", EXIT_FAILURE);
	return (EXIT_SUCCESS);
}

// fprintf(stderr, "errno: %d (%s)\n", errno, strerror(errno));
