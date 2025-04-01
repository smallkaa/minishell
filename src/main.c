#include "minishell.h"
uint8_t	run_command_mode(t_mshell *mshell, char *input)
{
	t_cmd	*cmd;
	uint8_t	exit_status;

	cmd = run_parser(mshell, input);
	if (!cmd)
		return (EXIT_FAILURE);

	exit_status = run_executor(cmd);
	free_cmd(cmd);
	cmd = NULL;
	return (exit_status);
}


uint8_t	run_script_mode(t_mshell *mshell, const char *file)
{
	char	*input;
	uint8_t	exit_status;
	t_cmd	*cmd;
	int		in_fd;

	in_fd = open(file, O_RDONLY);
	if (in_fd < 0)
	{
		print_error("minishell: cannot open script file\n");
		return (EXIT_FAILURE);
	}
	input = NULL;
	while ((input = get_next_line(in_fd)) != NULL)
	{
		cmd = run_parser(mshell, input);
		if (!cmd)
		{
			free(input);
			input = NULL;
			continue ;
		}
		exit_status = run_executor(cmd);
		free_cmd(cmd);
		cmd = NULL;

		free(input);
		input = NULL;
	}
	close(in_fd);
	return (exit_status);
}

uint8_t	run_interactive_mode(t_mshell *mshell)
{
	char	*input;
	uint8_t	exit_status;
	t_cmd	*cmd;


	while (1)
	{

		input = readline("minishell: ");

		// check for EOF / Ctrl+D
		if (!input)
			return (EXIT_FAILURE);

		// printf("Input: [%s]\n", input); //test

		// Step 2: add input to history
		if (*input)
			add_history(input);

		debug_printf("Return: %s\n", input); // test print statment

		// Step 3: process input

		cmd = run_parser(mshell, input);
		if (!cmd)
		{
			free(input);
			input = NULL;
			continue ;
		}

		exit_status = run_executor(cmd);
		free_cmd(cmd);
		cmd = NULL;
		free(input);
		input = NULL;
	}
	return (exit_status);
}

int	main(int argc, char **argv, char **envp)
{
	t_mshell	*minishell;
	uint8_t		exit_status;

	setup_signal_handlers(); // Set up signal handlers
	minishell = init_mshell(envp);
	if (!minishell)
		return (EXIT_FAILURE);

	// Handle -c option
	if (argc >= 3 && !ft_strncmp(argv[1], "-c", 3))
		exit_status = run_command_mode(minishell, argv[2]);

	// Handle script mode
	else if (argc == 2)
		exit_status = run_script_mode(minishell, argv[1]);

	// Handle interactive mode

	//------------------- Step 1 -----------------------------------------------//
	//--------------------comment for big test ---------------------------------//

	else
		exit_status = run_interactive_mode(minishell);

	//--------------------comment for big test ---------------------------------//


	//------------------- Step 2 -----------------------------------------------//
	//--------------------uncomment for big test -------------------------------//

	// else if (isatty(fileno(stdin)))
	// 	exit_status = run_interactive_mode(minishell);
	// else
	// {
	// 	// Read one line from stdin (used by testers)
	// 	char *line = get_next_line(fileno(stdin));
	// 	if (!line)
	// 		exit_status = EXIT_FAILURE;
	// 	else
	// 	{
	// 		char *trimmed = ft_strtrim(line, "\n");
	// 		free(line);
	// 		exit_status = run_command_mode(minishell, trimmed);
	// 		free(trimmed);
	// 	}
	// }

	//--------------------uncomment for big test -------------------------------//

	free_minishell(minishell);
	rl_clear_history();
	return (exit_status);
}
