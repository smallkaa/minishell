#include "minishell.h"

void append_char_to_result(char c, char **result)
{
	char str[2] = {c, '\0'};
	*result = append_to_result(*result, str);
}

void handle_single_quote(const char *input, size_t *i,
	int *single_q, char **result, int double_q)
{
	if (double_q)
	{
		// внутри двойных кавычек — оставить '
		append_char_to_result(input[*i], result);
	}
	else
	{
		*single_q = !(*single_q); // включаем или выключаем режим '...'
	}
	(*i)++;
}


void handle_double_quote(const char *input, size_t *i,
	int *double_q, char **result, int single_q)
{
	if (single_q)
	{
		// внутри одинарных кавычек — оставить "
		append_char_to_result(input[*i], result);
	}
	else
	{
		*double_q = !(*double_q); // включаем или выключаем режим "..."
	}
	(*i)++;
}


void handle_backslash(const char *input, size_t *i,
	char **result, int single_q)
{
	if (!input[*i + 1])
	{
		append_char_to_result('\\', result);
		(*i)++;
		return;
	}

	(*i)++;

	// внутри одинарных кавычек — сохраняем как есть
	if (single_q)
	{
		append_char_to_result('\\', result);
		append_char_to_result(input[*i], result);
		(*i)++;
		return;
	}

	char next = input[*i];

	// только определённые символы экранируются
	if (next == '$' || next == '\\' || next == '"' || next == '\'')
	{
		append_char_to_result(next, result);
		(*i)++;
	}
	else
	{
		// экранирование ничего не делает — оставляем как есть
		append_char_to_result('\\', result);
		append_char_to_result(next, result);
		(*i)++;
	}
}


/**
 * @brief Handle tilde expansion or literal '~'.
 *
 * Uses quote_style to decide if tilde is literal or expanded via expand_tilde.
 *
 * @param input       Input string containing '~'.
 * @param i           Pointer to current index; advanced as needed.
 * @param result      Pointer to result string.
 * @param mshell      Shell context.
 * @param quote_style Quote style: 0 (none), 1 (single), 2 (double).
 */
void handle_tilde(const char *input, size_t *i, char **result,
    t_mshell *mshell, int quote_style)
{
    int single_q = (quote_style == 1);
    int double_q = (quote_style == 2);
	if (single_q || double_q)
	{
		append_char_to_result('~', result);
		(*i)++;
	}
    else
    {
        char *home = expand_tilde(input, i, mshell, quote_style);
		if (home)
		{
			*result = append_to_result(*result, home);
			free(home);
		}
	}
}

/**
 * @brief Handle cases: no character or special $? expansions.
 *
 * @param input  Input string.
 * @param i      Pointer to current index; may be advanced.
 * @param result Pointer to result string.
 * @param mshell Shell context for exit status.
 * @return 1 if handled, 0 otherwise.
 */
static int handle_dollar_special(const char *input, size_t *i,
    char **result, t_mshell *mshell)
{
    if (input[*i] == '\0')
    {
        append_char_to_result('$', result);
        return (1);
    }
    if (input[*i] == '?')
    {
        char *exit_code = get_exit_code(mshell);
        if (exit_code)
        {
            *result = append_to_result(*result, exit_code);
            free(exit_code);
        }
        (*i)++;
        return (1);
    }
    return (0);
}

/**
 * @brief Handle variable-name expansion after '$'.
 *
 * @param input  Input string.
 * @param i      Pointer to current index; advanced past name.
 * @param result Pointer to result string.
 * @param mshell Shell context for env lookup.
 */
static void handle_dollar_var(const char *input, size_t *i,
    char **result, t_mshell *mshell)
{
    size_t start = *i;
    while (input[*i] && (ft_isalnum(input[*i]) || input[*i] == '_'))
        (*i)++;
    char *name = ft_substr(input, start, *i - start);
    if (!name)
        return;
    char *val = get_env_value(name, mshell);
    free(name);
    if (val)
    {
        *result = append_to_result(*result, val);
        free(val);
    }
}

/**
 * @brief Handle single-digit positional-parameter expansion after '$'.
 *
 * @param input  Input string.
 * @param i      Pointer to current index; advanced past the digit.
 * @param result Pointer to result string.
 * @param mshell Shell context for env lookup.
 */
static void handle_dollar_digit(const char *input, size_t *i,
    char **result, t_mshell *mshell)
{
    char name[2] = {input[*i], '\0'};
    char *val = get_env_value(name, mshell);
    if (val)
    {
        *result = append_to_result(*result, val);
        free(val);
    }
    (*i)++;
}

/**
 * @brief Main handler for '$' expansions, dispatching to helpers.
 *
 * @param input    Input string.
 * @param i        Pointer to current index; advanced as needed.
 * @param result   Pointer to result string.
 * @param mshell   Shell context for env and exit status.
 * @param single_q Non-zero if inside single quotes.
 * @param double_q Non-zero if inside double quotes (unused).
 */
void handle_dollar(const char *input, size_t *i, char **result,
    t_mshell *mshell, int single_q)
{
    if (single_q)
    {
        append_char_to_result('$', result);
        (*i)++;
        return;
    }
    (*i)++;
    if (handle_dollar_special(input, i, result, mshell))
        return;
    if (ft_isalpha(input[*i]) || input[*i] == '_')
    {
        handle_dollar_var(input, i, result, mshell);
        return;
    }
    if (ft_isdigit(input[*i]))
    {
        handle_dollar_digit(input, i, result, mshell);
        return;
    }
    append_char_to_result('$', result);
}



