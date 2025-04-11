#include "minishell.h"

void	handle_pipe_creation(t_cmd *cmd, int *pipe_fd)
{
	if (cmd->next && pipe(pipe_fd) == -1)
	{
		perror("-exec_in_pipes: pipe");
		exit(EXIT_FAILURE);
	}
}
