#include "../include/minishell.h"

int	minishell(char **envp)
{
	char	*input;
	int		exit_status;

	// uncoment for parser
	t_cmd	*cmd;
	t_minishell	*minishell;

	//remove(void) for parser
	(void)envp;

	while (1)
	{
		// Step 1: read input from terminal, return a line for parser
		input = readline("minishell:> "); // add some fearures like "username@hostname:> "

		// check for EOF / Ctrl+D
		if (!input)
		{
			printf("\nExit minishell.\n"); // set error func later
			return (EXIT_SUCCESS);
		}

		// Step 2: add input to history
		if (*input)
			add_history(input);  // need to be freed?

		debug_printf("Return: %s\n", input); // test print statment

		// Step 3: init shell structure
		minishell = init_minishell();
		if (!minishell)
			return (EXIT_FAILURE);

		// Step 3: process input
		// use input from readline and return commands table for executor

		cmd = run_parser(minishell, input);
		if (!cmd)
			return (EXIT_FAILURE);
		// Step 4: use command table and execute commands one by one, void func
		exit_status = run_executor(cmd, envp);

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

	int status;

	status = minishell(envp);
	printf("\n[DEBUG]: main() exit status (%d)\n", status);

	return (status);
}
