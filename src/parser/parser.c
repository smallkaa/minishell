#include "../include/minishell.h"
// Initialize a token array
static t_TokenArray	*token_array_init(void)
{
	t_TokenArray	*array;

	array = (t_TokenArray *)malloc(sizeof(t_TokenArray));
	if (!array)
	{
		print_error("Failed to allocate token array\n");
		exit(1);
	}
	array->capacity = 16;
	array->tokens = (t_Token *)malloc(sizeof(t_Token) * array->capacity);
	if (!array->tokens)
	{
		print_error("Failed to allocate tokens buffer\n");
		free(array);
		exit(1);
	}
	array->count = 0;
	return (array);
}

// Add a token to the array
static void	token_array_add(t_TokenArray *array, t_Token token)
{
	if (array->count >= array->capacity)
	{
		array->capacity *= 2;
		array->tokens = (t_Token *)realloc(array->tokens, \
		sizeof(t_Token) * array->capacity);
		if (!array->tokens)
		{
			print_error("Failed to reallocate tokens buffer\n");
			exit(1);
		}
	}
	array->tokens[array->count++] = token;
}

// Free token array resources
static void	token_array_free(t_TokenArray *array)
{
	int	i;

	if (!array)
	{
		return ;
	}
	i = 0;
	while (i < array->count)
	{
		free_token(&array->tokens[i]);
		i++;
	}
	free(array->tokens);
	free(array);
}
//--------------------------------------

static int	count_new_tokens(t_TokenArray *tokens)
{
	int		i;
	int		count;

	i = 0;
	count = 0;
	while (i < tokens->count)
	{
		if (tokens->tokens[i].type == TOKEN_WORD)
		{
			// Первый токен считаем отдельно
			count++;
			i++;

			// Проверяем наличие следующих токенов-слов
			if (i < tokens->count && tokens->tokens[i].type == TOKEN_WORD)
			{
				// Остальные считаем как один объединенный токен
				count++;
				while (i < tokens->count && tokens->tokens[i].type == TOKEN_WORD)
					i++;
			}
		}
		else
		{
			count++;
			i++;
		}
	}
	return (count);
}

static void	fill_new_tokens(t_TokenArray *new_tokens, t_TokenArray *old_tokens)
{
	int		i;
	int		j;
	char	*grouped_str;

	i = 0;
	j = 0;
	while (i < old_tokens->count)
	{
		if (old_tokens->tokens[i].type == TOKEN_WORD)
		{
			// Добавляем первый токен как есть
			new_tokens->tokens[j++] = old_tokens->tokens[i++];

			// Проверяем, есть ли следующие токены-слова для объединения
			if (i < old_tokens->count && old_tokens->tokens[i].type == TOKEN_WORD)
			{
				// Инициализируем объединенную строку значением второго токена
				grouped_str = ft_strdup(old_tokens->tokens[i++].value);

				// Добавляем все последующие токены к объединенной строке
				while (i < old_tokens->count && old_tokens->tokens[i].type == TOKEN_WORD)
				{
					char *temp = ft_strjoin(grouped_str, " ");
					free(grouped_str);
					if (!temp)
						return; // Здесь нужна обработка ошибки

					grouped_str = ft_strjoin(temp, old_tokens->tokens[i].value);
					free(temp);
					if (!grouped_str)
						return; // Здесь нужна обработка ошибки

					i++;
				}

				// Создаем новый токен для объединенной строки
				new_tokens->tokens[j].type = TOKEN_WORD;
				new_tokens->tokens[j].value = grouped_str;
				j++;
			}
		}
		else
		{
			new_tokens->tokens[j++] = old_tokens->tokens[i++];
		}
	}
	new_tokens->count = j; // Обновляем реальное количество токенов
}


int	group_word_tokens(t_TokenArray *tokens)
{
	t_TokenArray	new_tokens;
	int				new_count;

	if (!tokens || tokens->count <= 1)
		return (0);

	new_count = count_new_tokens(tokens);
	new_tokens.tokens = malloc(sizeof(t_Token) * new_count);
	if (!new_tokens.tokens)
		return (-1);

	new_tokens.capacity = new_count;
	fill_new_tokens(&new_tokens, tokens);

	// Освобождаем старые токены, кроме тех, которые мы скопировали в новый массив
	free(tokens->tokens);

	// Устанавливаем новый массив
	tokens->tokens = new_tokens.tokens;
	tokens->count = new_tokens.count;
	tokens->capacity = new_tokens.capacity;

	return (0);
}




t_cmd	*create_command_from_tokens(t_mshell *shell, t_TokenArray *tokens)
{
	t_list	*cmd_list;
	t_cmd	*current;
	t_cmd	*head;
	t_cmd	*cmd_ptr;
	int		i;

	cmd_list = NULL;
	current = NULL;
	i = 0;
	while (tokens && i < tokens->count)
	{
		if (tokens->tokens[i].type == TOKEN_WORD)
			handle_word_token(shell, &cmd_list, &current,
				tokens->tokens[i].value);
		else if (tokens->tokens[i].type == TOKEN_PIPE)
			current = NULL;
		else if (is_input_redir(tokens->tokens[i].type))
		{
			if (handle_input_redir(shell, &cmd_list, &current,
					&tokens->tokens[i]) < 0)
				return (free_cmd_list(&cmd_list), NULL);
			i++;
		}
		else if (is_output_redir(tokens->tokens[i].type))
		{
			if (handle_output_redir(shell, &cmd_list, &current,
					&tokens->tokens[i]) < 0)
				return (free_cmd_list(&cmd_list), NULL);
			i++;
		}
		i++;
	}
	if (current && current->argv)
	{
		int j = 0;
		while (current->argv[j])
			j++;
		current->argv[j] = NULL;
	}
	head = finalize_cmd_list(&cmd_list);
	cmd_ptr = head;
	while (cmd_ptr)
	{
		cmd_ptr->binary = find_binary(cmd_ptr);
		cmd_ptr = cmd_ptr->next;
	}
	return (head);
}


t_cmd *run_parser(t_mshell *minishell, char *input)
{
    //char *expanded_input;
    t_TokenArray *tokens;
    t_Token token;
    t_cmd *cmd;
    int i;

//    debug_printf("\nExpanding: %s\n\n", input);

//    expanded_input = expand_env_variables(input, minishell);
//    if (!expanded_input)
//        return (NULL);

    debug_printf("\nTokenizing: %s\n\n", input);
    tokenizer_init(input);

    tokens = token_array_init();
    do {
        token = get_next_token();
        if (token.type != TOKEN_EOF)
            token_array_add(tokens, token);
    } while (token.type != TOKEN_EOF);
	// Ilia: close for tests
    //group_word_tokens(tokens);//TODO: error handling

 	expand_tokens(tokens, minishell);
	strip_words(tokens);

    cmd = create_command_from_tokens(minishell, tokens);
    //free(expanded_input);

    tokenizer_cleanup();
    debug_printf("Found %d token(s):\n", tokens->count);
    i = 0;
    while (i < tokens->count)
    {
        debug_printf("\nToken %d:\n", i);
        print_token(tokens->tokens[i]);
        explain_token(tokens->tokens[i]);
        i++;
    }
    token_array_free(tokens);
    debug_print_parsed_commands(cmd);

    // //debug
    // if (!cmd)
    // {
    //     print_error("[TEST]: run_parser(), No input from the user side / empty prompt\n");
    //     print_error("[TEST]: It's no error, just continue waiting for the next command\n");
    // }
    return (cmd);
}
