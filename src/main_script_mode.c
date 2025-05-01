/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_script_mode.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pvershin <pvershin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/01 15:22:02 by pvershin          #+#    #+#             */
/*   Updated: 2025/05/01 15:22:05 by pvershin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// Открывает файл скрипта для чтения.
// Возвращает файловый дескриптор или -1 в случае ошибки.
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

// Обрабатывает одну строку из скрипта: парсит и выполняет.
// Возвращает статус выполнения команды в этой строке.
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
	free_cmd(cmd);
	return (status);
}

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
