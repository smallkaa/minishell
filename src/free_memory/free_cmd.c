#include "minishell.h"

static void	free_redirs(t_list *redirs)
{
	t_list	*tmp;
	t_redir	*redir;

	while (redirs)
	{
		tmp = redirs->next;
		redir = redirs->content;
		if (redir)
			free(redir->filename);
		free(redir);
		free(redirs);
		redirs = tmp;
	}
}

void	free_cmd(t_cmd *cmd)
{
	t_cmd	*next;
	int		i;

	if (!cmd)
		return ;
	while (cmd)
	{
		i = 0;
		if (cmd->argv)
		{
			while (cmd->argv[i])
				free(cmd->argv[i++]);
			free(cmd->argv);
		}
		if (cmd->binary)
			free(cmd->binary);
		if (cmd->redirs)
			free_redirs(cmd->redirs);
		next = cmd->next;
		free(cmd);
		cmd = next;
	}
}
