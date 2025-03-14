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
	char	working_dir[PATH_MAX];

	ft_bzero(working_dir, PATH_MAX);
	if (!getcwd(working_dir, PATH_MAX))
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
 * - If arguments are provided, prints an error message and exits with status `2`.
 * - If executed in a pipeline, the process exits with the appropriate status.
 *
 * @param cmd Pointer to the command structure.
 * @return `EXIT_SUCCESS` if successful, `EXIT_FAILURE` if an error occurs.
 */
uint8_t	handle_pwd(t_cmd *cmd)
{
	uint8_t	exit_status;
	char	*inv_flag;

	if (!cmd->argv[1])
		exit_status = exec_pwd(cmd);
	else if (cmd->argv[1][0] == '-')
	{
		inv_flag = cmd->argv[1];
		write(STDERR_FILENO, "minishell: pwd: ", 16);
		write(STDERR_FILENO, inv_flag, 2);
		write(STDERR_FILENO, ": invalid option\n", 17);
		print_error("pwd: usage: pwd\n");
		exit_status = 2;
	}
	else
		exit_status = exec_pwd(cmd);
	cmd->minishell->exit_status = exit_status;
	if (cmd->in_pipe)
		exit(exit_status);
	return (exit_status);
}

