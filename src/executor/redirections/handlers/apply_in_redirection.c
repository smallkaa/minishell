#include "minishell.h"

int	open_input_file(char *filename)
{
	int	in_fd;

	in_fd = open(filename, O_RDONLY);
	if (in_fd < 0)
	{
		print_error("-minishell: ");
		perror(filename);
	}
	return (in_fd);
}

uint8_t	apply_input_fd(int in_fd)
{
	if (dup2(in_fd, STDIN_FILENO) == -1)
	{
		if (close(in_fd) == -1)
			perror_return("-minishell: close", EXIT_FAILURE);
		perror_return("-minishell: dup2", EXIT_FAILURE);
	}
	if (close(in_fd) == -1)
		perror_return("-minishell: close", EXIT_FAILURE);
	return (EXIT_SUCCESS);
}

uint8_t	apply_in_redirection(t_cmd *cmd)
{
	t_list	*current_redir;
	t_redir	*redir;
	int		in_fd;
	uint8_t	exit_status;

	exit_status = EXIT_SUCCESS;
	if (!cmd->redirs)
		return (EXIT_SUCCESS);
	current_redir = cmd->redirs;
	while (current_redir)
	{
		redir = (t_redir *)current_redir->content;
		if (redir->type == R_INPUT)
		{
			in_fd = open_input_file(redir->filename);
			if (in_fd < 0)
				return (EXIT_FAILURE);
			exit_status = apply_input_fd(in_fd);
			if (exit_status != EXIT_SUCCESS)
				return (exit_status);
		}
		current_redir = current_redir->next;
	}
	return (exit_status);
}
