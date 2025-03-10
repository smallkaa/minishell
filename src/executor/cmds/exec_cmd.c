#include "minishell.h"

static void execute_command(t_cmd *cmd)
{
	if (cmd->binary == NULL)
	{
		if (is_builtin(cmd))
			exec_builtin(cmd);
		else
		{
			command_not_found_handle(cmd);
			_exit(127);
		}
	}
	execve(cmd->binary, cmd->argv, cmd->minishell->env);
	perror("execve");
	_exit(127);
}
static void child_process(t_cmd *cmd, int in_fd, int fds[2])
{
	if (in_fd != STDIN_FILENO)
	{
		if (dup2(in_fd, STDIN_FILENO) == -1)
		{
			perror("dup2 in_fd");
			_exit(1);
		}
		close(in_fd);
	}

	if (cmd->next)
	{
		if (dup2(fds[1], STDOUT_FILENO) == -1)
		{
			perror("dup2 fds[1]");
			_exit(1);
		}
	}

	if (fds[0] >= 0)
		close(fds[0]);
	if (fds[1] >= 0)
		close(fds[1]);

	if (cmd->in_redir || cmd->out_redir)
		handle_redirections(cmd, STDIN_FILENO);
	execute_command(cmd);
}
static int parent_process(t_cmd *cmd, int in_fd, int fds[2], pid_t pid)
{
	int status;
	int new_in_fd = STDIN_FILENO;

	if (cmd->next)
		close(fds[1]); // ✅ Only close fds[1] if it was initialized

	if (in_fd != 0)
		close(in_fd);

	if (cmd->next)
		new_in_fd = fds[0]; // ✅ Keep fds[0] for next cmd
	else if (fds[0] >= 0)
		close(fds[0]);  // ✅ Close fds[0] only if initialized

	waitpid(pid, &status, 0);

	if (WIFEXITED(status))
		update_last_exit_status(cmd, WEXITSTATUS(status));
	else if (WIFSIGNALED(status))
		update_last_exit_status(cmd, 128 + WTERMSIG(status));

	return new_in_fd;
}

void exec_cmd(t_cmd *cmd)
{
	int in_fd = STDIN_FILENO;
	int fds[2] = {-1, -1};
	pid_t pid;
	int cmd_count = 0; // Track how many commands in the pipeline

	while (cmd)
	{
		if (++cmd_count > MAX_CMDS)
		{
			print_error("Too many commands in pipeline.");
			return;
		}

		if (cmd->next)
		{
			if (pipe(fds) == -1)
			{
				perror("pipe");
				exit(1);
			}
		}
		else
		{
			fds[0] = -1;
			fds[1] = -1;
		}

		pid = fork();
		if (pid == -1)
		{
			perror("fork");
			exit(1);
		}

		if (pid == 0)
			child_process(cmd, in_fd, fds);
		else
			in_fd = parent_process(cmd, in_fd, fds, pid);

		cmd = cmd->next;
	}
}
