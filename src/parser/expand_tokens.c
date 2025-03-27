#include "minishell.h"

static void	expand_token_value(t_Token *token, t_mshell *ms)
{
	char	*expanded;

	expanded = expand_env_variables(token->value, ms);
	if (!expanded)
		return ;
	free(token->value);
	token->value = expanded;
}

void expand_tokens(t_TokenArray *tokens, t_mshell *ms)
{
    int i;

    i = 0;
    while (i < tokens->count)
    {
        if (tokens->tokens[i].type == TOKEN_WORD)
        {
            // Расширяем только если токен не в одинарных кавычках
            if (!tokens->tokens[i].in_single_quotes)
                expand_token_value(&tokens->tokens[i], ms);
        }
        i++;
    }
}


