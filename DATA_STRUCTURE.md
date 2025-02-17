## enum e_redir_type

__R_INPUT:__	< file redirection, stdin from file.

__R_OUTPUT:__	> file redirection, stdout to file.

__R_APPEND:__	>> file redirection. stdout to file in append mode.

__R_HEREDOC:__	<< delimiter redirection. Shell reads lines until it find the delimiter, then supply lines on stdin.

```c
typedef enum e_redir_type
{
	R_INPUT,	// "<"
	R_OUTPUT,	// ">"
	R_APPEND,	// ">>"
	R_HEREDOC	// "<<"
}	t_redir_type;
```

## struct s_redir

__type:__ which kind of redirection.

__filename:__ for R_INPUT, R_OUTPUT, or R_APPEND - filename to open, for R_HEREDOC, - delimiter string.

```c
typedef struct s_redir
{
	t_redir_type	type;
	char			*filename;	// or delimiter for heredoc
}	t_redir;
```

__argv:__		a NULL-terminated array of strings.

__argv[0]:__	command name ("ls", "grep").

__argv[1]:__	argv[n-1], arguments (["-l", "myfile"]).

__argv[n]:__	NULL, required by execve().

__binary:__		Binary from PATH env for execve().

__in_redir:__	a pointer to t_redir struct for input redirection.

__out_redir:__	a pointer to t_redir struct for out redirection.

__next:__		points to the next command in a pipeline.
				Example: ls -l | grep minishell | wc -l
				cmd1 "ls -l", cmd2 "grep minishell", cmd3 "wc -l", cdm4 "NULL".

## struct s_command
```c
typedef struct s_command
{
	char				**argv;
	char				*binary;
	t_redir				*in_redir;
	t_redir				*out_redir;
	struct s_command	*next;
}	t_cmd;
```
## Examples

### Example 1: Single Command with No Redirection
```bash
ls -l
```
```c
t_cmd *cmd = malloc(sizeof(t_cmd));
cmd->argv = malloc(3 * sizeof(char *));

cmd->argv[0] = ft_strdup("ls");
cmd->argv[1] = ft_strdup("-l");
cmd->argv[2] = NULL;

cmd->in_redir = NULL;
cmd->out_redir = NULL;

cmd->next = NULL;
```
### Example 2: Single Command with One Input Redirection
```bash
grep "hive" < input.txt
```
```c
t_cmd *cmd = malloc(sizeof(t_cmd));
cmd->argv = malloc(3 * sizeof(char *));

cmd->argv[0] = ft_strdup("grep");
cmd->argv[1] = ft_strdup("hive");
cmd->argv[2] = NULL;

cmd->in_redir = malloc(sizeof(t_redir));
cmd->in_redir->type = R_INPUT;
cmd->in_redir->filename = ft_strdup("input.txt");

cmd->next = NULL;
```
### Example 3: Pipeline of Two Commands with Redirections
```bash
cat file1 > out.txt | grep "Error" >> log.txt
```
```c
// cmd1: "cat file1 > out.txt"
t_cmd *cmd1 = malloc(sizeof(t_cmd));
cmd1->argv = malloc(3 * sizeof(char *));

cmd1->argv[0] = strdup("cat");
cmd1->argv[1] = strdup("file1");
cmd1->argv[2] = NULL;

cmd1->out_redir = malloc(sizeof(t_redir));
cmd1->out_redir->type = R_OUTPUT;
cmd1->out_redir->filename = ft_strdup("out.txt");

// cmd2: "grep 'pattern' >> log.txt"
t_cmd *cmd2 = malloc(sizeof(t_cmd));
cmd2->argv = malloc(3 * sizeof(char *));

cmd2->argv[0] = ft_strdup("grep");
cmd2->argv[1] = ft_strdup("Error");
cmd2->argv[2] = NULL;

cmd2->out_redir = malloc(sizeof(t_redir));
cmd2->out_redir->type = R_APPEND;
cmd2->out_redir->filename = ft_strdup("log.txt");

// Link commands for the pipeline
cmd1->next = cmd2;
cmd2->next = NULL;
```
### Example 4: Multiple Input Redirections
```bash
grep "hive" < input1.txt < input2.txt
```
```c
t_cmd *cmd = malloc(sizeof(t_cmd));
cmd->argv = malloc(3 * sizeof(char *));

cmd->argv[0] = ft_strdup("grep");
cmd->argv[1] = ft_strdup("hive");
cmd->argv[2] = NULL;

// Build redirections: first node for input1.txt, then a second for input2.txt.
cmd->in_redir = malloc(sizeof(t_redir));
cmd->in_redir->type = R_INPUT;
cmd->in_redir->filename = ft_strdup("input1.txt");

// rewrite *in_redir, Executor/Shell works with the lasr file
cmd->in_redir->type = R_INPUT;
cmd->in_redir->filename = ft_strdup("input2.txt");

cmd->next = NULL;
```
### Example 5: Multiple Input Redirections
```bash
grep "hive" < input.txt > output.txt
```
```c
t_cmd *cmd = malloc(sizeof(t_cmd));
cmd->argv = malloc(3 * sizeof(char *));

cmd->argv[0] = ft_strdup("grep");
cmd->argv[1] = ft_strdup("hive");
cmd->argv[2] = NULL;

// Create input redirection node
cmd->in_redir = malloc(sizeof(t_redir));
cmd->in_redir->type = R_INPUT;
cmd->in_redir->filename = ft_strdup("input.txt");

// Create output redirection node
cmd->out_redir = malloc(sizeof(t_redir));
cmd->out_redir->type = R_OUTPUT;
cmd->out_redir->filename = ft_strdup("output.txt");

cmd->next = NULL;
```
