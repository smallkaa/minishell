#include "minishell.h"

bool	command_too_long(char **argv)
{
	size_t	total_len;
	int		i;

	total_len = 0;
	i = 0;
	while (argv[i])
	{
		total_len += ft_strlen(argv[i]) + 1;
		if (total_len >= CMD_MAX_SIZE)
			return (true);
		i++;
	}
	return (false);
}
