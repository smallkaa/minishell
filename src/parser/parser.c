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
	int i = 0;
	int count = 0;

	while (i < tokens->count)
	{
		if (tokens->tokens[i].type == TOKEN_EMPTY)
		{
			i++;
			continue;
		}
		if (tokens->tokens[i].type != TOKEN_WORD)
		{
			count++;
			i++;
			continue;
		}

		// Начало группы склеиваемых токенов
		count++; // мы создадим хотя бы 1 токен
		i++;

		while (i < tokens->count &&
			   tokens->tokens[i].type == TOKEN_WORD &&
			   tokens->tokens[i].needs_join == 0)
		{
			i++; // склеивается, не требует нового токена
		}
	}
	return count;
}


static void	fill_new_tokens(t_TokenArray *new_tokens, t_TokenArray *old_tokens)
{
	//printf("DEBUG: fill_new_tokens — count=%d\n", old_tokens->count);

	int		i = 0;
	int		j = 0;

	while (i < old_tokens->count)
	{
		t_Token *tok = &old_tokens->tokens[i];
		//printf("DEBUG: i=%d type=%d value='%s'\n", i, tok->type,
//			tok->value ? tok->value : "(null)");
		if (tok->type == TOKEN_EMPTY)
		{
			i++;
			continue;
		}
		if (tok->type == TOKEN_WORD && tok->value == NULL)
		{
			i++;
			continue;
		}
		if (tok->type != TOKEN_WORD)
		{
			new_tokens->tokens[j++] = *tok;
			i++;
			continue;
		}

		// Начинаем группу — даже если needs_join == 1
		char *grouped = ft_strdup(tok->value);
		bool single_q = tok->in_single_quotes;
		bool double_q = tok->in_double_quotes;
		int curr_qstyle = tok->quote_style;

		i++;

		// Склеиваем только последующие токены с needs_join == 0
		while (i < old_tokens->count &&
			   old_tokens->tokens[i].type == TOKEN_WORD && old_tokens->tokens[i].type != TOKEN_EMPTY &&
			   old_tokens->tokens[i].needs_join == 0)
		{
			//printf("  INNER: i=%d type=%d value='%s' needs_join=%d\n", i,
			//	old_tokens->tokens[i].type,
			//	old_tokens->tokens[i].value ? old_tokens->tokens[i].value : "(null)",
			//	old_tokens->tokens[i].needs_join);
			if (old_tokens->tokens[i].type == TOKEN_EMPTY)
			{
				i++;
				continue;
			}
			char *tmp = ft_strjoin(grouped, old_tokens->tokens[i].value);
			free(grouped);
			grouped = tmp;

			if (old_tokens->tokens[i].in_single_quotes)
				single_q = true;
			if (old_tokens->tokens[i].in_double_quotes)
				double_q = true;
			i++;
		}

		t_Token new_token = {0};
		new_token.type = TOKEN_WORD;
		new_token.value = grouped;
		new_token.in_single_quotes = single_q;
		new_token.in_double_quotes = double_q;
		new_token.quote_style = curr_qstyle;
		new_tokens->tokens[j++] = new_token;
	}
	new_tokens->count = j;
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
	t_list	*cmd_list = NULL;
	t_cmd	*current = NULL;
	int		i = 0;

	while (tokens && i < tokens->count)
	{
		t_TokenType type = tokens->tokens[i].type;

		// PIPE: начинаем новую команду
		if (type == TOKEN_PIPE)
		{
			current = NULL;
			i++;
			continue;
		}

		// если это redir — создаём команду, если её ещё нет
		if (is_input_redir(type) || is_output_redir(type))
		{
			if (!is_valid_redir_target(tokens, i))
			{
				print_error("syntax error near unexpected token `newline'\n");
				free_cmd_list(&cmd_list);
				return (NULL);
			}
			if (!current)
			{
				current = create_empty_command(shell);
				if (!current)
					return (free_cmd_list(&cmd_list), NULL);
				ft_lstadd_back(&cmd_list, ft_lstnew(current));
			}
			if (is_input_redir(type))
			{
				if (handle_input_redir(shell, &cmd_list, &current, &tokens->tokens[i]) < 0)
					return (free_cmd_list(&cmd_list), NULL);
			}
			else if (is_output_redir(type))
			{
				if (handle_output_redir(shell, &cmd_list, &current, &tokens->tokens[i]) < 0)
					return (free_cmd_list(&cmd_list), NULL);
			}
			i += 2; // редирект + его аргумент
			continue;
		}

		// если это слово (команда или аргумент)
		if (type == TOKEN_WORD)
		{
			if (!current)
			{
				current = create_empty_command(shell);
				if (!current)
					return (free_cmd_list(&cmd_list), NULL);
				ft_lstadd_back(&cmd_list, ft_lstnew(current));
			}
			handle_word_token(shell, &cmd_list, &current, tokens->tokens[i].value);
			i++;
			continue;
		}

		// игнорируем любые другие токены
		i++;
	}

	// финализируем argv
	t_cmd *head = finalize_cmd_list(&cmd_list);
	t_cmd *cmd_ptr = head;
	while (cmd_ptr)
	{
		// гарантируем null-терминацию
		if (cmd_ptr->argv)
		{
			int j = 0;
			while (j < MAX_ARGS && cmd_ptr->argv[j])
				j++;
			while (j < MAX_ARGS)
				cmd_ptr->argv[j++] = NULL;		
		}
		cmd_ptr->binary = find_binary(cmd_ptr);
		cmd_ptr = cmd_ptr->next;
	}
	return head;
}

t_cmd	*run_parser(t_mshell *minishell, char *input)
{
	t_Tokenizer		*tokenizer;
	t_TokenArray	*tokens;
	t_Token			token;
	t_cmd			*cmd;
	int				i;
	const char	*err_msg = NULL;
	int			err_code = 0;

	debug_printf("\nTokenizing: %s\n\n", input);
	tokenizer = tokenizer_create(input);
	if (!tokenizer)
		return (NULL);

	tokens = token_array_init();
	do {
		token = get_next_token(tokenizer);
		if (token.type != TOKEN_EOF)
			token_array_add(tokens, token);
	} while (token.type != TOKEN_EOF);

	expand_tokens(tokens, minishell);

	if (tokens->count == 1 && tokens->tokens[0].type == TOKEN_WORD)
{
	err_code = known_unsupported_cmd(tokens->tokens[0].value, &err_msg);
	if (err_code)
	{
		if (err_msg)
			print_error((char *)err_msg);
		else
			print_error("minishell: unsupported syntax\n");
		token_array_free(tokens);
		tokenizer_destroy(tokenizer);
		minishell->exit_status = err_code;
		return (NULL);
	}
}
	
	group_word_tokens(tokens); // TODO: error handling
	strip_words(tokens);

	cmd = create_command_from_tokens(minishell, tokens);

	tokenizer_destroy(tokenizer);

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

	return (cmd);
}

