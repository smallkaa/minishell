#include "minishell.h"

char	**setup_env(char **envp)
{
	int	i;
	int	envp_len;
	char **env;

	envp_len = ft_arr_size(envp);
	env = malloc((envp_len + 1) * sizeof(char *));
	if (!env)
	{
		print_error("Error (setup_envp): env malloc failed\n");
		return (NULL);
	}
	i = 0;
	while(envp[i])
	{
		env[i] = ft_strdup(envp[i]);
		i++;
	}
	env[i] = NULL;
	return (env);
}
