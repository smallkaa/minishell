#include "minishell.h"

// read, parse, execute
void	minishell_loop(void)
{
	char	*line;
	char	**args;
	int	status;

	while(status)
	{
		printf("> ");
		line = read_line();
		args = split_line(line);
		status = execute(args);
	}
	free(line);
	free(args);
}

int	main(int argc, char **argv)
{
	// Config files ?


	minishell_loop(); // basic program loop

	// Cleanup

	return (EXIT_SUCCESS);
}
