/*
** read_scene.c -- Two-pass scene file parser
**
** This is the main entry point for loading a scene description from disk.
** The scene file format is a simple, tab-indented declarative text format
** that begins with the magic header "# SCENE RT".
**
** Parsing strategy (two-pass):
**   Pass 1 (get_quantities): Scans the entire file to count how many LIGHT,
**     MATERIAL, PRIMITIVE, and OBJECT blocks exist. This lets us pre-allocate
**     arrays of the exact size needed, avoiding dynamic resizing.
**   Pass 2: Rewinds the file, then reads in two phases:
**     (a) Global attributes (MAXDEPTH, RENDER resolution, SUPER sampling)
**         are read until the first blank line.
**     (b) Type-specific blocks (CAMERA, LIGHT, MATERIAL, PRIMITIVE, OBJECT)
**         are dispatched to their respective parsers.
**
** A DEFAULT material (index 0) is always created with a hot-pink diffuse
** color. This makes missing or mis-named materials immediately obvious
** in the rendered image -- any object showing hot pink has a material
** assignment problem.
**
** Scene file structure example:
**   # SCENE RT
**   MAXDEPTH	5
**   RENDER	800 600
**   SUPER	2
**                          <-- blank line separates globals from blocks
**   CAMERA
**   	LOC	0 0 -10
**   	DIR	0 0 0
**   	...
**                          <-- blank line separates blocks
**   LIGHT
**   	LOC	5 5 -5
**   	...
*/

#include "rt.h"

/*
** scene_attributes -- Parse a single global setting line.
**
** Each global attribute line is tab-delimited: "KEY\tVALUE".
** Recognized keys:
**   MAXDEPTH - Maximum recursion depth for reflection/refraction rays.
**              Clamped to at least 1 (a value of 0 would mean no shading).
**   RENDER   - Image resolution as "width height" (space-separated after tab).
**   SUPER    - Supersampling factor for depth-of-field. 0 = disabled.
**              Higher values produce smoother DOF at the cost of render time.
*/
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

/*
** call_type -- Dispatch a block header to the appropriate type-specific parser.
**
** When the main loop encounters a non-indented line (a block header like
** "PRIMITIVE" or "CAMERA"), this function trims whitespace and calls the
** matching parser. Each parser then reads subsequent indented lines until
** it hits a blank line (block terminator).
**
** Note: PRIMITIVE uses 'if' (not 'else if') followed by OBJECT also using
** 'if' -- this is a minor style inconsistency from the original code but
** does not cause bugs because a line cannot match both "PRIMITIVE" and
** "OBJECT".
*/
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

/*
** get_quantities -- Pass 1: Count scene elements for pre-allocation.
**
** Reads every line of the file, looking for block header keywords
** (LIGHT, MATERIAL, PRIMITIVE, OBJECT). Each match increments the
** corresponding counter in the environment struct.
**
** After counting, allocates pointer arrays for each element type.
** Note: e->materials is incremented by 1 before allocation to make
** room for the DEFAULT material at index 0.
**
** Also validates that no line ends with a trailing tab (a common
** copy-paste error in scene files).
*/
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

/*
** init_read_scene -- Initialize parsing state and create the DEFAULT material.
**
** After pass 1 (get_quantities) has allocated the arrays, this function:
**   1. Rewinds the file stream to the beginning for pass 2.
**   2. Resets all element counters to 0 (they were used for counting in pass 1
**      and will now be used as insertion indices during pass 2).
**   3. Creates the DEFAULT material at index 0 with a hot-pink diffuse color
**      (RGB ~(255, 0, 222)). This serves as a "missing texture" indicator --
**      any object referencing an undefined material name falls back to index 0,
**      making the error visually obvious in the render.
*/
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

/*
** read_scene -- Main entry point for loading a scene file.
**
** Opens the file, validates the "# SCENE RT" magic header, runs
** initialization (two-pass counting + default material), then parses
** the file in two phases:
**   Phase 1: Read global attributes (MAXDEPTH, RENDER, SUPER) until
**            the first blank line.
**   Phase 2: Read type-specific blocks (CAMERA, LIGHT, MATERIAL,
**            PRIMITIVE, OBJECT) until EOF. Each block is separated
**            by blank lines.
**
** Parameters:
**   file - Path to the scene description file.
**   e    - Pointer to the environment struct that will be populated.
*/
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
	/* Phase 1: Global scene attributes, terminated by blank line */
	while (getline(&line, &len, stream) != -1)
	{
		line[strcspn(line, "\n")] = '\0';
		if (line[0] == '\0')
			break ;
		scene_attributes(e, line);
	}
	/* Phase 2: Type-specific blocks until EOF */
	while (getline(&line, &len, stream) != -1)
	{
		line[strcspn(line, "\n")] = '\0';
		call_type(e, stream, &line);
	}
	free(line);
	fclose(stream);
}
