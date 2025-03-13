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
static void append_char_to_buffer(char c, size_t *buffer_index) {
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

t_Token get_next_token() {
    t_Token token = {TOKEN_EOF, NULL};
    
    skip_whitespace();
    
    // Check for end of input
    if (*current_input == '\0') {
        token.type = TOKEN_EOF;
        return token;
    }
    
    // Handle special characters (but not \ or ;)
    if (ft_is_special_char(*current_input)) {
        switch (*current_input) {
            case '|':
                token.type = TOKEN_PIPE;
                current_input++;
                break;
            case '<':
                token.type = TOKEN_REDIRECT_IN;
                current_input++;
                break;
            case '>':
                token.type = TOKEN_REDIRECT_OUT;
                current_input++;
                if (*current_input == '>') {
                    token.type = TOKEN_APPEND_OUT;
                    current_input++;
                }
                break;
            case '&':
                token.type = TOKEN_BACKGROUND;
                current_input++;
                break;
        }
        return token;
    }
    
    // Handle words (including quoted strings)
    size_t buffer_index = 0;
    
    while (*current_input) {
        // If not in quotes, whitespace or special chars end the token
        if (ft_isspace((unsigned char)*current_input) || ft_is_special_char(*current_input)) {
            break;
        }
        
        // Handle single quotes - ignore all special characters
        if (*current_input == '\'') {
            // Add the opening quote to the token
            append_char_to_buffer(*current_input, &buffer_index);
            current_input++;
            
            // Process everything inside quotes literally
            while (*current_input && *current_input != '\'') {
                append_char_to_buffer(*current_input, &buffer_index);
                current_input++;
            }
            
            // Add closing quote if present
            if (*current_input == '\'') {
                append_char_to_buffer(*current_input, &buffer_index);
                current_input++;
            }
            // Note: If no closing quote, just continue processing
            continue;
        }
        
        // Handle double quotes - ignore all special characters except $
        if (*current_input == '"') {
            // Add the opening quote to the token
            append_char_to_buffer(*current_input, &buffer_index);
            current_input++;
            
            while (*current_input && *current_input != '"') {
                // If $ found inside double quotes, preserve its special meaning
                append_char_to_buffer(*current_input, &buffer_index);
                current_input++;
            }
            
            // Add closing quote if present
            if (*current_input == '"') {
                append_char_to_buffer(*current_input, &buffer_index);
                current_input++;
            }
            // Note: If no closing quote, just continue processing
            continue;
        }
        
        // Regular character
        append_char_to_buffer(*current_input, &buffer_index);
        current_input++;
    }
    
    // Null-terminate the buffer
    token_buffer[buffer_index] = '\0';
    
    // Only create a WORD token if we have content
    if (buffer_index > 0) {
        token.type = TOKEN_WORD;
        token.value = strdup(token_buffer);
        if (!token.value) {
            print_error("Failed to allocate token value\n");
            exit(1);
        }
    }
    
    return token;
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
int strip_words(t_TokenArray *tokens)
{
    int i;
    size_t j;
    size_t k;
    char *new_value;
    char *str;
    size_t len;

    if (!tokens || !tokens->tokens)
        return (-1);
    
    i = 0;
    while (i < tokens->count)
    {
        if (tokens->tokens[i].type == TOKEN_WORD && tokens->tokens[i].value)
        {
            str = tokens->tokens[i].value;
            len = ft_strlen(str);
            
            // First check if it's fully enclosed in quotes for efficiency
            if (is_enclosed_in_quotes(str))
            {
                tokens->tokens[i].value = remove_enclosing_quotes(str);
                if (!tokens->tokens[i].value)
                    return (-1);
            }
            // Handle embedded quotes
            else if (ft_strchr(str, '\'') || ft_strchr(str, '"'))
            {
                new_value = malloc(len + 1);
                if (!new_value)
                    return (-1);
                
                j = 0; // input index
                k = 0; // output index
                
                while (j < len)
                {
                    // Skip quotes
                    if (str[j] == '\'' || str[j] == '"')
                        j++;
                    else
                        new_value[k++] = str[j++];
                }
                new_value[k] = '\0';
                
                free(tokens->tokens[i].value);
                tokens->tokens[i].value = new_value;
            }
        }
        i++;
    }
    return (0);
}


