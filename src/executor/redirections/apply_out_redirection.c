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
	{
		print_error("Error: unknown redirection mode\n");
		return (EXIT_FAILURE);
	}

	out_fd = open(cmd->out_redir->filename, mode, 0644);
	if (out_fd < 0)
	{
		ft_putstr_fd("-minishell: ", STDERR_FILENO);
		perror(cmd->out_redir->filename);
		return(EXIT_FAILURE);
	}
	if (dup2(out_fd, STDOUT_FILENO) == -1)
	{
		// fprintf(stderr, "errno: %d (%s)\n", errno, strerror(errno));
		ft_putstr_fd("-minishell: ", STDERR_FILENO);
		perror("out_fd");
		if (close(out_fd) == -1)
		{
			ft_putstr_fd("-minishell: ", STDERR_FILENO);
			perror("out_fd");
			return(EXIT_FAILURE);
		}
		return(EXIT_FAILURE);
	}
	if (close(out_fd) == -1)
	{
		ft_putstr_fd("-minishell: ", STDERR_FILENO);
		perror("out_fd");
		return(EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}
