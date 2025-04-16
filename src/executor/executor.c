/**
 * @file executor.c
 * @brief Functions for executing built-in and external commands in Minishell.
 */
#include "minishell.h"

// void	print_pid(const char *label)
// {
// 	printf("DEBUG: %s - PID: %d, PPID: %d\n", label, getpid(), getppid());
// }
bool is_builtin(t_cmd *cmd)
{
	const t_builtin_disp *table;
	size_t size;
	size_t i;

	table = get_builtin_table(&size);
	i = 0;
	if (!cmd->argv[0])
		return (false);
	while (i < size)
	{
		if (ft_strcmp(cmd->argv[0], (char *)table[i].name) == 0)
			return (true);
		i++;
	}
	return (false);
}

static bool is_exit_command(t_cmd *cmd)
{
	int i;

	i = 0;
	if (!cmd->argv[0])
		return (false);
	while (cmd->argv[i])
		i++;
	return (ft_strcmp(cmd->argv[i - 1], "exit") == 0);
}

static void cleanup_and_exit(t_cmd *cmd, int exit_status)
{
	t_mshell *minishell;

	minishell = cmd->minishell;
	free_cmd(cmd);
	free_minishell(minishell);
	rl_clear_history();
	exit(exit_status);
}
static char *get_last_meaningful_arg(t_cmd *cmd)
{
	int i = 0;

	while (cmd->argv[i])
		i++;

	if (i == 0)
		return cmd->argv[0];

	if ((ft_strcmp(cmd->argv[0], "export") == 0) && ft_strchr(cmd->argv[i - 1], '='))
		return cmd->argv[0]; // treat 'export VAR=VAL' as just 'export'

	return cmd->argv[i - 1];
}

static uint8_t execute_pipeline_or_binary(t_cmd *cmd)
{
	t_mshell *minishell;
	uint8_t exit_status;

	minishell = cmd->minishell;
	exit_status = exec_in_pipes(cmd);
	minishell->exit_status = exit_status;
	if (is_exit_command(cmd))
		cleanup_and_exit(cmd, exit_status);
	return (exit_status);
}

static uint8_t execute_builtin(t_cmd *cmd)
{
	t_mshell *minishell;
	uint8_t exit_status;

	minishell = cmd->minishell;
	exit_status = exec_in_current_process(cmd);
	minishell->exit_status = exit_status;
	if (ft_strcmp(cmd->argv[0], "exit") == 0)
		cleanup_and_exit(cmd, exit_status);
	return (exit_status);
}
void update_underscore(t_cmd *cmd, char *binary_path)
{
	char *last_arg;

	if (!cmd || !cmd->argv || !cmd->argv[0])
		return;
	if (is_builtin(cmd))
	{
		last_arg = get_last_meaningful_arg(cmd);
		if (last_arg)
			set_variable(cmd->minishell, "_", last_arg, 1);
	}
	else
	{
		if (binary_path)
			set_variable(cmd->minishell, "_", binary_path, 1);
		else
			set_variable(cmd->minishell, "_", cmd->argv[0], 1);
	}
	update_env(cmd->minishell);
}

bool command_too_long(char **argv)
{
	size_t total_len;
	int i;

	total_len = 0;
	i = 0;
	while (argv[i])
	{
		total_len += ft_strlen(argv[i]) + 1;
		if (total_len >= CMD_MAX_SIZE)
			return (true);
		i++;
	}
	return (false);
}

uint8_t run_executor(t_cmd *cmd)
{
	t_mshell *minishell;
	uint8_t exit_status;

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

	update_underscore(cmd, cmd->binary);

	exit_status = apply_heredocs(cmd);
	if (exit_status != EXIT_SUCCESS)
		return (exit_status);

	if (!is_builtin(cmd) || cmd->next)
		exit_status = execute_pipeline_or_binary(cmd);
	else
		exit_status = execute_builtin(cmd);
	return (exit_status);
}
