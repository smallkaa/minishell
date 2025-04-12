#include "minishell.h"
#include "tokenizer.h"

// Создаёт пустую команду с argv[0] = ""
t_cmd *create_empty_command(t_mshell *shell)
{
	t_cmd	*cmd;

	cmd = malloc(sizeof(t_cmd));
	if (!cmd)
		return (NULL);
	memset(cmd, 0, sizeof(t_cmd));

	cmd->argv = malloc(sizeof(char *) * 2);
	if (!cmd->argv)
	{
		free(cmd);
		return (NULL);
	}
	cmd->argv[0] = ft_strdup("");
	if (!cmd->argv[0])
	{
		free(cmd->argv);
		free(cmd);
		return (NULL);
	}
	cmd->argv[1] = NULL;
	cmd->minishell = shell;
	cmd->next = NULL;
	cmd->redirs = NULL;

	return (cmd);
}

int	is_input_redir(t_TokenType type)
{
	return (type == TOKEN_REDIRECT_IN || type == TOKEN_HEREDOC);
}

int	is_output_redir(t_TokenType type)
{
	return (type == TOKEN_REDIRECT_OUT || type == TOKEN_APPEND_OUT);
}

void	free_list_nodes_only(t_list **lst)
{
	t_list	*tmp;
	t_list	*next;

	tmp = *lst;
	while (tmp)
	{
		next = tmp->next;
		free(tmp);
		tmp = next;
	}
	*lst = NULL;
}

t_cmd	*finalize_cmd_list(t_list **cmd_list)
{
	t_cmd	*head;
	t_cmd	*prev;
	t_list	*iter;

	head = NULL;
	prev = NULL;
	iter = *cmd_list;
	while (iter)
	{
		if (!head)
			head = (t_cmd *)iter->content;
		if (prev)
			prev->next = (t_cmd *)iter->content;
		prev = (t_cmd *)iter->content;
		iter = iter->next;
	}
	free_list_nodes_only(cmd_list);
	return (head);
}

void	free_cmd_list(t_list **cmd_list)
{
	t_list	*tmp;
	t_cmd	*cmd;

	tmp = *cmd_list;
	while (tmp)
	{
		cmd = (t_cmd *)tmp->content;
		free_cmd(cmd);
		tmp = tmp->next;
	}
	// ft_lstclear(cmd_list, NULL); // Нельзя это вызывать с NULL
}

void	handle_word_token(t_mshell *shell, t_list **cmd_list,
	t_cmd **current, char *value)
{
	int	arg_index;

	arg_index = 0;
	if (!(*current))
	{
		*current = malloc(sizeof(t_cmd));
		if (!(*current))
			return ;
		ft_bzero(*current, sizeof(t_cmd));
		(*current)->argv = malloc(sizeof(char *) * (MAX_ARGS + 1));
		if (!(*current)->argv)
			return (free(*current), *current = NULL, (void)0);
		(*current)->argv[0] = ft_strdup(value);
		(*current)->minishell = shell;
		ft_lstadd_back(cmd_list, ft_lstnew(*current));
	}
	else
	{
		while ((*current)->argv[arg_index])
			arg_index++;
		if (arg_index < MAX_ARGS)
			(*current)->argv[arg_index++] = ft_strdup(value);
	}
}

int	handle_input_redir(t_mshell *shell, t_list **cmd_list,
	t_cmd **current, t_Token *token)
{
	t_redir	*redir;

	if (!(*current))
	{
		*current = create_empty_command(shell);
		if (!(*current))
			return (-1);
		ft_lstadd_back(cmd_list, ft_lstnew(*current));
	}
	if (!(token + 1) || (token + 1)->type != TOKEN_WORD)
	{
		print_error("Syntax error: missing name for redirection\n");
		return (-1);
	}
	redir = malloc(sizeof(t_redir));
	if (!redir)
		return (-1);
	redir->type = (token->type == TOKEN_REDIRECT_IN) ? R_INPUT : R_HEREDOC;
	redir->filename = ft_strdup((token + 1)->value);
	if (!redir->filename)
		return (free(redir), -1);
	ft_lstadd_back(&(*current)->redirs, ft_lstnew(redir));
	return (0);
}

int	handle_output_redir(t_mshell *shell, t_list **cmd_list,
	t_cmd **current, t_Token *token)
{
	t_redir	*redir;

	if (!(*current))
	{
		*current = create_empty_command(shell);
		if (!(*current))
			return (-1);
		ft_lstadd_back(cmd_list, ft_lstnew(*current));
	}
	if (!(token + 1) || (token + 1)->type != TOKEN_WORD)
	{
		print_error("Syntax error: missing name for redirection\n");
		return (-1);
	}
	redir = malloc(sizeof(t_redir));
	if (!redir)
		return (-1);
	redir->type = (token->type == TOKEN_REDIRECT_OUT) ? R_OUTPUT : R_APPEND;
	redir->filename = ft_strdup((token + 1)->value);
	if (!redir->filename)
		return (free(redir), -1);
	ft_lstadd_back(&(*current)->redirs, ft_lstnew(redir));
	return (0);
}