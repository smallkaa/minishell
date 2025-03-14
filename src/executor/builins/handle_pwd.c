/**
 * @file handle_pwd.c
 * @brief Implementation of the `pwd` built-in command in Minishell.
 */
#include "minishell.h"

/**
 * @brief Executes the `pwd` command to print the current working directory.
 *
 * Retrieves the current working directory using `getcwd()` and prints it.
 * If `getcwd()` fails, an error handler is invoked.
 *
 * @param cmd Pointer to the command structure.
 * @return `EXIT_SUCCESS` if successful, `EXIT_FAILURE` if `getcwd()` fails.
 */
static uint8_t	exec_pwd(t_cmd *cmd)
{
	char	*working_dir;
	char	buf[MS_PATHMAX];

	ft_bzero(buf, MS_PATHMAX);
	working_dir = getcwd(buf, MS_PATHMAX);
	if (!working_dir)
	{
		cmd_error_handler(cmd);
		return (EXIT_FAILURE);
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

