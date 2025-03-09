#include "minishell.h"

static int	ft_putstr_custom(t_cmd *cmd, char *str)
{
	ssize_t	written;

	written = write(STDOUT_FILENO, str, ft_strlen(str));
	if (written == -1)
	{
		error_handler(cmd);
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}

static uint8_t	print_content(t_cmd *cmd, int i, int *newline_flag)
{
	if (!cmd->argv[1])
		return (ft_putstr_custom(cmd, "\n"));
	while (cmd->argv[i])
	{
		if (!cmd->argv[i])
		{
			print_error("Error: print_content, string == NULL\n");
			return (EXIT_FAILURE);
		}
		if (ft_putstr_custom(cmd, cmd->argv[i]) == EXIT_FAILURE)
			return (EXIT_FAILURE);
		if (cmd->argv[i + 1] && ft_putstr_custom(cmd, " ") == EXIT_FAILURE)
			return (EXIT_FAILURE);
		i++;
	}
	if (*newline_flag && ft_putstr_custom(cmd, "\n") == EXIT_FAILURE)
		return (EXIT_FAILURE);
	return (EXIT_SUCCESS);
}

static int	handle_echo_flags(t_cmd *cmd, int *newline_flag)
{
	int	i;
	int	j;

	i = 1;
	while (cmd->argv[i] && cmd->argv[i][0] == '-' && cmd->argv[i][1] == 'n')
	{
		j = 1;
		while (cmd->argv[i][j] == 'n')
			j++;
		if (cmd->argv[i][j] != '\0')
			break ;
		*newline_flag = 0;
		i++;
	}
	return (i);
}

uint8_t	handle_echo(t_cmd *cmd)
{
	int		i;
	uint8_t	exit_status;
	int		newline_flag;

	newline_flag = 1;
	i = handle_echo_flags(cmd, &newline_flag);
	exit_status = print_content(cmd, i, &newline_flag);
	cmd->minishell->exit_status = exit_status;
	if (cmd->in_pipe)
		exit(exit_status);
	return (exit_status);
}
