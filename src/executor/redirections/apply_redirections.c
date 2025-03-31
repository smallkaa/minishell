#include "minishell.h"


uint8_t	process_extra_output(t_list	*list)
{
	int		fd;
	char	*filename;

	filename = (char *)list->content;  // Assuming the content is the filename

	// Check if the file can be created or written to
	fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (fd == -1)
	{
		perror(filename);  // Print the error message (e.g., "Permission denied")
		return (EXIT_FAILURE);
	}
	close(fd); // Close the file after creating/truncating
	return (EXIT_SUCCESS); // Return success status
}


uint8_t	process_extra_input(t_list	*list)
{
	char	*filename;

	filename = (char *)list->content;  // Assuming the content is the filename

	// Check if the file exists and can be read
	if (access(filename, F_OK) != 0)
	{
		perror(filename);  // File does not exist
		return (EXIT_FAILURE);
	}
	if (access(filename, R_OK) != 0)
	{
		perror(filename);  // Permission denied
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS); // Return success status
}

uint8_t	traverse_redirections(t_cmd *cmd)
{
	t_list	*current;
	uint8_t	exit_status;

	// Process output redirections
	if (cmd->extra_out_redirs)
	{
		current = cmd->extra_out_redirs;
		while (current)
		{
			exit_status = process_extra_output(current);
			if (exit_status != EXIT_SUCCESS)
				return (exit_status); // Return immediately on failure
			current = current->next;
		}
	}

	// Process input redirections
	if (cmd->extra_in_redirs)
	{
		current = cmd->extra_in_redirs;
		while (current)
		{
			exit_status = process_extra_input(current);
			if (exit_status != EXIT_SUCCESS)
				return (exit_status); // Return immediately on failure
			current = current->next;
		}
	}

	return (EXIT_SUCCESS); // Return success if all redirections succeeded
}


uint8_t	apply_redirections(t_cmd *cmd)
{
	uint8_t	exit_status;

	if (!cmd)
	{
		print_error("minishell: cmd struct not found\n");
		return (EXIT_FAILURE);
	}
	exit_status = traverse_redirections(cmd);
	if (exit_status != EXIT_SUCCESS)
		return (exit_status);

	if (cmd->in_redir && cmd->in_redir->type == R_HEREDOC)
		return (apply_heredoc(cmd));

	else if (cmd->in_redir && cmd->in_redir->type == R_INPUT)
		return (apply_in_redirection(cmd));

	else if (cmd->out_redir && cmd->out_redir->type == R_APPEND)
		return (apply_out_redirection(cmd));

	else if (cmd->out_redir && cmd->out_redir->type == R_OUTPUT)
		return (apply_out_redirection(cmd));

	else
		return (EXIT_SUCCESS);
}
