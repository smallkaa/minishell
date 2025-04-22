/**
 * @file tokenizer4.c
 * @brief Token validation and stripping utility functions.
 */

#include <stdlib.h>
#include <string.h>
#include "../include/minishell.h"

/**
 * @brief Checks if a string is enclosed in single quotes.
 *
 * @param str The string to check.
 * @return true if enclosed in single quotes, false otherwise.
 */
bool is_in_single_quotes(char *str)
{
    size_t len;
    if (!str || !*str)
        return (false);
    len = ft_strlen(str);
    if (len < 2)
        return (false);
    return (str[0] == '\'' && str[len - 1] == '\'');
}

/**
 * @brief Checks if a string is enclosed in double quotes.
 *
 * @param str The string to check.
 * @return true if enclosed in double quotes, false otherwise.
 */
bool is_in_double_quotes(char *str)
{
    size_t len;
    if (!str || !*str)
        return (false);
    len = ft_strlen(str);
    if (len < 2)
        return (false);
    return (str[0] == '"' && str[len - 1] == '"');
}

/**
 * @brief Strips surrounding quotes from a string while preserving inner quotes.
 *
 * @param str The input string.
 * @param quote_style 1 for single quotes, 2 for double quotes.
 * @return Newly allocated string without outer quotes, or NULL on error.
 */
static char *strip_quotes_preserving_inner(const char *str, int quote_style)
{
    char   *result;
    size_t len = ft_strlen(str);
    size_t i = 0, j = 0;
    bool   in_squote = false, in_dquote = false;

    result = malloc(len + 1);
    if (!result)
        return (NULL);
    while (str[i])
    {
        if (str[i] == '\'' && quote_style == 1 && !in_dquote)
        {
            in_squote = !in_squote;
            i++;
        }
        else if (str[i] == '"' && quote_style == 2 && !in_squote)
        {
            in_dquote = !in_dquote;
            i++;
        }
        else
            result[j++] = str[i++];
    }
    result[j] = '\0';
    return (result);
}

/**
 * @brief Strips outer quotes from all tokens of type TOKEN_WORD.
 *
 * Iterates through tokens and replaces values enclosed in matching
 * quotes with unquoted strings.
 *
 * @param tokens Array of tokens to process.
 * @return 0 on success, -1 on error.
 */
int strip_words(t_TokenArray *tokens)
{
    int   i;
    char *str;
    char *result;

    if (!tokens || !tokens->tokens)
        return (-1);
    i = 0;
    while (i < tokens->count)
    {
        t_Token *tok = &tokens->tokens[i];
        if (tok->type == TOKEN_WORD && tok->value)
        {
            if (tok->in_single_quotes && tok->in_double_quotes)
            {
                i++;
                continue;
            }
            str = tok->value;
            result = strip_quotes_preserving_inner(str, tok->quote_style);
            if (!result)
                return (-1);
            free(str);
            tok->value = result;
        }
        i++;
    }
    return (0);
}