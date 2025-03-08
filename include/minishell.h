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

#define HASH_SIZE 128

/**
 * @struct	s_mshell_var
 * @brief	Represents a single environment variable in minishell.
 *
 * - `key`:			The key of the environment variable (e.g., "PATH").
 * - `value`:		The value associated with the variable (e.g., "/usr/bin:/bin").
 * - `exported`:	Indicates if the variable is exported (1) or local (0).
 * - `next`:		Pointer to the next variable in the linked list (for handling collisions in hash table).
 */
typedef struct s_mshell_var
{
	char				*key;
	char				*value;
	int					exported;
	struct s_mshell_var	*next;
}	t_mshell_var;

/**
 * @struct	s_hash_table
 * @brief	Represents the hash table for environment variables.
 *
 * - `buckets`:	An array of linked lists (chained hashing) for
 * 				storing environment variables.
 *				The index in the array is determined by a hash function.
 */
typedef struct s_hash_table
{
	t_mshell_var	*buckets[HASH_SIZE];
}	t_hash_table;

/**
 * @struct	s_mshell
 * @brief	Represents a general minishell structure.
 *
 * - `last_exit_stats`:	Int, the last exit status of
 * 						cmd, builtin, or cmd in pipe
 */
typedef struct s_mshell
{
	char			**env;
	t_hash_table	*hash_table;
	int				exit_stat;
}	t_mshell;


// init minishell
t_mshell	*init_mshell(char **envp);
char		**setup_env(char **envp);
t_hash_table	*setup_hash_table(t_mshell *minishell);


void		find_executable(t_cmd *cmd);

// parser
t_cmd		*run_parser(t_mshell *shell, char *input);

// executor
void			run_executor(t_cmd *cmd);

int			ft_arr_size(char **arr);

// exit utils !!!!!!!
void		print_error_exit(char *cmd, int exit_status); // to be fixed
void		print_error(char *cmd);
void		update_last_exit_status(t_cmd *cmd, int status);
void		command_not_found_handle(t_cmd *cmd);
void		error_handler(t_cmd *cmd);


bool		is_debug_mode(void);
void		debug_printf(const char *format, ...);

void	free_minishell(t_mshell *minishell);

#endif /* MINISHELL_H */
