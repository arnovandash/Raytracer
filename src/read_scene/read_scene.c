/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_scene.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adippena <angusdippenaar@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/07/08 20:00:42 by adippena          #+#    #+#             */
/*   Updated: 2016/09/04 15:14:36 by adippena         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

static void	scene_attributes(t_env *e, char *line)
{
	t_split_string	split;
	t_split_string	render;

	split = nstrsplit(line, '\t');
	if (split.strings[0][0] != '#' && split.words != 2)
		err(FILE_FORMAT_ERROR, "Scene arrtibutes", e);
	if (!strcmp(split.strings[0], "MAXDEPTH"))
		e->maxdepth = MAX(atoi(split.strings[1]), 1);
	if (!strcmp(split.strings[0], "RENDER"))
	{
		render = nstrsplit(split.strings[1], ' ');
		if (render.words != 2)
			err(FILE_FORMAT_ERROR, "RENDER [tab] x-res y-res", e);
		e->x = atoi(render.strings[0]);
		e->y = atoi(render.strings[1]);
	}
	if (!strcmp(split.strings[0], "SUPER"))
		e->super = MAX(atoi(split.strings[1]), 0);
	free_split(&split);
}

static void	call_type(t_env *e, FILE *stream, char **line)
{
	char	*temp_line;

	temp_line = strtrim(*line);
	if (!strcmp(temp_line, "PRIMITIVE"))
		get_primitive_attributes(e, stream);
	if (!strcmp(temp_line, "OBJECT"))
		get_object_attributes(e, stream);
	else if (!strcmp(temp_line, "CAMERA"))
		get_camera_attributes(e, stream);
	else if (!strcmp(temp_line, "LIGHT"))
		get_light_attributes(e, stream);
	else if (!strcmp(temp_line, "MATERIAL"))
		get_material_attributes(e, stream);
	free(temp_line);
}

static void	get_quantities(t_env *e, FILE *stream)
{
	char	*line = NULL;
	size_t	len = 0;
	size_t	num;

	num = 1;
	while (getline(&line, &len, stream) != -1 && ++num)
	{
		line[strcspn(line, "\n")] = '\0';
		if (line[strlen(line) - 1] == '\t')
			err(FILE_FORMAT_ERROR, "Line ends in a tab", e);
		char *trimmed_line = strtrim(line);
		(!strcmp(trimmed_line, "LIGHT")) ? ++e->lights : 0;
		(!strcmp(trimmed_line, "MATERIAL")) ? ++e->materials : 0;
		(!strcmp(trimmed_line, "PRIMITIVE")) ? ++e->prims : 0;
		(!strcmp(trimmed_line, "OBJECT")) ? ++e->objects : 0;
		free(trimmed_line);
	}
	free(line);
	printf("%d:\tLIGHTS\n%d:\tMATERIALS\n%d:\tPRIMITIVES\n%d:\tOBJECTS\n",
		(int)e->lights, (int)e->materials, (int)e->prims, (int)e->objects);
	e->light = (t_light **)malloc(sizeof(t_light *) * e->lights);
	e->material = (t_material **)malloc(sizeof(t_material *) * ++e->materials);
	e->prim = (t_prim **)malloc(sizeof(t_prim *) * e->prims);
	e->object = (t_object **)malloc(sizeof(t_object *) * e->objects);
}

static void	init_read_scene(t_env *e, FILE *stream)
{
	get_quantities(e, stream);
	fseek(stream, 0, SEEK_SET);
	e->lights = 0;
	e->materials = 0;
	e->prims = 0;
	e->objects = 0;
	e->material[0] = (t_material *)malloc(sizeof(t_material));
	init_material(e->material[0]);
	free(e->material[0]->name);
	e->material[0]->name = NULL;
	e->material[e->materials]->name = strdup("DEFAULT");
	++e->materials;
}

void		read_scene(char *file, t_env *e)
{
	char			*line = NULL;
	size_t			len = 0;
	FILE			*stream;

	stream = fopen(file, "r");
	if (stream == NULL)
		err(FILE_OPEN_ERROR, "Scene file", e);
	if (getline(&line, &len, stream) == -1)
		err(FILE_FORMAT_ERROR, "Scene file must start with '# SCENE RT'", e);
	line[strcspn(line, "\n")] = '\0';
	if (strcmp(line, "# SCENE RT"))
		err(FILE_FORMAT_ERROR, "Scene file must start with '# SCENE RT'", e);
	init_read_scene(e, stream);
	while (getline(&line, &len, stream) != -1)
	{
		line[strcspn(line, "\n")] = '\0';
		if (line[0] == '\0')
			break ;
		scene_attributes(e, line);
	}
	while (getline(&line, &len, stream) != -1)
	{
		line[strcspn(line, "\n")] = '\0';
		call_type(e, stream, &line);
	}
	free(line);
	fclose(stream);
}
