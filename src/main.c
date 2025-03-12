#include "minishell.h"

void	run_minishell(t_mshell	*mshell)
{
	char	*input;
	uint8_t	exit_status;
	t_cmd	*cmd;

	while (1)
	{
		// Step 1: read input from terminal, return a line for parser
		input = readline("minishell: ");

		// check for EOF / Ctrl+D
		if (!input)
			return ;

		// Step 2: add input to history
		if (*input)
			add_history(input);  // needs to be freed?


		debug_printf("Return: %s\n", input); // test print statment

		// Step 3: process input
		// use input from readline and return commands table for executor
		cmd = run_parser(mshell, input);
		if (!cmd)
			continue ;

		exit_status = run_executor(cmd);
		printf("run_minishell(): Exit status run_executor       : %d\n", exit_status);
		printf("run_minishell(): Exit status mshell->exit_status: %d\n", mshell->exit_status);

		free(input);
	}
}

int	main(int argc, char **argv, char **envp)
{

	(void)argc;
	(void)argv;
	t_mshell	*minishell;


	setup_signal_handlers(); // Set up signal handlers

	minishell = init_mshell(envp);
	if (!minishell)
	{
		return (EXIT_FAILURE);
	}
	run_minishell(minishell);

	free_minishell(minishell);

	return (EXIT_SUCCESS);
}
