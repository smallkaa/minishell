
	// test -------------------------------------------------//

	// int j = 0;
	// while(cmd->argv[j])
	// {
	// 	printf("---argv[%d]: {%s}\n", j, cmd->argv[j]);
	// 	j++;
	// }
	// printf("---argv[%d]: {%s}\n", j, cmd->argv[j]);
	// printf("---binary (%p)\n", cmd->binary);

	// // //-----------------------------------------------
	// // Printing redirections

	// j = 0;
	// if (cmd->redirs)
	// {
	// 	t_list *current_redir = cmd->redirs;
	// 	while (current_redir)
	// 	{
	// 		t_redir *redir = (t_redir *)current_redir->content;

	// 		printf("---redirection[%d]: ", j);
	// 		if (redir->type == R_INPUT)
	// 			printf("'<' ");
	// 		else if (redir->type == R_OUTPUT)
	// 			printf("'>' ");
	// 		else if (redir->type == R_APPEND)
	// 			printf("'>>' ");
	// 		else if (redir->type == R_HEREDOC)
	// 			printf("'<<' ");

	// 		if (redir->filename)
	// 			printf("\"%s\"\n", redir->filename);
	// 		else
	// 			printf("(NULL)\n");

	// 		printf("---expand: %s\n", redir->expand ? "true" : "false");

	// 		current_redir = current_redir->next;
	// 		j++;
	// 	}
	// }
	// else
	// {
	// 	printf("---No redirections found\n");
	// }

	// end test -----------------------------------------------//
