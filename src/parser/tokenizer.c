#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "minishell.h"

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
