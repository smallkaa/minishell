/**
 * @file pwd.c
 * @brief Implementation of the `pwd` built-in command in Minishell.
 *
 * This file provides logic for the `pwd` command, which prints the current
 * working directory. It handles valid and invalid arguments and falls back to
 * the `PWD` environment variable when `getcwd()` fails.
 */
#include "minishell.h"

/**
 * @brief Prints an error message for an invalid option passed to
 * a built-in command.
 *
 * Formats the message similarly to Bash when a command receives
 * an invalid flag.
 * Shows only the first two characters of the invalid option, and prints
 * a usage line.
 *
 * Example:
 * ```
 * minishell: pwd: -x: invalid option
 * minishell: pwd: usage: pwd
 * ```
 *
 * @param cmd_name The name of the command (e.g., `"pwd"`).
 * @param option The invalid option provided by the user (e.g., `"-x"`).
 * @return Always returns `2`, the standard exit status for invalid option usage.
 */
static uint8_t	invalid_opt_exit(const char *cmd_name, const char *option)
{
	uint8_t	exit_status;
	char	error_buf[ERROR_BUF_SIZE];
	char	opt_buf[3];

	ft_strlcpy(opt_buf, option, 3);
	ft_strlcpy(error_buf, "minishell: ", ERROR_BUF_SIZE);
	ft_strlcat(error_buf, cmd_name, ERROR_BUF_SIZE);
	ft_strlcat(error_buf, ": ", ERROR_BUF_SIZE);
	ft_strlcat(error_buf, opt_buf, ERROR_BUF_SIZE);
	ft_strlcat(error_buf, ": invalid option\n", ERROR_BUF_SIZE);
	ft_strlcat(error_buf, cmd_name, ERROR_BUF_SIZE);
	ft_strlcat(error_buf, ": usage: ", ERROR_BUF_SIZE);
	ft_strlcat(error_buf, cmd_name, ERROR_BUF_SIZE);
	ft_strlcat(error_buf, "\n", ERROR_BUF_SIZE);
	if (write(STDERR_FILENO, error_buf, ft_strlen(error_buf)) < 0)
		write(STDERR_FILENO, "minishell: error: failed to print error\n", 40);
	exit_status = 2;
	return (exit_status);
}

/**
 * @brief Executes the core functionality of the `pwd` command.
 *
 * Attempts to retrieve the current working directory using `getcwd()`.
 * If `getcwd()` fails (e.g., due to directory removal or permission issues),
 * the function falls back to the `PWD` environment variable.
 *
 * @param cmd Pointer to the command structure with shell context.
 * @return `EXIT_SUCCESS` (0) if a valid directory is printed,
 *         `EXIT_FAILURE` (1) if both `getcwd()` and `PWD` fail.
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
		{
			perror(cmd->binary);
			return (EXIT_FAILURE);
		}
		printf("%s\n", w_dir);
		return (EXIT_SUCCESS);
	}
	printf("%s\n", working_dir);
	return (EXIT_SUCCESS);
}

/**
 * @brief Handles execution of the `pwd` built-in command.
 *
 * The command prints the absolute path of the current working directory.
 *
 * Behavior:
 * - If no arguments are provided: prints the current working directory.
 * - If an argument starts with `-`: prints an invalid option error and usage.
 * - Otherwise: behaves like plain `pwd`.
 *
 * @param cmd Pointer to the command structure containing arguments
 * and environment.
 * @return `EXIT_SUCCESS` (0) on success,
 *         `EXIT_FAILURE` (1) if directory retrieval fails,
 *         `2` if an invalid option is detected.
 */
uint8_t	handle_pwd(t_cmd *cmd)
{
	uint8_t	exit_status;

	if (!cmd)
		return (no_cmd_error("pwd"));
	if (!cmd->argv[1])
		exit_status = exec_pwd(cmd);
	else if (cmd->argv[1][0] == '-' && cmd->argv[1][1] != '\0')
		exit_status = invalid_opt_exit("pwd", cmd->argv[1]);
	else
		exit_status = exec_pwd(cmd);
	return (exit_status);
}
