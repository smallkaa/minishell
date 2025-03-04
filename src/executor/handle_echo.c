#include "minishell.h"

/**
 * Writes a string to the specified file descriptor.
 * - If `str` is `NULL` or `fd` is invalid, prints an error message.
 * - Uses `write()` to output the string.
 *
 * @param str   The string to be written.
 * @param fd    The file descriptor to write to (e.g., STDOUT_FILENO).
 *
 * @return      Returns the number of bytes written.
 *              Returns `-1` if an error occurs.
 */
static int	ft_putstr_custom(char *str, int fd)
{
	if (!str || fd < 0)
	{
		print_error("minishell: Error writing output\n");
		return (-1);
	}
	return (write(fd, str, ft_strlen(str)));
}

/**
 * Prints command arguments to `stdout`, separated by spaces.
 * - Handles multiple words correctly.
 * - Prints a newline unless `newline_flag` is set to `0`.
 *
 * @param cmd           The command structure containing arguments.
 * @param i             The starting index of arguments after
 *                      flag processing.
 * @param newline_flag  Pointer to an integer flag
 *                      (1 = print newline, 0 = no newline).
 *
 * @return              Returns `EXIT_SUCCESS (0)` on success.
 *                      Returns `EXIT_FAILURE (1)` on failure.
 */
static int	print_content(t_cmd *cmd, int i, int *newline_flag)
{
	int	status;

	status = EXIT_SUCCESS;
	while (cmd->argv[i])
	{
		if (ft_putstr_custom(cmd->argv[i], STDOUT_FILENO) < 0)
			status = EXIT_FAILURE;
		if (cmd->argv[i + 1] && ft_putstr_custom(" ", STDOUT_FILENO) < 0)
			status = EXIT_FAILURE;
		i++;
	}
	if (*newline_flag == 1 && ft_putstr_custom("\n", STDOUT_FILENO) < 0)
		status = EXIT_FAILURE;
	return (status);
}

/**
 * Parses `-n` flags for `echo` command.
 * - If multiple `-n` flags are present (`echo -n -n`), all are processed.
 * - Stops flag processing when a non-`-n` argument is found.
 * - Updates `newline_flag` to `0` if `-n` is detected.
 *
 * @param cmd           The command structure containing arguments.
 * @param newline_flag  Pointer to an integer flag
 *                      (1 = print newline, 0 = no newline).
 *
 * @return              Returns the index of the first non-flag argument.
 */
static int	handle_echo_flags(t_cmd *cmd, int *newline_flag)
{
	int	i;
	int	j;

	i = 1;
	while (cmd->argv[i] && cmd->argv[i][0] == '-' && cmd->argv[i][1] == 'n')
	{
		j = 1;
		while (cmd->argv[i][j] == 'n')
			j++;
		if (cmd->argv[i][j] != '\0')
			break ;
		*newline_flag = 0;
		i++;
	}
	return (i);
}

/**
 * Executes the `echo` command.
 * - Processes `-n` flags using `handle_echo_flags()`.
 * - Prints remaining arguments using `print_content()`.
 * - Updates `last_exit_status` based on success or failure.
 * - If inside a pipeline (`cmd->in_pipe`), exits after execution.
 *
 * Error Handling:
 * - If `cmd->argv[1]` is NULL, prints only a newline.
 * - If writing to `stdout` fails, returns `EXIT_FAILURE (1)`.
 *
 * @param cmd   The command structure containing arguments.
 *
 * @return      Returns `EXIT_SUCCESS (0)` if successful.
 *              Returns `EXIT_FAILURE (1)` if writing to `stdout` fails.
 */
void	handle_echo(t_cmd *cmd)
{
	int		i;
	int		status;
	int		newline_flag;

	if (!cmd->argv[1])
	{
		ft_putstr_fd("\n", STDOUT_FILENO);
		update_last_exit_status(cmd, EXIT_SUCCESS);
		if (cmd->in_pipe)
			exit (EXIT_SUCCESS);
		return ;
	}
	newline_flag = 1;
	i = handle_echo_flags(cmd, &newline_flag);
	status = print_content(cmd, i, &newline_flag);
	if (status < 0)
		status = EXIT_FAILURE;
	else
		status = EXIT_SUCCESS;
	update_last_exit_status(cmd, status);
	if (cmd->in_pipe)
		exit (status);
	return ;
}
