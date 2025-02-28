
# include <stdlib.h>
# include <stdio.h>
# include <unistd.h>
# include <stdbool.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <sys/wait.h>
# include <fcntl.h>
# include <string.h>
# include <signal.h>

//Definition of AT_* constants
# include <fcntl.h>

# include "libft.h"
typedef enum e_redir_type
{
	R_INPUT,
	R_OUTPUT,
	R_APPEND,
	R_HEREDOC
}	t_redir_type;

typedef struct s_redir
{
	t_redir_type	type;
	char			*filename;
}	t_redir;

typedef struct s_cmd
{
	char				**argv;
	char				*binary;
	t_redir				*in_redir;
	t_redir				*out_redir;
	struct s_cmd	*next;
}	t_cmd;

void	print_error_exit(char *cmd, int exit_status)
{
	ft_putstr_fd(cmd, STDERR_FILENO);
	perror(": ");
	exit(exit_status);
}

bool	is_builtin(t_cmd *cmd)
{
	(void)cmd;
	// do
	return (false);
}

void	exec_builtin(t_cmd *cmd)
{
	(void)cmd;
	// do
	return ;
}

void	handle_heredoc_redirection(t_cmd *cmd)
{
	(void)cmd;

	// Heredoc logic to be implemented here
	return ;
}

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
	printf("[DEBUG]: execute() input cmd: [%s]\n", cmd->argv[0]);

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
	if (cmd->next)
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
	if (cmd->in_redir || cmd->out_redir)
		handle_redirection(cmd, in_fd);
	execute(cmd, in_fd, envp);
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
	while (waitpid(-1, NULL, 0) > 0);
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


// cc -Wall -Wextra -Werror test_run_executor.c -Ilibft -Llibft -lft -o test_run_executor
int main(int argc, char **argv, char **envp)
{
	(void)argc;
	(void)argv;

/*	// Test 1: simulate `ls -l | wc -l`
	t_cmd cmd1 =
	{
		.argv = (char *[]){"ls", "-l", NULL},
		.binary = "/bin/ls",
		.in_redir = NULL,
		.out_redir = NULL,
		.next = NULL
	};

	t_cmd cmd2 =
	{
		.argv = (char *[]){"wc", "-l", NULL},
		.binary = "/usr/bin/wc",
		.in_redir = NULL,
		.out_redir = NULL,
		.next = NULL
	};

	cmd1.next = &cmd2;

	printf("Executing: ls -l | wc -l\n");
	run_executor(&cmd1, envp);
	*/
/*
	// Test 2: simulate `ls -a`
	t_cmd cmd1 =
	{
		.argv = (char *[]){"ls", "-a", NULL},
		.binary = "/bin/ls",
		.in_redir = NULL,
		.out_redir = NULL,
		.next = NULL
	};

	printf("Executing: ls -a\n");
	run_executor(&cmd1, envp);
*/
	// Test 3: simulate `ls | grep .c | wc -l`
	t_cmd cmd1 =
	{
		.argv = (char *[]){"ls", NULL},
		.binary = "/bin/ls",
		.next = NULL
	};

	t_cmd cmd2 =
	{
		.argv = (char *[]){"grep", ".c", NULL},
		.binary = "/usr/bin/grep",
		.next = NULL
	};

	t_cmd cmd3 =
	{
		.argv = (char *[]){"wc", "-l", NULL},
		.binary = "/usr/bin/wc",
		.next = NULL
	};
	cmd1.next = &cmd2;
	cmd2.next = &cmd3;

	printf("Executing: ls | grep .c | wc -l\n");
	run_executor(&cmd1, envp);

	return (EXIT_SUCCESS);
}
