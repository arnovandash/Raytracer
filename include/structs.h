/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   structs.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adippena <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/07/26 14:48:30 by adippena          #+#    #+#             */
/*   Updated: 2016/09/03 15:12:40 by adippena         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRUCTS_H
# define STRUCTS_H

/*
** 3D VECTOR STRUCTURE
*/
typedef struct	s_vector
{
	double	x;
	double	y;
	double	z;
}				t_vector;

/*
** MATERIAL AND COLOUR STRUCTURES
*/
typedef struct	s_colour
{
	double	r;
	double	g;
	double	b;
	double	intensity;
}				t_colour;

typedef struct	s_material
{
	char		*name;
	double		reflect;
	double		refract;
	double		ior;
	t_colour	diff;
	t_colour	spec;
}				t_material;

/*
** OBJECT STRUCTURE
*/

typedef struct	s_face
{
	t_vector	*v0;
	t_vector	*v1;
	t_vector	*v2;
	t_vector	*n;
}				t_face;

typedef struct	s_object
{
	char		*name;
	t_face		**face;
	size_t		faces;
	size_t		material;
	t_vector	**v;
	size_t		verticies;
	t_vector	**vn;
	size_t		vnormals;
	t_vector	box[2];
}				t_object;

typedef struct	s_prim
{
	t_vector	loc;
	t_vector	dir;
	t_vector	normal;
	t_vector	loc_bak;
	int			type;
	int			s_bool;
	size_t		material;
	double		radius;
	double		angle;
	double		limit;
}				t_prim;

/*
** RAY, CAMERA AND LIGHT STRUCTURE
*/
typedef struct	s_ray
{
	int			inter;
	double		ior;
	t_vector	loc;
	t_vector	dir;
	t_object	*o_in;
}				t_ray;

typedef struct	s_camera
{
	t_vector	loc;
	t_vector	dir;
	t_vector	up;
	t_vector	u;
	t_vector	v;
	t_vector	l;
	double		stepx;
	double		stepy;
	double		a;
}				t_camera;

typedef struct	s_light
{
	t_vector	loc;
	t_colour	colour;
	double		lm;
	double		half;
}				t_light;

typedef struct	s_stats
{
	_Atomic size_t	rays;
	_Atomic size_t	primary_rays;
	_Atomic size_t	reflection_rays;
	_Atomic size_t	refraction_rays;
	_Atomic size_t	shadow_rays;
	_Atomic size_t	intersection_tests;
	_Atomic size_t	threads;
}				t_stats;

/*
** ENVIRNMENT STRUCTURE
*/
typedef struct	s_env
{
	SDL_Window		*win;
	SDL_Surface		*win_img;
	SDL_Surface		*img;
	SDL_Surface		*dof;
	uint32_t		*px;
	uint32_t		*dx;
	char			*file_name;
	t_ray			ray;
	t_camera		camera;
	t_prim			*p_hit;
	size_t			s_num;
	size_t			hit_type;
	t_prim			**prim;
	size_t			prims;
	t_face			*o_hit;
	t_object		*object_hit;
	t_object		**object;
	size_t			objects;
	t_light			**light;
	size_t			lights;
	t_material		**material;
	size_t			materials;
	double			t;
	int				maxdepth;
	size_t			super;
	size_t			flags;
	size_t			x;
	size_t			y;
}				t_env;

typedef struct	s_quadratic
{
	double	a;
	double	b;
	double	c;
	double	discr;
}				t_quad;

#endif
