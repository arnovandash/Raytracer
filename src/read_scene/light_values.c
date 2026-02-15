/*
** light_values.c -- Parse LIGHT block from scene file.
**
** Each LIGHT block defines a point light source with the following attributes:
**
**   LOC       - Position of the light in world space.
**   COLOUR    - Light color as a hex "RRGGBB" string (e.g., "FFFFFF" = white).
**   INTENSITY - Brightness value, converted to lumens internally by
**               multiplying by 3.415 (derived from 683 lm/W * 0.005).
**               This constant maps the scene file's abstract intensity units
**               to a physically-inspired luminous flux value used in shading.
**   HALF      - Falloff half-distance. Controls how quickly the light
**               attenuates with distance. A value of 0 means no falloff
**               (light reaches everywhere equally).
**
** The raytracer supports multiple lights. During rendering, each light
** contributes independently to the diffuse and specular shading of a
** surface point, and each casts its own shadow rays.
*/

#include "rt.h"

/*
** set_light_values -- Assign a parsed key-value pair to the current light.
**
** The intensity conversion (value * 3.415) maps from the scene file's
** user-friendly units to internal luminous flux. The constant 3.415 comes
** from 683 lumens/watt (luminous efficacy at 555nm) * 0.005 (scaling factor),
** giving a perceptually reasonable brightness range for scene values of 1-100.
*/
static void	set_light_values(t_env *e, char *pt1, char *pt2)
{
	t_split_string	values;

	values = nstrsplit(pt2, ' ');
	if (!strcmp(pt1, "LOC"))
		e->light[e->lights]->loc = get_vector(e, values);
	else if (!strcmp(pt1, "COLOUR"))
		e->light[e->lights]->colour = get_colour(e, values);
	else if (!strcmp(pt1, "INTENSITY"))
		e->light[e->lights]->lm = atof(values.strings[0]) * 3.415;
	else if (!strcmp(pt1, "HALF"))
		e->light[e->lights]->half = atof(values.strings[0]);
	free_split(&values);
}

/*
** init_light -- Set sensible defaults for a new light.
**
** Defaults to a white point light at position (0, 0, 5) with intensity 1.0
** (3.415 lumens) and no distance falloff (half = 0).
*/
static void	init_light(t_light *l)
{
	l->loc = (t_vector){0.0, 0.0, 5.0};
	l->colour = (t_colour){1.0, 1.0, 1.0, 1.0};
	l->lm = 3.415;
	l->half = 0.0;
}

/*
** get_light_attributes -- Read all lines of a LIGHT block.
**
** Allocates a new t_light, initializes it with defaults, then reads
** tab-delimited attribute lines until a blank line terminates the block.
** After parsing, increments the global light counter so the next LIGHT
** block writes to the next array slot.
*/
void		get_light_attributes(t_env *e, FILE *stream)
{
	t_split_string	attr;
	char			*line = NULL;
	size_t			len = 0;

	attr.words = 0;
	e->light[e->lights] = (t_light *)malloc(sizeof(t_light));
	init_light(e->light[e->lights]);
	while (getline(&line, &len, stream) != -1)
	{
		line[strcspn(line, "\n")] = '\0';
		if (line[0] == '\0')
			break ;
		attr = nstrsplit(line, '\t');
		if (attr.words < 2)
			err(FILE_FORMAT_ERROR, "Light attributes", e);
		set_light_values(e, attr.strings[0], attr.strings[1]);
		free_split(&attr);
	}
	free(line);
	++e->lights;
}
