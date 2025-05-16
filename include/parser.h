/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pvershin <pvershin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 21:20:20 by pvershin          #+#    #+#             */
/*   Updated: 2025/05/16 10:17:49 by pvershin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_H
# define PARSER_H

# include <minishell.h>

typedef struct s_wordinfo
{
	char			*grouped;
	char			*tmp;
	bool			single_q;
	bool			double_q;
	int				qstyle;
	t_Token			new_token;
}					t_wordinfo;

typedef struct s_redir_ctx
{
	t_mshell		*shell;
	t_TokenArray	*tokens;
	t_list			**cmd_list;
	t_cmd			**current;
	int				*i;
}					t_redir_ctx;

typedef struct s_parse_ctx
{
	t_mshell		*shell;
	t_TokenArray	*tokens;
	t_list			**cmd_list;
	t_cmd			**current;
	t_redir_ctx		redir;
	int				i;
}					t_parse_ctx;
bool				is_skippable_token(t_Token *tok);
int				process_non_word(t_TokenArray *new_tokens, int *j,
						t_Token *tok);
int				process_word(t_TokenArray *new_tokens,
						t_TokenArray *old_tokens, int *i, int *j);
int					count_new_tokens(t_TokenArray *tokens);
void				process_pipe_token(t_cmd **current, int *i);
int					init_redir_command_if_needed(t_redir_ctx *ctx);
int					apply_redirection(t_redir_ctx *ctx, t_TokenType type);
t_TokenArray		*token_array_init(void);
void				token_array_add(t_TokenArray *array, t_Token token, t_mshell *shell);
void				*token_array_free(t_TokenArray *array);
int					group_word_tokens(t_TokenArray *tokens, t_mshell *shell);
t_cmd				*create_command_from_tokens(t_mshell *shell,
						t_TokenArray *tokens);
int					handle_redir(t_parse_ctx *ctx);
int					fill_new_tokens(t_TokenArray *new_tokens,
						t_TokenArray *old_tokens);
int	handle_allocation_error(t_mshell *msh);
int	cleanup_and_return_error(t_TokenArray *new_tokens_array, t_mshell *msh);
void	replace_old_tokens(t_TokenArray *old, t_TokenArray *new_tokens_array);
int	check_for_unsupported_syntax(t_mshell *shell, char *input);



#endif