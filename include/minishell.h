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
# include <errno.h>

# include "libft.h"
# include "executor.h"
# include "tokenizer.h"
# include "signals.h"
# include "command.h"

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
	char	**env;
	char	**local_var;
	int		exit_stat;
}	t_minishell;


// init minishell
t_minishell	*init_minishell(char **envp);
void		find_executable(t_cmd *cmd);

// parser
t_cmd		*run_parser(t_minishell *shell, char *input);

// executor
int			run_executor(t_cmd *cmd);

int			ft_arr_size(char **arr);

// exit utils !!!!!!! 
void		print_error_exit(char *cmd, int exit_status); // to be fixed
void		print_error(char *cmd);
void		update_last_exit_status(t_cmd *cmd, int status);
void		command_not_found_handle(t_cmd *cmd);
void		error_handler(t_cmd *cmd);


bool		is_debug_mode(void);
void		debug_printf(const char *format, ...);

#endif /* MINISHELL_H */
