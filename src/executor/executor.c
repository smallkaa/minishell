/**
 * @file executor.c
 * @brief Functions for executing built-in and external commands in Minishell.
 */
#include "minishell.h"

static uint8_t	execute_pipeline_or_binary(t_cmd *cmd)
{
	t_mshell	*minishell;
	uint8_t		exit_status;

	minishell = cmd->minishell;
	exit_status = exec_in_pipes(cmd);
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
	uint8_t		exit_status;

	if (!cmd)
		return (error_return("run_executor: no cmd found\n", EXIT_FAILURE));
	minishell = cmd->minishell;
	if (!minishell || !minishell->env || !minishell->hash_table)
		return (error_return("run_executor: no mshell found\n", EXIT_FAILURE));

	// test -------------------------------------------------//

	// int j = 0;
	// while(cmd->argv[j])
	// {
	// 	printf("---argv[%d]: {%s}\n", j, cmd->argv[j]);
	// 	j++;
	// }
	// printf("---argv[%d]: {%s}\n", j, cmd->argv[j]);
	// printf("---binary (%p)\n", cmd->binary);

	// // //-----------------------------------------------
	// // Printing redirections

	// j = 0;
	// if (cmd->redirs)
	// {
	// 	t_list *current_redir = cmd->redirs;
	// 	while (current_redir)
	// 	{
	// 		t_redir *redir = (t_redir *)current_redir->content;

	// 		printf("---redirection[%d]: ", j);
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
	// 		j++;
	// 	}
	// }
	// else
	// {
	// 	printf("---No redirections found\n");
	// }

	// end test -----------------------------------------------//

	// int i;

	if (command_too_long(cmd->argv))
		return (error_return("run_executor: command too long\n", 2));
	exit_status = apply_heredocs(cmd);
	if (exit_status != EXIT_SUCCESS)
		return (exit_status);
	if (!is_builtin(cmd) || cmd->next)
		exit_status = execute_pipeline_or_binary(cmd);
	else
		exit_status = execute_builtin(cmd);
	if (cmd->next == NULL) 
        update_underscore(cmd, cmd->binary);
	return (exit_status);
}
