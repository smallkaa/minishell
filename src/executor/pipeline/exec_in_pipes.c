/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_in_pipes.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Pavel Vershinin <pvershin@student.hive.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 14:47:01 by Ilia Munaev       #+#    #+#             */
/*   Updated: 2025/05/08 14:33:20 by Pavel Versh      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file exec_in_pipes.c
 * @brief Executes a sequence of piped commands in Minishell.
 *
 * This file contains the logic for setting up and executing commands connected
 * by pipes. Each command is forked, connected via file descriptors, and run
 * with proper error handling and file descriptor cleanup.
 */
#include "minishell.h"

/**
 * @brief Creates a new pipe if the current command has a successor.
 *
 * If the current command has a `next` command (i.e., it is part of a pipeline),
 * this function creates a new pipe and updates `pipe_fd`.
 *
 * @param cmd Pointer to the current command.
 * @param pipe_fd Array to hold read and write pipe descriptors.
 */
void	handle_pipe_creation(t_cmd *cmd, int *pipe_fd)
{
	if (cmd->next)
	{
		if (pipe(pipe_fd) == -1)
		{
			perror("-exec_in_pipes: pipe");
			exit(EXIT_FAILURE);
		}
	}
}

/**
 * @brief Closes unnecessary file descriptors and prepares for the next pipe.
 *
 * - Closes the write-end of the current pipe.
 * - Closes the previous input file descriptor if it's not `STDIN_FILENO`.
 * - Updates `*in_fd` to be the read-end of the current pipe for the next
 * command.
 *
 * @param in_fd Pointer to the input file descriptor to update.
 * @param pipe_fd Array holding the current pipe's file descriptors.
 */
void	close_fds_and_prepare_next(t_cmd *cmd, int *in_fd, int *pipe_fd)
{
	if (pipe_fd[1] >= 0 && close(pipe_fd[1]) == -1)
	{
		perror("-exec_in_pipes: close pipe_fd[1]");
		free_minishell(&cmd->minishell);
		_exit(EXIT_FAILURE);
	}
	if (*in_fd != STDIN_FILENO && *in_fd > STDIN_FILENO && close(*in_fd) == -1)
	{
		perror("-exec_in_pipes: close in_fd");
		free_minishell(&cmd->minishell);
		_exit(EXIT_FAILURE);
	}
	*in_fd = pipe_fd[0];
	pipe_fd[0] = -1;
	pipe_fd[1] = -1;
}

/**
 * @brief Initializes the `t_pipe_info` structure used to manage pipeline
 * execution.
 *
 * Resets and assigns all internal fields including file descriptors and
 * process ID
 * storage. This structure is passed throughout the pipeline to track context.
 *
 * @param info Pointer to the `t_pipe_info` structure to initialize.
 * @param cmd_list The head of the command list forming the pipeline.
 * @param pipe_fd Array to hold the pipe descriptors.
 * @param pids Array to store child process IDs.
 */
void	init_pipe_info(t_pipe_info *info, t_cmd *cmd_list,
						int pipe_fd[2], pid_t pids[MAX_CMDS])
{
	static int	idx;

	info->in_fd = STDIN_FILENO;
	info->pipe_fd = pipe_fd;
	info->pids = pids;
	idx = 0;
	info->idx = &idx;
	info->cmd_list = cmd_list;
	pipe_fd[0] = -1;
	pipe_fd[1] = -1;
}
