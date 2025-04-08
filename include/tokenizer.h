#ifndef TOKENIZER_H
# define TOKENIZER_H

# include <minishell.h>

typedef enum e_possible_tokens
{
	TOKEN_WORD,
	TOKEN_PIPE,
	TOKEN_REDIRECT_IN,
	TOKEN_REDIRECT_OUT,
	TOKEN_APPEND_OUT,
	TOKEN_BACKGROUND,
	TOKEN_HEREDOC,
	TOKEN_EOF
}	t_TokenType;


typedef struct s_token
{
	char	*value;
	t_TokenType	type;
	int	in_single_quotes;
	int	in_double_quotes;
}	t_Token;

// Token array for storing all tokens
typedef struct s_tokens_array
{
	t_Token	*tokens;
	int		count;
	int		capacity;
}	t_TokenArray;

// Initialize the tokenizer with input string
void	tokenizer_init(const char *input);

// Get the next token from the input
t_Token	get_next_token(void);

// Free resources used by a token
void	free_token(t_Token *token);

// Free tokenizer resources
void	tokenizer_cleanup(void);
char *expand_env_variables(const char *input,  t_mshell *minishell);
void	print_token(t_Token token);
void	explain_token(t_Token token);
void debug_print_parsed_commands(t_cmd *cmd);
int	strip_words(t_TokenArray *tokens);
bool	is_in_single_quotes(char *str);
bool	is_in_double_quotes(char *str);
void	expand_tokens(t_TokenArray *tokens, t_mshell *ms);

int			is_input_redir(t_TokenType type);
int			is_output_redir(t_TokenType type);
t_cmd		*finalize_cmd_list(t_list **cmd_list);
void		free_cmd_list(t_list **cmd_list);
void		handle_word_token(t_mshell *shell, t_list **cmd_list,
				t_cmd **current, char *value);
int			handle_input_redir(t_mshell *shell, t_list **cmd_list,
				t_cmd **current, t_Token *token);
int			handle_output_redir(t_mshell *shell, t_list **cmd_list,
				t_cmd **current, t_Token *token);
void	free_list_nodes_only(t_list **lst);

#endif
