#include "minishell.h"

int	run_minishell(t_minishell *minishell)
{
	char	*input;
	int		exit_status;

	// uncoment for parser
	t_cmd	*cmd;

	while (1)
	{
		// Step 1: read input from terminal, return a line for parser
		input = readline("minishell: ");

		// check for EOF / Ctrl+D
		if (!input)
			return (EXIT_SUCCESS);

		// Step 2: add input to history
		if (*input)
			add_history(input);  // need to be freed?


		debug_printf("Return: %s\n", input); // test print statment

		// Step 3: process input
		// use input from readline and return commands table for executor

		cmd = run_parser(minishell, input);
		if (!cmd)
			continue;

		// Step 4: use command table and execute commands one by one, void func
		exit_status = run_executor(cmd);

		//	free_cmds(cmd);
		free(input);
	}
	return (exit_status);
}

// Compile in src/ dir with
// cc -I../include main.c -lreadline
int	main(int argc, char **argv, char **envp)
{
	//remove(void) for parser
	(void)argc;
	(void)argv;
	t_minishell	*minishell;
	int status;
	
	setup_signal_handlers(); // Set up signal handlers

	minishell = init_minishell(envp);
	if (!minishell)
	{
		print_error("Error: init_minishell failed\n");
		return (EXIT_FAILURE);
	}
	status = run_minishell(minishell);
	printf("\n[DEBUG]: main() exit status (%d)\n", status);
	return (status);
}
