/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   prim_values.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adippena <angusdippenaar@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/07/09 09:54:48 by adippena          #+#    #+#             */
/*   Updated: 2016/09/04 15:07:08 by adippena         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

static int		get_type(char *type_str)
{
	int				type;

	type = -1;
	if (!strcmp(type_str, "sphere"))
		type = PRIM_SPHERE;
	else if (!strcmp(type_str, "plane"))
		type = PRIM_PLANE;
	else if (!strcmp(type_str, "hemi_sphere"))
		type = PRIM_HEMI_SPHERE;
	else if (!strcmp(type_str, "cone"))
		type = PRIM_CONE;
	else if (!strcmp(type_str, "cylinder"))
		type = PRIM_CYLINDER;
	else if (!strcmp(type_str, "disk"))
		type = PRIM_DISK;
	return (type);
}

size_t		get_material_number(t_env *e, char *str)
{
	size_t	material;
	char	*warn;

	material = e->materials;
	while (material--)
		if (!strcmp(e->material[material]->name, str))
			return (material);
	asprintf(&warn, "\e[208m    WARNING: Material name: %s \
		 is not a defined material name\n", str);
	dprintf(2, "%s", warn);
	free(warn);
	return (0);
}

static void		set_primitive_values(t_env *e, char *pt1, char *pt2)
{
	t_split_string	values;

	values = nstrsplit(pt2, ' ');
	if (!strcmp(pt1, "TYPE"))
		if ((e->prim[e->prims]->type = get_type(values.strings[0])) == -1)
			err(FILE_FORMAT_ERROR, "Unknown primitive type", e);
	if (!strcmp(pt1, "LOC"))
		e->prim[e->prims]->loc = get_vector(e, values);
	else if (!strcmp(pt1, "DIR"))
		e->prim[e->prims]->dir = get_unit_vector(e, values);
	else if (!strcmp(pt1, "NORMAL"))
		e->prim[e->prims]->normal = get_unit_vector(e, values);
	else if (!strcmp(pt1, "RADIUS"))
		e->prim[e->prims]->radius = atof(values.strings[0]);
	else if (!strcmp(pt1, "ANGLE"))
	{
		e->prim[e->prims]->angle = atof(values.strings[0]) * M_PI / 180;
		e->prim[e->prims]->cos_angle = cos(e->prim[e->prims]->angle);
		e->prim[e->prims]->sin_angle = sin(e->prim[e->prims]->angle);
	}
	else if (!strcmp(pt1, "MATERIAL"))
		e->prim[e->prims]->material = get_material_number(e, values.strings[0]);
	else if (!strcmp(pt1, "LIMIT"))
		e->prim[e->prims]->limit = atof(values.strings[0]);
	free_split(&values);
}

static void		init_primitive(t_prim *p)
{
	p->type = PRIM_SPHERE;
	p->loc = (t_vector){0.0, 0.0, 0.0};
	p->loc_bak = (t_vector){0.0, 0.0, 0.0};
	p->dir = (t_vector){0.0, 0.0, 1.0};
	p->normal = (t_vector){0.0, 0.0, 1.0};
	p->radius = 1.0;
	p->angle = 0.523599;
	p->cos_angle = cos(p->angle);
	p->sin_angle = sin(p->angle);
	p->material = 0;
	p->s_bool = 0;
	p->limit = -1;
}

void get_primitive_attributes(t_env *e, FILE *stream)
{
	t_split_string	attr;
	char			*line = NULL;
	size_t			len = 0;

	attr.words = 0;
	e->prim[e->prims] = (t_prim *)malloc(sizeof(t_prim));
	init_primitive(e->prim[e->prims]);
	while (getline(&line, &len, stream) != -1)
	{
		line[strcspn(line, "\n")] = '\0';
		if (line[0] == '\0')
			break ;
		attr = nstrsplit(line, '\t');
		if (attr.words < 2)
			err(FILE_FORMAT_ERROR, "Primitive attributes", e);
		set_primitive_values(e, attr.strings[0], attr.strings[1]);
		free_split(&attr);
	}
	free(line);
	++e->prims;
}
