#include "minishell.h"

static uint8_t	validate_dots(t_cmd *cmd)
{
	if (ft_strcmp(cmd->argv[0], ".") == 0 && !cmd->argv[1])
	{
		print_error("-minishell: .: filename argument required\n");
		print_error(".: usage: . filename [arguments]\n");
		return (2);
	}
	else if (ft_strcmp(cmd->argv[0], "..") == 0	&& !cmd->argv[1])
	{
		print_error("..: command not found\n");
		return (127);
	}
	return (EXIT_SUCCESS);
}

static void	execute_command(t_cmd *cmd)
{
	uint8_t	exit_status;

	if (cmd->binary == NULL)
	{
		if (is_builtin(cmd))
		{
			exit_status = exec_builtins(cmd);
			free_minishell(cmd->minishell);
			_exit(exit_status);
		}
		else
			cmd_missing_command_error(cmd);
	}
	exit_status = validate_dots(cmd);
	if (exit_status != EXIT_SUCCESS )
		_exit(exit_status);

	execve(cmd->binary, cmd->argv, cmd->minishell->env);
	child_execve_error(cmd);
}

static int	handle_dup_and_close(int in_fd, int *fds, t_cmd *cmd)
{
	if (apply_redirections(cmd) != EXIT_SUCCESS)
		return (-1);

	if (!cmd->in_redir && in_fd != STDIN_FILENO)
	{
		if (dup2(in_fd, STDIN_FILENO) == -1)
			return (-1);
		close(in_fd);
	}
	if (!cmd->out_redir && cmd->next && dup2(fds[1], STDOUT_FILENO) == -1)
		return (-1);

	if (fds[0] != -1)
		close(fds[0]);
	if (fds[1] != -1)
		close(fds[1]);
	return (EXIT_SUCCESS);
}

static int	handle_child(t_cmd *cmd, int in_fd, int *fds)
{
	if (handle_dup_and_close(in_fd, fds, cmd) == -1)
		_exit(EXIT_FAILURE);
	execute_command(cmd);
	return (EXIT_SUCCESS);
}

static int	wait_for_children(pid_t *pids, int count, uint8_t *exit_status)
{
	int i = 0;
	while (i < count)
	{
		int status;
		if (waitpid(pids[i], &status, 0) != -1 && i == count - 1)
		{
			if (WIFEXITED(status))
				*exit_status = WEXITSTATUS(status);
			else if (WIFSIGNALED(status))
				*exit_status = 128 + WTERMSIG(status);
			else
				*exit_status = EXIT_FAILURE;
		}
		i++;
	}
	return (EXIT_SUCCESS);
}

static int	create_pipe_if_needed(t_cmd *cmd, int *fds)
{
	fds[0] = -1;
	fds[1] = -1;
	if (cmd->next && pipe(fds) == -1)
	{
		perror("-minishell: pipe");
		return (-1);
	}
	return (0);
}

static pid_t	fork_command(t_cmd *cmd, int in_fd, int *fds)
{
	pid_t pid = fork();
	if (pid == -1)
	{
		perror("-minishell: fork");
		return (-1);
	}
	else if (pid == 0)
	{
		handle_child(cmd, in_fd, fds);
	}
	return (pid);
}

static void	cleanup_parent_fds(int *in_fd, int *fds)
{
	if (*in_fd != STDIN_FILENO)
		close(*in_fd);
	if (fds[1] != -1)
		close(fds[1]);
	*in_fd = fds[0];
}

static bool	check_pipeline_limit(int cmd_count)
{
	if (cmd_count >= MAX_CMDS)
	{
		print_error("-minishell: too many commands in pipeline\n");
		return (true);
	}
	return (false);
}

uint8_t	exec_in_child_process(t_cmd *cmd)
{
	uint8_t		exit_status = EXIT_FAILURE;
	int			in_fd = STDIN_FILENO;
	int			fds[2];
	int			cmd_count = 0;
	pid_t		pids[MAX_CMDS];

	while (cmd)
	{
		if (check_pipeline_limit(cmd_count))
			return (EXIT_FAILURE);

		if (create_pipe_if_needed(cmd, fds) == -1)
			return (EXIT_FAILURE);

		pids[cmd_count] = fork_command(cmd, in_fd, fds);
		if (pids[cmd_count] == -1)
			return (EXIT_FAILURE);

		cleanup_parent_fds(&in_fd, fds);

		cmd = cmd->next;
		cmd_count++;
	}
	wait_for_children(pids, cmd_count, &exit_status);
	return (exit_status);
}
