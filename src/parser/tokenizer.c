#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/minishell.h"

t_Tokenizer	*tokenizer_create(const char *input)
{
	t_Tokenizer	*tokenizer;

	tokenizer = (t_Tokenizer *)malloc(sizeof(t_Tokenizer));
	if (!tokenizer)
		return (NULL);
	tokenizer->input = input;
	tokenizer->input_base = input;
	tokenizer->buffer_size = ft_strlen(input) + 1;
	tokenizer->buffer = (char *)malloc(tokenizer->buffer_size);
	if (!tokenizer->buffer)
	{
		free(tokenizer);
		return (NULL);
	}
	return (tokenizer);
}

void	tokenizer_destroy(t_Tokenizer *tokenizer)
{
	if (!tokenizer)
		return ;
	free(tokenizer->buffer);
	free(tokenizer);
}



// Public API function - Free a token's resources
void free_token(t_Token *token) {
    if (token && token->value) {
        free(token->value);
        token->value = NULL;
    }
}

// Check if character is a shell operator
static int ft_is_special_char(char c) {
    return (c == '|' || c == '<' || c == '>' || c == '&');
}




// Public API function - Get the next token from the input


t_Token	get_next_token(t_Tokenizer *tokenizer)
{
	t_Token	token;
	size_t	index;
	int		in_single_quote;
	int		in_double_quote;
	int		saw_space;

	ft_bzero(&token, sizeof(t_Token));
	token.type = TOKEN_WORD;
	saw_space = 0;

	if (!tokenizer || !tokenizer->buffer || tokenizer->buffer_size == 0)
	{
		write(2, "token_buffer not initialized\n", 30);
		exit(1);
	}

	if (!*tokenizer->input)
	{
		token.type = TOKEN_EOF;
		return (token);
	}

	while (*tokenizer->input && (*tokenizer->input == ' ' || *tokenizer->input == '\t'))
	{
		saw_space = 1;
		tokenizer->input++;
	}
	token.needs_join = saw_space;

	if (!*tokenizer->input)
	{
		token.type = TOKEN_EOF;
		return (token);
	}

	if (*tokenizer->input == '$' && *(tokenizer->input + 1) == '"')
	{
		index = 0;
		token.in_double_quotes = 1;
		tokenizer->buffer[index++] = *tokenizer->input++;
		tokenizer->buffer[index++] = *tokenizer->input++;
		while (*tokenizer->input && *tokenizer->input != '"')
			tokenizer->buffer[index++] = *tokenizer->input++;
		if (*tokenizer->input == '"')
			tokenizer->buffer[index++] = *tokenizer->input++;
		tokenizer->buffer[index] = '\0';
		token.value = ft_strdup(tokenizer->buffer);
		token.needs_join = saw_space;
		return (token);
	}

	if (*tokenizer->input == '"')
	{
		index = 0;
		tokenizer->input++;
		token.in_double_quotes = 1;
		token.quote_style = 2;
		while (*tokenizer->input && *tokenizer->input != '"')
		{
			if (index < tokenizer->buffer_size - 1)
				tokenizer->buffer[index++] = *tokenizer->input;
			tokenizer->input++;
		}
		if (*tokenizer->input == '"')
			tokenizer->input++;
		tokenizer->buffer[index] = '\0';
		token.value = ft_strdup(tokenizer->buffer);
		token.needs_join = saw_space;
		return (token);
	}

	if (*tokenizer->input == '\'')
	{
		index = 0;
		tokenizer->input++;
		token.in_single_quotes = 1;
		token.quote_style = 1;
		while (*tokenizer->input && *tokenizer->input != '\'')
		{
			if (index < tokenizer->buffer_size - 1)
				tokenizer->buffer[index++] = *tokenizer->input;
			tokenizer->input++;
		}
		if (*tokenizer->input == '\'')
			tokenizer->input++;
		tokenizer->buffer[index] = '\0';
		token.value = ft_strdup(tokenizer->buffer);
		token.needs_join = saw_space;
		return (token);
	}

	if (*tokenizer->input == '<' && *(tokenizer->input + 1) == '<')
	{
		tokenizer->input += 2;
		token.type = TOKEN_HEREDOC;
		token.value = ft_strdup("<<");
		return (token);
	}
	if (*tokenizer->input == '>' && *(tokenizer->input + 1) == '>')
	{
		tokenizer->input += 2;
		token.type = TOKEN_APPEND_OUT;
		token.value = ft_strdup(">>");
		return (token);
	}

	if (ft_is_special_char(*tokenizer->input))
	{
		token.value = malloc(2);
		token.value[0] = *tokenizer->input++;
		token.value[1] = '\0';
		return (token);
	}

	index = 0;
	in_single_quote = 0;
	in_double_quote = 0;
	while (*tokenizer->input)
	{
		char c = *tokenizer->input;

		if ((c == ' ' || c == '\t') && !in_single_quote && !in_double_quote)
			break;
		if (ft_is_special_char(c) && !in_single_quote && !in_double_quote)
			break;
		if (c == '\'' && !in_double_quote)
		{
			in_single_quote = !in_single_quote;
			token.in_single_quotes = 1;
		}
		else if (c == '"' && !in_single_quote)
		{
			in_double_quote = !in_double_quote;
			token.in_double_quotes = 1;
		}
		if (index < tokenizer->buffer_size - 1)
			tokenizer->buffer[index++] = c;
		tokenizer->input++;
	}
	tokenizer->buffer[index] = '\0';

	if (in_single_quote)
		token.quote_style = 1;
	else if (in_double_quote)
		token.quote_style = 2;
	else
		token.quote_style = 0;

	token.value = ft_strdup(tokenizer->buffer);
	token.needs_join = saw_space;

	return (token);
}









bool	is_in_single_quotes(char *str)
{
	size_t	len;

	if (!str || !*str)
		return (false);

	len = ft_strlen(str);
	if (len < 2)
		return (false);

	return (str[0] == '\'' && str[len - 1] == '\'');
}


bool	is_in_double_quotes(char *str)
{
	size_t	len;

	if (!str || !*str)
		return (false);

	len = ft_strlen(str);
	if (len < 2)
		return (false);

	return (str[0] == '"' && str[len - 1] == '"');
}

static char	*strip_quotes_preserving_inner(const char *str, int quote_style)
{
	char	*result;
	size_t	len = ft_strlen(str);
	size_t	j = 0;
	size_t	i = 0;
	bool	in_squote = false;
	bool	in_dquote = false;

	result = malloc(len + 1);
	if (!result)
		return (NULL);

	while (str[i])
	{
		if (str[i] == '\'' && quote_style == 1 && !in_dquote)
		{
			in_squote = !in_squote;
			i++;
			continue;
		}
		else if (str[i] == '"' && quote_style == 2 && !in_squote)
		{
			in_dquote = !in_dquote;
			i++;
			continue;
		}
		else
		{
			result[j++] = str[i++];
		}
	}
	result[j] = '\0';
	return result;

}






/**
 * @brief Processes TOKEN_WORD tokens to strip enclosing quotes.
 *
 * Goes through all tokens of type TOKEN_WORD and if a token value is
 * enclosed in matching quotes (either single or double), removes those quotes.
 *
 * @param tokens Array of tokens to process.
 * @return 0 on success, -1 on error.
 */
int strip_words(t_TokenArray *tokens)
{
	int		i;
	char	*str;
	char	*result;

	if (!tokens || !tokens->tokens)
		return (-1);

	i = 0;
	while (i < tokens->count)
	{
		t_Token *tok = &tokens->tokens[i];

		if (tok->type == TOKEN_WORD && tok->value)
		{
			if (tok->in_single_quotes && tok->in_double_quotes)
			{
				// вложенные кавычки — не трогаем
				i++;
				continue;
			}
			str = tok->value;
			result = strip_quotes_preserving_inner(str, tok->quote_style);
			if (!result)
				return (-1);
			free(str);
			tok->value = result;
		}
		i++;
	}
	return (0);
}






