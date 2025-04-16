#include "minishell.h"

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
 * @param arg The argument string containing a variable name
 * (and optional value).
 * @return `EXIT_SUCCESS` (0) if the variable was successfully processed.
 *         `EXIT_FAILURE` (1) if the argument is invalid.
 */
static uint8_t	process_export_arg(t_cmd *cmd, char *arg)
{
	char			*eq;
	int				assigned;
	t_mshell_var	*pair;

	pair = split_key_value(arg);
	if (!is_valid_varname(pair->key))
	{
		export_error(pair);
		free(pair->key);
		free(pair->value);
		free(pair);
		return (EXIT_FAILURE);
	}
	eq = ft_strchr(arg, '=');
	if (eq != NULL)
		assigned = 1;
	else
		assigned = 0;
	set_variable(cmd->minishell, pair->key, pair->value, assigned);
	free(pair->key);
	free(pair->value);
	free(pair);
	return (EXIT_SUCCESS);
}

/**
 * @brief Prints the bash-like usage message for invalid options.
 */
static void	print_export_usage(char *arg)
{
	print_error("minishell: export: ");
	print_error(arg);
	print_error(": invalid option\n" \
		"export: usage: export [-fn] [name[=value] ...] or export -p\n");
}

/**
 * @brief Checks if the given argument is an invalid export "option".
 *
 * Since we're not supporting any valid options, anything starting
 * with '-' is invalid.
 */
static bool	is_invalid_option(const char *arg)
{
	if (!arg)
		return (false);
	if (arg[0] == '-' && arg[1] != '\0')
		return (true);
	return (false);
}

/**
 * @brief Handles the `export` built-in command in Minishell
 * (no options supported).
 *
 * Behavior:
 * - If `export` is called **without arguments**,
 * it prints the current exported variables.
 * - If called with arguments:
 *   -- If argument starts with '-', print usage & return
 * 2 immediately (invalid option).
 *   -- Else, validate the variable name.
 * If invalid => not a valid identifier => exit code 1.
 * - If multiple invalid variables appear, the final exit code is
 * 1 if at least one was invalid.
 * - If executed in a pipeline, the process exits with the appropriate status.
 *
 * @param cmd Pointer to the command structure.
 * @return 0 if all OK, 1 if invalid identifier, 2 if invalid option.
 */
uint8_t	handle_export(t_cmd *cmd)
{
	uint8_t	exit_status;
	int		i;
	uint8_t	ret;

	exit_status = 0;
	if (!cmd->argv[1])
	{
		handle_sorted_env(cmd->minishell);
		return (EXIT_SUCCESS);
	}
	i = 1;
	while (cmd->argv[i])
	{
		if (is_invalid_option(cmd->argv[i]))
		{
			print_export_usage(cmd->argv[i]);
			return (2);
		}
		ret = process_export_arg(cmd, cmd->argv[i]);
		if (ret == EXIT_FAILURE)
			exit_status = EXIT_FAILURE;
		i++;
	}
	update_env(cmd->minishell);
	return (exit_status);
}
