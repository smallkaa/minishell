/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ilia Munaev <ilyamunaev@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 08:10:49 by pvershin          #+#    #+#             */
/*   Updated: 2025/05/10 19:47:43 by Ilia Munaev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"



int	main(int argc, char **argv, char **envp)
{
	t_mshell	*minishell;
	uint8_t		exit_status;

	// if (argc != 2)
	// {
	// 	print_error("Error: usage ./minishel\n");
	// 	return(EXIT_SUCCESS);
	// }
	(void)argv;
	(void)argc;
	setup_signal_handlers();
	minishell = init_mshell(envp);
	if (!minishell)
		return (EXIT_FAILURE);
	// if (argc >= 3 && !ft_strncmp(argv[1], "-c", 3))
	// 	exit_status = run_command_mode(minishell, argv[2]);
	// else if (argc == 2)
	// 	exit_status = run_script_mode(minishell, argv[1]);
	// else
	// {
	if (BIGTEST == 1 && !is_input_interactive())
		exit_status = run_non_interactive_mode(minishell);
	else
		exit_status = run_interactive_mode(minishell);
	// }
	free_minishell(&minishell);
	rl_clear_history();
	return (exit_status);
}
