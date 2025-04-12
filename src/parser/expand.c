#include "libft.h"
#include "minishell.h"
#include <stdlib.h>
#include <stdio.h>

char *get_exit_code(t_mshell *minishell)
{
    return (ft_itoa(minishell->exit_status));
}

char *get_env_value(const char *var, t_mshell *minishell)
{
    int i;
    size_t var_len;
    char *value;

    if (!var)
    {
		return (NULL);
	}
	if (ft_strcmp(((char *)var),"?")==0)
		return get_exit_code(minishell);
	value = ms_getenv(minishell, (char *)var);
    if (value)
        return (ft_strdup(value));
    if (!minishell->env)
        return (ft_strdup(""));
    var_len = ft_strlen(var);
    i = 0;
    while (minishell->env[i])
    {
        if (ft_strncmp(minishell->env[i], var, var_len) == 0 && minishell->env[i][var_len] == '=')
            return (ft_strdup(minishell->env[i] + var_len + 1));
        i++;
    }
    return (ft_strdup(""));
}

char *append_to_result(char *result, const char *append)
{
    char *new_result;
    
    if (!result || !append)
        return (NULL);
        
    new_result = ft_strjoin(result, append);
    free(result);
    
    return (new_result);
}








char	*handle_escape(const char *input, size_t *i, int single_q)
{
	char	single_char[2];

	(*i)++;
	if (single_q) // No escape processing inside single quotes
	{

		single_char[0] = '\\';
		single_char[1] = input[*i];
		return (ft_strdup(single_char));
	}

	if (input[*i] == 'n')
		return (ft_strdup("\n"));
	if (input[*i] == 't')
		return (ft_strdup("\t"));
	if (input[*i] == '\\')
		return (ft_strdup("\\"));
	if (input[*i] == '\"')
		return (ft_strdup("\""));
	if (input[*i] == '\'')
		return (ft_strdup("\'"));
	if (input[*i] == '$')
		return (ft_strdup("$"));
	single_char[0] = input[*i];
	single_char[1] = '\0';
	return (ft_strdup(single_char));
}



char *expand_tilde(const char *input, size_t *i, t_mshell *mshell, int single_q, int double_q)
{
	char *home;

	if (single_q || double_q)
	{
		(*i)++;
		return (ft_strdup("~"));
	}
	if (*i > 0 && input[*i - 1] != ' ' && input[*i - 1] != '\t')
	{
		(*i)++;
		return (ft_strdup("~"));
	}
	if (input[*i + 1] != '\0' && input[*i + 1] != ' ' &&
		input[*i + 1] != '\t' && input[*i + 1] != '/')
	{
		(*i)++;
		return (ft_strdup("~"));
	}
	home = ms_getenv(mshell, "HOME");
	(*i)++;
	if (!home || !*home)
		return (ft_strdup("~"));
	return (ft_strdup(home));
}





char *expand_env_variables(const char *input, t_mshell *minishell, int quote_style)
{

	char *result = ft_strdup("");
	size_t i = 0;
	int single_q = (quote_style == 1);
	int double_q = (quote_style == 2);

	if (!input || !result)
		return NULL;

	while (input[i])
	{
		if (input[i] == '\'')
			handle_single_quote(input, &i, &single_q, &result, double_q);
		else if (input[i] == '"')
			handle_double_quote(input, &i, &double_q, &result, single_q);
		else if (input[i] == '\\' && input[i + 1] == '$')
		{
			// Экранированный доллар → не expand
			append_char_to_result('$', &result);
			i += 2;
		}
		else if (input[i] == '\\')
		{
			handle_backslash(input, &i, &result, single_q);
		}
		else if (input[i] == '$')
		{
			if (!input[i + 1] ||
				!(ft_isalpha(input[i + 1]) || input[i + 1] == '_' ||
					input[i + 1] == '?' || ft_isdigit(input[i + 1])))
			{
				append_char_to_result('$', &result);
				i++;
				continue;
			}
			handle_dollar(input, &i, &result, minishell, single_q, double_q);
		}
		
		else if (input[i] == '~')
			handle_tilde(input, &i, &result, minishell, single_q, double_q);
		else
		{
			append_char_to_result(input[i], &result);
			i++;
		}
	}
	return result;
}











