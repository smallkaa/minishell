#include "libft.h"
#include "minishell.h"
#include <stdlib.h>
#include <stdio.h>

static char *get_exit_code(t_mshell *minishell)
{
    return (ft_itoa(minishell->exit_status));
}

static char *get_env_value(const char *var, t_mshell *minishell)
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


static char *process_variable(const char *input, size_t *i, t_mshell *minishell, char *result)
{
    char *substr;
    char *var_value;

    (*i)++;
    // Special case: $ at end of string or $ followed by non-variable character
    if (!input[*i] || (!ft_isalnum(input[*i]) && input[*i] != '_' && input[*i] != '?')) {
        result = append_to_result(result, "$");
        (*i)--; // Move back since we'll increment in the main loop
        return result;
    }

    size_t start = *i;
    // Check for special case of $? first
    if (input[*i] == '?') {
        (*i)++;  // Move past the question mark
        var_value = get_exit_code(minishell);
        if (!var_value)
            return (free(result), NULL);
        result = append_to_result(result, var_value);
        free(var_value);
        (*i)--;  // Adjust index as we'll increment in the main loop
        return result;
    }

    // Process regular variables
    while (input[*i] && (ft_isalnum(input[*i]) || input[*i] == '_'))
        (*i)++;

    substr = ft_substr(input, start, *i - start);
    if (!substr)
        return (free(result), NULL);

    var_value = get_env_value(substr, minishell);
    free(substr);
    if (!var_value)
        return (free(result), NULL);

    result = append_to_result(result, var_value);
    free(var_value);
    (*i)--;
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

static char *expand_tilde(const char *input, size_t *i, t_mshell *mshell)
{
    char *home;
    char *expanded;
    size_t len = 1; // Start with just the ~

    // Only expand if:
    // 1. ~ is at start of input, OR
    // 2. ~ is after whitespace, OR
    // 3. ~ is after = (for assignments)
    if (!(*i == 0 || ft_isspace(input[*i - 1]) || input[*i - 1] == '='))
        return (ft_strdup("~"));

    // Check what follows the ~
    if (input[*i + 1] == '\0' || ft_isspace(input[*i + 1]))
    {
        // Case: just ~
        home = ms_getenv(mshell, "HOME");
        return (home ? ft_strdup(home) : ft_strdup("~"));
    }
    else if (input[*i + 1] == '/')
    {
        // Case: ~/path
        home = ms_getenv(mshell, "HOME");
        if (!home)
            return (ft_strdup("~"));

        // Calculate the length of the path part (after ~/)
        while (input[*i + 1 + len] && !ft_isspace(input[*i + 1 + len]))
            len++;

        // Join HOME with the path part (excluding the ~)
        expanded = ft_strjoin(home, input + *i + 1);
        *i += len; // Advance past the path part
        return (expanded);
    }

    // All other cases (like ~username) remain literal
    return (ft_strdup("~"));
}


char	*expand_env_variables(const char *input,  t_mshell *minishell)
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
        if (input[i] == '~' && !single_q)
        {
            append = expand_tilde(input, &i, minishell);
        }
        // else 
		if (handle_quotes(input[i], &single_q, &double_q))
		{
			char single_char[2];
			single_char[0] = input[i];
			single_char[1] = '\0';
			append = ft_strdup(single_char);
		}
		//append = ft_strdup(""); // Ignore quotes in result
		else if (input[i] == '\\' && input[i + 1])
			append = handle_escape(input, &i, single_q);
		else if (input[i] == '$' && input[i + 1] && !single_q)
			append = process_variable(input, &i,  minishell, ft_strdup(""));
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

