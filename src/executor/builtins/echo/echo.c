#include "minishell.h"

/**
 * @brief Writes a string to the standard output.
 *
 * This function uses `write()` to print a string to `STDOUT_FILENO`.
 * If the write operation fails, it calls an error handler and returns
 * `EXIT_FAILURE`.
 *
 * @param cmd Pointer to the command structure for error handling.
 * @param str The string to be printed.
 * @return `EXIT_SUCCESS` (0) on success, `EXIT_FAILURE` (1) on failure.
 */
static int	ft_putstr_custom(char *str)
{
	ssize_t	written;

	written = write(STDOUT_FILENO, str, ft_strlen(str));
	if (written == -1)
	{
		perror("minishell: write");
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}

/**
 * @brief Prints the arguments provided to the `echo` command.
 *
 * Iterates over the command arguments, printing each word followed by a space,
 * unless it is the last word. If the `-n` flag is **not** set, a newline
 * is printed at the end.
 *
 * @param cmd Pointer to the command structure.
 * @param i Starting index of arguments to print.
 * @param newline_flag Pointer to an integer indicating if a newline
 *                     should be printed (`1` = newline, `0` = no newline).
 * @return `EXIT_SUCCESS` (0) on success, `EXIT_FAILURE` (1) on failure.
 */
static uint8_t	print_content(t_cmd *cmd, int i, int *newline_flag)
{
	if (!cmd->argv[1])
		return (ft_putstr_custom("\n"));
	while (cmd->argv[i])
	{
		if (!cmd->argv[i])
		{
			print_error("Error: print_content, string == NULL\n");
			return (EXIT_FAILURE);
		}
		if (ft_putstr_custom(cmd->argv[i]) == EXIT_FAILURE)
			return (EXIT_FAILURE);
		if (cmd->argv[i + 1] && ft_putstr_custom(" ") == EXIT_FAILURE)
			return (EXIT_FAILURE);
		i++;
	}
	if (*newline_flag && ft_putstr_custom("\n") == EXIT_FAILURE)
		return (EXIT_FAILURE);
	return (EXIT_SUCCESS);
}

/**
 * @brief Handles `-n` flags in the `echo` command.
 *
 * Parses arguments that start with `-n`, ensuring they contain only `n`
 * characters.
 * If valid, it **disables** the newline at the end of the output.
 *
 * @param cmd Pointer to the command structure.
 * @param newline_flag Pointer to an integer (`1` = newline, `0` = no newline).
 * @return The index from where to start printing arguments.
 */
static int	handle_echo_flags(t_cmd *cmd, int *newline_flag)
{
	int	i;

	i = 1;
	while (cmd->argv[i]
		&& is_echo_flag(cmd->argv[i])
		&& !is_wrapped_in_quotes(cmd->argv[i]))
	{
		*newline_flag = 0;
		i++;
	}
	return (i);
}

/**
 * @brief Handles the `echo` built-in command in Minishell.
 *
 * Implements the `echo` functionality, supporting the `-n` flag
 * to suppress the trailing newline. Calls helper functions to parse flags
 * and print content.
 *
 * **Behavior:**
 * - If `-n` is **present**, `echo` will print without a newline.
 * - If no arguments are given, `echo` prints only a newline.
 * - Otherwise, it prints all arguments separated by spaces.
 *
 * @param cmd Pointer to the command structure.
 * @return `EXIT_SUCCESS` (0) on success, `EXIT_FAILURE` (1) on failure.
 */
uint8_t	handle_echo(t_cmd *cmd)
{
	int		i;
	uint8_t	exit_status;
	int		newline_flag;

	newline_flag = 1;
	i = handle_echo_flags(cmd, &newline_flag);
	exit_status = print_content(cmd, i, &newline_flag);
	return (exit_status);
}
