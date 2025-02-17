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

/*
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
*/
void	execute(t_cmd *cmd, int in_fd, char **envp)
{
	if (cmd->binary == NULL)
		print_error_exit(cmd->binary, EXIT_FAILURE);
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
	execve(cmd->binary, cmd->argv, envp);
	print_error_exit("execve", EXIT_FAILURE);
}

void	handle_out_redirection(t_cmd *cmd)
{
	int	out;
	int	mode;

	if (!cmd->out_redir)
		return ;

	mode = O_WRONLY | O_CREAT;
	if (cmd->out_redir->type == R_OUTPUT)
		mode |= O_TRUNC;
	else if (cmd->out_redir->type == R_APPEND)
		mode |= O_APPEND;
	else
		print_error_exit("redirection", EXIT_FAILURE);
	out = open(cmd->out_redir->filename, mode, 0644);
	if (out < 0)
		print_error_exit("write", EXIT_FAILURE);
	dup2(out, STDOUT_FILENO);
	if (close(out) == -1)
		print_error_exit("close", EXIT_FAILURE);
}

void	handle_heredoc_redirection(t_cmd *cmd)
{
	// Heredoc logic to be implemented here
}

void	handle_stdin_redirection(t_cmd *cmd)
{
	int	in;

	if (!cmd->in_redir)
		return ;
	in = open(cmd->in_redir->filename, O_RDONLY);
	if (in < 0)
		print_error_exit("read", EXIT_FAILURE);
	if (dup2(in, STDIN_FILENO) == -1)
	{
		if (close(in) == -1)
			print_error_exit("close", EXIT_FAILURE);
		print_error_exit("dup2", EXIT_FAILURE);
	}
	if (close(in) == -1)
		print_error_exit("close", EXIT_FAILURE);

}

void	handle_in_redirection(t_cmd *cmd)
{
	if (!cmd->in_redir)
		return ;
	if (cmd->in_redir->type == R_INPUT)
		handle_stdin_redirection(cmd);
	else if (cmd->in_redir->type == R_HEREDOC)
		handle_heredoc_redirection(cmd);
	else
		print_error_exit("redirection", EXIT_FAILURE);
}

void	handle_redirection(t_cmd *cmd, int in_fd)
{
	if (cmd->in_redir)
		handle_in_redirection(cmd);
	else
		if (dup2(in_fd, STDIN_FILENO) == -1)
		{
			if (close(in_fd) == -1)
				print_error_exit("close", EXIT_FAILURE);
			print_error_exit("dup2", EXIT_FAILURE);
		}
	if (cmd->out_redir)
		handle_out_redirection(cmd);
}

void	exec_fork_child(t_cmd *cmd, int in_fd, int fd[2], char **envp)
{
	if (cmd->next)  // If there's a next command, set up output redirection
	{
		if (dup2(fd[1], STDOUT_FILENO) == -1)
		{
			if (close(fd[1]) == -1)
				print_error_exit("close", EXIT_FAILURE);
			print_error_exit("dup2", EXIT_FAILURE);
		}
		if (close(fd[1]) == -1)
			print_error_exit("close", EXIT_FAILURE);
		if (close(fd[0]) == -1)
			print_error_exit("close", EXIT_FAILURE);
	}
	if (cmd->in_redir || cmd->out_redir)  // Handle redirections
		handle_redirection(cmd, in_fd);
	execute(cmd, in_fd, envp);  // Execute the command
}

void	exec_cmd(t_cmd *cmd, char **envp)
{
	int		fd[2];
	int		in_fd;
	pid_t	pid;

	in_fd = 0;
	while (cmd)
	{
		if (cmd->next && pipe(fd) == -1)
			print_error_exit("pipe", EXIT_FAILURE);
		pid = fork();
		if (pid == -1)
			print_error_exit("fork", EXIT_FAILURE);
		if (pid == 0)
			exec_fork_child(cmd, in_fd, fd, envp);
		// Parent process: Close pipe write end, move to next command
		if (cmd->next)
		{
			if(close(fd[1]) == -1)
				print_error_exit("close", EXIT_FAILURE);
			in_fd = fd[0];  // Pass read end for the next command
		}
		cmd = cmd->next;
	}
	while (waitpid(-1, NULL, 0) > 0);  // Parent waits for all child processes
}

void	run_executor(t_cmd *cmd, char **envp)
{
	if (is_builtin(cmd) && !cmd->next)
	{
		exec_builtin(cmd);
		return ;
	}
	exec_cmd(cmd, envp);
}
