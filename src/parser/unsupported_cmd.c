#include "minishell.h"


/**
 * @brief Check if the input matches a known unsupported command pattern.
 *
 * @param cmd The command string to test (e.g., "()").
 * @param message Pointer to receive the error message (set to NULL if not matched).
 * @return int Error code (e.g., 258) or 0 if the command is supported.
 */
int	known_unsupported_cmd(const char *cmd, const char **message)
{
	const t_unsupported_cmd	issues[] = {
		{"()", 2, "syntax error near unexpected token `)'\n"},
		{"|||",2, "syntax error near unexpected token `||'"},
		// Add more unsupported patterns here
		{NULL, 0, NULL} // End marker
	};

	int	i = 0;
	while (issues[i].pattern)
	{
		if (ft_strcmp((char *)cmd, (char *)issues[i].pattern) == 0)
		{
			if (message)
				*message = issues[i].error_message;
			return (issues[i].error_code);
		}
		i++;
	}
	if (message)
		*message = NULL;
	return (0);
}
