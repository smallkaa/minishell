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
	shlvl = str_shlvl ? ft_atoi(str_shlvl) + 1 : 1;
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

static void execute_command(t_cmd *cmd)
{
	uint8_t exit_status;

	if (is_minishell_executable(cmd))
	{
		if (update_shlvl(cmd) == EXIT_FAILURE)
			_exit(EXIT_FAILURE);
	}
	if (ft_strcmp(cmd->argv[0], "") == 0)
	{
		print_error("Command '' not found\n");
		_exit(127);
	}
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
	if (exit_status != EXIT_SUCCESS)
		_exit(exit_status);

	execve(cmd->binary, cmd->argv, cmd->minishell->env);
	child_execve_error(cmd);
}

/* ── Redirection helpers ─────────────────────────────────────────────── */

bool cmd_has_any_input_redir(t_cmd *c)
{
	for (t_list *n = c->redirs; n; n = n->next)
	{
		t_redir *r = n->content;
		if (r->type == R_INPUT || r->type == R_HEREDOC)
			return true;
	}
	return false;
}

uint8_t apply_output_redirs(t_cmd *c)
{
	t_list *n = c->redirs;
	while (n)
	{
		t_redir *r = n->content;
		int fd = -1;

		if (r->type == R_OUTPUT)
			fd = open(r->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		else if (r->type == R_APPEND)
			fd = open(r->filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
		else
		{
			n = n->next;
			continue;
		}
		if (fd == -1)
		{
			perror("apply_output_redirs: open");
			return (EXIT_FAILURE);
		}
		dup2(fd, r->target_fd);
		close(fd);
		n = n->next;
	}
	return (EXIT_SUCCESS);
}

uint8_t apply_input_redirs(t_cmd *c, int prev_pipe_fd)
{
	t_list *n = c->redirs;
	while (n)
	{
		t_redir *r = n->content;
		if (r->type == R_HEREDOC)
		{
			dup2(r->fd, STDIN_FILENO);
			close(r->fd);
		}
		else if (r->type == R_INPUT)
		{
			int fd = open(r->filename, O_RDONLY);
			if (fd == -1)
			{
				perror("apply_input_redirs: open:");
				return (EXIT_FAILURE);
			}
			dup2(fd, STDIN_FILENO);
			close(fd);
		}
		n = n->next;
	}
	if (!cmd_has_any_input_redir(c) && prev_pipe_fd != STDIN_FILENO)
		dup2(prev_pipe_fd, STDIN_FILENO);

	return (EXIT_SUCCESS);
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

/* ── Pipeline execution ───────────────────────────────────────────────── */

uint8_t exec_in_child_process(t_cmd *cmd)
{
	int in_fd = STDIN_FILENO;
	int pipe_fd[2] = {-1, -1};
	pid_t pids[MAX_CMDS];
	int idx = 0;
	uint8_t exit_status = 0;

	while (cmd)
	{
		if (cmd->next && pipe(pipe_fd) == -1)
			perror("pipe");

		pid_t pid = fork();
		if (pid == -1)
			perror("fork");

		if (pid == 0)
		{
			if (cmd->next)
				dup2(pipe_fd[1], STDOUT_FILENO);
			exit_status = apply_input_redirs(cmd, in_fd);
			if (exit_status != EXIT_SUCCESS)
				exit(exit_status);
			exit_status = apply_output_redirs(cmd);
			if (exit_status != EXIT_SUCCESS)
				exit(exit_status);
			exit_status = close_unused_fds(in_fd, pipe_fd);
			if (exit_status != EXIT_SUCCESS)
				exit(exit_status);
			execute_command(cmd);
			perror("execve failed");
			exit(EXIT_FAILURE);
		}

		pids[idx++] = pid;
		close(pipe_fd[1]);
		if (in_fd != STDIN_FILENO)
			close(in_fd);
		in_fd = pipe_fd[0];
		pipe_fd[0] = pipe_fd[1] = -1;
		cmd = cmd->next;
	}

	for (int i = 0; i < idx; ++i)
	{
		int status;
		waitpid(pids[i], &status, 0);
		if (i == idx - 1)
			exit_status = WIFEXITED(status) ? WEXITSTATUS(status)
												: 128 + WTERMSIG(status);
	}
	return exit_status;
}

/* ── Heredoc setup ───────────────────────────────────────────────────── */

int new_heredoc_fd(const char *delim)
{
	int p[2];
	if (pipe(p) == -1)
		perror("pipe");
	if (write_heredoc_to_pipe(p[1], delim) != EXIT_SUCCESS)
		perror("write heredoc");
	close(p[1]);
	return p[0];
}

bool is_heredoc(t_redir *r)
{
	return (r->type == R_HEREDOC);
}

void setup_all_heredocs(t_cmd *cmd)
{
	while (cmd)
	{
		t_list *rlist = cmd->redirs;
		while (rlist)
		{
			t_redir *r = rlist->content;
			if (is_heredoc(r))
				r->fd = new_heredoc_fd(r->filename);
			rlist = rlist->next;
		}
		cmd = cmd->next;
	}
}
