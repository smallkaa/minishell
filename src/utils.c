#include "minishell.h"

/**
 * Prints an error message to `stderr` when a command is not found
 * and exits with the last recorded exit status of the shell.
 *
 * - Uses the `write()` system call for direct output.
 * - Displays the command name inside single quotes.
 * - Exits with the shell's last exit status (`cmd->minishell->exit_status`).
 *
 * @param cmd  The command structure containing the attempted command.
 *			 - `cmd->argv[0]` holds the name of the missing command.
 *			 - `cmd->minishell->exit_status` stores the exit status.
 *
 * @return	 This function does not return (calls `exit()`).
 */
void	command_not_found_handle(t_cmd *cmd)
{
	// write(STDERR_FILENO, "Command '", 9);
	write(STDERR_FILENO, cmd->argv[0], ft_strlen(cmd->argv[0]));
	write(STDERR_FILENO, ": command not found\n", 20);
}

/**
 * cmd_error_handler - Prints an error message for a failed command execution.
 *
 * Format:
 *   minishell: <command>: <arg> (if available): <error_message>
 *
 * Behavior:
 * - Extracts the error message from `strerror(errno)`.
 * - Handles cases where `cmd` or `cmd->argv[0]` is NULL.
 * - Uses a buffer (`ERROR_BUF_SIZE 256`) to construct the message safely.
 * - Ensures no buffer overflows using `ft_strlcpy()` and `ft_strlcat()`.
 * - Writes the formatted error message to `STDERR_FILENO` in a single
 *   `write()` call.
 * - If `write()` fails, prints a fallback error message.
 *
 * @param cmd Pointer to the `t_cmd` structure containing command arguments.
 */
void	cmd_error_handler(t_cmd *cmd)
{
	char	error_buf[ERROR_BUF_SIZE];
	int		err_num;

	if (!cmd || !cmd->argv || !cmd->argv[0])
	{
		write(STDERR_FILENO, "minishell: invalid command structure\n", 37);
		return ;
	}
	err_num = errno;
	ft_strlcpy(error_buf, "minishell: ", ERROR_BUF_SIZE);
	ft_strlcat(error_buf, cmd->argv[0], ERROR_BUF_SIZE);
	if (cmd->argv[1])
	{
		ft_strlcat(error_buf, ": ", ERROR_BUF_SIZE);
		ft_strlcat(error_buf, cmd->argv[1], ERROR_BUF_SIZE);
	}
	ft_strlcat(error_buf, ": ", ERROR_BUF_SIZE);
	ft_strlcat(error_buf, strerror(err_num), ERROR_BUF_SIZE);
	ft_strlcat(error_buf, "\n", ERROR_BUF_SIZE);
	if (write(STDERR_FILENO, error_buf, ft_strlen(error_buf)) < 0)
		write(STDERR_FILENO, "minishell: error: failed to print error\n", 40);
}

void	env_error_handler(t_cmd *cmd)
{
	char	error_buf[ERROR_BUF_SIZE];
	int		err_num;

	if (!cmd || !cmd->argv || !cmd->argv[0])
	{
		write(STDERR_FILENO, "minishell: invalid command structure\n", 37);
		return ;
	}
	err_num = ENOENT;
	ft_strlcpy(error_buf, "", ERROR_BUF_SIZE);
	ft_strlcat(error_buf, cmd->argv[0], ERROR_BUF_SIZE);
	if (cmd->argv[1])
	{
		ft_strlcat(error_buf, ": ", ERROR_BUF_SIZE);
		ft_strlcat(error_buf, cmd->argv[1], ERROR_BUF_SIZE);
	}
	ft_strlcat(error_buf, ": ", ERROR_BUF_SIZE);
	ft_strlcat(error_buf, strerror(err_num), ERROR_BUF_SIZE);
	ft_strlcat(error_buf, "\n", ERROR_BUF_SIZE);
	if (write(STDERR_FILENO, error_buf, ft_strlen(error_buf)) < 0)
		write(STDERR_FILENO, "minishell: error: failed to print error\n", 40);
}

void	print_error(char *cmd)
{
	ft_putstr_fd(cmd, STDERR_FILENO);
}

bool is_debug_mode()
{
	return (getenv("MINISHELL_DEBUG") != NULL);
}

void debug_printf(const char *format, ...)
{
	if(is_debug_mode())
	{
		va_list args;
		va_start(args, format);
		vprintf(format, args);
		va_end(args);
	}
}
int	ft_arr_size(char **arr)
{
	int	len;

	len = 0;
	while (arr && arr[len])
		len++;
	return (len);
}

/**
 * @brief Retrieves the value of an environment variable from the hash table.
 *
 * This function searches for an environment variable stored in the Minishell
 * hash table by computing its hash index and traversing the linked list of
 * stored variables.
 *
 * @param mshell Pointer to the Minishell structure containing the hash table.
 * @param key The name of the environment variable to retrieve.
 * @return Pointer to the value of the variable if found, or NULL if not found.
 *
 * @note All values stors in the hash table for faster search, export, and unset
 */
char	*ms_getenv(t_mshell *mshell, char *key)
{
	unsigned int	index;
	t_mshell_var	*curr;

	index = hash_function(key);
	curr = mshell->hash_table->buckets[index];
	while (curr)
	{
		if (ft_strcmp(curr->key, key) == 0)
			return (curr->value);
		curr = curr->next;
	}
	return (NULL);
}
