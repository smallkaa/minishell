#include "minishell.h"

bool has_heredoc(t_cmd *cmd)
{
	t_list *node = cmd->redirs;

	while (node)
	{
		t_redir *redir = (t_redir *)node->content;
		if (redir->type == R_HEREDOC)
			return true;
		node = node->next;
	}
	return false;
}

bool has_input_redirection(t_cmd *cmd)
{
	t_list *node = cmd->redirs;

	while (node)
	{
		t_redir *redir = (t_redir *)node->content;
		if (redir->type == R_INPUT || redir->type == R_HEREDOC)
			return true;
		node = node->next;
	}
	return false;
}
void	close_unused_heredocs_child(t_cmd *current, t_cmd *full_cmd_list)
{
	t_cmd	*cmd = full_cmd_list;
	while (cmd)
	{
		/* If this is not the current command, close all its heredoc FDs */
		if (cmd != current)
		{
			t_list	*node = cmd->redirs;
			while (node)
			{
				t_redir *redir = (t_redir *)node->content;
				if (redir->type == R_HEREDOC && redir->fd >= 0)
				{
					// Close the FD in the child (do not modify redir->fd here)
					if (close(redir->fd) == -1)
						perror("close_unused_heredocs_child: close");
					// Note: We do not set redir->fd = -1 in the child;
					// each child has its own copy of the FDs.
				}
				node = node->next;
			}
		}
		cmd = cmd->next;
	}
}

void	child_process(t_cmd *cmd, int in_fd, int *pipe_fd, t_cmd *full_cmd_list)
{
	// print_pid("I am a child");

	// In the child, close all heredoc FDs that are not used by the current command.
	close_unused_heredocs_child(cmd, full_cmd_list);

	// Setup STDOUT for piping if needed.
	if (cmd->next && !has_input_redirection(cmd->next))
	{
		if (dup2(pipe_fd[1], STDOUT_FILENO) == -1)
		{
			perror("-exec_in_pipes: dup2 pipe_fd[1]");
			_exit(EXIT_FAILURE);
		}
	}

	// Setup STDIN if needed.
	if (in_fd != STDIN_FILENO)
	{
		if (dup2(in_fd, STDIN_FILENO) == -1)
		{
			perror("-exec_in_pipes: dup2 in_fd");
			_exit(EXIT_FAILURE);
		}
	}

	// Apply redirections for the current command.
	if (apply_redirections(cmd) != EXIT_SUCCESS)
	{
		if (close_unused_fds(in_fd, pipe_fd) != EXIT_SUCCESS)
			_exit(EXIT_FAILURE);
		_exit(EXIT_FAILURE);
	}

	// Close FDs that are no longer needed (this function takes care of the pipe ends).
	if (close_unused_fds(in_fd, pipe_fd) != EXIT_SUCCESS)
		_exit(EXIT_FAILURE);

	// Execute the command.
	execute_command(cmd);
}



void handle_child_and_track(t_cmd *cmd, t_pipe_info *info)
{
	// printf("DEBUG: in handle_child_and_track()\n");

	pid_t pid = fork();
	if (pid == -1)
	{
		perror("-exec_in_pipes: fork");
		exit(EXIT_FAILURE);
	}

	info->pids[*info->idx] = pid;

	if (pid == 0)
	{
		child_process(cmd, info->in_fd, info->pipe_fd, info->cmd_list);
	}

	(*info->idx)++;
}
