/**
 * @file handle_pwd.c
 * @brief Functions for handling the `pwd` built-in command in Minishell.
 */
#include "minishell.h"

/**
 * @brief Executes the `pwd` command to print the current working directory.
 *
 * This function retrieves and prints the current working directory using
 * `getcwd()`.
 * If `getcwd()` fails (e.g., due to the working directory being deleted
 * or inaccessible),
 * it falls back to retrieving the `PWD` environment variable. If `PWD` is
 * also unavailable, an error handler is invoked.
 *
 * @param cmd Pointer to the command structure containing shell context.
 * @return `EXIT_SUCCESS` (0) if the directory is printed successfully.
 *         `EXIT_FAILURE` (1) if both `getcwd()` and `PWD` retrieval fail.
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
			return (cmd_error_handler(cmd, EXIT_FAILURE));
		printf("%s\n", w_dir);
		return (EXIT_SUCCESS);
	}
	printf("%s\n", working_dir);
	return (EXIT_SUCCESS);
}

/**
 * @brief Handles the `pwd` built-in command in Minishell.
 *
 * The `pwd` command prints the current working directory.
 *
 * Behavior:
 * - If no arguments are given, it prints the current directory.
 * - If arguments are provided:
 * - If the first argument is an invalid option (e.g., `pwd -x`), an error
 *  is printed.
 * - Otherwise, it behaves like `pwd` with no arguments.
 * - If executed within a pipeline, the function ensures the process exits
 *   correctly.
 *
 * @param cmd Pointer to the command structure.
 * @return `EXIT_SUCCESS` (0) if the working directory is printed successfully.
 *         `EXIT_FAILURE` (1) if an error occurs (e.g., invalid option).
 *         `2` if an invalid option is detected.
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
	return (exit_status);
}
