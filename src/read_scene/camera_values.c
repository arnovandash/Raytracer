/*
** camera_values.c -- Parse CAMERA block from scene file.
**
** The camera defines the viewpoint for the raytracer. It uses a "look-at"
** model specified by three vectors and an optional aperture:
**
**   LOC      - Camera position in world space (eye point).
**   DIR      - The point the camera is looking at (NOT a direction vector).
**              The actual view direction is computed later as (DIR - LOC).
**   UP       - The "up" reference vector, used to construct the camera's
**              orthonormal basis (u, v, n) during camera_setup.
**   APERTURE - Controls depth-of-field blur. A value of 0 gives a perfect
**              pinhole camera (everything in focus). Larger values simulate
**              a wider lens aperture, blurring objects outside the focal plane.
**
** The camera basis vectors (u, v, n) and image-plane stepping are computed
** later by camera_setup.c, not here. This file only reads the raw values
** from the scene file.
*/

#include "rt.h"

/*
** set_camera_values -- Assign a parsed key-value pair to the camera struct.
**
** Parameters:
**   e   - Environment struct containing the camera.
**   pt1 - The attribute key (e.g., "LOC", "DIR", "UP", "APERTURE").
**   pt2 - The attribute value string (e.g., "0 5 -10" or "0.5").
*/
static void	set_camera_values(t_env *e, char *pt1, char *pt2)
{
	t_split_string	values;

	values = nstrsplit(pt2, ' ');
	if (!strcmp(pt1, "LOC"))
		e->camera.loc = get_vector(e, values);
	else if (!strcmp(pt1, "DIR"))
		e->camera.dir = get_vector(e, values);
	else if (!strcmp(pt1, "UP"))
		e->camera.up = get_vector(e, values);
	else if (!strcmp(pt1, "APERTURE"))
		e->camera.a = atof(pt2);
	free_split(&values);
}

/*
** get_camera_attributes -- Read all lines of a CAMERA block.
**
** Reads tab-delimited "KEY\tVALUE" lines from the scene file stream until
** a blank line (block terminator) or EOF is encountered. Each line is split
** on tabs and dispatched to set_camera_values.
**
** Parameters:
**   e      - Environment struct to populate.
**   stream - File stream positioned just after the "CAMERA" header line.
*/
void		get_camera_attributes(t_env *e, FILE *stream)
{
	t_split_string	attr;
	char			*line = NULL;
	size_t			len = 0;

	attr.words = 0;
	while (getline(&line, &len, stream) != -1)
	{
		line[strcspn(line, "\n")] = '\0';
		if (line[0] == '\0')
			break ;
		attr = nstrsplit(line, '\t');
		if (attr.words < 2)
			err(FILE_FORMAT_ERROR, "Camera attributes", e);
		set_camera_values(e, &attr.strings[0][0], &attr.strings[1][0]);
		free_split(&attr);
	}
	free(line);
}
