#include "libft.h"
#include <stdlib.h>
#include <stdio.h>

static char *get_exit_code(char **envp)
{
    int code;

	(void)envp;
	code = 0;
    return (ft_itoa(code));
}

static char *get_env_value(const char *var, char **envp)
{
    int i;
    size_t var_len;
    char *value;

    if (!var)
    {
		return (NULL);
	}
	if (ft_strcmp(((char *)var),"?")==0)
		return get_exit_code(envp);
    value = getenv(var);
    if (value)
        return (ft_strdup(value));
    if (!envp)
        return (ft_strdup(""));
    var_len = ft_strlen(var);
    i = 0;
    while (envp[i])
    {
        if (ft_strncmp(envp[i], var, var_len) == 0 && envp[i][var_len] == '=')
            return (ft_strdup(envp[i] + var_len + 1));
        i++;
    }
    return (ft_strdup(""));
}

static char *append_to_result(char *result, char *append)
{
    char *temp;

    if (!append)
        return (result);
    temp = ft_strjoin(result, append);
    free(result);
    if (!temp)
        return (NULL);
    return (temp);
}

static char *process_variable(const char *input, size_t *i, char **envp, char *result)
{
    char *substr;
    char *var_value;

    (*i)++;
    size_t start = *i;
    while (input[*i] && (ft_isalnum(input[*i]) || input[*i] == '_'  || input[*i] == '?')) //TODO SHould we process ? in other cases then $?
        (*i)++;
    substr = ft_substr(input, start, *i - start);
    if (!substr)
        return (free(result), NULL);
    var_value = get_env_value(substr, envp);
    free(substr);
    if (!var_value)
        return (free(result), NULL);
    result = append_to_result(result, var_value);
    free(var_value);
    return (result);
}


static int	handle_quotes(char c, int *single_q, int *double_q)
{
	if (c == '\'' && !(*double_q))
	{
		*single_q = !(*single_q);
		return (1);
	}
	if (c == '\"' && !(*single_q))
	{
		*double_q = !(*double_q);
		return (1);
	}
	return (0);
}

static char	*handle_escape(const char *input, size_t *i, int single_q)
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
	single_char[0] = input[*i];
	single_char[1] = '\0';
	return (ft_strdup(single_char));
}

static char	*process_char(const char *input, size_t *i, int single_q)
{
	char	single_char[2];

	single_char[0] = input[*i];
	single_char[1] = '\0';
	if (single_q && input[*i] == '$') // Treat `$` as a literal in single quotes
		return (ft_strdup(single_char));
	return (ft_strdup(single_char));
}

char	*expand_env_variables(const char *input, char **envp)
{
	char	*result;
	size_t	i;
	int		single_q;
	int		double_q;
	char	*append;

	if (!input)
		return (NULL);
	result = ft_strdup("");
	if (!result)
		return (NULL);
	i = 0;
	single_q = 0;
	double_q = 0;
	while (input[i])
	{
		if (handle_quotes(input[i], &single_q, &double_q))
			append = ft_strdup(""); // Ignore quotes in result
		else if (input[i] == '\\' && input[i + 1])
			append = handle_escape(input, &i, single_q);
		else if (input[i] == '$' && input[i + 1] && !single_q)
			append = process_variable(input, &i, envp, ft_strdup(""));
		else
			append = process_char(input, &i, single_q);
		if (!append)
			return (free(result), NULL);
		result = append_to_result(result, append);
		free(append);
		i++;
	}
	return (result);
}
