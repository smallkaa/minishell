#include "minishell.h"

// void	cleanup_heredoc(t_cmd *cmd)
// {
// 	if (cmd->in_redir && cmd->in_redir->type == R_HEREDOC)
// 	{
// 		if (unlink(HEREDOC_TFILE) == -1)
// 			print_error_exit("unlink", EXIT_FAILURE);
// 	}
// }

void	update_last_exit_status(t_cmd *cmd, int status)
{
	cmd->minishell->exit_stat = status;
}
