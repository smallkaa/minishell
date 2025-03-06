#include "libft.h"
#include <stdlib.h>

static char *get_env_value(const char *var, char **envp)
{
    int i;
    size_t var_len;
    char *value;

    if (!var)
        return (NULL);
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
    while (input[*i] && (ft_isalnum(input[*i]) || input[*i] == '_'))
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

char *expand_env_variables(const char *input, char **envp)
{
    char *result;
    char single_char[2];
    size_t i;

    if (!input)
        return (NULL);
    result = ft_strdup("");
    if (!result)
        return (NULL);
    i = 0;
    while (input[i])
    {
        if (input[i] == '$' && input[i + 1])
            result = process_variable(input, &i, envp, result);
        else
        {
            single_char[0] = input[i];
            single_char[1] = '\0';
            result = append_to_result(result, single_char);
            i++;
        }
        if (!result)
            return (NULL);
    }
    return (result);
}
