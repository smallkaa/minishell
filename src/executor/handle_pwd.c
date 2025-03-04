#include "minishell.h"

/**
 * Executes the `pwd` (print working directory) command.
 * - Calls `getcwd()` to retrieve the current working directory.
 * - Prints the working directory followed by a newline.
 * - If the working directory cannot be retrieved (e.g., path too long),
 *   prints an error message and returns `EXIT_FAILURE`.
 * - If the command is part of a pipeline (`cmd->in_pipe` is `true`),
 *   it exits the process after execution.
 *
 * @param cmd   The command structure containing execution context.
 *              - cmd->in_pipe: Indicates if the command is in a pipeline.
 *
 * @return      Returns `EXIT_SUCCESS (0)` if the working directory
 *              is printed successfully.
 *              Returns `EXIT_FAILURE (1)` if `getcwd()` fails.
 *              If in a pipeline, calls `exit(status)`.
 */
int	exec_pwd(t_cmd *cmd)
{
	char	*working_dir;
	char	buf[PATH_MAX];

	ft_bzero(buf, PATH_MAX);
	working_dir = getcwd(buf, PATH_MAX);
	if (!working_dir)
	{
		print_error("minishell: exec_pwd, pathname too long\n");
		update_last_exit_status(cmd, EXIT_FAILURE);
		if (cmd->in_pipe)
			exit (EXIT_FAILURE);
		return (EXIT_FAILURE);
	}
	printf("%s\n", working_dir);
	return (EXIT_SUCCESS);
}

/**
 * Handles the execution of the `pwd` command.
 * - Ensures that `pwd` is called with zero arguments.
 * - Calls `exec_pwd(cmd)` to print the working directory.
 * - Updates the shell’s last exit status.
 * - If `pwd` is part of a pipeline (`cmd->in_pipe`), it exits the process.
 *
 * Error Handling:
 * - If extra arguments are present (`cmd->argv[1]` exists),
 *   prints an error and returns `EXIT_FAILURE`.
 *
 * @param cmd   The command structure containing arguments.
 *              - cmd->argv[0] should contain "pwd".
 *              - cmd->argv[1] (if exists) triggers an error.
 *              - cmd->in_pipe indicates if it's part of a pipeline.
 *
 * @return      Returns `EXIT_SUCCESS (0)` on successful execution.
 *              Returns `EXIT_FAILURE (1)` if an error occurs
 *              (e.g., extra arguments).
 *              If in a pipeline, calls `exit(status)`.
 */
void	handle_pwd(t_cmd *cmd)
{
	int	status;

	if (cmd->argv[1])
	{
		update_last_exit_status(cmd, EXIT_FAILURE);
		print_error("minishell: handle_pwd, too many args\n");
		if (cmd->in_pipe)
			exit (EXIT_FAILURE);
		return ;
	}
	else
		status = exec_pwd(cmd);
	update_last_exit_status(cmd, status);
	if (cmd->in_pipe)
		exit (status);
	return ;
}
