#include "minishell.h"

// uint8_t	wait_for_children(pid_t *pids, int count)
// {
// 	int		i;
// 	int		status;
// 	uint8_t	exit_status;

// 	i = 0;
// 	exit_status = EXIT_SUCCESS;
// 	while (i < count)
// 	{
// 		waitpid(pids[i], &status, 0);
// 		if (i == count - 1)
// 		{
// 			if (WIFEXITED(status))
// 				exit_status = WEXITSTATUS(status);
// 			else if (WIFSIGNALED(status))
// 				exit_status = 128 + WTERMSIG(status);
// 			else
// 				exit_status = EXIT_FAILURE;
// 		}
// 		i++;
// 	}
// 	return (exit_status);
// }
uint8_t	wait_for_children(pid_t *pids, int count)
{
	int		i;
	int		status;
	uint8_t	exit_status;

	i = 0;
	exit_status = EXIT_SUCCESS;
	while (i < count)
	{
		waitpid(pids[i], &status, 0);

		if (i == count - 1) // last process in the pipeline
		{
			if (WIFEXITED(status))
				exit_status = WEXITSTATUS(status);
			else if (WIFSIGNALED(status))
			{
				int sig = WTERMSIG(status);
				if (sig == SIGPIPE)
					exit_status = EXIT_SUCCESS; // Bash behavior
				else
					exit_status = 128 + sig; // Typical Bash-style signal exit
			}
			else
				exit_status = EXIT_FAILURE;
		}
		i++;
	}
	return exit_status;
}
