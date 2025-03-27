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


t_Token get_next_token(void)
{
    t_Token token;
    size_t buf_i;
    char c;
    char quote_char;
    int escaped = 0;

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

    while (*current_input)
    {
        c = *current_input;
        
        // Если находимся внутри кавычек или символ не пробел и не спец. символ
        if (quote_char || (!ft_isspace(c) && !ft_is_special_char(c)))
        {
            if (c == '\\' && !escaped && (!quote_char || quote_char == '"'))
            {
                // Экранирование следующего символа
                escaped = 1;
                current_input++;
                continue;
            }
            
            if ((c == '"' || c == '\'') && !escaped)
            {
                if (quote_char == 0)
                {
                    // Начало кавычек
                    quote_char = c;
                    if (c == '"')
                        token.in_double_quotes = 1;
                    else if (c == '\'')
                        token.in_single_quotes = 1;
                }
                else if (c == quote_char)
                {
                    // Конец кавычек того же типа
                    quote_char = 0;
                }
                // В любом случае добавляем кавычку в буфер
                append_char_to_buffer(c, &buf_i);
            }
            else
            {
                // Обычный символ или экранированный символ
                append_char_to_buffer(c, &buf_i);
            }
            
            current_input++;
            escaped = 0; // Сбрасываем флаг экранирования после обработки символа
        }
        else
        {
            // Выходим из цикла, если достигли пробела или спец. символа и не внутри кавычек
            break;
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



static char *strip_quotes(const char *str)
{
    char *result;
    size_t len = ft_strlen(str);
    size_t result_len = 0;
    int i;
    int in_quotes = 0;
    char quote_type = 0;
    int escaped = 0;
    
    result = malloc(len + 1);
    if (!result)
        return NULL;
    
    result_len = 0;
    for (i = 0; str[i]; i++)
    {
        // Обрабатываем экранирование
        if (str[i] == '\\' && !escaped && (!in_quotes || quote_type == '"'))
        {
            escaped = 1;
            result[result_len++] = str[i]; // Сохраняем символ экранирования
            continue;
        }
        
        // Обрабатываем кавычки
        if ((str[i] == '"' || str[i] == '\'') && !escaped)
        {
            if (!in_quotes)
            {
                in_quotes = 1;
                quote_type = str[i];
                // Не добавляем кавычку в результат
            }
            else if (str[i] == quote_type)
            {
                in_quotes = 0;
                quote_type = 0;
                // Не добавляем кавычку в результат
            }
            else
            {
                // Другой тип кавычек внутри кавычек - добавляем как символ
                result[result_len++] = str[i];
            }
        }
        else
        {
            // Обычный символ или экранированный символ
            result[result_len++] = str[i];
        }
        
        escaped = 0; // Сбрасываем флаг экранирования после обработки символа
    }
    
    result[result_len] = '\0';
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
    int i;
    char *str;
    char *result;

    if (!tokens || !tokens->tokens)
        return (-1);
    i = 0;
    while (i < tokens->count)
    {
        if (tokens->tokens[i].type == TOKEN_WORD && tokens->tokens[i].value)
        {
            str = tokens->tokens[i].value;
            
            // Создаем новую строку без кавычек
            result = strip_quotes(str);
            
            if (!result)
                return (-1);
                
            free(str); // Освобождаем оригинальную строку
            tokens->tokens[i].value = result; // Присваиваем новую строку без кавычек
        }
        i++;
    }
    return (0);
}




