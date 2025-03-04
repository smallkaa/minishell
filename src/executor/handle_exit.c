#include "minishell.h"

/**
 * Handles the `exit` command.
 * - If no command or no arguments are provided,
 * 	 exits with status `EXIT_FAILURE`.
 * - If more than one argument is provided,
 * 	 prints "too many arguments" and returns `EXIT_FAILURE` without exiting.
 * - If a single numeric argument is provided,
 * 	 converts it to an integer using `ft_atoi()`, else set to exit code to zero.
 * - Saves exit status to shell structer.
 * - Terminates the shell using `exit(status)`,
 * 	 where `status` is the provided exit code.
 *
 * @param cmd	The command structure containing arguments.
 * 				- cmd->argv[0] should contain "exit".
 * 				- cmd->argv[1] (optional) is the exit status argument.
 * 				- If cmd->argv[2] exists, the function returns an error.
 *
 * @return		Returns `EXIT_FAILURE (1)` if "too many arguments" is detected.
 * 				Does not return if `exit()` is called.
 */
int	handle_exit(t_cmd *cmd)
{
	int	status;

	if (!cmd || !cmd->argv[0])
	{
		ft_putstr_fd("minishell: handle_exit() no arguments found\n", STDERR_FILENO);
		return (EXIT_FAILURE);
	}
	if (cmd->argv[1] && cmd->argv[2])
	{
		ft_putstr_fd("minishell: handle_exit() too many arguments\n", STDERR_FILENO);
		return (EXIT_FAILURE);
	}
	if (!cmd->argv[1])
		status = 0;
	else
		status = ft_atoi(cmd->argv[1]);
	update_last_exit_status(cmd, status);
	exit(status);
}
