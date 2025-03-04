#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdlib.h>
# include <stdio.h>
# include <unistd.h>
# include <stdbool.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <sys/wait.h>
# include <fcntl.h>
# include <string.h>
# include <signal.h>
#include <sys/types.h>

//Definition of AT_* constants
# include <fcntl.h>
# include "libft.h"
# include "tokenizer.h"
# include "signals.h"

// Maximum number of arguments per command.
# define MAX_ARGS 64

// Maximum number of commands in a pipeline.
# define MAX_CMDS 10

// Delimiter used for tokenizing input.
# define DELIM " "

// Heredoc temp file
# define HEREDOC_TFILE "heredoc_tmp.txt"

typedef struct s_cmd t_cmd;
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
 * @struct s_shell
 * @brief Represents a general minishell structure.
 *
 * - `last_exit_stats`:	Int, the last exit status of 
 * 						cmd, builtin, or cmd in pipe
 */
typedef struct s_shell
{
    int	l_exit_stat;  // Stores the last exit status ($?)	
}	t_shell;

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
	t_shell				*shell;
}	t_cmd;


// utils
void	print_error_exit(char *cmd, int exit_status);
void	print_error(char *cmd);
void	cleanup_heredoc(t_cmd *cmd);
bool	is_debug_mode(void);
void	debug_printf(const char *format, ...);

// redirections <, <<
void	handle_in_redirection(t_cmd *cmd, char **envp);
void	handle_heredoc(t_cmd *cmd, char **envp);

// redirections >, >>
void	handle_out_redirection(t_cmd *cmd);
bool	is_builtin(t_cmd *cmd);

// builtin
void	exec_builtin(t_cmd *cmd);
int		handle_exit(t_cmd *cmd);
int		handle_echo(t_cmd *cmd);

// parser
t_cmd	*run_parser(char	*input);

// executor
void	run_executor(t_cmd *cmd, char **envp);
void	execute(t_cmd *cmd, int in_fd, char **envp);

// executor utils
void	update_last_exit_status(t_cmd *cmd, int status);

#endif /* MINISHELL_H */
