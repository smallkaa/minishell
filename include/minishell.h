#ifndef MINISHELL_H
# define MINISHELL_H

#include <stdlib.h>
#include <stdio.h>

#define BUF_SIZE 1024

typedef struct s_command
{
	char	*cmd;					// Command name (e.g., "ls", "grep")
	char	**args;					// Array of arguments (e.g., ["-al"])
	char	*input;					// Input redirection (e.g., "infile")
	char	*output;				// Output redirection (e.g., "out.txt")
	char	*error;					// Error redirection (e.g., "errfile")
}	t_command;

typedef struct s_command_table
{
	t_command	*command;			// Array of command structs
	int	num_commands;				// Number of commands in the pipeline
};

#endif // MINISHELL_H
