#include "minishell.h"

static void expand_token_value(t_Token *token, t_mshell *ms)
{
    char *expanded;

    if (!token || !token->value)
        return;
    
    
    if (token->value[0] == '"' && token->value[ft_strlen(token->value) - 1] == '"')
    {
    }
    else if (token->value[0] == '\'' && token->value[ft_strlen(token->value) - 1] == '\'')
    {
    }
    
    // Выполняем расширение переменных и тильды
    expanded = expand_env_variables(token->value, ms);
    
    if (expanded)
    {
        free(token->value);
        token->value = expanded;
    }
}


void expand_tokens(t_TokenArray *tokens, t_mshell *ms)
{
    int i;

    i = 0;
    while (i < tokens->count)
    {
        if (tokens->tokens[i].type == TOKEN_WORD)
        {
            // Используем существующую функцию expand_token_value
            expand_token_value(&tokens->tokens[i], ms);
        }
        i++;
    }
}



