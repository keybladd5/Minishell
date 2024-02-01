# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: polmarti <polmarti@student.42barcel>       +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/02/01 13:43:32 by polmarti          #+#    #+#              #
#    Updated: 2024/02/01 13:43:35 by polmarti         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = Minishell

SRCS =  src/minishell.c

INCLUDES = inc/minishell.h

FLAGS = -Wall -Wextra -Werror -MMD -g

OBJS = $(SRCS:.c=.o)

DEPS = $(SRCS:.c=.d)

RDLINE_ROOT	:=	readline/

CC = gcc

%.o:%.c Makefile libft/libft.a inc/minishell.h
	$(CC) $(FLAGS) -I ./ -c $< -o $@

$(NAME): $(OBJS)
	$(CC) $(FLAGS) libft/libft.a $(OBJS) -o $(NAME)

all: makelibft rdline $(NAME)

makelibft: 
	$(MAKE) -C libft/

rdline :
	@echo "${YELLOW}Compiling Readline...${NC}"
	@cd ./readline/ &> /dev/null && ./configure &> /dev/null
	@make -C ./readline/ &> /dev/null
	@echo "${GREEN}Readline Compiled${NC}"

clean:
	rm -rf $(OBJS) $(DEPS)
	rm -rf $(OBJS_BONUS) $(DEPS_BONUS)
	$(MAKE) -C libft clean

fclean: clean
	rm -f $(NAME)
	rm -f $(NAME_BONUS)

re: fclean all

-include $(DEPS)

.PHONY: all clean fclean re makelibft