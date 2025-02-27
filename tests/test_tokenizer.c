#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokenizer.h"

// Token array for storing all tokens
typedef struct {
    Token *tokens;
    int count;
    int capacity;
} TokenArray;

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

// Process input string, collect all tokens, then print them
static void process_input(const char *input) {
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
    printf("Found %d tokens:\n", tokens->count);
    for (int i = 0; i < tokens->count; i++) {
        printf("\nToken %d:\n", i + 1);
        print_token(tokens->tokens[i]);
        explain_token(tokens->tokens[i]);
    }
    
    // Free the token array
    token_array_free(tokens);
}

static void run_test_case(const char *input) {
    printf("\n-------------------------------------------\n");
    printf("TEST CASE: %s\n", input);
    printf("-------------------------------------------\n");
    
    process_input(input);
    printf("\n");
}

static void run_wild_test_cases() {
    // Test Case 1: Mixed Quotes with Special Characters
    run_test_case("grep 'text with \"quotes\" inside' | sed \"s/$USER/'$HOME'/g\" > output.txt");
    
    // Test Case 2: Unclosed Quotes and Edge Cases
    run_test_case("echo \"unclosed quote | grep 'closed quote' && echo $TERM");
    
    // Test Case 3: Complex Nesting and Spaces
    run_test_case("find . -name \"file with 'single' quotes and $VARS\" -exec echo '\"$PATH\"' \\;");
}

int main() {
    char input[1024];
    
    printf("Enter a shell command (or press Enter to run test cases): ");
    if (fgets(input, sizeof(input), stdin) == NULL) {
        fprintf(stderr, "Error reading input\n");
        return 1;
    }
    
    // Remove trailing newline if present
    size_t len = strlen(input);
    if (len > 0 && input[len-1] == '\n') {
        input[len-1] = '\0';
    }
    
    // Run test cases if empty input
    if (len <= 1) {
        run_wild_test_cases();
        return 0;
    }
    
    // Process the user input
    process_input(input);
    
    return 0;
}
