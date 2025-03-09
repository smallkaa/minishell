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
	cmd->minishell->exit_status = status;
}
bool	is_valid_varname(const char *key_value_pair)
{
	int	i;

	if (!key_value_pair)
		return (false);
	if (!(ft_isalpha(key_value_pair[0]) || key_value_pair[0] == '_'))
		return (false);
	i = 1;
	while (key_value_pair[i] && key_value_pair[i] != '=')
	{
		if (!(ft_isalnum(key_value_pair[i]) || key_value_pair[i] == '_'))
			return (false);
		i++;
	}
	return (true);
}
