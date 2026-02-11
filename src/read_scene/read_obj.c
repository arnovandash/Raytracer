/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_obj.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adippena <angusdippenaar@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/08/11 19:48:22 by adippena          #+#    #+#             */
/*   Updated: 2016/09/04 15:09:01 by adippena         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

static void	make_box(t_object *o)
{
	size_t	vertex;

	vertex = 1;
	o->box[0] = (t_vector){o->v[0]->x, o->v[0]->y, o->v[0]->z};
	o->box[1] = o->box[0];
	while (vertex < o->verticies)
	{
		if (o->v[vertex]->x < o->box[0].x)
			o->box[0].x = o->v[vertex]->x;
		if (o->v[vertex]->y < o->box[0].y)
			o->box[0].y = o->v[vertex]->y;
		if (o->v[vertex]->z < o->box[0].z)
			o->box[0].z = o->v[vertex]->z;
		if (o->v[vertex]->x > o->box[1].x)
			o->box[1].x = o->v[vertex]->x;
		if (o->v[vertex]->y > o->box[1].y)
			o->box[1].y = o->v[vertex]->y;
		if (o->v[vertex]->z > o->box[1].z)
			o->box[1].z = o->v[vertex]->z;
		++vertex;
	}
}

static void	read_vertex(t_object *o, t_split_string *values)
{
	o->v[o->verticies] = (t_vector *)malloc(sizeof(t_vector));
	o->v[o->verticies]->x = atof(values->strings[1]);
	o->v[o->verticies]->y = atof(values->strings[2]);
	o->v[o->verticies]->z = atof(values->strings[3]);
	++o->verticies;
}

static void	read_vnormal(t_object *o, t_split_string *values)
{
	o->vn[o->vnormals] = (t_vector *)malloc(sizeof(t_vector));
	o->vn[o->vnormals]->x = atof(values->strings[1]);
	o->vn[o->vnormals]->y = atof(values->strings[2]);
	o->vn[o->vnormals]->z = atof(values->strings[3]);
	++o->vnormals;
}

static void	read_face(t_object *o, t_split_string *values)
{
	o->face[o->faces] = (t_face *)malloc(sizeof(t_face));
	o->face[o->faces]->v0 = o->v[atoi(values->strings[1]) - 1];
	o->face[o->faces]->v1 = o->v[atoi(values->strings[2]) - 1];
	o->face[o->faces]->v2 = o->v[atoi(values->strings[3]) - 1];
	o->face[o->faces]->n =\
		o->vn[atoi(strrchr(values->strings[1], '/') + 1) - 1];
	++o->faces;
}

void		read_obj(t_env *e, FILE *stream)
{
	t_split_string	values;
	char			*line = NULL;
	size_t			len = 0;
	t_object		*o;

	o = e->object[e->objects];
	while (getline(&line, &len, stream) != -1)
	{
		if (line[0] != '#' || line[0] != 's')
		{
			values = nstrsplit(line, ' ');
			if (!strcmp(values.strings[0], "v") && values.words == 4)
				read_vertex(o, &values);
			if (!strcmp(values.strings[0], "vn") && values.words == 4)
				read_vnormal(o, &values);
			if (!strcmp(values.strings[0], "f") && values.words == 4)
				read_face(o, &values);
			free_split(&values);
		}
	}
	free(line);
	make_box(o);
}
