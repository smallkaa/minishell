#include "minishell.h"

bool	is_builtin(t_cmd *cmd)
{
	char	**builtin;
	int		i;

	builtin = cmd->minishell->builtin;
	i = 0;
	while (builtin[i] && ft_strcmp(cmd->argv[0], builtin[i]) != 0)
		i++;
	return (builtin[i] != NULL);
}

uint8_t	run_executor(t_cmd *cmd)
{
	t_mshell	*minishell;
	char		**env;
	t_hash_table	*hash_table;

	minishell = cmd->minishell;
	if (!minishell)
	{
		print_error("minishell: run_executor, no minishell found\n");
		return (EXIT_FAILURE);
	}
	env = cmd->minishell->env;
	hash_table = cmd->minishell->hash_table;
	if (!env || !hash_table)
	{
		print_error("minishell: run_executor no env/hash_table found\n");
		return (EXIT_FAILURE);
	}
	cmd->in_pipe = false;
	if (is_builtin(cmd) && !cmd->next)
		return (exec_builtin(cmd));
	exec_cmd(cmd);
	return (cmd->minishell->exit_status);
}
