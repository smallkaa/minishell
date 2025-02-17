#include "minishell.h"
/*
https://www.gnu.org/software/bash/manual/html_node/Command-Execution-Environment.html

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

bool	is_builtin(t_cmd *cmd)
{
	// do
	return (true);
}

void	exec_builtin(t_cmd *cmd)
{

}
void exec_cmd(t_cmd *cmd, char **envp)
{
	int		fd[2];
	int		in_fd;
	pid_t	pid;
	int		in;
	int		out;

	in_fd = 0;

	while (cmd)
	{
		if (cmd->next)
		{
			if (pipe(fd) == -1) // create a pipe
				print_error_exit("pipe", EXIT_FAILURE);
		}
		pid = fork(); // fork the process / create a child process
		if (pid == -1)
			print_error_exit("fork", -1);

		if (pid == 0) // Child process reads from pipe
		{
			if (cmd->in_redir && cmd->in_redir->type == R_INPUT)  // Read from input file
			{
				in = open(cmd->in_redir->filename, O_RDONLY);
				if (in < 0)
					print_error_exit("read", -1);

				dup2(in, STDIN_FILENO);
				if (close(in) == -1)
					print_error_exit("close", -1);
			}
			else
				dup2(in_fd, STDIN_FILENO);  // Read from previous command

			if (cmd->out_redir && cmd->out_redir->type == R_OUTPUT) // Write output file
			{
				out = open(cmd->out_redir->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
				if (out < 0)
					print_error_exit("write", -1);
				dup2(out, STDOUT_FILENO);
				if (close(out) == -1)
					print_error_exit("close", -1);
			}
			else if (cmd->out_redir && cmd->out_redir->type == R_APPEND) // Append to output file
			{
				out = open(cmd->out_redir->filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
				if (out < 0)
					print_error_exit("write", -1);
				dup2(out, STDOUT_FILENO);
				if (close(out) == -1)
					print_error_exit("close", -1);
			}
			else if (cmd->next)
			{
				dup2(fd[1], STDOUT_FILENO);  // Pipe output to next command
				if (close(fd[1]) == -1)
					print_error_exit("close", -1);
			}
			if (cmd->next)
			{
				if (close(fd[0]) == -1)
					print_error_exit("close", -1);
			}
			execve(cmd->binary, cmd->argv, envp);
				print_error_exit("execve", EXIT_FAILURE);
		}
		if (waitpid(pid, NULL, 0) == -1)  // Parent waits for child
			print_error_exit("waitpid", EXIT_FAILURE);
		if (cmd->next)
		{
			if (close(fd[1]) == -1)  // Close write end of pipe
				print_error_exit("close", -1);
			in_fd = fd[0]; // Store read end for next command
		}
		cmd = cmd->next;
	}
}

void	run_executor(t_cmd *cmd, char **envp)
{
	if (is_biltin(cmd) && !cmd->next)
	{
		exec_builtin(cmd);
	}
	exec_cmd(cmd, envp);
}
