#include "../include/minishell.h"

t_cmd	* run_parser(char	*input, t_builtin const * builtins)
{
//if user typed "exit"
if(0 == ft_strcmp(input,"exit"))
{
	t_cmd cmd1 =
	{
		.argv = (char *[]){"exit", "", NULL},
		.binary = NULL,
		.in_redir = NULL,
		.out_redir = NULL,
		.next = NULL
	};
	return &cmd1;
}
return NULL;
}