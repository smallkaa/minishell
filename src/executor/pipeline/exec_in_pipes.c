#include "minishell.h"

static uint8_t validate_dots(t_cmd *cmd)
{
	if (ft_strcmp(cmd->argv[0], ".") == 0 && !cmd->argv[1])
	{
		print_error("-minishell: .: filename argument required\n");
		print_error(".: usage: . filename [arguments]\n");
		return (2);
	}
	else if (ft_strcmp(cmd->argv[0], "..") == 0 && !cmd->argv[1])
	{
		print_error("..: command not found\n");
		return (127);
	}
	return (EXIT_SUCCESS);
}

uint8_t update_shlvl(t_cmd *cmd)
{
	int shlvl;
	char *str_shlvl;
	char *new_shlvl;

	str_shlvl = ms_getenv(cmd->minishell, "SHLVL");
	if (str_shlvl)
		shlvl = ft_atoi(str_shlvl) + 1;
	else
		shlvl = 1;
	new_shlvl = ft_itoa(shlvl);
	if (!new_shlvl)
		return (EXIT_FAILURE);
	set_variable(cmd->minishell, "SHLVL", new_shlvl, 1);
	free(new_shlvl);
	update_env(cmd->minishell);
	return (EXIT_SUCCESS);
}

bool is_minishell_executable(t_cmd *cmd)
{
	return (ft_strcmp(cmd->argv[0], "./minishell") == 0);
}

static uint8_t execute_command(t_cmd *cmd)
{
	uint8_t exit_status;

	if (is_minishell_executable(cmd))
	{
		if (update_shlvl(cmd) == EXIT_FAILURE)
			_exit(EXIT_FAILURE);
	}
	if (ft_strcmp(cmd->argv[0], "") == 0 && !cmd->redirs)
	{
		print_error("Command '' not found\n");
		_exit(127);
	}
	if (ft_strcmp(cmd->argv[0], "") == 0 && cmd->redirs)
	{
		// print_error("Command '' not found\n");
		_exit(EXIT_SUCCESS);
	}
	if (cmd->binary == NULL)
	{
		if (is_builtin(cmd))
		{
			// printf("DEBUG: execute_command() is_builtin success\n");

			exit_status = exec_builtins(cmd, 0);
			free_minishell(cmd->minishell);

			// printf("DEBUG: execute_command() exiting...\n");

			_exit(exit_status);
		}
		else
			cmd_missing_command_error(cmd);
	}
	exit_status = validate_dots(cmd);
	if (exit_status != EXIT_SUCCESS)
		_exit(exit_status);

	// printf("DEBUG: execute_command() ready for execve()\n");

	execve(cmd->binary, cmd->argv, cmd->minishell->env);
	child_execve_error(cmd);
	return (EXIT_FAILURE);
}

uint8_t close_unused_fds(int in_fd, int *pipe_fd)
{
	if (in_fd != STDIN_FILENO)
		if (close(in_fd) == -1)
			perror_return("-close_unused_fds: close in_fd ", EXIT_FAILURE);
	if (pipe_fd[0] != -1)
		if (close(pipe_fd[0]) == -1)
			perror_return("-close_unused_fds: close pipe_fd[0]", EXIT_FAILURE);
	if (pipe_fd[1] != -1)
		if (close(pipe_fd[1]) == -1)
			perror_return("-close_unused_fds: close pipe_fd[1]", EXIT_FAILURE);
	return (EXIT_SUCCESS);
}

uint8_t exec_in_pipes(t_cmd *cmd)
{
	int in_fd;
	int pipe_fd[2];
	pid_t pid;
	pid_t pids[MAX_CMDS];
	int idx;
	uint8_t exit_status;
	int i;
	int status;

	idx = 0;
	in_fd = STDIN_FILENO;
	exit_status = EXIT_SUCCESS;
	pipe_fd[0] = -1;
	pipe_fd[1] = -1;
	while (cmd)
	{
		// printf("DEBUG: exec_in_pipes: in while loop\n");

		if (cmd->next && pipe(pipe_fd) == -1)
		{
			perror("exec_in_pipes: pipe");
			_exit(EXIT_FAILURE);
		}
		// printf("DEBUG: exec_in_pipes: create a pipe\n");

		pid = fork();
		if (pid == -1)
		{
			perror("exec_in_pipes: fork");
			_exit(EXIT_FAILURE);
		}
		// printf("DEBUG: exec_in_pipes: fork success\n");

		if (pid == 0)
		{
			if (cmd->next)
				if (dup2(pipe_fd[1], STDOUT_FILENO) == -1)
				{
					perror("-exec_in_pipes: dup2 pipe_fd[1]");
					exit(EXIT_FAILURE);
				}
			// printf("DEBUG: dup2(pipe_fd[1], STDOUT_FILENO) success\n");

			if (in_fd != STDIN_FILENO)
				if (dup2(in_fd, STDIN_FILENO) == -1)
				{
					perror("-exec_in_pipes: dup2 in_fd");
					exit(EXIT_FAILURE);
				}

			// printf("DEBUG: in_fd == STDIN_FILENO\n");

			exit_status = apply_redirections(cmd);

			if (exit_status != EXIT_SUCCESS)
				exit(exit_status);

			// printf("DEBUG: apply_redirections() success\n");

			exit_status = close_unused_fds(in_fd, pipe_fd);

			if (exit_status != EXIT_SUCCESS)

				exit(exit_status);

			// printf("DEBUG: close_unused_fds() success\n");

			return (execute_command(cmd));
		}

		pids[idx++] = pid;

		t_list *rlist = cmd->redirs;
		while (rlist)
		{
			t_redir *r = (t_redir *)rlist->content;
			if (r->type == R_HEREDOC && r->fd != -1)
			{
				close(r->fd);
				r->fd = -1;
			}
			rlist = rlist->next;
		}

		if (pipe_fd[1] >= 0)
			if (close(pipe_fd[1]) == -1)
			{
				perror("-exec_in_pipes: close pipe_fd[1]");
				exit(EXIT_FAILURE);
			}

		if (in_fd != STDIN_FILENO && in_fd > STDIN_FILENO)
			if (close(in_fd) == -1)
			{
				perror("-exec_in_pipes: close in_fd");
				exit(EXIT_FAILURE);
			}
		in_fd = pipe_fd[0];
		pipe_fd[0] = pipe_fd[1] = -1;
		cmd = cmd->next;
	}

	i = 0;
	while (i < idx)
	{
		waitpid(pids[i], &status, 0);
		if (i == idx - 1)
		{
			if (WIFEXITED(status))
				exit_status = WEXITSTATUS(status);
			else if (WIFSIGNALED(status))
				exit_status = 128 + WTERMSIG(status);
			else
				exit_status = EXIT_FAILURE;
		}
		i++;
	}
	return (exit_status);
}
