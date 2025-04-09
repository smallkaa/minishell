# Detect OS
UNAME := $(shell uname)

# Default flags
LDFLAGS :=
CFLAGS  := -g -Wall -Wextra -Werror -Wuninitialized

# macOS-specific flags for Readline (2DO: fix extern void rl_replace_line(const char *, int) in signals.c)
ifeq ($(UNAME), Darwin)
	READLINE_PATH := $(shell brew --prefix readline 2>/dev/null)
	ifneq ($(READLINE_PATH),)
		LDFLAGS += -L$(READLINE_PATH)/lib
		CFLAGS  += -I$(READLINE_PATH)/include
		CFLAGS	+= -Wno-strict-prototypes
	endif
endif

NAME := minishell

CC      := cc
RM      := rm -rf

SRC_DIRS := src \
			src/parser \
			src/executor \
			src/mshell \
			src/mshell/env \
			src/mshell/varables_hash_table \
			src/executor/builtins \
			src/executor/builtins/cd \
			src/executor/builtins/echo \
			src/executor/builtins/env \
			src/executor/builtins/exit \
			src/executor/builtins/export \
			src/executor/builtins/pwd \
			src/executor/builtins/unset \
			src/executor/pipeline \
			src/executor/redirections 

OBJ_DIR  := obj
LIBFT_DIR := libs/libft


# Collect all source files
SRC_FILES := $(wildcard $(addsuffix /*.c, $(SRC_DIRS)))

# Convert source files to object files, preserving directory structure
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
	$(CC) $(CFLAGS) $(OBJ_FILES) $(LIBFT) -o $(NAME) $(LDFLAGS) -lreadline
	@echo "\033[32m\"$(NAME)\": successfully created!\033[0m"
	find . -type f \( -name "*.c" -o -name "*.h" -o -name "Makefile" \) -exec awk 'FNR==1{print "File: "FILENAME "\n"}{print}' {} + > sources_dump.txt
# Rule for compiling object files with correct paths
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

.PHONY: all clean fclean re docs dclean
