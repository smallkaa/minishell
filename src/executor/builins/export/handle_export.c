/**
 * @file handle_export.c
 * @brief Functions for handling the `export` built-in command in Minishell.
 */
#include "minishell.h"
/**
 * @brief Frees the dynamically allocated memory for sorted environment
 *        variable keys.
 *
 * This function iterates through an array of dynamically allocated
 * environment variable keys, freeing each key and then freeing the array itself.
 *
 * @param keys Array of allocated strings containing environment variable keys.
 * @param num_keys Number of keys in the array.
 */
void	free_keys(char **keys, int num_kyes)
{
	int	i;

	if (!keys)
		return ;
	i = 0;
	while (i < num_kyes)
	{
		free(keys[i]);
		i++;
	}
	free(keys);
}

/**
 * @brief Validates if a given string is a valid variable name for Minishell.
 *
 * A valid variable name must:
 * - Start with an alphabetic character (`A-Z`, `a-z`) or an underscore (`_`).
 * - Contain only alphanumeric characters (`A-Z`, `a-z`, `0-9`) or
 * underscores (`_`).
 * - Stop validation at the first `=` character or at the end of the string.
 *
 * This function ensures that variables added to the environment conform to
 * valid naming conventions.
 *
 * @param key The string representing the variable name (e.g., `"MYVAR=value"`).
 * @return `true` if the name is valid, `false` otherwise.
 */
bool	is_valid_varname(const char *key)
{
	int	i;

	if (!key || !(ft_isalpha(key[0]) || key[0] == '_'))
		return (false);
	i = 1;
	while (key[i] && key[i] != '=')
	{
		if (!(ft_isalnum(key[i]) || key[i] == '_'))
			return (false);
		i++;
	}
	return (true);
}

/**
 * @brief Processes a single argument for the `export` command.
 *
 * This function:
 * - Validates whether the argument represents a valid variable name.
 * - If valid, updates or adds the variable in the shell’s environment.
 * - If an `=` is present, assigns the variable a value.
 *
 * If the variable name is invalid, an error message is printed.
 *
 * @param cmd Pointer to the command structure containing environment context.
 * @param arg The argument string containing a variable name (and optional
 *            value).
 * @return `EXIT_SUCCESS` (0) if the variable was successfully processed.
 *         `EXIT_FAILURE` (1) if the argument is invalid.
 *
 * */
static uint8_t	process_export_arg(t_cmd *cmd, char *arg)
{
	char			*eq;
	int				assigned;
	t_mshell_var	*pair;

	pair = split_key_value(arg);
	if (!is_valid_varname(pair->key))
	{
		print_error("-minishell: export: '");
		print_error(pair->key);
		print_error("': not a valid identifier\n");
		return (EXIT_FAILURE);
	}
	eq = ft_strchr(arg, '=');
	assigned = 0;
	if (eq)
		assigned = 1;
	set_variable(cmd->minishell, pair->key, pair->value, assigned);
	free(pair->key);
	free(pair->value);
	free(pair);
	return (EXIT_SUCCESS);
}

/**
 * @brief Handles the `export` built-in command in Minishell.
 *
 * Behavior:
 * - If `export` is called **without arguments**, it prints the current exported
 *   variables.
 * - If called with arguments, it processes and updates the environment
 *   variables accordingly.
 * - If executed in a pipeline, the process exits with the appropriate status.
 *
 * @param cmd Pointer to the command structure.
 * @return `EXIT_SUCCESS` (0) if all operations were successful.
 *         `EXIT_FAILURE` (1) if an invalid argument was provided.
 */
uint8_t	handle_export(t_cmd *cmd)
{
	int		i;
	uint8_t	exit_status;

	if (!cmd->argv[1])
	{
		handle_sorted_env(cmd->minishell);
		exit_status = EXIT_SUCCESS;
		return (exit_status);
	}
	i = 1;
	while (cmd->argv[i])
	{
		exit_status = process_export_arg(cmd, cmd->argv[i]);
		i++;
	}
	update_env(cmd->minishell);
	return (exit_status);
}
