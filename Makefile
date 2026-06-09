NAME        = codexion
CC          = cc
CFLAGS      = -Wall -Wextra -Werror -I includes -pthread
# CFLAGS		= -Wall -Wextra -Werror -I includes -fsanitize=thread -g

SRCS_DIR    = srcs
SRCS        = $(SRCS_DIR)/main.c \
              $(SRCS_DIR)/init.c \
              $(SRCS_DIR)/simulation.c \
              $(SRCS_DIR)/dongles.c \
              $(SRCS_DIR)/utils.c \
              $(SRCS_DIR)/heap.c \
			  $(SRCS_DIR)/supervisor.c 

OBJS        = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

lint:
	@echo "Running norminette ..."
	@norminette $(SRCS) includes/codexion.h || echo "Norminette found errors."

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re