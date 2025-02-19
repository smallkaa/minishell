#include "minishell.h"

void	handle_out_redirection(t_cmd *cmd)
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
		print_error_exit("redirection", EXIT_FAILURE);
	out = open(cmd->out_redir->filename, mode, 0644);
	if (out < 0)
		print_error_exit("write", EXIT_FAILURE);
	dup2(out, STDOUT_FILENO);
	if (close(out) == -1)
		print_error_exit("close", EXIT_FAILURE);
}

void	handle_in_redirection(t_cmd *cmd)
{
	int	in;

	if (!cmd->in_redir)
		return ;
	in = open(cmd->in_redir->filename, O_RDONLY);
	if (in < 0)
		print_error_exit("read", EXIT_FAILURE);
	if (dup2(in, STDIN_FILENO) == -1)
	{
		if (close(in) == -1)
			print_error_exit("close", EXIT_FAILURE);
		print_error_exit("dup2", EXIT_FAILURE);
	}
	if (close(in) == -1)
		print_error_exit("close", EXIT_FAILURE);
}
