#include "minishell.h"

char	**setup_builtin(void)
{
	char	**builtin;

	builtin = malloc(8 * sizeof(char *));
	if (!builtin)
	{
		print_error("Error (is_builtin): builtin malloc failed\n");
		return (NULL);
	}
	builtin[0] = ft_strdup("echo");
	builtin[1] = ft_strdup("cd");
	builtin[2] = ft_strdup("pwd");
	builtin[3] = ft_strdup("export");
	builtin[4] = ft_strdup("unset");
	builtin[5] = ft_strdup("env");
	builtin[6] = ft_strdup("exit");
	builtin[7] = NULL;
	return (builtin);
}
