#ifndef MINISHELL_H
# define MINISHELL_H

#include <stdlib.h>
#include <stdio.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>


#define MAX_ARGS 64 // ?
#define MAX_CMDS 10 // ?
#define DELIM " "


// simple command structure, used by parser
typedef struct s_cmd
{
	char	*cmd;					// Command name (e.g., "ls", "grep")
	char	**args;					// Array of arguments (e.g., ["-al"])
	char	*infile;				// Input redirection (e.g., "infile")
	char	*outfile;				// Output redirection (e.g., "out.txt")
	char	*error;					// Error redirection (e.g., "errfile") ?? check
}	t_cmd;


// kind of table, read page 3
// https://www.cs.purdue.edu/homes/grr/SystemsProgrammingBook/Book/Chapter5-WritingYourOwnShell.pdf
typedef struct s_cmds
{
	t_cmd	**cmds;					// arr of t_cmd cmd, for iteration in pipeline, NULL terminated

} t_cmds;

#endif // MINISHELL_H
