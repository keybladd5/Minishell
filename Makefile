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

NAME 		= Minishell

SRCS 		=  src/mminishell.c src/builtins.c src/executor.c \
			src/parser.c src/expansor.c src/redirs.c src/signs.c \
			src/typer.c src/errors.c src/here_doc.c src/append.c \
			 src/parser_utils.c src/ft_createtoken.c src/executor_utils.c

INCLUDES 	= inc/minishell.h

FLAGS 		= -Wall -Wextra -Werror -MMD -g #-fsanitize=address

FLAGS_LIB_SEARCH	:=	-lreadline -lhistory -ltermcap -lft

OBJS 		= $(SRCS:.c=.o)

DEPS		= $(SRCS:.c=.d)

RDLINE_ROOT	:=	inc/readline/

LIBFT_ROOT	:= inc/libft/

LIB_A		:=	$(RDLINE_ROOT)libreadline.a $(RDLINE_ROOT)libhistory.a \
				$(LIBFT_ROOT)libft.a

LIB_ADD_DIR	:=	-L $(RDLINE_ROOT) -L $(LIBFT_ROOT)

#COLORES
GREEN		=	\033[38;5;154m
RED			=	\033[38;5;197m
PURPLE		=	\033[38;5;171m
BLUE		=	\033[38;5;14m
NC			=	\033[0m
YELLOW		=	\033[38;5;190m

#SUCEDE AL HACER MAKE
all: librarys $(NAME)

#EJECUTA MAKEFILES DE LIBRERIAS
librarys :
	@$(MAKE) makelibft --no-print-directory
	@$(MAKE) rdline_condition --no-print-directory

#NORMA PARA COMPILAR MINISHELL
$(NAME): $(OBJS)
	@echo "${BLUE}Compiling Minishell${NC}"
	@gcc $(FLAGS) $(OBJS) $(LIB_ADD_DIR) $(FLAGS_LIB_SEARCH) $(LIB_A) -o $(NAME)
	@echo "${GREEN}Minishell Compiled${NC}"

#DEPENDENCIAS
%.o:%.c Makefile inc/libft/libft.a inc/minishell.h
	@$(CC) $(FLAGS)  $(INCLUDE) -c $< -o $@

#NORMA PARA LIBFT
makelibft:
	@echo "${BLUE}Compiling Libft...${NC}"
	@$(MAKE) -j -C inc/libft/ & pid=$$!; \
	echo "."; \
	while ps -p $$pid > /dev/null; do \
		sleep 1; \
		echo "."; \
	done; 
	@echo "${GREEN}Libft Compiled${NC}"

#NORMA CON CONDICION PARA READLINE, SI YA ESTA COMPILADA NO VUELVE A HACERLO PARA AHORRAR TIEMPO DE MAKE
rdline_condition:
	@echo "${BLUE}Compiling Readline...${NC}"
	@if [ ! -f ./inc/readline/libreadline.a ] || [ ! -f ./inc/readline/libhistory.a ]; then \
		$(MAKE) rdline; \
	else \
		echo "${PURPLE}Readline already compiled.${NC}"; \
	fi

#NORMA QUE SE EJECUTA EN CASO DE NO ESTAR COMPILADA LA READLINE
rdline:
	@cd ./inc/readline/ && ./configure &> /dev/null & pid=$$!; \
	echo "."; \
	while ps -p $$pid > /dev/null; do \
		sleep 1; \
		echo "."; \
	done;
	@make -j -C ./inc/readline/ &> /dev/null & pid=$$!; \
	echo "."; \
	while ps -p $$pid > /dev/null; do \
		sleep 1; \
		echo "."; \
	done; \
	echo "${GREEN}Readline Compiled${NC}"

#NORMA PARA LIMPIAR OBJETOS Y DEPENDENCIAS DE SRC/
sclean:
	@echo "${RED}Clean objects and deps...${NC}"
	@rm -rf $(OBJS) $(DEPS)
	@rm -rf $(OBJS_BONUS) $(DEPS_BONUS)
	@echo "${GREEN}Done clean ${NC}"

#NORMA PARA LIMPIAR OBJETOS Y DEPENDENCIAS Y LIBFT Y READLINE
clean:
	@echo "${RED}Clean objects and deps...${NC}"
	@rm -rf $(OBJS) $(DEPS)
	@rm -rf $(OBJS_BONUS) $(DEPS_BONUS)
	@echo "${RED}Clean Libft...${NC}"
	@$(MAKE) -C $(LIBFT_ROOT) clean
	@echo "${GREEN}Done clean ${NC}"
	@echo "${RED}Clean Readline...${NC}"
	@$(MAKE) -C ./inc/readline/ clean &> /dev/null  --no-print-directory
	@echo "${GREEN}Done clean ${NC}"

#NORMA PARA LIMPIAR TODO + BINARIO DE MINISHELL
fclean: clean
	@echo "${YELLOW}Clean binary...${NC}"
	@rm -f $(NAME)
	@rm -f $(NAME_BONUS)
	@echo "${GREEN}Done fclean ${NC}"

#NORMA PARA REALIZARLO TODO DE NUEVO
re: fclean all

-include $(DEPS)

.PHONY: all clean fclean sclean re makelibft
