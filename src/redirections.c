#include "minishell.h"

/**
 * Handles output redirection for a command.
 *
 * - Opens the specified output file with the correct mode
 * `O_TRUNC` or `O_APPEND`.
 * - Redirects `STDOUT_FILENO` to the opened file.
 * - Ensures the file is closed after redirection is set.
 *
 * @param cmd The command containing output redirection details.
 */
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
	if (dup2(out, STDOUT_FILENO) == -1)
	{
		if (close(out) == -1)
			print_error_exit("close", EXIT_FAILURE);
		print_error_exit("dup2", EXIT_FAILURE);
	}
	if (close(out) == -1)
		print_error_exit("close", EXIT_FAILURE);
}

/**
 * Handles input redirection for a command.
 *
 * - Opens the specified input file in read-only mode.
 * - Redirects `STDIN_FILENO` to the opened file.
 * - Ensures the file is closed after redirection is set.
 *
 * @param cmd The command containing input redirection details.
 */
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
