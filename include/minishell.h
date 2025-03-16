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
# include <stdint.h>
# include <limits.h>

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
 * - `val_assigned`:	Indicates if the variable has value (1) or not (0).
 * - `next`:		Pointer to the next variable in the linked list (for handling collisions in hash table).
 */
typedef struct s_mshell_var
{
	char				*key;
	char				*value;
	int					val_assigned;
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
	char			**builtin;
	uint8_t			exit_status;
}	t_mshell;


// init minishell
t_mshell		*init_mshell(char **envp);
char			**setup_env(char **envp);
char			**setup_builtin(void);
char			*find_binary(t_cmd *cmd);

// setup hash table
int		setup_hash_table(t_mshell	*mshell);
bool	add_env_entry(t_mshell_var *current, char **new_env, int *index);
bool	process_env_bucket(t_mshell_var *bucket, char **new_env, int *index);
void	set_variable(t_mshell *mshell,  char *key, char *value, int assigned);
void	update_existing_variable(t_mshell_var *current, t_mshell_var *mshell_var, int val_assigned, t_mshell	*mshell);
unsigned int	hash_function(const char *key);
void	update_env(t_mshell *mshell);
// parser
t_cmd		*run_parser(t_mshell *shell, char *input);

// executor
uint8_t	run_executor(t_cmd *cmd);

int			ft_arr_size(char **arr);




// some shit exit utils, to be cleaned !!!!!!!
void		print_error(char *cmd);
void		command_not_found_handle(t_cmd *cmd);

char		*ms_getenv(t_mshell *mshell, char *key);
bool		is_debug_mode(void);
void		debug_printf(const char *format, ...);

void		free_minishell(t_mshell	*mshell);
void		free_mshell_var(t_mshell_var *var);
void		free_cmd(t_cmd *cmd);

#endif /* MINISHELL_H */
