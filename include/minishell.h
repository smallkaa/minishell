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
# include <sys/types.h>

# include "libft.h"
# include "executor.h"
# include "tokenizer.h"
# include "signals.h"

// Maximum number of arguments per command.
# define MAX_ARGS 64

// Maximum number of commands in a pipeline.
# define MAX_CMDS 10

// Delimiter used for tokenizing input.
# define DELIM " "

/**
 * @struct s_minishell
 * @brief Represents a general minishell structure.
 *
 * - `last_exit_stats`:	Int, the last exit status of
 * 						cmd, builtin, or cmd in pipe
 */
typedef struct s_minishell
{
	int	exit_stat;
}	t_minishell;

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
	t_minishell			*minishell;
	bool				in_pipe;
}	t_cmd;

// init minishell
t_minishell	*init_minishell(void);

// parser
t_cmd		*run_parser(t_minishell *shell, char *input);

// executor
int		run_executor(t_cmd *cmd, char **envp);

// exit utils
void		print_error_exit(char *cmd, int exit_status); // to be fixed
void		print_error(char *cmd);

bool		is_debug_mode(void);
void		debug_printf(const char *format, ...);

#endif /* MINISHELL_H */
