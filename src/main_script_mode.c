/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_script_mode.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ilia Munaev <ilyamunaev@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/01 15:22:02 by pvershin          #+#    #+#             */
/*   Updated: 2025/05/05 23:21:54 by Ilia Munaev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Opens a script file for reading.
 *
 * Attempts to open the specified file in read-only mode. If the file
 * cannot be opened, an error message is printed to STDERR.
 *
 * @param file Path to the script file.
 * @return File descriptor on success, or -1 on failure.
 */
static int	open_script_file(const char *file)
{
	int	fd;

	fd = open(file, O_RDONLY);
	if (fd < 0)
	{
		print_error("minishell: cannot open script file: ");
		perror(file);
		return (-1);
	}
	return (fd);
}

/**
 * @brief Parses and executes a single line from the script.
 *
 * - Handles interruption signals (e.g., Ctrl+C).
 * - Parses the line into a command using the shell parser.
 * - Executes the command if parsing is successful.
 * - Updates the shell's `exit_status` with the command result.
 *
 * @param mshell The main shell context.
 * @param line The line from the script to parse and execute.
 * @return The exit status of the executed command.
 */
static uint8_t	process_script_line(t_mshell *mshell, char *line)
{
	t_cmd	*cmd;
	uint8_t	status;

	if (g_signal_flag)
	{
		mshell->exit_status = 130;
		g_signal_flag = 0;
		return (mshell->exit_status);
	}
	cmd = run_parser(mshell, line);
	if (!cmd)
	{
		return (mshell->exit_status);
	}
	status = run_executor(cmd);
	mshell->exit_status = status;
	free_cmd(&cmd);
	return (status);
}

/**
 * @brief Runs minishell in script mode by executing commands from a file.
 *
 * - Opens the script file.
 * - Reads and executes each line using the parser and executor.
 * - Frees memory after each line.
 * - Returns the exit status of the last executed line.
 *
 * @param mshell The minishell context containing state and environment.
 * @param file Path to the script file to execute.
 * @return Exit status of the last command in the script,
 * or `EXIT_FAILURE` if file couldn't be opened.
 */
uint8_t	run_script_mode(t_mshell *mshell, const char *file)
{
	char	*line;
	int		in_fd;
	uint8_t	last_line_status;

	in_fd = open_script_file(file);
	if (in_fd < 0)
		return (EXIT_FAILURE);
	last_line_status = EXIT_SUCCESS;
	while (1)
	{
		line = get_next_line(in_fd);
		if (!line)
			break ;
		last_line_status = process_script_line(mshell, line);
		free(line);
		line = NULL;
	}
	safe_close(&in_fd);
	return (last_line_status);
}
