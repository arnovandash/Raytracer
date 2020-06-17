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
#SDL2I		=	$(shell sdl2-config --cflags)
#SDL2L		=	$(shell sdl2-config --libs)
#SDL2I		=	`sdl2-config --cflags`
#SDL2L		=	`sdl2-config --libs`
SDL2I		=	-Dmain=SDL_main
SDL2L		=	-L ./libsdl -lmingw32 -lSDL2main -lSDL2 -mwindows
SDL2L		=	-lmingw32 -mwindows
INC			=	-I include $(SDL2I) $(LIBFTI)
LIBS		=	-lm $(SDL2L) $(LIBFTL)
# -Wno-deprecated-declarations added for gettimeofday() function in src/draw.c:124
CFLAGS		=	-Wall -Wextra -Werror -Wno-unused-result -Wno-deprecated-declarations -Ofast -pthread $(INC)
#LFLAGS		=	-lpthread $(INC) $(LIBS)
LFLAGS		=	$(INC) $(LIBS)
CC			=	gcc
LD			=	gcc

INCLUDE		=	include/defines.h include/diffuse.h include/draw.h include/intersect_cone.h include/intersect_cylinder.h include/intersect_object.h include/intersect_triangle.h include/in_shadow.h include/prototypes.h include/rt.h include/structs.h

SRC			=	src/camera_setup.c src/copy_env.c src/diffuse.c src/draw.c src/error.c src/export.c src/find_colour_struct.c src/free/free_light.c src/free/free_material.c src/free/free_object.c src/free/free_obj_vert.c src/free/free_prim.c src/get_normal.c src/half_bytes.c src/init_env.c src/intersect/intersect_box.c src/intersect/intersect_cone.c src/intersect/intersect_cylinder.c src/intersect/intersect_disk.c src/intersect/intersect_hemi_sphere.c src/intersect/intersect_object.c src/intersect/intersect_plane.c src/intersect/intersect_scene.c src/intersect/intersect_sphere.c src/intersect/intersect_triangle.c src/loop.c src/main.c src/read_scene/camera_values.c src/read_scene/light_values.c src/read_scene/material_values.c src/read_scene/object_values.c src/read_scene/primitive_values.c src/read_scene/read_colour.c src/read_scene/read_obj.c src/read_scene/read_scene.c src/read_scene/read_vector.c src/reflect.c src/refract.c src/save/save.c src/save/save_lights.c src/save/save_materials.c src/save/save_objects.c src/save/save_prims.c src/save/write_coord.c src/shadow.c src/user_input/cam_move.c src/user_input/grab.c src/user_input/init_keys.c src/user_input/key_press.c src/user_input/mouse_click.c src/vector_maths/colour_to_unit.c src/vector_maths/vector_add.c src/vector_maths/vector_comp.c src/vector_maths/vector_cross.c src/vector_maths/vector_div.c src/vector_maths/vector_dot.c src/vector_maths/vector_mult.c src/vector_maths/vector_normalize.c src/vector_maths/vector_project.c src/vector_maths/vector_rot.c src/vector_maths/vector_sub.c src/vector_maths/vector_unit.c

OBJ			=	$(SRC:src/%.c=build/%.o)

#all: lft rt
all: rt

build/%.o: src/%.c $(INCLUDE)
	@echo "\033[92m    CC    $@\033[0m"
	@$(CC) $(CFLAGS) -c $< -o $@

rt: $(OBJ)
	@echo "\033[92m    LD    $(NAME)\033[0m"
	@$(LD) $^ $(LFLAGS) -o $(NAME)

lft:
	@make -s -C libft all

clean:
	@echo "\033[92m    RM    object files\033[0m"
	@rm -rf build/*
#	@make -s -C libft clean

fclean: clean
	@echo "\033[92m    RM    $(NAME)\033[0m"
	@rm -f $(NAME)
	@rm -rf $(NAME).dSYM
#	@make -s -C libft fclean

re: fclean all
