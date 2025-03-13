/**
 * @file executor.c
 * @brief Functions for executing built-in and external commands in Minishell.
 */
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

/**
 * @brief Executes a command, either built-in or external.
 *
 * - If the command is built-in and there is no following command, executes it directly.
 * - Otherwise, marks `cmd->in_pipe` as `true` and executes the command externally.
 *
 * @param cmd Pointer to the command structure.
 * @return The exit status of the command.
 */
uint8_t run_executor(t_cmd *cmd)
{
	t_mshell	*minishell;

	// if (!cmd)
	// 	return (EXIT_FAILURE);

	minishell = cmd->minishell;
	if (!minishell || !minishell->env || !minishell->hash_table)
	{
		print_error("Error (run_executor): missing components\n");
		minishell->exit_status = EXIT_FAILURE;
		return (minishell->exit_status);
	}

	// test
	int i = 0;
	while(cmd->argv[i])
	{
		printf("Debug: argv[%d]: {%s}\n", i, cmd->argv[i]);
		i++;
	}
	printf("Debug: argv[%d]: {%s}\n", i, cmd->argv[i]);
	// end test

	if (is_builtin(cmd) && !cmd->next)
	{
		cmd->in_pipe = false;
		minishell->exit_status = exec_builtin(cmd);
		free_cmd(cmd);
		return (minishell->exit_status);
	}
	cmd->in_pipe = true;
	minishell->exit_status = exec_cmd(cmd);
	free_cmd(cmd);
	return (minishell->exit_status);
}

