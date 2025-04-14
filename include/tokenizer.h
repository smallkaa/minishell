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
	TOKEN_EMPTY,
	TOKEN_EOF
}	t_TokenType;


typedef struct s_token
{
	char	*value;
	t_TokenType	type;
	int	in_single_quotes;
	int	in_double_quotes;
	bool		needs_join;
	int		quote_style; // 0 = none, 1 = single, 2 = double
}	t_Token;

// Token array for storing all tokens
typedef struct s_tokens_array
{
	t_Token	*tokens;
	int		count;
	int		capacity;
}	t_TokenArray;

typedef struct s_tokenizer
{
	const char	*input;
	const char	*input_base;
	char		*buffer;
	size_t		buffer_size;
}	t_Tokenizer;



// Get the next token from the input
t_Token	get_next_token(t_Tokenizer *tokenizer);

// Free resources used by a token
void	free_token(t_Token *token);

char *expand_env_variables(const char *input, t_mshell *minishell, int quote_style);
void	print_token(t_Token token);
void	explain_token(t_Token token);
void debug_print_parsed_commands(t_cmd *cmd);
int	strip_words(t_TokenArray *tokens);
bool	is_in_single_quotes(char *str);
bool	is_in_double_quotes(char *str);
void	expand_tokens(t_TokenArray *tokens, t_mshell *ms);

t_Tokenizer	*tokenizer_create(const char *input);
void		tokenizer_destroy(t_Tokenizer *tokenizer);

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

void	append_char_to_result(char c, char **result);
void	handle_single_quote(const char *input, size_t *i, int *single_q, char **result, int double_q);
void	handle_double_quote(const char *input, size_t *i, int *double_q, char **result, int single_q);
void	handle_backslash(const char *input, size_t *i, char **result, int single_q);
void	handle_tilde(const char *input, size_t *i, char **result, t_mshell *mshell, int single_q, int double_q);
void	handle_dollar(const char *input, size_t *i, char **result, t_mshell *mshell, int single_q, int double_q);

char *get_env_value(const char *var, t_mshell *minishell);
char *append_to_result(char *result, const char *append);
char *handle_escape(const char *input, size_t *i, int single_q);
char *expand_tilde(const char *input, size_t *i, t_mshell *mshell, int single_q, int double_q);
char *get_exit_code(t_mshell *minishell);
bool	is_valid_redir_target(t_TokenArray *tokens, int i);
t_cmd	*create_empty_command(t_mshell *shell);


#endif
