#include "minishell.h"

/**
 * ft_putstr_custom - Writes a string to STDOUT with error handling.
 * @cmd: The command structure containing arguments.
 * @str: The string to print.
 *
 * Return: EXIT_SUCCESS (0) on success, EXIT_FAILURE (1) on failure.
 */
static int	ft_putstr_custom(t_cmd *cmd, char *str)
{
	ssize_t	written;

	written = write(STDOUT_FILENO, str, ft_strlen(str));
	if (written == -1)
	{
		error_handler(cmd);
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}

/**
 * print_content - Prints the arguments of the echo command.
 * @param cmd: The command structure containing arguments.
 * @param i: The starting index after processing flags.
 * @param newline_flag: A pointer to the newline flag (1 = print newline, 0 = no newline).
 *
 * Return: EXIT_SUCCESS (0) on success, EXIT_FAILURE (1) on failure.
 */
static int	print_content(t_cmd *cmd, int i, int *newline_flag)
{
	if (!cmd->argv[1])
		return (ft_putstr_custom(cmd, "\n"));
	while (cmd->argv[i])
	{
		if (!cmd->argv[i])
		{
			print_error("Error: print_content, string == NULL\n");
			return (EXIT_FAILURE);
		}
		if (ft_putstr_custom(cmd, cmd->argv[i]) == EXIT_FAILURE)
			return (EXIT_FAILURE);
		if (cmd->argv[i + 1] && ft_putstr_custom(cmd, " ") == EXIT_FAILURE)
			return (EXIT_FAILURE);
		i++;
	}
	if (*newline_flag && ft_putstr_custom(cmd, "\n") == EXIT_FAILURE)
		return (EXIT_FAILURE);
	return (EXIT_SUCCESS);
}

/**
 * handle_echo_flags - Parses and handles the -n flags for echo.
 * @param cmd: The command structure containing arguments.
 * @param newline_flag: Pointer to an integer flag (1 = print newline, 0 = no newline).
 *
 * Return: The index of the first non-flag argument.
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
 * handle_echo - Implements the 'echo' command.
 * @param cmd: The command structure containing arguments.
 */
void	handle_echo(t_cmd *cmd)
{
	int	i;
	int	status;
	int	newline_flag;
	
	newline_flag = 1;
	i = handle_echo_flags(cmd, &newline_flag);
	status = print_content(cmd, i, &newline_flag);
	update_last_exit_status(cmd, status);
	if (cmd->in_pipe)
		exit(status);
}
