#ifndef EXECUTOR_H
#define EXECUTOR_H

// Heredoc temp file
#define HEREDOC_TFILE "heredoc_tmp.txt"

#define ERROR_BUF_SIZE 256

// limits for file name and full path
#define MS_PATHMAX 4096
#define NAME_MAX 255

/*------FORWARD DECLARATIONS-----------------------------------------*/

typedef struct s_cmd t_cmd;
typedef struct s_mshell t_mshell;
typedef struct s_mshell_var t_mshell_var;
typedef struct s_hash_table t_hash_table;
typedef enum e_redir_type t_redir_type;
typedef struct s_builtin_dispatch t_builtin_dispatch;

/*------FUNCTIONS---------------------------------------------------*/

// standart cmd exec functions
uint8_t	exec_in_current_process(t_cmd *cmd);
uint8_t	exec_in_pipes(t_cmd *cmd);

// builtin
bool	is_builtin(t_cmd *cmd);
uint8_t	exec_builtins(t_cmd *cmd);
const	t_builtin_dispatch *get_builtin_table(size_t *size);

// exit
uint8_t handle_exit(t_cmd *cmd);
long long ft_atoll_exit(const char *str, bool *overflow);

// echo
uint8_t handle_echo(t_cmd *cmd);

// pwd
uint8_t handle_pwd(t_cmd *cmd);

// env
uint8_t handle_env(t_cmd *cmd);

// export
uint8_t handle_export(t_cmd *cmd);
void handle_sorted_env(t_mshell *mshell);
void free_keys(char **keys, int num_kyes);
uint8_t handle_unset(t_cmd *cmd);
t_mshell_var *create_new_variable(char *key, char *value, int assigned);

// cd
uint8_t handle_cd(t_cmd *cmd);
bool is_valid_value(t_cmd *cmd);
bool get_directory(char *cwd, t_cmd *cmd);

// redirections <, <<, >, >>
uint8_t apply_redirections(t_cmd *cmd);
uint8_t apply_heredocs(t_cmd *cmd);

// utils
bool is_valid_varname(const char *key_value_pair);
bool is_valid_identifier(const char *s);
t_mshell_var *split_key_value(char *key_value_pair);
bool is_pipeline_limit(int *cmd_count);

// exit utils

void child_execve_error(t_cmd *cmd);
uint8_t perror_return(char *msg, u_int8_t exit_status);
void cmd_missing_command_error(t_cmd *cmd);
void export_error(t_mshell_var *pair);
void unset_error(char *str);

#endif /* EXECUTOR_H */
