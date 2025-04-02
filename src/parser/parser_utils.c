#include "../include/minishell.h"
// Helper function to print token information
void	print_token(t_Token token)
{
	const char	*type_names[] = {
		"WORD", "PIPE", "REDIRECT_IN", "REDIRECT_OUT",
		"APPEND_OUT", "BACKGROUND", "EOF"
	};

	debug_printf("Token: { type: %s", type_names[token.type]);
	if (token.type == TOKEN_WORD)
	{
		debug_printf(", value: %s", token.value);
	}
	debug_printf(" }\n");
}

// Function to demonstrate how the tokenizer handles quotes
void	explain_token(t_Token token)
{
	const char	*str;
	int			in_single_quote;
	int			in_double_quote;
	size_t		i;

	str = token.value;
	in_single_quote = 0;
	in_double_quote = 0;
	if (token.type != TOKEN_WORD || !token.value)
		return ;
	debug_printf("  Analysis:\n");
	debug_printf("    ");
	i = 0;
	while (i < strlen(str))
	{
		if (str[i] == '\'' && !in_double_quote)
		{
			in_single_quote = !in_single_quote;
			debug_printf("%s'%s", in_single_quote ? "\033[33m" : "\033[0m", in_single_quote ? "\033[33m" : "\033[0m");
		}
		else if (str[i] == '"' && !in_single_quote)
		{
			in_double_quote = !in_double_quote;
			debug_printf("%s\"%s", in_double_quote ? "\033[36m" : "\033[0m", in_double_quote ? "\033[36m" : "\033[0m");
		}
		else if (str[i] == '$' && (in_double_quote || !in_single_quote))
		{
			debug_printf("\033[32m$\033[0m");
		}
		else
		{
			char color_code[10] = "\033[0m";
			if (in_single_quote) ft_strcpy(color_code, "\033[33m");
			else if (in_double_quote) ft_strcpy(color_code, "\033[36m");
			debug_printf("%s%c\033[0m", color_code, str[i]);
		}
		i++;
	}
	debug_printf("\n");
	debug_printf("\033[0m");
}

void debug_print_single_redirection(t_redir *redir)
{
    if (!redir)
        return;
    printf("    Redirection: ");
    if (redir->type == R_INPUT)
        printf("< ");
    else if (redir->type == R_OUTPUT)
        printf("> ");
    else if (redir->type == R_APPEND)
        printf(">> ");
    else if (redir->type == R_HEREDOC)
        printf("<< ");
    else
        printf("[Unknown Type %d] ", redir->type); // Для отладки неизвестных типов

    printf("\"%s\"", redir->filename ? redir->filename : "(NULL Filename!)");
    printf("\n");
}


void debug_print_parsed_commands(t_cmd *cmd)
{
    if (!is_debug_mode())
         return;

    int cmd_count = 1;
    t_list *redir_node; // Узел для итерации по списку редиректов
    t_redir *redir;     // Указатель на данные редиректа

    printf("\n==== Parsed Command Structure (New) ====\n");
    while (cmd)
    {
        printf("Command %d:\n", cmd_count);
        printf("  Executable: %s\n", cmd->binary ? cmd->binary : "(NULL)");

        if (cmd->argv)
        {
            printf("  Arguments: ");
            for (int i = 0; cmd->argv[i]; i++)
                printf("\"%s\" ", cmd->argv[i]);
            printf("\n");
        }
        // Печать всех редиректов из списка cmd->redirs
        printf("  Redirections (in order):\n");
        redir_node = cmd->redirs; // Получаем начало списка редиректов
        if (!redir_node)
        {
            printf("    (None)\n"); // Сообщение, если редиректов нет
        }
        while (redir_node)
        {
            // Получаем указатель на структуру t_redir из узла списка
            redir = (t_redir *)redir_node->content;
            // Печатаем информацию об этом редиректе
            debug_print_single_redirection(redir);
            // Переходим к следующему узлу
            redir_node = redir_node->next;
        }
        // Печать информации о пайпе
        if (cmd->next)
            printf("  Piped to next command ->\n");

        // Переход к следующей команде
        cmd = cmd->next;
        cmd_count++;
        // Добавляем пустую строку для лучшей читаемости между командами
        if (cmd) printf("\n");
    }
    printf("======================================\n\n");
}

// --- Функции освобождения памяти (исправленные) ---

/**
 * @brief Освобождает память, выделенную для одного редиректа (t_redir).
 * Используется как функция-делитер для ft_lstclear.
 * @param content Указатель на содержимое узла списка (t_redir *).
 */
void	free_redir(void *content)
{
	t_redir	*redir;

	if (!content)
		return ;
	redir = (t_redir *)content;
	// Освобождаем строку с именем файла/ограничителем
	free(redir->filename);
	redir->filename = NULL;
	// Освобождаем саму структуру редиректа
	free(redir); // [cite: 1]
}

/**
 * @brief Освобождает память, выделенную для одной команды (t_cmd).
 * Включая ее аргументы, путь к бинарнику и список редиректов.
 * @param cmd Указатель на структуру команды для освобождения.
 */
void	free_command(t_cmd *cmd)
{
	int	i;

	if (!cmd)
		return ;
	// Освобождаем путь к бинарнику (если он был скопирован)
	free(cmd->binary); // [cite: 1]
	cmd->binary = NULL;
	// Освобождаем массив аргументов (argv)
	if (cmd->argv)
	{
		i = 0;
		while (cmd->argv[i])
		{
			free(cmd->argv[i]); // Освобождаем каждую строку аргумента [cite: 1]
			cmd->argv[i] = NULL;
			i++;
		}
		free(cmd->argv); // Освобождаем сам массив указателей [cite: 1]
		cmd->argv = NULL;
	}
	// *** Строки для argc и argv_capacity удалены ***
	// Освобождаем список редиректов, используя ft_lstclear и free_redir
	if (cmd->redirs) // [cite: 1]
	{
		ft_lstclear(&cmd->redirs, free_redir); // [cite: 1]
		cmd->redirs = NULL; // Убедимся, что указатель обнулен
	}
	// Не освобождаем cmd->minishell, так как он общий
	// Освобождаем саму структуру команды
	free(cmd); // [cite: 1]
}

/**
 * @brief Освобождает всю цепочку команд (связанный список t_cmd).
 * @param head Указатель на УКАЗАТЕЛЬ на голову списка команд.
 */
void	free_command_list(t_cmd **head)
{
	t_cmd	*current;
	t_cmd	*next;

	if (!head || !*head)
		return ;
	current = *head;
	while (current != NULL)
	{
		next = current->next; // Сохраняем указатель на следующую команду [cite: 1]
		free_command(current); // Освобождаем текущую команду
		current = next; // Переходим к следующей
	}
	*head = NULL; // Устанавливаем голову списка в NULL после очистки
}
