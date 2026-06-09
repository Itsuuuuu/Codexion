NAME        = codexion
CC          = cc
# CFLAGS      = -Wall -Wextra -Werror -I includes -pthread
CFLAGS		= -Wall -Wextra -Werror -I includes -fsanitize=thread -g

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



# codexion
# cc -Wall -Wextra -Werror -I includes -fsanitize=thread -g -c srcs/main.c -o srcs/main.o
# cc -Wall -Wextra -Werror -I includes -fsanitize=thread -g -c srcs/init.c -o srcs/init.o
# cc -Wall -Wextra -Werror -I includes -fsanitize=thread -g -c srcs/simulation.c -o srcs/simulation.o
# cc -Wall -Wextra -Werror -I includes -fsanitize=thread -g -c srcs/dongles.c -o srcs/dongles.o
# cc -Wall -Wextra -Werror -I includes -fsanitize=thread -g -c srcs/utils.c -o srcs/utils.o
# cc -Wall -Wextra -Werror -I includes -fsanitize=thread -g -c srcs/heap.c -o srcs/heap.o
# cc -Wall -Wextra -Werror -I includes -fsanitize=thread -g -c srcs/supervisor.c -o srcs/supervisor.o
# cc -Wall -Wextra -Werror -I includes -fsanitize=thread -g -o codexion srcs/main.o srcs/init.o srcs/simulation.o srcs/dongles.o srcs/utils.o srcs/heap.o srcs/supervisor.o
# guifouqu@c1r1p12 ~/Desktop/Codexion
#  % ./codexion 3 400 100 100 100 10 196 fifo > output
# LLVMSymbolizer: error reading file: No such file or directory
# ==================
# WARNING: ThreadSanitizer: unlock of an unlocked mutex (or by a wrong thread) (pid=3300660)
#     #0 pthread_mutex_unlock <null> (codexion+0x467b86)
#     #1 supervisor_loop /home/guifouqu/Desktop/Codexion/srcs/supervisor.c:50:5 (codexion+0x4bc9b5)
#     #2 start_simulation /home/guifouqu/Desktop/Codexion/srcs/supervisor.c:74:2 (codexion+0x4bca33)
#     #3 main /home/guifouqu/Desktop/Codexion/srcs/main.c:99:7 (codexion+0x4ba2cf)

#   Location is stack of main thread.

#   Location is global '??' at 0x7ffebc86a000 ([stack]+0x000000020650)

#   Mutex M0 (0x7ffebc88a650) created at:
#     #0 pthread_mutex_init <null> (codexion+0x44b47d)
#     #1 init_simulation /home/guifouqu/Desktop/Codexion/srcs/init.c:67:6 (codexion+0x4ba450)
#     #2 main /home/guifouqu/Desktop/Codexion/srcs/main.c:94:7 (codexion+0x4ba2a2)

# SUMMARY: ThreadSanitizer: unlock of an unlocked mutex (or by a wrong thread) (/home/guifouqu/Desktop/Codexion/codexion+0x467b86) in pthread_mutex_unlock
# ==================
# ThreadSanitizer: reported 1 warnings