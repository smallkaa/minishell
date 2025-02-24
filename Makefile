NAME := minishell

CC      := cc
CFLAGS  := -Wall -Wextra -Werror
RM      := rm -rf

SRC_DIRS := src/executor src/parser
OBJ_DIR  := obj
LIBFT_DIR := libs/libft

#src
SRC_FILES := $(wildcard $(addsuffix /*.c, $(SRC_DIRS))) main.c
OBJ_FILES := $(patsubst %.c, $(OBJ_DIR)/%.o, $(notdir $(SRC_FILES)))

INCLUDES := -Iinclude -I$(LIBFT_DIR)

# Libft
LIBFT := $(LIBFT_DIR)/libft.a

# Compile all
$(NAME): $(OBJ_FILES) $(LIBFT)
	$(CC) $(CFLAGS) $(OBJ_FILES) $(LIBFT) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIRS)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

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

re: fclean all

.PHONY: all clean fclean re
