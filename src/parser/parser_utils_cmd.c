/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils_cmd.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pvershin <pvershin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 12:45:40 by pvershin          #+#    #+#             */
/*   Updated: 2025/04/23 12:45:47 by pvershin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

/**
 * @brief Print command arguments in debug mode.
 *
 * @param argv Null-terminated array of argument strings.
 */
static void	print_cmd_arguments(char **argv)
{
	int	i;

	if (!argv)
		return ;
	printf("  Arguments: ");
	i = 0;
	while (i < MAX_ARGS && argv[i])
	{
		printf("    RAW argv[%d] = %p\n", i, (void *)argv[i]);
		if (argv[i])
			printf("\"%s\" ", argv[i]);
		else
			printf("(NULL ARG!) ");
		i++;
	}
	printf("\n");
}

/**
 * @brief Print all redirections in debug mode.
 *
 * @param redir_node List of t_redir nodes.
 */
/**
 * @brief Print a single redirection in debug mode.
 *
 * @param redir Pointer to the t_redir structure.
 */
static void	debug_print_single_redirection(t_redir *redir)
{
	if (!redir)
		return ;
	printf("    Redirection: ");
	if (redir->type == R_INPUT)
		printf("< ");
	else if (redir->type == R_OUTPUT)
		printf("> ");
	else if (redir->type == R_APPEND)
		printf(">> ");
	else if (redir->type == R_HEREDOC)
		printf("<< ");
	else
		printf("[Unknown Type %d] ", redir->type);
	if (redir->filename)
		printf("\"%s\"", redir->filename);
	else
		printf("(NULL Filename!)");
	printf("\n");
}

/**
 * @brief Print all redirections in debug mode.
 *
 * @param redir_node List of t_redir nodes.
 */
static void	print_cmd_redirections(t_list *redir_node)
{
	t_redir	*redir;

	printf("  Redirections (in order):\n");
	if (!redir_node)
	{
		printf("    (None)\n");
		return ;
	}
	while (redir_node)
	{
		redir = (t_redir *)redir_node->content;
		debug_print_single_redirection(redir);
		redir_node = redir_node->next;
	}
}

/**
 * @brief Print parsed commands in debug mode.
 *
 * @param cmd Head of the command list.
 */
void	debug_print_parsed_commands(t_cmd *cmd)
{
	int	cmd_count;

	if (!is_debug_mode())
		return ;
	cmd_count = 1;
	printf("\n==== Parsed Command Structure ====\n");
	while (cmd)
	{
		printf("Command %d:\n", cmd_count);
		if (cmd->binary)
			printf("  Executable: %s\n", cmd->binary);
		else
			printf("  Executable: (NULL)\n");
		print_cmd_arguments(cmd->argv);
		print_cmd_redirections(cmd->redirs);
		if (cmd->next)
			printf("  Piped to next command ->\n");
		cmd = cmd->next;
		cmd_count++;
		if (cmd)
			printf("\n");
	}
	printf("======================================\n\n");
}
