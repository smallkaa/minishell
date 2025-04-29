/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_command.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ilia Munaev <ilyamunaev@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 14:46:52 by Ilia Munaev       #+#    #+#             */
/*   Updated: 2025/04/29 22:31:23 by Ilia Munaev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file execute_command.c
 * @brief Handles execution of commands (built-in and external) in Minishell.
 *
 * Contains logic to:
 * - Validate and execute built-in commands.
 * - Execute external binaries using `execve`.
 * - Handle syntax and shell-specific cases such as `.`/`..`, `env`,
 * and empty inputs.
 * - Manage cleanup and exit codes inside child processes.
 */
#include "minishell.h"

/**
 * @brief Executes a built-in command in a child process and exits.
 *
 * This function is called in the forked child process when the command
 * is a built-in with no external binary path. It executes the built-in,
 * frees the shell context, and exits with the appropriate status code.
 *
 * @param cmd Pointer to the command structure.
 */
static void	handle_builtin_and_exit(t_cmd *cmd)
{
	uint8_t	exit_status;

	exit_status = exec_builtins(cmd);
	free_minishell(cmd->minishell); /// must be here to free if builtin in pipe
	_exit(exit_status);
}
char **build_envp(char **env)
{
	int		count;
	char	**new_env;
	int		i;

	if (!env)
		return (NULL);

	// Count number of environment variables
	count = 0;
	while (env[count])
		count++;

	// Allocate array of char* (+1 for NULL terminator)
	new_env = malloc(sizeof(char *) * (count + 1));
	if (!new_env)
		return (NULL);

	// Copy each string
	i = 0;
	while (i < count)
	{
		new_env[i] = ft_strdup(env[i]);
		if (!new_env[i])
		{
			// Free previously allocated strings if duplication fails
			while (--i >= 0)
				free(new_env[i]);
			free(new_env);
			return (NULL);
		}
		i++;
	}
	new_env[i] = NULL;
	return (new_env);
}
/**
 * @brief Executes an external binary command using `execve()`.
 *
 * First validates special dot (`.`) and dot-dot (`..`) commands.
 * If validation passes, it attempts to execute the binary with `execve`.
 * On failure, it handles and prints an appropriate error message.
 *
 * @param cmd Pointer to the command structure.
 */
void	exec_cmd(t_cmd *cmd)
{
    // char **envp;
	uint8_t	exit_status;

    // envp = build_envp(cmd->minishell->env);
	// free_minishell(cmd->minishell);

	exit_status = validate_dots(cmd);
	if (exit_status != EXIT_SUCCESS)
	{
		free_minishell(cmd->minishell); // must be here, tested for . and ..
		_exit(exit_status);
	}

	signal(SIGPIPE, SIG_DFL);

	// printf("\n-------------DEBUG: exec_cmd() pid=%d\n", getpid());
	// execve(cmd->binary, cmd->argv, envp);
	execve(cmd->binary, cmd->argv, cmd->minishell->env);
	// free_minishell(cmd->minishell);

	// ft_free_arrstrs(envp);
	child_execve_error(cmd);
}

/**
 * @brief Entry point for executing a command in a child process.
 *
 * Handles several cases:
 * - Skips execution if the command is empty.
 * - Exits immediately if there was a previous syntax error.
 * - Updates `SHLVL` if this is a recursive call to `./minishell`.
 * - Processes empty strings as invalid commands.
 * - For built-ins without a binary, calls the built-in directly.
 * - For valid binaries, delegates to `exec_cmd()`.
 *
 * @param cmd Pointer to the command structure.
 */
void	execute_command(t_cmd *cmd)
{

	uint8_t	exit_status;

	if (!cmd || !cmd->argv || !cmd->argv[0])
	{
		// 	free_minishell(cmd->minishell);
		_exit(EXIT_SUCCESS);
	}
	if (cmd->minishell->syntax_exit_status != 0)
	{
		exit_status = cmd->minishell->syntax_exit_status;
		// if (cmd->minishell)
		// 	free_minishell(cmd->minishell); // tested no need... test again if needed
		_exit(exit_status);
	}
	if (is_minishell_executable(cmd) && update_shlvl(cmd) == EXIT_FAILURE)
	{
		free_minishell(cmd->minishell); // must be here. tested
		_exit(EXIT_FAILURE);
	}
	if (ft_strcmp(cmd->argv[0], "") == 0)
		handle_empty_command(cmd);
	if (!cmd->binary)
	{
		if (is_builtin(cmd))
		{
			if (ft_strcmp(cmd->argv[0], "env") == 0)
				update_underscore(cmd, cmd->binary);
			handle_builtin_and_exit(cmd);
		}
		else
			cmd_missing_command_error(cmd);
	}
	exec_cmd(cmd);
}
