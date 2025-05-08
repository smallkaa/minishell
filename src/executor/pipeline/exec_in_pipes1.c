/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_in_pipes1.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Pavel Vershinin <pvershin@student.hive.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/08 14:26:18 by Pavel Versh       #+#    #+#             */
/*   Updated: 2025/05/08 14:33:53 by Pavel Versh      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	setup_sigint_ignore(struct sigaction *sa_int_original)
{
	struct sigaction	sa_int_ignore;

	sa_int_ignore.sa_handler = SIG_IGN;
	sigemptyset(&sa_int_ignore.sa_mask);
	sa_int_ignore.sa_flags = 0;
	sigaction(SIGINT, &sa_int_ignore, sa_int_original);
}

static void	restore_sigint(const struct sigaction *sa_int_original)
{
	sigaction(SIGINT, sa_int_original, NULL);
}

static void	print_newline_if_sigint(uint8_t exit_status)
{
	if (exit_status == (128 + SIGINT) && isatty(STDOUT_FILENO))
		write(STDOUT_FILENO, "\n", 1);
}

/**
 * @brief Iterates through the list of commands and sets up their pipes and
 * execution.
 *
 * For each command:
 * - Creates a pipe if needed.
 * - Forks and executes the command using `handle_child_and_track()`.
 * - Cleans up file descriptors and prepares for the next command.
 *
 * @param info Pointer to the pipeline info context.
 */
static void	process_pipeline_commands(t_pipe_info *info)
{
	t_cmd	*cmd;

	cmd = info->cmd_list;
	while (cmd)
	{
		handle_pipe_creation(cmd, info->pipe_fd);
		handle_child_and_track(cmd, info);
		close_fds_and_prepare_next(cmd, &info->in_fd, info->pipe_fd);
		cmd = cmd->next;
	}
}

/**
 * @brief Entry point to execute a list of piped commands.
 *
 * Initializes the pipe context and processes each command in sequence.
 * Waits for all child processes to complete and returns the final exit status.
 *
 * @param cmd_list Head of the command list forming the pipeline.
 * @return Exit status of the last command in the pipeline.
 */
uint8_t	exec_in_pipes(t_cmd *cmd_list)
{
	t_pipe_info			info;
	pid_t				pids[MAX_CMDS];
	int					pipe_fd[2];
	uint8_t				exit_status;
	struct sigaction	sa_int_original;

	if (!cmd_list)
		return (EXIT_SUCCESS);
	setup_sigint_ignore(&sa_int_original);
	init_pipe_info(&info, cmd_list, pipe_fd, pids);
	process_pipeline_commands(&info);
	close_all_heredoc_fds(cmd_list);
	exit_status = wait_for_children(pids, *info.idx);
	restore_sigint(&sa_int_original);
	print_newline_if_sigint(exit_status);
	return (exit_status);
}
