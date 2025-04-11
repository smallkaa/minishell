#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/minishell.h"

static const char *current_input = NULL;
static const char *input_base = NULL; 
static char *token_buffer = NULL;
static size_t token_buffer_size = 0;

// Public API function - Initialize the tokenizer
void tokenizer_init(const char *input) {
    current_input = input;
	input_base = input;
    
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
	int saw_space = 0;
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
	{
		saw_space = 1;
		current_input++;
	}
	token.needs_join = saw_space;

	if (!*current_input) {
        token.type = TOKEN_EOF;
        return token;
    }
	// $"..." должно быть отдельным токеном
    if (*current_input == '$' && *(current_input + 1) == '"')
	{
		size_t index = 0;
		token.type = TOKEN_WORD;
		token.in_double_quotes = 1;

		token_buffer[index++] = *current_input++; // $
		token_buffer[index++] = *current_input++; // "

		// Считываем строку до закрывающей "
		while (*current_input && *current_input != '"')
			token_buffer[index++] = *current_input++;

		if (*current_input == '"')
			token_buffer[index++] = *current_input++;

		token_buffer[index] = '\0';
		token.value = ft_strdup(token_buffer);
		token.needs_join = saw_space;
		return token;
	}

	if (*current_input == '"')
	{
		size_t index = 0;
		current_input++; // Пропускаем первую "
	
		token.type = TOKEN_WORD;
		token.in_double_quotes = 1;
		token.quote_style = 2;
		while (*current_input && *current_input != '"')
		{
			if (index < token_buffer_size - 1)
				token_buffer[index++] = *current_input;
			current_input++;
		}
	
		if (*current_input == '"')
			current_input++; // Пропустить закрывающую "
	
		token_buffer[index] = '\0';
		token.value = ft_strdup(token_buffer);
		token.needs_join = saw_space;
		return token;
	}
	
	if (*current_input == '\'')
	{
		size_t index = 0;
		current_input++; // пропускаем первую '

		token.type = TOKEN_WORD;
		token.in_single_quotes = 1;
		token.quote_style = 1;


		while (*current_input && *current_input != '\'')
		{
			if (index < token_buffer_size - 1)
				token_buffer[index++] = *current_input;
			current_input++;
		}

		if (*current_input == '\'')
			current_input++; // пропускаем закрывающую '

		token_buffer[index] = '\0';
		token.value = ft_strdup(token_buffer);
		token.needs_join = saw_space;
		return token;
	}


    if (*current_input == '<' && *(current_input + 1) == '<') {
        current_input += 2;
        token.type = TOKEN_HEREDOC;
        token.value = ft_strdup("<<");
        return token;
    }
    if (*current_input == '>' && *(current_input + 1) == '>') {
        current_input += 2;
        token.type = TOKEN_APPEND_OUT;
        token.value = ft_strdup(">>");
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

	if (in_single_quote)
			token.quote_style = 1;
		else if (in_double_quote)
			token.quote_style = 2;
		else
			token.quote_style = 0;
    

    
    // Проверяем на открытые кавычки
    if (in_single_quote || in_double_quote) {
        // Здесь можно добавить обработку ошибки открытых кавычек,
        // но для простоты просто закроем их
    }
    
    // Создаем копию значения
    token.value = ft_strdup(token_buffer);
    token.needs_join = saw_space;

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






