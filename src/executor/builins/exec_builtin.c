/**
 * @file exec_builtin.c
 * @brief Function for handle built-in commands in Minishell.
 */
#include "minishell.h"

/**
 * @brief Executes a built-in command in Minishell.
 *
 * This function determines which built-in command is being executed by
 * comparing the first argument (`cmd->argv[0]`) against known built-in commands.
 * If a match is found, the corresponding handler function is called.
 *
 * Supported built-in commands:
 * - `exit`   → Calls `handle_exit()`
 * - `echo`   → Calls `handle_echo()`
 * - `pwd`    → Calls `handle_pwd()`
 * - `cd`     → Calls `handle_cd()`
 * - `env`    → Calls `handle_env()`
 * - `export` → Calls `handle_export()`
 * - `unset`  → Calls `handle_unset()`
 *
 * @param cmd Pointer to the command structure containing the command arguments.
 * @return The exit status of the built-in command execution.
 *         - `EXIT_SUCCESS` if no matching built-in command is found.
 *         - Otherwise, the return value of the corresponding built-in handler.
 */
uint8_t	exec_builtin(t_cmd *cmd)
{
	if (ft_strcmp(cmd->argv[0], "exit") == 0)
		return (handle_exit(cmd));
	else if (ft_strcmp(cmd->argv[0], "echo") == 0)
		return (handle_echo(cmd));
	else if (ft_strcmp(cmd->argv[0], "pwd") == 0)
		return (handle_pwd(cmd));
	else if (ft_strcmp(cmd->argv[0], "cd") == 0)
		return (handle_cd(cmd));
	else if (ft_strcmp(cmd->argv[0], "env") == 0)
		return (handle_env(cmd));
	else if (ft_strcmp(cmd->argv[0], "export") == 0)
		return (handle_export(cmd));
	else if (ft_strcmp(cmd->argv[0], "unset") == 0)
		return (handle_unset(cmd));
	else
		return (EXIT_SUCCESS);
}
