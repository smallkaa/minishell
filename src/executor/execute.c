#include "minishell.h"

/*
Command substitution, commands grouped with parentheses,
and asynchronous commands are invoked in a subshell environment
that is a duplicate of the shell environment, except that traps caught by
the shell are reset to the values that the shell inherited from
its parent at invocation. Builtin commands that are invoked as part of
a pipeline are also executed in a subshell environment.
Changes made to the subshell environment cannot affect
the shell’s execution environment.

All of the Bash builtins return an exit status of zero
if they succeed and a non-zero status on failure, so they may be used by
the conditional and list constructs. All builtins return an exit status of 2
to indicate incorrect usage, generally invalid options or missing arguments
*/
/**
 * Executes a command by replacing the current process with the new command.
 *
 * @param cmd	The command to execute.
 * @param in_fd	The file descriptor for input redirection.
 * @param env	The environment variables array.
 */
void	execute(t_cmd *cmd, int in_fd)
{
	if (cmd->binary == NULL)
	{
		printf("DEBUG: execute() cmd->binary == NULL status: %d\n", cmd->minishell->exit_status);
		command_not_found_handle(cmd);
	}
	if (in_fd != 0)
	{
		if (dup2(in_fd, STDIN_FILENO) == -1)
		{
			if (close(in_fd) == -1)
				print_error_exit("close", EXIT_FAILURE);
			print_error_exit("dup2", EXIT_FAILURE);
		}
		if (close(in_fd) == -1)
			print_error_exit("close", EXIT_FAILURE);
	}
	if (is_builtin(cmd))
		exec_builtin(cmd);
	execve(cmd->binary, cmd->argv, cmd->minishell->env);
}
