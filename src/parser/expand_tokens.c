#include "minishell.h"

static void	expand_token_value(t_Token *token, t_mshell *ms)
{
	char	*expanded;

	if (!token || !token->value)
		return;
	if (ft_strncmp(token->value, "$\"", 2) == 0)
	{
		size_t len = ft_strlen(token->value);
		// Найти первую и последнюю кавычку
		if (len >= 3 && token->value[len - 1] == '"')
		{
			char *content = ft_substr(token->value, 2, len - 3); // без $ и кавычек
			if (content)
			{
				free(token->value);
				token->value = content;
			}
		}
	}
	expanded = expand_env_variables(token->value, ms);
	if (expanded)
	{
		free(token->value);
		token->value = expanded;

		// Обновление типа токена на основе его значения
		if (!token->in_single_quotes) // Только если не в одинарных кавычках
		{
			if (ft_strcmp(expanded, "|") == 0)
				token->type = TOKEN_PIPE;
			else if (ft_strcmp(expanded, "<") == 0)
				token->type = TOKEN_REDIRECT_IN;
			else if (ft_strcmp(expanded, ">") == 0)
				token->type = TOKEN_REDIRECT_OUT;
			else if (ft_strcmp(expanded, ">>") == 0)
				token->type = TOKEN_APPEND_OUT;
			// можно добавить больше проверок, если нужно
		}
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



