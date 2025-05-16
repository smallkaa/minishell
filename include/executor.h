/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imunaev- <imunaev-@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 17:46:42 by imunaev-          #+#    #+#             */
/*   Updated: 2025/05/16 10:57:18 by imunaev-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXECUTOR_H
# define EXECUTOR_H

// Maximum number of commands in a pipeline.
# define MAX_CMDS 1000

/*------FORWARD DECLARATIONS-----------------------------------------*/

typedef struct s_cmd				t_cmd;
typedef struct s_mshell				t_mshell;
typedef struct s_mshell_var			t_mshell_var;
typedef struct s_hash_table			t_hash_tbl;
typedef enum e_redir_type			t_redir_type;
typedef struct s_redir				t_redir;
typedef struct s_builtin_dispatch	t_builtin_disp;

typedef struct s_exit_info
{
	const char	*prefix;
	const char	*arg;
	const char	*msg;
	int			code;
	t_mshell	*mshell;
	t_cmd		*cmd;
	t_cmd		*origin_head;
}	t_exit_info;

typedef struct s_pipe_info
{
	int		in_fd;
	int		*pipe_fd;
	pid_t	*pids;
	int		*idx;
	t_cmd	*cmd_list;
}	t_pipe_info;

typedef struct s_heredoc_ctx
{
	int			pipe_fd;
	const char	*delim;
	t_mshell	*mshell;
	int			expand;
	size_t		*total;
}	t_heredoc_ctx;

typedef struct s_heredoc_exec_ctx
{
	t_cmd		*cmd;
	const char	*delim;
	t_cmd		*current;
	t_cmd		*full_cmd_list;
	int			*pipe_fd;
}	t_heredoc_exec_ctx;

/*------FUNCTIONS---------------------------------------------------*/

// executor utils
bool					is_valid_varname(const char *key);
bool					is_exit_command(t_cmd *cmd);
void					cleanup_and_exit(t_cmd *cmd, int exit_status);
bool					command_too_long(char **argv);
void					update_underscore(t_cmd *cmd, char *binary_path);

// parent process
uint8_t					exec_in_current_process(t_cmd *cmd);

// pipes
uint8_t					exec_in_pipes(t_cmd *cmd);
void					execute_command(t_cmd *cmd);
// void					handle_child_and_track(t_cmd *cmd,
//t_pipe_info *info, int level);
void					handle_child_and_track(t_cmd *cmd, t_pipe_info *info);
uint8_t					wait_for_children(pid_t *pids, int count);
uint8_t					close_unused_fds(int in_fd, int *pipe_fd);

// pipe utils
bool					is_minishell_executable(t_cmd *cmd);
uint8_t					validate_dots(t_cmd *cmd);
uint8_t					update_shlvl(t_cmd *cmd);
void					handle_empty_command(t_cmd *cmd);
uint8_t					close_unused_fds(int in_fd, int *pipe_fd);
void					handle_pipe_creation(t_cmd *cmd, int *pipe_fd);
void					close_fds_and_prepare_next(t_cmd *cmd, int *in_fd,
							int *pipe_fd);
void					init_pipe_info(t_pipe_info *info, t_cmd *cmd_list,
							int pipe_fd[2], pid_t pids[MAX_CMDS]);

// builtin
const t_builtin_disp	*get_builtin_table(size_t *size);
bool					is_builtin(t_cmd *cmd);
uint8_t					exec_builtins(t_cmd *cmd);

// exit
uint8_t					handle_exit(t_cmd *cmd);
long long				ft_atoll_exit(const char *str, bool *overflow);
uint8_t					get_exit_status(char *arg);
bool					is_valid_numeric_exit_arg(const char *arg);

// echo
uint8_t					handle_echo(t_cmd *cmd);
int						is_echo_flag(char *arg);
int						is_wrapped_in_quotes(char *s);

// pwd
uint8_t					handle_pwd(t_cmd *cmd);

// env
uint8_t					handle_env(t_cmd *cmd);

// export
uint8_t					handle_export(t_cmd *cmd);
int						handle_sorted_env(t_mshell *mshell);
int						collect_keys(t_hash_tbl *ht, char ***keys, int *count);

// unset
uint8_t					handle_unset(t_cmd *cmd);
t_mshell_var			*create_new_var(char *key, char *value, int assigned);

// cd
uint8_t					handle_cd(t_cmd *cmd);
bool					get_directory(char *cwd, t_cmd *cmd);
void					update_pwd_variables(t_cmd *cmd, char *old_cwd);
uint8_t					cd_too_many_args(void);
uint8_t					change_and_update_pwd(t_cmd *cmd);

// redirections <, >, >>
uint8_t					apply_redirections(t_cmd *cmd);

// heredoc <<
uint8_t					apply_heredocs(t_cmd *cmd);
bool					heredoc_exceeds_limit(size_t total_written);
bool					is_heredoc(t_redir *redirection);
void					close_all_heredoc_fds(t_cmd *cmd_list);
int						write_heredoc_to_pipe(t_cmd *cmd,
							int pipe_fd, const char *delim);
void					close_unused_heredocs_child(t_cmd *current,
							t_cmd *full_cmd_list);
int						handle_heredoc_redirection(t_redir *r);
t_redir					*find_redir_by_delim(t_list *redirs,
							const char *delim);
int						handle_heredoc_parent(pid_t pid, int *pipe_fd,
							t_cmd *full_cmd_list);
void					handle_heredoc_child(t_heredoc_exec_ctx ctx);

// utils
t_mshell_var			*split_key_value(char *key_value_pair);
void					safe_close(int *fd);

// exit utils
int						perror_return(const char *msg, int exit_status);
uint8_t					perror_exit_child(t_cmd *cmd,
							const char *msg, u_int8_t exit_status);
void					cmd_missing_command_error(t_cmd *cmd);
void					export_error(t_mshell_var *pair);
u_int8_t				unset_error(const char *str);
int						error_return(const char *msg, int exit_status);
uint8_t					no_cmd_error(const char *msg);
void					child_execve_error(t_cmd *cmd);
void					handle_is_directory(t_cmd *cmd);
void					handle_not_found_or_command(t_cmd *cmd);
void					handle_permission_denied(t_cmd *cmd);
void					handle_exec_format_error(t_cmd *cmd);
void					handle_generic_execve_error(t_cmd *cmd);
void					print_and_exit(t_exit_info info);
t_cmd					*get_cmd_head(t_cmd *cmd);

#endif /* EXECUTOR_H */
