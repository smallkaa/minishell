NAME := minishell

CC      := cc
CFLAGS  := #-Wall -Wextra -Werror
RM      := rm -rf

SRC_DIRS := src src/executor src/parser
OBJ_DIR  := obj
LIBFT_DIR := libs/libft

# Collect all source files
SRC_FILES := $(wildcard $(addsuffix /*.c, $(SRC_DIRS)))

# Convert source files to object files, preserving directory structure
OBJ_FILES := $(patsubst src/%.c, $(OBJ_DIR)/%.o, $(SRC_FILES))

INCLUDES := -Iinclude -I$(LIBFT_DIR)

# Libft
LIBFT := $(LIBFT_DIR)/libft.a

# Compile all
$(NAME): $(OBJ_FILES) $(LIBFT)
	$(CC) $(CFLAGS) $(OBJ_FILES) $(LIBFT) -o $(NAME) -lreadline

# Rule for compiling object files with correct paths
$(OBJ_DIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Build libft
$(LIBFT):
	make -C $(LIBFT_DIR)

# Clean
clean:
	$(RM) $(OBJ_DIR)
	make -C $(LIBFT_DIR) clean

fclean: clean
	$(RM) $(NAME)
	make -C $(LIBFT_DIR) fclean

re: fclean $(NAME)

.PHONY: all clean fclean re
