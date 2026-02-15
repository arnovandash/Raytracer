/*
** material_values.c -- Parse MATERIAL block from scene file.
**
** Materials define the surface appearance of objects in the raytracer.
** Each material has the following properties:
**
**   NAME     - Unique identifier used by PRIMITIVEs and OBJECTs to reference
**              this material by name.
**   DIFFUSE  - Base color and intensity for Lambertian (matte) shading.
**              Given as "RRGGBB intensity" where RRGGBB is hex color and
**              intensity is a 0-1 multiplier controlling how much diffuse
**              light the surface reflects.
**   SPECULAR - Highlight color and intensity for Phong specular reflections.
**              Controls the appearance of shiny highlights from light sources.
**   REFLECT  - Reflectivity coefficient (0-1). 0 = no mirror reflection,
**              1 = perfect mirror. Intermediate values blend reflected color
**              with the surface's own diffuse/specular shading.
**   REFRACT  - Transparency coefficient (0-1). 0 = fully opaque,
**              1 = fully transparent. Controls how much light passes through
**              the object via refraction rays.
**   IOR      - Index of refraction (e.g., 1.0 = vacuum/air, 1.33 = water,
**              1.5 = glass, 2.42 = diamond). Used by Snell's law to bend
**              refraction rays at surface boundaries.
**
** The DEFAULT material (index 0, hot pink) is created automatically by
** read_scene.c and is used as a fallback for objects with unresolved
** material names.
*/

#include "rt.h"

/*
** to_range -- Clamp a value to the interval [min, max].
**
** Used to ensure material properties stay within physically meaningful
** bounds (e.g., reflectivity must be between 0 and 1).
*/
static double	to_range(double value, double min, double max)
{
	value = (value < min) ? min : value;
	value = (value > max) ? max : value;
	return (value);
}

/*
** set_material_values -- Assign a parsed key-value pair to the current material.
**
** NAME replaces the default name via strdup (the old name is freed first).
** DIFFUSE and SPECULAR are parsed as hex color + optional intensity via
** get_colour(). REFLECT, REFRACT are clamped to [0,1]. IOR is stored as-is.
*/
static void		set_material_values(t_env *e, char *pt1, char *pt2)
{
	t_split_string	values;

	values = nstrsplit(pt2, ' ');
	if (!strcmp(pt1, "NAME"))
	{
		free(e->material[e->materials]->name);
		e->material[e->materials]->name = NULL;
		e->material[e->materials]->name = strdup(values.strings[0]);
	}
	else if (!strcmp(pt1, "DIFFUSE"))
		e->material[e->materials]->diff = get_colour(e, values);
	else if (!strcmp(pt1, "SPECULAR"))
		e->material[e->materials]->spec = get_colour(e, values);
	else if (!strcmp(pt1, "REFLECT"))
		e->material[e->materials]->reflect =
			to_range(atof(values.strings[0]), 0.0, 1.0);
	else if (!strcmp(pt1, "REFRACT"))
		e->material[e->materials]->refract =
			to_range(atof(values.strings[0]), 0.0, 1.0);
	else if (!strcmp(pt1, "IOR"))
		e->material[e->materials]->ior = atof(values.strings[0]);
	free_split(&values);
}

/*
** init_material -- Set sensible defaults for a new material.
**
** The default diffuse color is hot pink (R=1.0, G=0.0, B=0.87), chosen
** deliberately as a "missing texture" color -- it stands out obviously
** in renders, making it easy to spot objects that failed to load their
** intended material. This is a common game-engine convention (e.g.,
** Source engine's famous pink-and-black checkerboard).
**
** Default specular is white at 50% intensity. No reflection or refraction.
** IOR of 1.0 matches vacuum/air (no bending of refraction rays).
*/
void			init_material(t_material *m)
{
	m->name = strdup("UNNAMED");
	m->reflect = 0.0;
	m->refract = 0.0;
	m->ior = 1;
	m->diff = (t_colour){1.0, 0.0, 0.870588235294, 1.0};
	m->spec = (t_colour){1.0, 1.0, 1.0, 0.5};
}

/*
** get_material_attributes -- Read all lines of a MATERIAL block.
**
** Allocates a new t_material, initializes it with defaults (hot-pink),
** then reads tab-delimited attribute lines until a blank line terminates
** the block. After parsing, increments the material counter.
*/
void	get_material_attributes(t_env *e, FILE *stream)
{
	t_split_string	attr;
	char			*line = NULL;
	size_t			len = 0;

	attr.words = 0;
	e->material[e->materials] = (t_material *)malloc(sizeof(t_material));
	init_material(e->material[e->materials]);
	while (getline(&line, &len, stream) != -1)
	{
		line[strcspn(line, "\n")] = '\0';
		if (line[0] == '\0')
			break ;
		attr = nstrsplit(line, '\t');
		if (attr.words < 2)
			err(FILE_FORMAT_ERROR, "Material attributes", e);
		set_material_values(e, attr.strings[0], attr.strings[1]);
		free_split(&attr);
	}
	free(line);
	++e->materials;
}
