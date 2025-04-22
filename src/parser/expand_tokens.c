#include "minishell.h"

static void	expand_token_value(t_Token *token, t_mshell *ms)
{
	char	*expanded;
	size_t	len;
	char	*content;

	//	printf("TOKEN: value before expand = '%s'\n", token->value);
	if (!token || !token->value)
		return ;
	// Только если полностью в одинарных кавычках (не раскрывать $)
	if (token->quote_style == 1)
	{
		len = ft_strlen(token->value);
		if (len >= 2 && token->value[0] == '\'' && token->value[len
			- 1] == '\'')
			return ;
	}
	// $"" → просто обрезаем
	if (ft_strncmp(token->value, "$\"", 2) == 0)
	{
		len = ft_strlen(token->value);
		if (len >= 3 && token->value[len - 1] == '"')
		{
			content = ft_substr(token->value, 2, len - 3);
			if (content)
			{
				free(token->value);
				token->value = content;
			}
		}
	}
	if (!ft_strchr(token->value, '$') && !ft_strchr(token->value, '~')
		&& !ft_strchr(token->value, '\\'))
	{
		// Нет переменных, нечего раскрывать
		return ;
	}
	expanded = expand_env_variables(token->value, ms, token->quote_style);
	if (expanded)
	{
		if (expanded[0] == '\0' && token->quote_style == 0)
		{
			free(token->value);
			token->value = NULL;
			token->type = TOKEN_EMPTY;
			free(expanded);
			return ;
		}
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

void	expand_tokens(t_TokenArray *tokens, t_mshell *ms)
{
	int	i;

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
