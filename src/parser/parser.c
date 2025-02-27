#include "../include/minishell.h"
// Initialize a token array
static TokenArray* token_array_init() {
    TokenArray *array = (TokenArray*)malloc(sizeof(TokenArray));
    if (!array) {
        fprintf(stderr, "Failed to allocate token array\n");
        exit(1);
    }
    
    array->capacity = 16;
    array->tokens = (Token*)malloc(sizeof(Token) * array->capacity);
    if (!array->tokens) {
        fprintf(stderr, "Failed to allocate tokens buffer\n");
        free(array);
        exit(1);
    }
    
    array->count = 0;
    return array;
}

// Add a token to the array
static void token_array_add(TokenArray *array, Token token) {
    // Expand array if needed
    if (array->count >= array->capacity) {
        array->capacity *= 2;
        array->tokens = (Token*)realloc(array->tokens, sizeof(Token) * array->capacity);
        if (!array->tokens) {
            fprintf(stderr, "Failed to reallocate tokens buffer\n");
            exit(1);
        }
    }
    
    array->tokens[array->count++] = token;
}

// Free token array resources
static void token_array_free(TokenArray *array) {
    if (!array) return;
    
    for (int i = 0; i < array->count; i++) {
        free_token(&array->tokens[i]);
    }
    
    free(array->tokens);
    free(array);
}


// Helper function to print token information
static void print_token(Token token) {
    const char *type_names[] = {
        "WORD", "PIPE", "REDIRECT_IN", "REDIRECT_OUT",
        "APPEND_OUT", "BACKGROUND", "EOF"
    };
    
    printf("Token: { type: %s", type_names[token.type]);
    
    if (token.type == TOKEN_WORD) {
        printf(", value: \"%s\"", token.value);
    }
    
    printf(" }\n");
}

// Function to demonstrate how the tokenizer handles quotes
static void explain_token(Token token) {
    if (token.type != TOKEN_WORD || !token.value)
        return;
    
    printf("  Analysis:\n");
    const char *str = token.value;
    int in_single_quote = 0;
    int in_double_quote = 0;
    
    printf("    ");
    for (size_t i = 0; i < strlen(str); i++) {
        if (str[i] == '\'' && !in_double_quote) {
            in_single_quote = !in_single_quote;
            printf("%s'%s", in_single_quote ? "\033[33m" : "\033[0m", in_single_quote ? "\033[33m" : "\033[0m");
        } 
        else if (str[i] == '"' && !in_single_quote) {
            in_double_quote = !in_double_quote;
            printf("%s\"%s", in_double_quote ? "\033[36m" : "\033[0m", in_double_quote ? "\033[36m" : "\033[0m");
        } 
        else if (str[i] == '$' && (in_double_quote || !in_single_quote)) {
            printf("\033[32m$\033[0m");
        } 
        else {
            char color_code[10] = "\033[0m";
            if (in_single_quote) strcpy(color_code, "\033[33m");
            else if (in_double_quote) strcpy(color_code, "\033[36m");
            printf("%s%c\033[0m", color_code, str[i]);
        }
    }
    printf("\n");
    
    // Reset color in case something went wrong
    printf("\033[0m");
}

t_cmd	* run_parser(char	*input)
{
    printf("\nTokenizing: %s\n\n", input);
    
    tokenizer_init(input);
    
    // First, parse all tokens and accumulate them
    TokenArray *tokens = token_array_init();
    Token token;
    
    do {
        token = get_next_token();
        if (token.type != TOKEN_EOF) {
            token_array_add(tokens, token);
        }
    } while (token.type != TOKEN_EOF);
    
    tokenizer_cleanup();
    
    // Now, print all collected tokens
    printf("Found %d token(s):\n", tokens->count);
    for (int i = 0; i < tokens->count; i++) {
        printf("\nToken %d:\n", i);
        print_token(tokens->tokens[i]);
        explain_token(tokens->tokens[i]);
    }
    
 

if (tokens->count == 1 && tokens->tokens[0].type == TOKEN_WORD && ft_strcmp(tokens->tokens[0].value, "exit") == 0)
{
	printf("EXIT!\n");
   // Free the token array
    token_array_free(tokens);

	return NULL;
    //return handle_exit_command();
}
/*	if(0 == ft_strcmp(input,"exit"))
	{
		t_cmd cmd1 =
		{
			.argv = (char *[]){"exit", "", NULL},
			.binary = NULL,
			.in_redir = NULL,
			.out_redir = NULL,
			.next = NULL
		};
		return &cmd1;
	}
	return NULL;*/
return NULL;
}