#include "../include/minishell.h"

t_cmd	* run_parser(char	*input)
{
//2do - trim all spaces before and after
//if user typed "exit"
//char *trimmed_input;
//trimmed_input = ft_strtrim(input, " ");
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