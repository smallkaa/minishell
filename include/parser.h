/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Pavel Vershinin <pvershin@student.hive.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 21:20:20 by pvershin          #+#    #+#             */
/*   Updated: 2025/05/08 14:17:15 by Pavel Versh      ###   ########.fr       */
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
void				process_non_word(t_TokenArray *new_tokens, int *j,
						t_Token *tok);
void				process_word(t_TokenArray *new_tokens,
						t_TokenArray *old_tokens, int *i, int *j);
int					count_new_tokens(t_TokenArray *tokens);
void				process_pipe_token(t_cmd **current, int *i);
int					init_redir_command_if_needed(t_redir_ctx *ctx);
int					apply_redirection(t_redir_ctx *ctx, t_TokenType type);
t_TokenArray		*token_array_init(void);
void				token_array_add(t_TokenArray *array, t_Token token);
void				token_array_free(t_TokenArray *array);
int					group_word_tokens(t_TokenArray *tokens);
t_cmd				*create_command_from_tokens(t_mshell *shell,
						t_TokenArray *tokens);
int					handle_redir(t_parse_ctx *ctx);

#endif