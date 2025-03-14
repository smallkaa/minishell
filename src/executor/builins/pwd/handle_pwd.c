/**
 * @file handle_pwd.c
 * @brief Implementation of the `pwd` built-in command in Minishell.
 */
#include "minishell.h"
/**
 * @brief Executes the `pwd` command to print the current working directory.
 *
 * Attempts to retrieve the current working directory using `getcwd()`.
 * If `getcwd()` fails (e.g., due to a deleted working directory), it falls
 * back to the value of the `PWD` environment variable. If `PWD` is also unavailable,
 * an error handler is invoked.
 *
 * @param cmd Pointer to the command structure.
 * @return `EXIT_SUCCESS` if the directory is printed successfully,
 *         `EXIT_FAILURE` if both `getcwd()` and `PWD` retrieval fail.
 */
static uint8_t	exec_pwd(t_cmd *cmd)
{
	char	*w_dir;
	char	working_dir[MS_PATHMAX];

	ft_bzero(working_dir, MS_PATHMAX);
	if (!getcwd(working_dir, MS_PATHMAX))
	{
		w_dir = ms_getenv(cmd->minishell, "PWD");
		if (!w_dir)
			return(cmd_error_handler(cmd, EXIT_FAILURE));
		printf("%s\n", w_dir);
		return (EXIT_SUCCESS);
	}
	printf("%s\n", working_dir);
	return (EXIT_SUCCESS);
}

/**
 * @brief Handles the `pwd` built-in command.
 *
 * - If no arguments are given, prints the current working directory.
 * - If arguments are provided, prints an error message and
 *   exits with status `2`.
 * - If executed in a pipeline, the process exits with the appropriate status.
 *
 * @param cmd Pointer to the command structure.
 * @return `EXIT_SUCCESS` if successful, `EXIT_FAILURE` if an error occurs.
 */
uint8_t	handle_pwd(t_cmd *cmd)
{
	uint8_t	exit_status;

	if (!cmd->argv[1])
		exit_status = exec_pwd(cmd);
	else if (cmd->argv[1][0] == '-' && cmd->argv[1][1] != '\0')
		exit_status = invalid_opt_exit("pwd", cmd->argv[1]);
	else
		exit_status = exec_pwd(cmd);
	cmd->minishell->exit_status = exit_status;
	return (exit_status);
}
