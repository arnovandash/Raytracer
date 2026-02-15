/*
** save_objects.c -- Serialize OBJ mesh objects to scene file
**
** OBJECT blocks reference external .obj mesh files (Wavefront OBJ format).
** Each object stores the file path and the material to apply to all its
** faces. The material is stored internally as an index into the materials
** array, so we look up the material name for serialization.
*/

#include "rt.h"

/*
** Writes a single OBJECT block: the OBJ file path and its material name.
** The material index (obj->material) is resolved to a name via the
** materials array for human-readable output.
*/
static void	save_object(t_object *obj, t_material **mat, int fd)
{
	dprintf(fd, "\n\tOBJECT\n");
	dprintf(fd, "\t\tFILE\t\t%s\n", obj->name);
	dprintf(fd, "\t\tMATERIAL\t%s\n", mat[obj->material]->name);
}

/*
** Iterates over all mesh objects and serializes each one.
*/
void		save_objects(t_object **o, size_t objects, t_material **m, int fd)
{
	size_t	i;

	i = 0;
	while (i < objects)
		save_object(o[i++], m, fd);
}
