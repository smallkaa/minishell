#include "minishell.h"

static void	handle_out_redirection(t_cmd *cmd)
{
	int	out;
	int	mode;

	if (!cmd->out_redir)
		return ;
	mode = O_WRONLY | O_CREAT;
	if (cmd->out_redir->type == R_OUTPUT)
		mode |= O_TRUNC;
	else if (cmd->out_redir->type == R_APPEND)
		mode |= O_APPEND;
	else
		fatal_error("redirection", EXIT_FAILURE);
	out = open(cmd->out_redir->filename, mode, 0644);
	if (out < 0)
		fatal_error("write", EXIT_FAILURE);
	if (dup2(out, STDOUT_FILENO) == -1)
	{
		if (close(out) == -1)
			fatal_error("close", EXIT_FAILURE);
		fatal_error("dup2", EXIT_FAILURE);
	}
	if (close(out) == -1)
		fatal_error("close", EXIT_FAILURE);
}

static void	handle_in_redirection(t_cmd *cmd)
{
	int	in;

	if (!cmd->in_redir)
		return ;
	in = open(cmd->in_redir->filename, O_RDONLY);
	if (in < 0)
		fatal_error("read", EXIT_FAILURE);
	if (dup2(in, STDIN_FILENO) == -1)
	{
		if (close(in) == -1)
			fatal_error("close", EXIT_FAILURE);
		fatal_error("dup2", EXIT_FAILURE);
	}
	if (close(in) == -1)
		fatal_error("close", EXIT_FAILURE);
}

void	handle_redirections(t_cmd *cmd, int in_fd)
{
	int	status;

	if (cmd->in_redir)
		handle_in_redirection(cmd);
	else
	{
		status = dup2(in_fd, STDIN_FILENO);
		if (status == -1)
		{
			if (close(in_fd) == -1)
				fatal_error("close", EXIT_FAILURE);
			fatal_error("dup2", EXIT_FAILURE);
		}
	}
	if (cmd->out_redir)
		handle_out_redirection(cmd);
}
