/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   write_heredoc_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pvershin <pvershin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 14:00:09 by pvershin          #+#    #+#             */
/*   Updated: 2025/05/02 15:36:31 by pvershin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Handles SIGINT in heredoc (child process).
 */
void	heredoc_sigint_handler(int sig)
{
	(void)sig;
	write(STDOUT_FILENO, "\n", 1);
	_exit(1);
}

/**
 * @brief Setup signal handlers inside heredoc child process.
 */
void	setup_heredoc_signals(void)
{
	//signal(SIGINT, heredoc_sigint_handler);
	//signal(SIGQUIT, SIG_IGN);
	struct sigaction sa;

	ft_memset(&sa, 0, sizeof(sa)); // Инициализация структуры нулями
	sa.sa_handler = heredoc_sigint_handler;
	sa.sa_flags = 0; // Определите нужные флаги (например, SA_RESTART)
	sigemptyset(&sa.sa_mask);
	if (sigaction(SIGINT, &sa, NULL) == -1)
		perror("sigaction(SIGINT) error in heredoc child"); // Обработка ошибки

	signal(SIGQUIT, SIG_IGN);
}

/**
 * @brief Child: Reads user input and writes heredoc content to pipe.
 */
int	run_heredoc_child(int pipe_fd, const char *delim, t_mshell *mshell, int expand_flag)
{
	char	*line;
	char	*expanded_line;
	size_t	total_written;
	int     write_status;

	setup_heredoc_signals();
	disable_echoctl();
	line = NULL;
	total_written = 0;
	write_status = EXIT_SUCCESS;
	while (read_next_heredoc_line(&line, delim))
	{
		total_written += ft_strlen(line) + 1;
		if (heredoc_exceeds_limit(total_written))
		{
			free(line);
			return (error_return("heredoc: large input\n", WRITE_HERED_ERR));
		}
		if (expand_flag)
        {
            expanded_line = expand_env_variables(line, mshell, 0);
            if (!expanded_line)
			{
                perror_return("heredoc expansion failed", WRITE_HERED_ERR);
                expanded_line = line;
            }
			else
                free(line);
            line = NULL;
            
            write_status = write_heredoc_line(pipe_fd, expanded_line);
            free(expanded_line);
            if (write_status == WRITE_HERED_ERR)
                 return (WRITE_HERED_ERR);
        }
        else
        {
            write_status = write_heredoc_line(pipe_fd, line);
            free(line);
            line = NULL;
            if (write_status == WRITE_HERED_ERR)
                return (WRITE_HERED_ERR);
        }
	}
	if (line)
		free(line);
	return (EXIT_SUCCESS);
}
