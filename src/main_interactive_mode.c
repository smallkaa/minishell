/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_interactive_mode.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ilia Munaev <ilyamunaev@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/08 10:34:54 by Pavel Versh       #+#    #+#             */
/*   Updated: 2025/05/14 17:05:06 by Ilia Munaev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static uint8_t	handle_signal_interrupt(t_mshell *mshell, char **input)
{
	if (g_signal_flag)
	{
		mshell->exit_status = 130;
		g_signal_flag = 0;
		if (input && *input)
			free_ptr((void **)input);
		return (1);
	}
	return (0);
}

static uint8_t	handle_eof_input(char *input)
{
	if (!input)
	{
		printf("exit\n");
		return (1);
	}
	return (0);
}

static uint8_t	process_input(t_mshell *mshell, char *input)
{
	t_cmd	*cmd;
	uint8_t	exit_status;

	if (ft_strlen(input) == 0)
		return (mshell->exit_status);
	cmd = run_parser(mshell, input);
	if (!cmd && handle_null_command(mshell))
		return (mshell->exit_status);
	if (handle_signal_after_parse(mshell, &cmd, &input))
		return (mshell->exit_status);
	exit_status = run_executor(cmd);
	mshell->exit_status = exit_status;
	free_cmd(&cmd);
	return (exit_status);
}

static void	handle_newline_if_signal(t_mshell *mshell)
{
	if (mshell->exit_status == (128 + SIGINT)
		|| mshell->exit_status == (128 + SIGQUIT))
		rl_on_new_line();
}

uint8_t	run_interactive_mode(t_mshell *mshell)
{
	char	*input;
	uint8_t	exit_status;

	exit_status = mshell->exit_status;
	rl_event_hook = readline_interrupt_hook; // not allowed
	while (!mshell->allocation_error)
	{
		if (handle_signal_interrupt(mshell, NULL))
			continue ;
		input = read_user_input();
		if (handle_signal_interrupt(mshell, &input))
			continue ;
		if (handle_eof_input(input))
			break ;
		exit_status = process_input(mshell, input);
		if (input)
			free_ptr((void **)&input);
		handle_signal_after_exec(mshell);
		handle_newline_if_signal(mshell);
	}
	if (mshell->allocation_error)
		print_error("Allocation error!\n");
	rl_event_hook = NULL;
	return (exit_status);
}
