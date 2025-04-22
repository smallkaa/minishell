/**
 * @file free_cmd.c
 * @brief Functions for freeing memory allocated for command structures.
 */
#include "minishell.h"

/**
 * @brief Frees a list of redirection structures.
 *
 * Iterates through a linked list of redirection nodes (`t_list`),
 * freeing each redirection’s filename, structure, and the list node itself.
 *
 * @param redirs Pointer to the head of the redirection list.
 */
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

/**
 * @brief Frees a linked list of command structures.
 *
 * Frees memory for:
 * - Arguments (`argv`)
 * - Binary path
 * - Redirections (`redirs`)
 * - The `t_cmd` structure itself
 *
 * Iterates through the list, properly freeing nested structures.
 *
 * @param cmd Pointer to the head of the command list.
 */
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
