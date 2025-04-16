#ifndef COMMAND_H
# define COMMAND_H
# include "libft.h"

// Maximum number of arguments per command.
# define MAX_ARGS 64

// Maximum number of commands in a pipeline.
# define MAX_CMDS 1000

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
 * @brief Represents a single redirection in a command.
 *
 * - type:      The redirection type (input, output, append, heredoc).
 * - filename:  File name for input/output, or delimiter for heredoc.
 * - target_fd: File descriptor to redirect
 * (typically STDIN_FILENO or STDOUT_FILENO).
 * - fd:        Runtime file descriptor used (especially for heredoc).
 * - expand:    Indicates whether to perform variable expansion (for heredocs).
 */
typedef struct s_redir
{
	t_redir_type	type;
	char			*filename;
	int				target_fd;
	int				fd;
}	t_redir;
/**
 * @struct s_cmd
 * @brief Represents a command node in a pipeline.
 *
 * - argv:      NULL-terminated list of arguments for the command.
 * - binary:    Resolved binary path to be executed.
 * - next:      Pointer to the next command in a pipeline.
 * - minishell: Pointer to the main shell state/context.
 * - redirs:    List of redirections associated with
 * this command (t_list of t_redir*).
 *
 * Example pipeline: `ls -l | grep minishell | wc -l`
 * - cmd1: "ls -l"
 * - cmd2: "grep minishell"
 * - cmd3: "wc -l"
 */
typedef struct s_cmd
{
	char		**argv;
	char		*binary;
	t_cmd		*next;
	t_mshell	*minishell;
	t_list		*redirs;
}	t_cmd;

#endif /* COMMAND_H */
