# Detect OS
UNAME := $(shell uname)
BIGTEST ?= 0

# Default flags
LDFLAGS :=
CFLAGS  := -g -O0 -Wall -Wextra -Werror -Wuninitialized
CFLAGS  += -DBIGTEST=$(BIGTEST)

# macOS-specific flags for Readline (2DO: fix extern void rl_replace_line(const char *, int) in signals.c)
ifeq ($(UNAME), Darwin)
	READLINE_PATH := $(shell brew --prefix readline 2>/dev/null)
	ifneq ($(READLINE_PATH),)
		LDFLAGS += -L$(READLINE_PATH)/lib
		CFLAGS  += -I$(READLINE_PATH)/include
		CFLAGS  += -Wno-strict-prototypes
	endif
endif

NAME := minishell

CC      := cc
RM      := rm -rf

# --- Explicit list of source files ---
SRC_FILES := \
	src/free_memory/free_cmd.c \
	src/free_memory/free_minishell.c \
	src/parser/parser_helpers2.c \
	src/parser/tokenizer2.c \
	src/parser/parser2.c \
	src/parser/parser_utils_explain.c \
	src/parser/unsupported_cmd.c \
	src/parser/tokenizer6.c \
	src/parser/parser1.c \
	src/parser/parser_helpers1.c \
	src/parser/tokenizer1.c \
	src/parser/tokenizer5.c \
	src/parser/parser5.c \
	src/parser/expand2.c \
	src/parser/expand_helpers_escape.c \
	src/parser/parser_utils_token.c \
	src/parser/parser_utils_free.c \
	src/parser/tokenizer4.c \
	src/parser/parser_helpers4.c \
	src/parser/parser4.c \
	src/parser/tokenizer3.c \
	src/parser/parser_utils_cmd.c \
	src/parser/parser_helpers3.c \
	src/parser/expand_tokens.c \
	src/parser/parser3.c \
	src/parser/expand_helpers_tilde.c \
	src/parser/expand_helpers_dollar.c \
	src/parser/expand_helpers_quotes.c \
	src/parser/expand.c \
	src/utils.c \
	src/main.c \
	src/main_utils.c \
	src/main_script_mode.c \
	src/mshell/init_minishell.c \
	src/mshell/find_binary_utils.c \
	src/mshell/env/setup_env.c \
	src/mshell/env/update_env.c \
	src/mshell/env/update_env_utils.c \
	src/mshell/find_binary.c \
	src/mshell/varables_hash_table/setup_hash_table_utils.c \
	src/mshell/varables_hash_table/setup_hash_table.c \
	src/executor/executor.c \
	src/executor/pipeline/pipe_utils.c \
	src/executor/pipeline/pipe_creators/handle_child_and_track.c \
	src/executor/pipeline/pipe_creators/wait_for_children.c \
	src/executor/pipeline/close_unused_fds.c \
	src/executor/pipeline/pipe_executors/execute_command.c \
	src/executor/pipeline/exec_in_pipes.c \
	src/executor/redirections/apply_redirections.c \
	src/executor/redirections/heredoc/write_heredoc_to_pipe.c \
	src/executor/redirections/heredoc/write_heredoc_utils.c \
	src/executor/redirections/heredoc/apply_heredoc_utilc.c \
	src/executor/redirections/heredoc/apply_heredocs.c \
	src/executor/builtins/unset/unset.c \
	src/executor/builtins/echo/echo_utils.c \
	src/executor/builtins/echo/echo.c \
	src/executor/builtins/pwd/pwd.c \
	src/executor/builtins/exec_builtins.c \
	src/executor/builtins/env/env.c \
	src/executor/builtins/builtin_utils.c \
	src/executor/builtins/exit/exit.c \
	src/executor/builtins/exit/exit_utils.c \
	src/executor/builtins/cd/cd_utils.c \
	src/executor/builtins/cd/cd.c \
	src/executor/builtins/export/export_utils_1.c \
	src/executor/builtins/export/export.c \
	src/executor/builtins/export/export_utils_2.c \
	src/executor/executor_helpers/command_too_long.c \
	src/executor/executor_helpers/update_underscore.c \
	src/executor/executor_helpers/executor_utils.c \
	src/executor/exec_in_current_process.c \
	src/errors/child_execve_error_utils.c \
	src/errors/error_utils_2.c \
	src/errors/error_utils_1.c \
	src/errors/child_execve_error.c \
	src/signals.c

# --- End of explicit list ---

OBJ_DIR  := obj
LIBFT_DIR := libs/libft

# Convert source files to object files, preserving directory structure
# This patsubst should still work with the explicit list
OBJ_FILES := $(patsubst src/%.c, $(OBJ_DIR)/%.o, $(SRC_FILES))

INCLUDES := -Iinclude -I$(LIBFT_DIR)

# Libft
LIBFT := $(LIBFT_DIR)/libft.a

# Define the documentation output directory
DOCS_DIR = docs

# Default Doxygen configuration file
DOXYFILE = Doxyfile

# Compile all
$(NAME): $(OBJ_FILES) $(LIBFT)
	$(CC) $(CFLAGS) $(OBJ_FILES) $(LIBFT) -g -o $(NAME) $(LDFLAGS) -lreadline
	@echo "\033[32m\"$(NAME)\": successfully created!\033[0m"

# Rule for compiling object files with correct paths
# This rule creates the necessary subdirectories in obj/
$(OBJ_DIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Build libft
$(LIBFT):
	make -C $(LIBFT_DIR)

# Norminette
norm:
	@if command -v norminette > /dev/null; then \
		norminette_output=$$(norminette .); \
		norminette_status=$$?; \
		if [ $$norminette_status -eq 0 ]; then \
			echo "\033[32m$$norminette_output\033[0m"; \
		else \
			echo "\033[31m$$(echo "$$norminette_output" | head -n 10)\033[0m"; \
		fi; \
	else \
		echo "\033[31mno norminette\033[0m"; \
	fi

# Rule to generate Doxygen documentation
docs:
	@echo "Generating documentation..."
	@doxygen $(DOXYFILE)
	@echo "\033[32mDocumentation generated in $(DOCS_DIR)/html/index.html\033[0m"

# Clean
clean:
	$(RM) $(OBJ_DIR)
	make -C $(LIBFT_DIR) clean

fclean: clean
	$(RM) $(NAME)
	make -C $(LIBFT_DIR) fclean
	rm -f sources_dump.txt

re: fclean $(NAME)

# Rule to clean up old documentation
dclean:
	@echo "Cleaning documentation files..."
	@rm -rf $(DOCS_DIR)
	@echo "\033[32mDocumentation cleaned.\033[0m"

.PHONY: all clean fclean re docs dclean norm
