#include "minishell.h"

/**
 * Handles the `exit` command.
 * - If no arguments are provided, exits with `EXIT_SUCCESS (0)`.
 * - If more than one argument is provided, prints an error and
 *   exits with `EXIT_FAILURE (1)`.
 * - If a single numeric argument is provided, converts it to an
 *   integer using `ft_atoi()`
 *   and exits with that status.
 * - Updates the shell's last exit status before terminating the process.
 *
 * Error Handling:
 * - If too many arguments are given (`cmd->argv[2]` exists), prints an error
 *   and exits with `EXIT_FAILURE`.
 * - If `cmd->argv[1]` is not a valid integer, `ft_atoi()` should handle
 *   it properly.
 *
 * @param cmd   The command structure containing arguments.
 *              - cmd->argv[0] should be `"exit"`.
 *              - cmd->argv[1] (optional) is the exit status argument.
 *              - cmd->argv[2] (if exists) triggers an error.
 *
 * @return      Does not return (calls `exit(status)`) unless an error occurs.
 */
void	handle_exit(t_cmd *cmd)
{
	int	status;

	if (cmd->argv[1] && cmd->argv[2])
	{
		update_last_exit_status(cmd, EXIT_FAILURE);
		print_error("minishell: handle_exit too many args\n");
		exit (EXIT_FAILURE);
	}
	if (!cmd->argv[1])
		status = EXIT_SUCCESS;
	else
		status = ft_atoi(cmd->argv[1]);
	update_last_exit_status(cmd, status);
	exit (status);
}
