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
	char *escaped = handle_escape(input, i, single_q);
	if (escaped)
	{
		*result = append_to_result(*result, escaped);
		free(escaped);
	}
}

void handle_tilde(const char *input, size_t *i, char **result,
	t_mshell *mshell, int single_q, int double_q)
{
	if (single_q || double_q)
	{
		append_char_to_result('~', result);
		(*i)++;
	}
	else
	{
		char *home = expand_tilde(input, i, mshell, single_q, double_q);
		if (home)
		{
			*result = append_to_result(*result, home);
			free(home);
		}
	}
}

void	handle_dollar(const char *input, size_t *i, char **result,
	t_mshell *mshell, int single_q, int double_q)
{
    (void)double_q;
	if (single_q)
	{
		append_char_to_result('$', result);
		(*i)++;
		return;
	}

	(*i)++;

	if (input[*i] == '\0')
	{
		append_char_to_result('$', result);
		return;
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
		return;
	}

	if (ft_isdigit(input[*i]))
	{
		char name[2] = {input[*i], '\0'};
		char *val = get_env_value(name, mshell);
		if (val)
		{
			*result = append_to_result(*result, val);
			free(val);
		}
		(*i)++;
		return;
	}

	if (ft_isalpha(input[*i]) || input[*i] == '_')
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
		return;
	}

	// если символ после $ невалиден
	append_char_to_result('$', result);
}

