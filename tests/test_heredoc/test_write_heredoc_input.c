#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <assert.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "libft.h"

#define HEREDOC_TFILE "heredoc_tmp.txt"

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
	bool			expand;
}	t_redir;

typedef struct s_cmd
{
	char				**argv;
	char				*binary;
	t_redir				*in_redir;
	t_redir				*out_redir;
	struct s_cmd		*next;
}	t_cmd;

void	fatal_error(char *cmd, int exit_status)
{
	ft_putstr_fd(cmd, STDERR_FILENO);
	perror(": ");
	exit(exit_status);
}

static void	write_heredoc_input(int tmp_fd, t_cmd *cmd, char **envp)
{
	char	*line;
	char	*input;

	(void)envp;

	while (1)
	{
		line = readline("> ");
		if (!line) // Ctrl+D (EOF)
			break;
		if (ft_strcmp(line, cmd->in_redir->filename) == 0)
		{
			free(line);
			break;
		}
		input = ft_strdup(line);
		if (write(tmp_fd, input, ft_strlen(input)) == -1)
			fatal_error("write", EXIT_FAILURE);
		if (write(tmp_fd, "\n", 1) == -1)
			fatal_error("write", EXIT_FAILURE);
		free(input);
		free(line);
	}
	if (close(tmp_fd) == -1)
		fatal_error("close", EXIT_FAILURE);
}

void	handle_heredoc(t_cmd *cmd, char **envp)
{
	int	tmp_fd;

	if (!cmd->in_redir || cmd->in_redir->type != R_HEREDOC)
		return;
	tmp_fd = open(HEREDOC_TFILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (tmp_fd < 0)
		fatal_error("heredoc", EXIT_FAILURE);
	write_heredoc_input(tmp_fd, cmd, envp);
}

static void	validate_heredoc_output(void)
{
	int		fd = open(HEREDOC_TFILE, O_RDONLY);
	if (fd < 0)
	{
		perror("Error opening heredoc_tmp.txt for reading");
		exit(EXIT_FAILURE);
	}

	char	buffer[256];
	ssize_t	bytes_read;

	printf("\nContents of heredoc_tmp.txt:\n");
	while ((bytes_read = read(fd, buffer, sizeof(buffer) - 1)) > 0)
	{
		buffer[bytes_read] = '\0';
		write(STDOUT_FILENO, buffer, bytes_read);
	}
	if (bytes_read == -1)
		fatal_error("Error reading file", EXIT_FAILURE);

	if (close(fd) == -1)
		fatal_error("close", EXIT_FAILURE);
}

int main(void)
{
	t_redir heredoc =
	{
		.type = R_HEREDOC,
		.filename = "EOF",
		.expand = false
	};

	t_cmd cmd =
	{
		.argv = (char *[]){"cat", NULL},
		.binary = "/bin/cat",
		.in_redir = &heredoc,
		.out_redir = NULL,
		.next = NULL
	};

	printf("Executing Heredoc. Type lines and finish with 'EOF':\n");
	handle_heredoc(&cmd, NULL);

	validate_heredoc_output();

	// // uncomment for delete temp file
	// sleep(5);
	// unlink(HEREDOC_TFILE);

	return (0);
}
