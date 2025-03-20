#include "minishell.h"

static void	wait_input(char *end_of_msg)
{
	char	*line;

	while (1)
	{
		line = readline("> ");
		if (!line)
			break ;
		if (ft_strcmp(line, end_of_msg) == 0)
		{
			free(line);
			break;
		}
		free(line);
	}
}

uint8_t	process_heredoc_builtin(t_cmd *cmd)
{
	if (!cmd->in_redir || !cmd->in_redir->filename || cmd->in_redir->type != R_HEREDOC)
	{
		print_error("Error: process_heredoc_builtin failed\n");
		return (EXIT_FAILURE);
	}
	wait_input(cmd->in_redir->filename);
	cmd->minishell->exit_status = exec_builtin(cmd);
	return (cmd->minishell->exit_status);
}

uint8_t	handle_heredoc(t_cmd *cmd)
{
	if (is_builtin(cmd) && !cmd->next)
		return (process_heredoc_builtin(cmd));
	return (EXIT_SUCCESS);
}
