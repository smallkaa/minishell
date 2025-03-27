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
    
    // Выделяем буфер размером с входную строку + 1 символ для нулевого символа
    token_buffer_size = ft_strlen(input) + 1;
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




// Public API function - Get the next token from the input


t_Token get_next_token(void)
{
    t_Token token = {0};
    token.type = TOKEN_WORD;
    
    // Проверка инициализации буфера
    if (!token_buffer || token_buffer_size == 0) {
        write(2, "token_buffer not initialized\n", 30);
        exit(1);
    }
    
    // Если достигнут конец строки, возвращаем TOKEN_EOF
    if (!*current_input) {
        token.type = TOKEN_EOF;
        return token;
    }
    
    // Пропустить пробелы
    while (*current_input && (*current_input == ' ' || *current_input == '\t'))
        current_input++;
    
    if (!*current_input) {
        token.type = TOKEN_EOF;
        return token;
    }
    
    // Проверяем спецсимволы (|, <, >, &) - они всегда отдельные токены
    if (ft_is_special_char(*current_input)) {
        char special_char = *current_input++;
        token.value = malloc(2);
        token.value[0] = special_char;
        token.value[1] = '\0';
        return token;
    }
    
    // Начинаем построение токена
    size_t index = 0;
    int in_single_quote = 0;
    int in_double_quote = 0;
    
    while (*current_input) {
        char c = *current_input;
        
        // Если нашли пробел вне кавычек - конец токена
        if ((c == ' ' || c == '\t') && !in_single_quote && !in_double_quote)
            break;
            
        // Если нашли спецсимвол вне кавычек - конец токена (и не двигаем указатель)
        if (ft_is_special_char(c) && !in_single_quote && !in_double_quote)
            break;
            
        // Обработка кавычек
        if (c == '\'' && !in_double_quote) {
            in_single_quote = !in_single_quote;
            token.in_single_quotes = 1;
        }
        else if (c == '"' && !in_single_quote) {
            in_double_quote = !in_double_quote;
            token.in_double_quotes = 1;
        }
        
        // Добавляем символ в токен
        if (index < token_buffer_size - 1) {
            token_buffer[index++] = c;
        }
        
        current_input++;
    }
    
    // Завершаем строку
    token_buffer[index] = '\0';
    
    // Проверяем на открытые кавычки
    if (in_single_quote || in_double_quote) {
        // Здесь можно добавить обработку ошибки открытых кавычек,
        // но для простоты просто закроем их
    }
    
    // Создаем копию значения
    token.value = ft_strdup(token_buffer);
    
    return token;
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
    int in_single_quotes = 0;
    int in_double_quotes = 0;
    
    result = malloc(len + 1);
    if (!result)
        return NULL;
    
    for (i = 0; str[i]; i++)
    {
        if (str[i] == '\\' && (str[i+1] == '\'' || str[i+1] == '"'))
        {
            // Экранированная кавычка - сохраняем её
            result[result_len++] = str[i+1];
            i++; // Пропускаем обратный слэш
        }
        else if (str[i] == '\'' && !in_double_quotes)
        {
            // Одинарные кавычки вне двойных
            in_single_quotes = !in_single_quotes;
        }
        else if (str[i] == '"' && !in_single_quotes)
        {
            // Двойные кавычки вне одинарных
            in_double_quotes = !in_double_quotes;
        }
        else if (str[i] == '\'' && in_double_quotes)
        {
            // Одинарные кавычки внутри двойных - сохраняем
            result[result_len++] = '\'';
        }
        else if (str[i] == '"' && in_single_quotes)
        {
            // Двойные кавычки внутри одинарных - сохраняем
            result[result_len++] = '"';
        }
        else
        {
            // Все остальные символы
            result[result_len++] = str[i];
        }
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





