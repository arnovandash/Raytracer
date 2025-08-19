# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: adippena <angusdippenaar@gmail.com>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2016/07/09 12:47:08 by adippena          #+#    #+#              #
#    Updated: 2016/09/06 12:02:40 by adippena         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		=	RT
LIBFTI		=	-I libft/include
LIBFTL		=	-L libft -lft
SDL2I		=	$(shell sdl2-config --cflags)
SDL2L		=	$(shell sdl2-config --libs)
INC			=	-I include $(SDL2I) $(LIBFTI)
LIBS		=	-lm $(SDL2L) $(LIBFTL)
CFLAGS		=	-Wall -Wextra -Werror -Wno-unused-result -O3 -pthread -std=c11 $(INC)
LFLAGS		=	-lpthread $(INC) $(LIBS)
CC			=	gcc
LD			=	gcc

INCLUDE		=	$(shell find include -name "*.h")
SRC			=	$(shell find src -name "*.c")
OBJ			=	$(SRC:src/%.c=build/%.o)

all: lft rt

build/%.o: src/%.c $(INCLUDE)
	@echo "\033[92m    CC    $@\033[0m"
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

rt: $(OBJ)
	@echo "\033[92m    LD    $(NAME)\033[0m"
	@$(LD) $^ $(LFLAGS) -o $(NAME)

lft:
	@make -s -C libft all

clean:
	@echo "\033[92m    RM    object files\033[0m"
	@rm -rf build
	@make -s -C libft clean

fclean: clean
	@echo "\033[92m    RM    $(NAME)\033[0m"
	@rm -f $(NAME)
	@rm -rf $(NAME).dSYM
	@make -s -C libft fclean

re: fclean all
