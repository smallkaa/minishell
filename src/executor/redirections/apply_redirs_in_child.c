#include "minishell.h"

void apply_redirs_in_child(t_cmd *cmd)
{
	t_list *n;
	t_redir *r;
	int fd;

	n = cmd->redirs;
    while (n)
    {
        r = n->content;

        if (r->type == R_HEREDOC)
		{
            dup2(r->hdoc_fd, r->target_fd);   /* usually target_fd == 0 */
            close(r->hdoc_fd);
        }
        else if (r->type == R_INPUT)
		{
            int fd = open(r->filename, O_RDONLY);
            if (fd == -1) perror("open");
            dup2(fd, r->target_fd);
            close(fd);
        }
        else if (r->type == R_OUTPUT)
		{
            int fd = open(r->filename, O_WRONLY|O_CREAT|O_TRUNC, 0644);
            if (fd == -1)
				perror("open");
            dup2(fd, r->target_fd);
            close(fd);
        }
        else if (r->type == R_APPEND)
		{
            fd = open(r->filename, O_WRONLY|O_CREAT|O_APPEND, 0644);
            if (fd == -1)
				perror("open");
          	dup2(fd, r->target_fd);
            close(fd);
        }
		n = n->next;
    }
}
