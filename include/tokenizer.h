#ifndef TOKENIZER_H
#define TOKENIZER_H

typedef enum {
    TOKEN_WORD,       // Regular word/command/argument
    TOKEN_PIPE,       // |
    TOKEN_REDIRECT_IN,// <
    TOKEN_REDIRECT_OUT,// >
    TOKEN_APPEND_OUT, // >>
    TOKEN_BACKGROUND, // &
    TOKEN_EOF         // End of input
} TokenType;

typedef struct {
    TokenType type;
    char *value;      // For TOKEN_WORD, contains the actual text
} Token;

// Token array for storing all tokens
typedef struct {
    Token *tokens;
    int count;
    int capacity;
} TokenArray;

// Initialize the tokenizer with input string
void tokenizer_init(const char *input);

// Get the next token from the input
Token get_next_token();

// Free resources used by a token
void free_token(Token *token);

// Free tokenizer resources
void tokenizer_cleanup();

#endif
