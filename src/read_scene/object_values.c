/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   object_values.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adippena <angusdippenaar@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/08/09 21:36:49 by adippena          #+#    #+#             */
/*   Updated: 2016/09/02 15:51:23 by adippena         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"
#include <libgen.h>

static void		get_quantities(t_object *o, FILE *stream)
{
	char	*line = NULL;
	size_t	len = 0;

	while (getline(&line, &len, stream) != -1)
	{
		if (!strncmp(line, "vn", 2))
			++o->vnormals;
		else if (line[0] == 'v')
			++o->verticies;
		else if (line[0] == 'f')
			++o->faces;
	}
	free(line);
	if ((o->face = (t_face **)malloc(sizeof(t_face *) * o->faces)) == NULL)
		perror("");
	if ((o->v = (t_vector **)malloc(sizeof(t_vector *) * o->verticies)) == NULL)
		perror("");
	if ((o->vn = (t_vector **)malloc(sizeof(t_vector *) * o->vnormals)) == NULL)
		perror("");
	o->faces = 0;
	o->verticies = 0;
	o->vnormals = 0;
	fseek(stream, 0, SEEK_SET);
}

static void		set_object_values(t_env *e, char *pt1, char *pt2)
{
	FILE	*stream;
	char	*file;

	if (!strcmp(pt1, "FILE"))
	{
		file = pt2;
		if ((stream = fopen(file, "r")) == NULL)
			asprintf(&file, "./%s/%s", dirname(e->file_name), pt2);
		if ((stream = fopen(file, "r")) == NULL)
			err(FILE_OPEN_ERROR, file, e);
		e->object[e->objects]->name = strdup(file);
		get_quantities(e->object[e->objects], stream);
		read_obj(e, stream);
		if (file != pt2)
			free(file);
		fclose(stream);
	}
	else if (!strcmp(pt1, "MATERIAL"))
		e->object[e->objects]->material = get_material_number(e, pt2);
}

static void		init_object(t_object *o)
{
	o->face = NULL;
	o->faces = 0;
	o->material = 0;
	o->v = NULL;
	o->verticies = 0;
	o->vn = NULL;
	o->vnormals = 0;
}

void			get_object_attributes(t_env *e, FILE *stream)
{
	t_split_string	attr;
	char			*line = NULL;
	size_t			len = 0;

	attr.words = 0;
	e->object[e->objects] = (t_object *)malloc(sizeof(t_object));
	init_object(e->object[e->objects]);
	while (getline(&line, &len, stream) != -1)
	{
		line[strcspn(line, "\n")] = '\0';
		if (line[0] == '\0')
			break ;
		attr = nstrsplit(line, '\t');
		if (attr.words < 2)
			err(FILE_FORMAT_ERROR, "Object attributes", e);
		set_object_values(e, attr.strings[0], attr.strings[1]);
		free_split(&attr);
	}
	free(line);
	++e->objects;
}
