#include "minishell.h"

static void	expand_token_value(t_Token *token, t_mshell *ms)
{
	char	*expanded;

	if (!token || !token->value)
		return ;

	// Только если полностью в одинарных кавычках (не раскрывать $)
	if (token->quote_style == 1)
	{
		size_t len = ft_strlen(token->value);
		if (len >= 2 && token->value[0] == '\'' && token->value[len - 1] == '\'')
			return ;
	}

	// $"" → просто обрезаем
	if (ft_strncmp(token->value, "$\"", 2) == 0)
	{
		size_t len = ft_strlen(token->value);
		if (len >= 3 && token->value[len - 1] == '"')
		{
			char *content = ft_substr(token->value, 2, len - 3);
			if (content)
			{
				free(token->value);
				token->value = content;
			}
		}
	}

	expanded = expand_env_variables(token->value, ms, token->quote_style);
	if (expanded)
	{
		free(token->value);
		token->value = expanded;

		if (!token->in_single_quotes)
		{
			if (ft_strcmp(expanded, "|") == 0)
				token->type = TOKEN_PIPE;
			else if (ft_strcmp(expanded, "<") == 0)
				token->type = TOKEN_REDIRECT_IN;
			else if (ft_strcmp(expanded, ">") == 0)
				token->type = TOKEN_REDIRECT_OUT;
			else if (ft_strcmp(expanded, ">>") == 0)
				token->type = TOKEN_APPEND_OUT;
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



