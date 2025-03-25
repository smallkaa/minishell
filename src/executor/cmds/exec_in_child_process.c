#include "minishell.h"

static uint8_t	validate_dot(t_cmd *cmd)
{
	if (ft_strcmp(cmd->argv[0], ".") == 0)
	{
		if (!cmd->argv[1])
		{
			print_error("-minishell: .: filename argument required\n");
			print_error(".: usage: . filename [arguments]\n");
			cmd->minishell->exit_status = 2;
			return (2);
		}
	}
	return (EXIT_SUCCESS);
}
/**
 * @brief Executes a command, either a built-in or an external binary.
 *
 * If the command is a built-in, it calls `exec_builtin()`. Otherwise, it
 * attempts to execute the binary using `execve()`. If execution fails,
 * an appropriate error message is printed, and the process exits.
 *
 * @param cmd Pointer to the command structure.
 */
static void	execute_command(t_cmd *cmd)
{
	if (cmd->binary == NULL)
	{
		if (is_builtin(cmd))
		_exit(exec_builtin(cmd));
		else
			cmd_missing_command_error(cmd);
	}
	if (validate_dot(cmd) == 2)
		_exit(2);

	execve(cmd->binary, cmd->argv, cmd->minishell->env);
	// fprintf(stderr, "errno = %d (%s)\n", errno, strerror(errno)); // test
	child_execve_error(cmd);
}


static void	child_process(t_cmd *cmd, int in_fd, int fds[2])
{


	if (in_fd != STDIN_FILENO && in_fd >= 0)
	{
		if (dup2(in_fd, STDIN_FILENO) == -1)
		{
			perror("dup2 in_fd->STDIN");
			_exit(EXIT_FAILURE);
		}
		if (close(in_fd) == -1)
		{
			perror("close in_fd");
			_exit(EXIT_FAILURE);
		}
	}

	// If there's a next command, redirect current cmd's output to pipe write-end
	if (cmd->next)
	{
		if (dup2(fds[1], STDOUT_FILENO) == -1)
		{
			perror("dup2 fds[1]->STDOUT");
			_exit(EXIT_FAILURE);
		}
	}

	// Close any pipe ends we don't need
	if (fds[0] >= 0)
	{
		if (close(fds[0]) == -1)
		{
			perror("close fds[0]");
			_exit(EXIT_FAILURE);
		}
	}
	if (fds[1] >= 0)
	{
		if (close(fds[1]) == -1)
		{
			perror("close fds[1]");
			_exit(EXIT_FAILURE);
		}
	}

	if (apply_redirections(cmd) != EXIT_SUCCESS)
		_exit(EXIT_FAILURE);

	execute_command(cmd);
}


/**
 * @brief Handles file descriptor cleanup and waits for the child process.
 *
 * The parent process manages pipes, closes unnecessary file descriptors,
 * and waits for the child process to finish. It also updates the shell's
 * exit status based on the child's termination status.
 *
 * @param cmd Pointer to the command structure.
 * @param in_fd File descriptor for input redirection.
 * @param fds Pipe file descriptors [read end, write end].
 * @param pid Process ID of the forked child process.
 * @return The exit status of the executed command.
 */
static uint8_t	parent_process(t_cmd *cmd, int in_fd, int fds[2], pid_t pid)
{
	int		status;
	uint8_t	exit_status;

	if (cmd->next)
		close(fds[1]);
	if (in_fd != STDIN_FILENO)
		close(in_fd);
	if (!cmd->next && fds[0] >= 0)
		close(fds[0]);
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		exit_status = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		exit_status = 128 + WTERMSIG(status);
	else
		exit_status = EXIT_FAILURE;
	return (exit_status);
}


/**
 * @brief Forks a new process and executes the command.
 *
 * This function creates a child process using `fork()`. If the fork is
 * successful, the child process executes `child_process()`, while the parent
 * process continues execution.
 *
 * @param cmd Pointer to the command structure.
 * @param in_fd File descriptor for input redirection.
 * @param fds Pipe file descriptors [read end, write end].
 * @return Process ID of the child process.
 */
static pid_t	fork_and_execute(const t_cmd *cmd, int in_fd, int fds[2])
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
		perror_return("-minishell: fork", EXIT_FAILURE);
	if (pid == 0)
		child_process((t_cmd *)cmd, in_fd, fds);
	return (pid);
}


uint8_t	exec_in_child_process(t_cmd *cmd)
{
	// printf("---In exec_in_child_process\n");

	uint8_t	exit_status;
	int		in_fd;
	int		fds[2];
	int		cmd_count;
	pid_t	pid;

	exit_status = EXIT_FAILURE;
	in_fd = 0;
	cmd_count = 0;
	while (cmd)
	{
		if (is_pipeline_limit(&cmd_count))
			return (exit_status);

		fds[0] = -1;
		fds[1] = -1;
		if (cmd->next && pipe(fds) == -1)
		{
			perror("pipe");
			return (exit_status);
		}
		pid = fork_and_execute(cmd, in_fd, fds);
		if (pid > 0)
			exit_status = parent_process(cmd, in_fd, fds, pid);
		if (cmd->next)
			in_fd = fds[0];
		else
			in_fd = STDIN_FILENO;
		cmd = cmd->next;
	}
	return (exit_status);
}

