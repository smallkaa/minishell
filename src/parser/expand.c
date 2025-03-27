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

static char *append_to_result(char *result, const char *append)
{
    char *new_result;
    
    if (!result || !append)
        return (NULL);
        
    new_result = ft_strjoin(result, append);
    free(result);
    
    return (new_result);
}



static char *process_variable(const char *input, size_t *i, t_mshell *minishell, char *result)
{
    char *substr;
    char *var_value;

    (*i)++;
    
    // Проверка на конец строки или неалфавитный символ после $
    if (!input[*i] || (!ft_isalnum(input[*i]) && input[*i] != '_' && input[*i] != '?')) 
    {
        result = append_to_result(result, "$");
        (*i)--; // Откатываемся, т.к. инкремент произойдет в основном цикле
        return result;
    }
    
    size_t start = *i;
    // Специальный случай для $?
    if (input[*i] == '?') 
    {
        (*i)++;
        var_value = get_exit_code(minishell);
        if (!var_value)
            return (free(result), NULL);
        result = append_to_result(result, var_value);
        free(var_value);
        (*i)--; // Корректировка индекса для следующей итерации
        return result;
    }
    
    // Обработка обычных переменных
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




static int handle_quotes(char c, int *single_q, int *double_q)
{
    if (c == '\'')
    {
        if (!*double_q) // Игнорируем одинарные кавычки внутри двойных
        {
            *single_q = !*single_q;
            return (1);
        }
    }
    else if (c == '"')
    {
        if (!*single_q) // Игнорируем двойные кавычки внутри одинарных
        {
            *double_q = !*double_q;
            return (1);
        }
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

static char *expand_tilde(const char *input, size_t *i, t_mshell *mshell, int single_q, int double_q)
{
    char *home;
    
    // Не расширяем тильду внутри кавычек
    if (single_q || double_q)
        return (ft_strdup("~"));
    
    // Проверяем, находится ли тильда в начале слова или после пробела/таба
    if (*i > 0 && input[*i - 1] != ' ' && input[*i - 1] != '\t')
    {
        // Тильда не в начале слова - не расширяем
        return (ft_strdup("~"));
    }
    
    // Проверяем, находится ли после тильды пробел, слеш или конец строки
    // Только в этих случаях расширяем тильду
    if (input[*i + 1] != '\0' && input[*i + 1] != ' ' && 
        input[*i + 1] != '\t' && input[*i + 1] != '/')
    {
        // После тильды идут другие символы - не расширяем
        return (ft_strdup("~"));
    }
    
    // Получаем значение домашнего каталога
    home = ms_getenv(mshell, "HOME");
    if (!home || !*home)
        return (ft_strdup("~"));
    
    return (ft_strdup(home));
}




char *expand_env_variables(const char *input, t_mshell *minishell)
{
    char *result;
    size_t i;
    int single_q = 0;
    int double_q = 0;
    char *append;

    if (!input)
    {
        debug_printf("expand_env_variables: input is NULL\n");
        return (NULL);
    }
    
    result = ft_strdup("");
    if (!result)
        return (NULL);
        
    i = 0;
    while (input[i])
    {
        // 1. Тильда - обрабатываем особо из-за кейса с кавычками
        if (input[i] == '~')
        {
            if (single_q || double_q)
            {
                // Тильда в кавычках - НЕ расширяем
                result = append_to_result(result, "~");
            }
            else
            {
                // Тильда не в кавычках - расширяем
                append = expand_tilde(input, &i, minishell, single_q, double_q);
                if (!append)
                    return (free(result), NULL);
                
                result = append_to_result(result, append);
                free(append);
            }
        }
        // 2. Кавычки - отслеживаем для определения контекста
        else if (handle_quotes(input[i], &single_q, &double_q))
        {
            char quote[2] = {input[i], '\0'};
            result = append_to_result(result, quote);
        }
        // 3. Экранирование
        else if (input[i] == '\\' && !single_q)
        {
            append = handle_escape(input, &i, single_q);
            if (!append)
                return (free(result), NULL);
            
            result = append_to_result(result, append);
            free(append);
        }
        // 4. Переменные окружения
        else if (input[i] == '$' && !single_q)
        {
            append = process_variable(input, &i, minishell, ft_strdup(""));
            if (!append)
                return (free(result), NULL);
            
            result = append_to_result(result, append);
            free(append);
        }
        // 5. Обычные символы
        else
        {
            append = process_char(input, &i, single_q);
            if (!append)
                return (free(result), NULL);
            
            result = append_to_result(result, append);
            free(append);
        }
        
        i++;
    }
    
    return (result);
}








