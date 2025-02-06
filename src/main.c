#include "minishell.h"

// read, parse, execute
void	minishell_loop(char **argv)
{
	char	*line;
	char	**args;
	int	status;

	status = 1;
	while(status)
	{
		printf("> "); // add some feature to display, working dir, hostname, username ...
		line = read_line(argv);
		if(line)
		{
			args = split_line(line);
			if (args)
			{
				status = execute(args);
			}
		}
	}
	free(line); // exemple
	free(args); // exemple
}

int	main(int argc, char **argv)
{
	// Config files ?

	minishell_loop(argv); // basic program loop

	// Cleanup

	return (EXIT_SUCCESS);
}
