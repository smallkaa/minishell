#include "minishell.h"

// uint8_t	apply_single_redirection(t_cmd *cmd, t_redir *redir)
// {
// 	uint8_t	exit_status;

// 	if (redir->type == R_INPUT)
// 		exit_status = apply_in_redirection(cmd);
// 	else if (redir->type == R_OUTPUT || redir->type == R_APPEND)
// 		exit_status = apply_out_redirection(cmd);
// 	// else if (redir->type == R_HEREDOC)
// 	// 	exit_status = apply_heredoc(cmd);
// 	else
// 	{
// 		print_error("Error: apply_redirections: invalid redirection type\n");
// 		return (EXIT_FAILURE);
// 	}
// 	return (exit_status);
// }

// uint8_t	apply_redirections(t_cmd *cmd)
// {
// 	t_redir	*redir;
// 	t_list	*current_redir;
// 	uint8_t	exit_status;

// 	if (!cmd)
// 	{
// 		print_error("minishell: cmd struct not found\n");
// 		return (EXIT_FAILURE);
// 	}
// 	if (!cmd->redirs)
// 		return (EXIT_SUCCESS);
// 	current_redir = cmd->redirs;
// 	while (current_redir)
// 	{
// 		redir = (t_redir *)current_redir->content;
// 		exit_status = apply_single_redirection(cmd, redir);
// 		if (exit_status != EXIT_SUCCESS)
// 			return (exit_status);
// 		current_redir = current_redir->next;
// 	}
// 	return (EXIT_SUCCESS);
// }
uint8_t apply_redirections(t_cmd *cmd)
{
	t_list *n = cmd->redirs;
	uint8_t status = EXIT_SUCCESS;

	while (n)
	{
		t_redir *r = n->content;

		if (r->type == R_HEREDOC)
		{
			if (dup2(r->fd, STDIN_FILENO) == -1)
				return perror_return("dup2 heredoc", EXIT_FAILURE);
			close(r->fd);
		}
		else if (r->type == R_INPUT)
		{
			int fd = open(r->filename, O_RDONLY);
			if (fd == -1)
				return perror_return(r->filename, EXIT_FAILURE);
			dup2(fd, STDIN_FILENO);
			close(fd);
		}
		else if (r->type == R_OUTPUT || r->type == R_APPEND)
		{
			int flags = O_WRONLY | O_CREAT | (r->type == R_APPEND ? O_APPEND : O_TRUNC);
			int fd = open(r->filename, flags, 0644);
			if (fd == -1)
				return perror_return(r->filename, EXIT_FAILURE);
			dup2(fd, STDOUT_FILENO);
			close(fd);
		}

		n = n->next;
	}
	return status;
}
