#include "minishell.h"
/*
int chdir(const char *path);

The chdir() function shall cause the directory named 
by the pathname pointed to by the path argument to become
the current working directory; that is, the starting point 
for path searches for pathnames not beginning with '/'.

Upon successful completion, 0 shall be returned. 
Otherwise, -1 shall be returned, the current working directory 
shall remain unchanged, and errno shall be set to indicate the error.

ERRORS:

1. ENOENT
bash: cd: gfgf: No such file or directory

2. EACCES
mkdir restricted_dir
chmod 000 restricted_dir  # No permissions
./minishell restricted_dir

bash: cd: test/: Permission denied

3. ENOTDIR 
bash: cd: executor.c: Not a directory

4. ENAMETOOLONG
LONG_PATH=$(printf 'a%.0s' {1..5000})  # Create a long path
./minishell "/tmp/$LONG_PATH"

bash: /tmp/aaaaaa......aaaaaaaa: File name too long

5. ELOOP
ln -s loop_dir loop_dir  # Create a symlink pointing to itself
./minishell cd loop_dir

bash: cd: loop_dir: Too many levels of symbolic links
*/

/**
 * handle_cd - Handles the 'cd' command to change the working directory.
 *
 * Behavior:
 * - If `cmd->argv[1]` is present, it attempts to change to the specified directory.
 * - If `cmd->argv[1]` is missing, it should default to `$HOME`.
 * - If `chdir()` fails, an error message is printed, and the exit status is updated.
 * - If the command is **executed inside a pipe**, it exits with the failure status.
 *
 * @param cmd Pointer to the `t_cmd` structure containing command arguments.
 */

void	handle_cd(t_cmd *cmd)
{
	int		status;
	char	*path;

	status = EXIT_SUCCESS;

	// if (!cmd->argv[1])
	// 	path = $HOME  // replace with func 
	// else	
	
	path = cmd->argv[1];
	if (chdir(path) != 0)
	{
		status = EXIT_FAILURE;
		update_last_exit_status(cmd, status);
		error_handler(cmd);
		if (cmd->in_pipe)
			exit (status);
		return ;
	}
	update_last_exit_status(cmd, status);
	if (cmd->in_pipe)
		exit (status);
	return ;
}
