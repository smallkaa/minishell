/**
 * @file executor.c
 * @brief Functions for executing built-in and external commands in Minishell.
 */
 #include "minishell.h"

bool	is_builtin(t_cmd *cmd)
{
	const t_builtin_dispatch	*table;
	size_t						size;
	size_t						i;

	table = get_builtin_table(&size);
	i = 0;
	while (i < size)
	{
		if (ft_strcmp(cmd->argv[0], (char *)table[i].name) == 0)
			return (true);
		i++;
	}
	return (false);
}

static bool	is_exit_command(t_cmd *cmd)
{
	int	i;

	i = 0;
	while (cmd->argv[i])
		i++;
	return (ft_strcmp(cmd->argv[i - 1], "exit") == 0);
}

static void	cleanup_and_exit(t_cmd *cmd, int exit_status)
{
	free_cmd(cmd);
	free_minishell(cmd->minishell);
	rl_clear_history();
	exit(exit_status);
}

static uint8_t	execute_pipeline_or_binary(t_cmd *cmd)
{
	t_mshell	*minishell;
	uint8_t		exit_status;

	minishell = cmd->minishell;
	exit_status = exec_in_child_process(cmd);
	minishell->exit_status = exit_status;
	if (is_exit_command(cmd))
		cleanup_and_exit(cmd, exit_status);
	return (exit_status);
}

static uint8_t	execute_builtin(t_cmd *cmd)
{
	t_mshell	*minishell;
	uint8_t		exit_status;

	minishell = cmd->minishell;
	exit_status = exec_in_current_process(cmd);
	minishell->exit_status = exit_status;
	if (ft_strcmp(cmd->argv[0], "exit") == 0)
		cleanup_and_exit(cmd, exit_status);
	return (exit_status);
}

uint8_t	run_executor(t_cmd *cmd)
{
	t_mshell	*minishell;
	int i;

	minishell = cmd->minishell;
	if (!minishell || !minishell->env || !minishell->hash_table)
	{
		print_error("Error (run_executor): missing components\n");
		return (EXIT_FAILURE);
	}
		// test -------------------------------------------------//

	// i = 0;
	// while(cmd->argv[i])
	// {
	// 	printf("---argv[%d]: {%s}\n", i, cmd->argv[i]);
	// 	i++;
	// }
	// printf("---argv[%d]: {%s}\n", i, cmd->argv[i]);
	// if (cmd->binary)
	// 	printf("---binary (%p)\n", cmd->binary);
	// else
	// 	printf("---cmd->binary: NOT found\n");

	//-----------------------------------------------
	// Printing redirections
	// i = 0;
	// if (cmd->redirs)
	// {
	// 	t_list *current_redir = cmd->redirs;
	// 	while (current_redir)
	// 	{
	// 		t_redir *redir = (t_redir *)current_redir->content;

	// 		printf("---redirection[%d]: ", i);
	// 		if (redir->type == R_INPUT)
	// 			printf("'<' ");
	// 		else if (redir->type == R_OUTPUT)
	// 			printf("'>' ");
	// 		else if (redir->type == R_APPEND)
	// 			printf("'>>' ");
	// 		else if (redir->type == R_HEREDOC)
	// 			printf("'<<' ");

	// 		if (redir->filename)
	// 			printf("\"%s\"\n", redir->filename);
	// 		else
	// 			printf("(NULL)\n");

	// 		printf("---expand: %s\n", redir->expand ? "true" : "false");

	// 		current_redir = current_redir->next;
	// 		i++;
	// 	}
	// }
	// else
	// {
	// 	printf("---No redirections found\n");
	// }


	// end test -----------------------------------------------//

	if (cmd && cmd->argv != NULL)
	{
		i = 0;
		while (cmd->argv[i] != NULL)
		i++;

		if (i > 1)
		{
			set_variable(cmd->minishell, "_", cmd->argv[i - 1], 1);
			update_env(cmd->minishell);
		}
		else
		{
			set_variable(cmd->minishell, "_", cmd->argv[0], 1);
			update_env(cmd->minishell);
		}
	}
	if (!is_builtin(cmd) || cmd->next)
		return (execute_pipeline_or_binary(cmd));
	else
		return (execute_builtin(cmd));
}

// 	if (!is_builtin(cmd) || cmd->next)
// 	{
// 		minishell->exit_status = exec_in_child_process(cmd);

// 		// need to clean?
// 		int i = 0;
// 		while(cmd->argv[i])
// 			i++;
// 		if (ft_strcmp(cmd->argv[i - 1], "exit") == 0)
// 		{
// 			exit_status = minishell->exit_status;
// 			free_cmd(cmd);
// 			free_minishell(minishell);
// 			rl_clear_history();
// 			exit(exit_status);
// 		}
// 		return (minishell->exit_status);
// 	}
// 	else
// 	{
// 		minishell->exit_status = exec_in_current_process(cmd);
// 		if (ft_strcmp(cmd->argv[0], "exit") == 0)
// 		{
// 			exit_status = minishell->exit_status;
// 			free_cmd(cmd);
// 			free_minishell(minishell);
// 			rl_clear_history();
// 			exit(exit_status);
// 		}
// 		return (minishell->exit_status);
// 	}
// }
