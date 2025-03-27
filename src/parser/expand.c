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
    char *result = ft_strdup("");
    size_t i = 0;
    int single_q = 0; // Флаг для отслеживания одинарных кавычек
    int double_q = 0; // Флаг для отслеживания двойных кавычек

    if (!input || !result)
        return NULL;

    while (input[i])
    {
        if (input[i] == '\'' && !double_q) // Одинарные кавычки вне двойных
        {
            single_q = !single_q;
            char quote[2] = {input[i], '\0'};
            result = append_to_result(result, quote);
        }
        else if (input[i] == '"' && !single_q) // Двойные кавычки вне одинарных
        {
            double_q = !double_q;
            char quote[2] = {input[i], '\0'};
            result = append_to_result(result, quote);
        }
        else if (input[i] == '\\' && !single_q) // Экранирование вне одинарных кавычек
        {
            char *escaped = handle_escape(input, &i, single_q);
            if (!escaped)
                return (free(result), NULL);
            result = append_to_result(result, escaped);
            free(escaped);
        }
        else if (input[i] == '$' && !single_q && input[i + 1]) // Переменная вне одинарных кавычек
        {
            i++; // Пропускаем $
            
            // Специальный случай для $?
            if (input[i] == '?')
            {
                char *exit_value = get_exit_code(minishell);
                if (!exit_value)
                    return (free(result), NULL);
                result = append_to_result(result, exit_value);
                free(exit_value);
                i++;
                continue;
            }
            
            // Позиционный параметр (начинается с цифры)
            if (ft_isdigit(input[i]))
            {
                char param_name[2] = {input[i], '\0'};
                char *param_value = get_env_value(param_name, minishell);
                if (!param_value)
                    return (free(result), NULL);
                
                result = append_to_result(result, param_value);
                free(param_value);
                i++; // Переходим к следующему символу
                continue;
            }
            
            // Обычная переменная (начинается с буквы или _)
            if (ft_isalpha(input[i]) || input[i] == '_')
            {
                size_t start = i;
                while (input[i] && (ft_isalnum(input[i]) || input[i] == '_'))
                    i++;
                
                char *var_name = ft_substr(input, start, i - start);
                if (!var_name)
                    return (free(result), NULL);
                
                char *var_value = get_env_value(var_name, minishell);
                free(var_name);
                if (!var_value)
                    return (free(result), NULL);
                
                result = append_to_result(result, var_value);
                free(var_value);
                continue; // Продолжаем с нового символа
            }
            
            // Если после $ не идет допустимое имя переменной, добавляем $ как обычный символ
            result = append_to_result(result, "$");
            // Не увеличиваем i, т.к. текущий символ нужно обработать в следующей итерации
            continue;
        }
        else if (input[i] == '~' && !single_q && !double_q) // Тильда вне кавычек
        {
            char *home = expand_tilde(input, &i, minishell, single_q, double_q);
            if (!home)
                return (free(result), NULL);
            result = append_to_result(result, home);
            free(home);
        }
        else // Обычный символ
        {
            char c[2] = {input[i], '\0'};
            result = append_to_result(result, c);
        }
        
        i++;
    }
    
    return result;
}










