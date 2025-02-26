#include "minishell.h"

// echo
// cd
// pwd
// export
// unset
// env
// exit
char	**setup_builtin(void)
{
	char	**builtin;

	builtin = malloc(8 * sizeof(char *));
	if (!builtin)
	{
		print_error("Error: is_builtin() malloc failed");
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

bool	is_builtin(t_cmd *cmd)
{
	char	**builtin;
	int		i;

	if (!cmd || !cmd->argv[0])
	{
		print_error("Error: is_builtin() no cmd provided");
		return (false);
	}
	builtin = setup_builtin();
	if (!builtin)
	{
		print_error("Error: setup_builtin() failed");
		return (false);
	}
	i = 0;
	while (builtin[i] && ft_strcmp(cmd->argv[0], builtin[i]) != 0)
		i++;
	ft_free_arrstrs(builtin);
	return (builtin[i] != NULL);
}

void	exec_builtin(t_cmd *cmd)
{
	// do
	(void) cmd;
	return ;
}
