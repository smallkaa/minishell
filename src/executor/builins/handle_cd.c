#include "minishell.h"

/**
 * handle_cd - Handles the 'cd' command to change the working directory.
 *
 * Behavior:
 * - If `cmd->argv[1]` is present, it attempts to change to the specified directory.
 * - If `cmd->argv[1]` is missing, it should default to `$HOME`.
 * - If `chdir()` fails, an error message is printed, and the exit status is updated.
 * - If the command is **executed inside a pipe**, it exits with the failure status.
 *
 * @param cmd Pointer to the `t_cmd` structure containing command arguments.
 * 
 * More: https://pubs.opengroup.org/onlinepubs/009695299/functions/chdir.html
 */
void	handle_cd(t_cmd *cmd)
{
	int		status;
	char	*path;

	status = EXIT_SUCCESS;

	// if (!cmd->argv[1])
	// 	path = $HOME  // replace with func 
	// else	
	
	path = cmd->argv[1];
	if (chdir(path) != 0)
	{
		status = EXIT_FAILURE;
		update_last_exit_status(cmd, status);
		error_handler(cmd);
		if (cmd->in_pipe)
			exit (status);
		return ;
	}
	update_last_exit_status(cmd, status);
	if (cmd->in_pipe)
		exit (status);
	return ;
}
