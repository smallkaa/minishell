#include "minishell.h"

uint8_t	apply_single_redirection(t_cmd *cmd, t_redir *redir)
{
	uint8_t	exit_status;

	if (redir->type == R_INPUT)
		exit_status = apply_in_redirection(cmd);
	else if (redir->type == R_OUTPUT || redir->type == R_APPEND)
		exit_status = apply_out_redirection(cmd);
	else if (redir->type == R_HEREDOC)
		exit_status = apply_heredoc(cmd);
	else
	{
		print_error("Error: apply_redirections: invalid redirection type\n");
		return (EXIT_FAILURE);
	}
	return (exit_status);
}

uint8_t	apply_redirections(t_cmd *cmd)
{
	t_redir	*redir;
	t_list	*current_redir;
	uint8_t	exit_status;

	if (!cmd)
	{
		print_error("minishell: cmd struct not found\n");
		return (EXIT_FAILURE);
	}
	if (!cmd->redirs)
		return (EXIT_SUCCESS);
	current_redir = cmd->redirs;
	while (current_redir)
	{
		redir = (t_redir *)current_redir->content;
		exit_status = apply_single_redirection(cmd, redir);
		if (exit_status != EXIT_SUCCESS)
			return (exit_status);
		current_redir = current_redir->next;
	}
	return (EXIT_SUCCESS);
}
// to test
// typedef struct s_redir
// {
//     char    *filename;
//     int     type;       // R_INPUT, R_OUTPUT, R_APPEND, R_HEREDOC
//     int     pipefd[2];   // Read-end [0], Write-end [1]
// } t_redir;

// uint8_t apply_single_redirection(t_redir *redir)
// {
//     int fd;

//     if (redir->type == R_HEREDOC)
//     {
//         if (dup2(redir->pipefd[0], STDIN_FILENO) == -1)
//         {
//             perror("minishell: dup2");
//             close(redir->pipefd[0]);
//             return (EXIT_FAILURE);
//         }
//         close(redir->pipefd[0]); // Close read-end after duplicating
//     }
//     else if (redir->type == R_INPUT)
//     {
//         fd = open(redir->filename, O_RDONLY);
//         if (fd == -1)
//         {
//             perror("minishell: open");
//             return (EXIT_FAILURE);
//         }
//         if (dup2(fd, STDIN_FILENO) == -1)
//         {
//             perror("minishell: dup2");
//             close(fd);
//             return (EXIT_FAILURE);
//         }
//         close(fd);
//     }
//     else if (redir->type == R_OUTPUT)
//     {
//         fd = open(redir->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
//         if (fd == -1)
//         {
//             perror("minishell: open");
//             return (EXIT_FAILURE);
//         }
//         if (dup2(fd, STDOUT_FILENO) == -1)
//         {
//             perror("minishell: dup2");
//             close(fd);
//             return (EXIT_FAILURE);
//         }
//         close(fd);
//     }
//     else if (redir->type == R_APPEND)
//     {
//         fd = open(redir->filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
//         if (fd == -1)
//         {
//             perror("minishell: open");
//             return (EXIT_FAILURE);
//         }
//         if (dup2(fd, STDOUT_FILENO) == -1)
//         {
//             perror("minishell: dup2");
//             close(fd);
//             return (EXIT_FAILURE);
//         }
//         close(fd);
//     }
//     return (EXIT_SUCCESS);
// }
