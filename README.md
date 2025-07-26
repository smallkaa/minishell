# Try it!
[![Open in Gitpod](https://gitpod.io/button/open-in-gitpod.svg)](https://gitpod.io/#https://github.com/smallkaa/minishell)

# Wiki Shell-Rebuild
- [Home](https://github.com/iliamunaev/Shell-Rebuild/wiki): Basic commands for contributors

- [Data Structure](https://github.com/iliamunaev/Shell-Rebuild/wiki/Data-Structure): data structures used in the project

- [Functions](https://github.com/iliamunaev/Shell-Rebuild/wiki/Functions): the scope of functions might be used in the project

- [Git Commit Message Standard](https://github.com/iliamunaev/Shell-Rebuild/wiki/Git-Commit-Message-Standard): rules for clean commit messages

- [Processes Analyzing Shell Commands](https://github.com/iliamunaev/Shell-Rebuild/wiki/Process-Management-Shell-Commands)

### Error tests
```bash
valgrind --track-fds=yes ./minishell
```
```bash
valgrind  --leak-check=full ./minishell
 ```
### Testers to try with
```bash
https://github.com/LucasKuhn/minishell_tester
```
```bash
https://github.com/zstenger93/42_minishell_tester
```


## Use supression file to avoid readline() leaks
```bash
valgrind --leak-check=full --show-leak-kinds=all --suppressions=mshell.supp ./minishell
```

all erros + supp
```bash
valgrind --leak-check=full --trace-children=yes --gen-suppressions=all --show-leak-kinds=all --track-fds=yes  --error-limit=no --suppressions=mshell.supp ./minishell
```

all errors
```bash
valgrind --leak-check=full  --track-origins=yes  --trace-children=yes --gen-suppressions=all --show-leak-kinds=all --track-fds=yes --error-limit=no  ./minishell
```

Find all memory allocation functions
```bash
grep -rnE '\b(malloc|calloc|ft_calloc|realloc|ft_realloc|strdup|ft_strdup|strndup|ft_strndup|ft_substr|asprintf|vasprintf)\b' ./src
```
