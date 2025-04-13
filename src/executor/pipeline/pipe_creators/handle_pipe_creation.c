#include "minishell.h"

void	handle_pipe_creation(t_cmd *cmd, int *pipe_fd)
{
	printf("DEBUG: in handle_pipe_creation()\n");

	if (cmd->next)
	{
		if (pipe(pipe_fd) == -1)
		{
			printf("DEBUG: handle_pipe_creation(): pipe created FAIL for '%s'\n", cmd->argv[0]);

			perror("-exec_in_pipes: pipe");
			exit(EXIT_FAILURE);
		}
		printf("DEBUG: handle_pipe_creation(): pipe created SUCCESS for '%s'\n", cmd->argv[0]);
	}
	else
		printf("DEBUG: handle_pipe_creation(): No pipe needed for '%s'\n", cmd->argv[0]);

}
