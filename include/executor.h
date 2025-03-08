#ifndef EXECUTOR_H
# define EXECUTOR_H

// Heredoc temp file
# define HEREDOC_TFILE "heredoc_tmp.txt"

//limits for file name and full path
# define PATH_MAX 4096
# define NAME_MAX 255

/*------FORWARD DECLARATIONS-----------------------------------------*/

typedef struct s_cmd	t_cmd;

/*------FUNCTIONS----------------------------------------------------*/

// standart cmd exec functions
void	execute(t_cmd *cmd, int in_fd);

// builtin exec functions
bool	is_builtin(t_cmd *cmd);
void	exec_builtin(t_cmd *cmd);
void	handle_exit(t_cmd *cmd);
void	handle_echo(t_cmd *cmd);
void	handle_pwd(t_cmd *cmd);
void	handle_cd(t_cmd *cmd);
void	handle_env(t_cmd *cmd);
void	handle_export(t_cmd *cmd);
void	print_export_env(t_cmd *cmd);
void	print_local_var(t_cmd *cmd);
bool	is_valid_varname(const char *key_value_pair);
int		arr_size(char **arr);

void	find_executable(t_cmd *cmd);

// redirections <, <<
void	handle_in_redirection(t_cmd *cmd);
void	handle_out_redirection(t_cmd *cmd);
void	handle_heredoc(t_cmd *cmd);

// exit utils
void	update_last_exit_status(t_cmd *cmd, int status);
void	command_not_found_handle(t_cmd *cmd);
void	error_handler(t_cmd *cmd);
// void	cleanup_heredoc(t_cmd *cmd);

#endif /* EXECUTOR_H */
