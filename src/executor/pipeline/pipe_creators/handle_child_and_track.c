#include "minishell.h"
bool	has_heredoc(t_cmd *cmd)
{
	t_list	*node = cmd->redirs;

	while (node)
	{
		t_redir *redir = (t_redir *)node->content;
		if (redir->type == R_HEREDOC)
			return true;
		node = node->next;
	}
	return false;
}

bool	has_input_redirection(t_cmd *cmd)
{
	t_list	*node = cmd->redirs;

	while (node)
	{
		t_redir *redir = (t_redir *)node->content;
		if (redir->type == R_INPUT || redir->type == R_HEREDOC)
			return true;
		node = node->next;
	}
	return false;
}

void	child_process(t_cmd *cmd, int in_fd, int *pipe_fd)
{
	if (cmd->next && !has_input_redirection(cmd->next))
	{
		if (dup2(pipe_fd[1], STDOUT_FILENO) == -1)
		{
			perror("-exec_in_pipes: dup2 pipe_fd[1]");
			_exit(EXIT_FAILURE);
		}
	}

	if (in_fd != STDIN_FILENO)
	{
		if (dup2(in_fd, STDIN_FILENO) == -1)
		{
			perror("-exec_in_pipes: dup2 in_fd");
			_exit(EXIT_FAILURE);
		}
	}

	if (apply_redirections(cmd) != EXIT_SUCCESS)
		_exit(EXIT_FAILURE);

	if (close_unused_fds(in_fd, pipe_fd) != EXIT_SUCCESS)
		_exit(EXIT_FAILURE);

	execute_command(cmd);
}


void	handle_child_and_track(t_cmd *cmd, t_pipe_info *info)
{
	printf("DEBUG: in handle_child_and_track()\n");

	pid_t pid = fork();
	if (pid == -1)
	{
		perror("-exec_in_pipes: fork");
		exit(EXIT_FAILURE);
	}

	info->pids[*info->idx] = pid;
	printf("DEBUG: handle_child_and_track(): fork for '%s'\n", cmd->argv[0]);

	if (pid == 0)
	{
		// Child process
		child_process(cmd, info->in_fd, info->pipe_fd);
		// We never return from child_process — it should _exit()
	}

	// Parent process: close heredoc read FDs
	t_list *redir_list = cmd->redirs;
	while (redir_list)
	{
		t_redir *redir = (t_redir *)redir_list->content;
		if (redir->type == R_HEREDOC && redir->fd >= 0)
		{
			close(redir->fd);
			redir->fd = -1;
		}
		redir_list = redir_list->next;
	}

	(*info->idx)++;
}
