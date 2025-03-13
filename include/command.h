#ifndef COMMAND_H
# define COMMAND_H

// Maximum number of arguments per command.
# define MAX_ARGS 64

// Maximum number of commands in a pipeline.
# define MAX_CMDS 300

/*------FORWARD DECLARATIONS-----------------------------------------*/

typedef struct s_mshell	t_mshell;

/**
 * @enum e_redir_type
 * @brief Enumeration for different types of I/O redirection.
 *
 * - `R_INPUT`:		`<` Redirects standard input from a file.
 * - `R_OUTPUT`:	`>` Redirects standard output to a file.
 * - `R_APPEND`:	`>>` Redirects standard output in append mode.
 * - `R_HEREDOC`:	`<<` Reads input until a specified delimiter is found.
 */
typedef enum e_redir_type
{
	R_INPUT,
	R_OUTPUT,
	R_APPEND,
	R_HEREDOC
}	t_redir_type;

/**
 * @struct s_redir
 * @brief Structure for handling I/O redirections.
 *
 * - `type`:		The type of redirection.
 * - `filename`:	For `R_INPUT`, `R_OUTPUT`, or `R_APPEND`,
 * 					this stores the filename.
 *					For `R_HEREDOC`, it stores the delimiter string.
 */
typedef struct s_redir
{
	t_redir_type	type;
	char			*filename;
	bool			expand;
}	t_redir;

/**
 * @struct s_cmd
 * @brief Represents a single command in a pipeline.
 *
 * - `argv`:		NULL-terminated array of command arguments.
 * - `binary`:		Binary from PATH env for execve().
 * - `in_redir`:	Pointer to input redirection struct, if any.
 * - `out_redir`:	Pointer to output redirection struct, if any.
 * - `next`:		Pointer to the next command in the pipeline.
 * - `shell`:		Pointer to shell structure.
 *
 * Example pipeline: `ls -l | grep minishell | wc -l`
 * - cmd1:	`ls -l`
 * - cmd2:	`grep minishell`
 * - cmd3:	`wc -l`
 * - cmd4:	`NULL` (end of pipeline)
 */
typedef struct s_cmd
{
	char				**argv;
	char				*binary;
	t_redir				*in_redir;
	t_redir				*out_redir;
	t_cmd				*next;
	t_mshell			*minishell;
	bool				in_pipe;
}	t_cmd;

# endif  /* COMMAND_H */


// beta
/*
typedef struct s_redir
{
	t_redir_type	type;
	char			*filename;
	struct s_redir	*next;
	bool			is_heredoc_expanded; //  for heredoc processing
}	t_redir;

typedef struct s_cmd
{
	char			**argv;
	char			*binary;
	t_redir			*redirs;   // linked list of redirections
	struct s_cmd	*next;
	t_mshell		*minishell;
	bool			in_pipe;
}	t_cmd;


# command
grep "hello" < input.txt > output.txt >> log.txt

# parser creates cmd struct:
cmd->argv = ["grep", "hello", NULL];
cmd->redirs:
	-> R_INPUT ("input.txt")
	-> R_OUTPUT ("output.txt")
	-> R_APPEND ("log.txt")

# executor in child process:
- Iterate redirections list:
	- open input.txt and dup2 to STDIN_FILENO
	- open output.txt for writing and dup2 to STDOUT_FILENO
	- open log.txt in append mode and redirect STDOUT again
# execute "grep hello"
execve("grep", ["grep", "hello"], env);
*/
