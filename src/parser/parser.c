/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pvershin <pvershin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/28 13:11:02 by pvershin          #+#    #+#             */
/*   Updated: 2025/03/06 11:39:11 by pvershin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

// Helper function to print token information
static void	print_token(t_Token token)
{
	const char	*type_names[] = {
		"WORD", "PIPE", "REDIRECT_IN", "REDIRECT_OUT",
		"APPEND_OUT", "BACKGROUND", "EOF"
	};

	debug_printf("Token: { type: %s", type_names[token.type]);
	if (token.type == TOKEN_WORD)
	{
		debug_printf(", value: \"%s\"", token.value);
	}
	debug_printf(" }\n");
}

// Function to demonstrate how the tokenizer handles quotes
static void	explain_token(t_Token token)
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


t_cmd *create_command_from_tokens(t_minishell *shell, t_TokenArray *tokens)
{
    t_cmd *head = NULL;
    t_cmd *current = NULL;
    int i = 0;
    int arg_index = 0;

    while (i < tokens->count)
    {
        if (tokens->tokens[i].type == TOKEN_WORD)
        {
            // If we don't have a command yet, create one
            if (!current)
            {
                t_cmd *new_cmd = (t_cmd *)malloc(sizeof(t_cmd));
                if (!new_cmd)
                    return NULL;
                memset(new_cmd, 0, sizeof(t_cmd));

                // Allocate space for arguments
                new_cmd->argv = (char **)malloc(sizeof(char *) * (tokens->count + 1));
                if (!new_cmd->argv)
                    return NULL;

                // Set binary name
                new_cmd->binary = strdup(tokens->tokens[i].value);
                new_cmd->argv[0] = strdup(tokens->tokens[i].value);
				new_cmd->minishell = shell;
                arg_index = 1;

                // Link new command in pipeline
                if (current)
                    current->next = new_cmd;
                else
                    head = new_cmd;

                current = new_cmd;
            }
            else
            {
                // Attach as argument to the current command
                current->argv[arg_index] = strdup(tokens->tokens[i].value);
                arg_index++;
            }
        }
        else if (tokens->tokens[i].type == TOKEN_PIPE)
        {
            // Close the current argument list
            if (current && current->argv)
                current->argv[arg_index] = NULL;

            // Start new command for next pipe segment
            current = NULL;
        }
        i++;
    }

    // Ensure last command argv is NULL-terminated
    if (current && current->argv)
        current->argv[arg_index] = NULL;



	find_binary(head);

    return head;
}

/* 
t_cmd *create_command_from_tokens(t_minishell *shell, t_TokenArray *tokens)
{
    if (tokens->count < 1 || tokens->tokens[0].type != TOKEN_WORD) {
        return NULL;
    }

    // Allocate memory for the command structure
    t_cmd *cmd = (t_cmd *)malloc(sizeof(t_cmd));
    if (!cmd) {
        print_error( "Failed to allocate command structure\n");
        return NULL;
    }

    // Initialize the structure
    cmd->binary = NULL;
    cmd->in_redir = NULL;
    cmd->out_redir = NULL;
    cmd->next = NULL;
	cmd->minishell = shell;
	cmd->binary = NULL;

    // Count how many word tokens we have for argv
    int argc = 0;
    for (int i = 0; i < tokens->count; i++) {
        if (tokens->tokens[i].type == TOKEN_WORD) {
            argc++;
        } else {
            // For simplicity, stop at the first non-word token
            break;
        }
    }

    // Allocate argv array (+ 1 for NULL terminator)
    cmd->argv = (char **)malloc((argc + 1) * sizeof(char *));
    if (!cmd->argv) {
        print_error( "Failed to allocate argv array\n");
        free(cmd);
        return NULL;
    }

    // Fill argv array with copies of token values
    for (int i = 0; i < argc; i++)
	{
        cmd->argv[i] = ft_strdup(tokens->tokens[i].value);
        if (!cmd->argv[i]) {
            print_error( "Failed to duplicate argument string\n");
            // Clean up previously allocated strings
            for (int j = 0; j < i; j++) {
                free(cmd->argv[j]);
            }
            free(cmd->argv);
            free(cmd);
            return NULL;
        }
    }
    cmd->argv[argc] = NULL;  // NULL-terminate the array

	// Ilia for Pavel -------------------------
	// find_binary() looking for execution command in PATH and assign value to cmd->binary
	find_binary(cmd);
    return cmd;
} */


t_cmd	*run_parser(t_minishell *minishell, char	*input)
{
    debug_printf("\nTokenizing: %s\n\n", input);
    tokenizer_init(input);

    // First, parse all tokens and accumulate them
    t_TokenArray *tokens = token_array_init();
    t_Token token;
    t_cmd *cmd;

    do {
        token = get_next_token();
        if (token.type != TOKEN_EOF) {
            token_array_add(tokens, token);
        }
    } while (token.type != TOKEN_EOF);
    cmd = create_command_from_tokens(minishell, tokens);
    tokenizer_cleanup();

    // Now, print all collected tokens
	
    debug_printf("Found %d token(s):\n", tokens->count);
    for (int i = 0; i < tokens->count; i++) {
        debug_printf("\nToken %d:\n", i);
        print_token(tokens->tokens[i]);
        explain_token(tokens->tokens[i]);
    }
    token_array_free(tokens);
	debug_print_parsed_commands(cmd);
    return cmd;
return NULL;
}
