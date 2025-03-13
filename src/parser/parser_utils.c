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

void debug_print_redirection(t_redir *redir, char *type)
{
    if (!redir)
        return;
    printf("  %s Redirection: ", type);
    if (redir->type == R_INPUT)
        printf("< ");
    else if (redir->type == R_OUTPUT)
        printf("> ");
    else if (redir->type == R_APPEND)
        printf(">> ");
    else if (redir->type == R_HEREDOC)
        printf("<< ");
    printf("\"%s\"\n", redir->filename);
}

void debug_print_parsed_commands(t_cmd *cmd)
{
	if(!is_debug_mode())
		return;
    int cmd_count = 1;

    printf("\n==== Parsed Command Structure ====\n");
    while (cmd)
    {
        printf("Command %d:\n", cmd_count);
        printf("  Executable: %s\n", cmd->binary ? cmd->binary : "(NULL)");

        // Print arguments
        if (cmd->argv)
        {
            printf("  Arguments: ");
            for (int i = 0; cmd->argv[i]; i++)
                printf("\"%s\" ", cmd->argv[i]);
            printf("\n");
        }

        // Print input and output redirections
        debug_print_redirection(cmd->in_redir, "Input");
        debug_print_redirection(cmd->out_redir, "Output");

        // Check if there's a next command in a pipeline
        if (cmd->next)
            printf("  Piped to next command ->\n");

        cmd = cmd->next;
        cmd_count++;
    }
    printf("==================================\n\n");
}