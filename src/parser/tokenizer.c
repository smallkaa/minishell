#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/minishell.h"

static const char *current_input = NULL;
static char *token_buffer = NULL;
static size_t token_buffer_size = 0;

// Public API function - Initialize the tokenizer
void tokenizer_init(const char *input) {
    current_input = input;
    
    // Initial buffer allocation
    token_buffer_size = 256;
    token_buffer = (char *)malloc(token_buffer_size);
    if (!token_buffer) {
        print_error("Failed to allocate token buffer\n");
        exit(1);
    }
}

// Public API function - Clean up resources
void tokenizer_cleanup() {
    free(token_buffer);
    token_buffer = NULL;
    current_input = NULL;
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

// Skip whitespace in the input
static void skip_whitespace() {
    while (*current_input && ft_isspace((unsigned char)*current_input)) {
        current_input++;
    }
}

// Add a character to the token buffer, expanding if necessary
static void append_char_to_buffer(char c, size_t *buffer_index) 
{
    // Check buffer capacity
    if (*buffer_index >= token_buffer_size - 1) {
        token_buffer_size *= 2;
        token_buffer = (char *)realloc(token_buffer, token_buffer_size);
        if (!token_buffer) {
            print_error("Failed to reallocate token buffer\n");
            exit(1);
        }
    }
    
    token_buffer[(*buffer_index)++] = c;
}

// Public API function - Get the next token from the input
t_Token	get_next_token(void)
{
	t_Token	token;
	size_t	buf_i;
	char	c;
	char	quote_char;

	token.value = NULL;
	token.type = TOKEN_WORD;
	token.in_single_quotes = 0;
	token.in_double_quotes = 0;

	if (!token_buffer || token_buffer_size == 0)
	{
		write(2, "token_buffer not initialized\n", 30);
		exit(1);
	}

	buf_i = 0;
	token_buffer[0] = '\0';
	quote_char = 0;
	skip_whitespace();

	while (*current_input && (quote_char || (!ft_isspace(*current_input) && !ft_is_special_char(*current_input))))
	{
		c = *current_input;
		if ((c == '"' || c == '\'') && quote_char == 0)
		{
			quote_char = c;
			if (c == '"')
				token.in_double_quotes = 1;
			else if (c == '\'')
				token.in_single_quotes = 1;
			current_input++;
		}
		else if (c == quote_char)
		{
			quote_char = 0;
			current_input++;
		}
		else
		{
			append_char_to_buffer(*current_input++, &buf_i);
		}
	}

	if (buf_i > 0)
	{
		token_buffer[buf_i] = '\0';
		token.value = ft_strdup(token_buffer);
		return (token);
	}

	if (*current_input && !ft_isspace(*current_input))
	{
		token_buffer[0] = *current_input++;
		token_buffer[1] = '\0';
		token.value = ft_strdup(token_buffer);
		token.type = TOKEN_WORD;
		return (token);
	}

	token.type = TOKEN_EOF;
	return (token);
}



/**
 * @brief Checks if a string is enclosed in matching quotes.
 *
 * This function verifies if a string starts and ends with the same type of quote.
 *
 * @param str The string to check.
 * @return true if the string is enclosed in matching quotes, false otherwise.
 */
static bool	is_enclosed_in_quotes(char *str)
{
	size_t	len;

	if (!str || !*str)
		return (false);
	
	len = ft_strlen(str);
	if (len < 2)
		return (false);
	
	return ((str[0] == '\'' && str[len - 1] == '\'') ||
			(str[0] == '"' && str[len - 1] == '"'));
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

/**
 * @brief Removes enclosing quotes from a string.
 *
 * Creates a new string without the first and last characters if they are quotes.
 * The original string is freed.
 *
 * @param str Pointer to the string to be modified.
 * @return The new string without quotes, or NULL if allocation fails.
 */
static char	*remove_enclosing_quotes(char *str)
{
	char	*new_str;
	size_t	len;

	len = ft_strlen(str);
	new_str = ft_substr(str, 1, len - 2);
	free(str);
	return (new_str);
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
int	strip_words(t_TokenArray *tokens)
{
	int		i;
	char	*str;
	size_t	len;

	if (!tokens || !tokens->tokens)
		return (-1);
	i = 0;
	while (i < tokens->count)
	{
		if (tokens->tokens[i].type == TOKEN_WORD
			&& tokens->tokens[i].value)
		{
			str = tokens->tokens[i].value;
			len = ft_strlen(str);
			if (len >= 2 && is_enclosed_in_quotes(str))
			{
				tokens->tokens[i].value = remove_enclosing_quotes(str);
				if (!tokens->tokens[i].value)
					return (-1);
			}
		}
		i++;
	}
	return (0);
}


