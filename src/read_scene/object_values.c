/*
** object_values.c -- Parse OBJECT block from scene file.
**
** OBJECTs are triangle meshes loaded from Wavefront OBJ files. Unlike
** primitives (which are defined by mathematical equations), mesh objects
** are defined by lists of triangular faces, making them suitable for
** complex geometry exported from 3D modeling software.
**
** Each OBJECT block has two attributes:
**   FILE     - Path to the .obj file. First tried as-is, then relative to
**              the scene file's directory (using dirname). This allows scene
**              files to reference OBJ files using relative paths.
**   MATERIAL - Name of the material to apply to all faces of this mesh.
**
** The OBJ loading is a two-pass process (similar to the scene parser):
**   Pass 1 (get_quantities): Counts vertices, vertex normals, and faces
**     to pre-allocate arrays of the exact size needed.
**   Pass 2 (read_obj): Rewinds and reads the actual vertex/normal/face data.
**
** After loading, an axis-aligned bounding box (AABB) is computed from
** the mesh vertices. During rendering, rays are first tested against
** this bounding box -- if the ray misses the box, all individual triangle
** intersection tests are skipped. This is a crucial optimization since
** mesh objects can have thousands of faces.
*/

#include "rt.h"
#include <libgen.h>

/*
** get_quantities -- Pass 1 of OBJ loading: count elements for pre-allocation.
**
** Scans every line of the OBJ file, counting lines starting with:
**   "vn" - Vertex normals (must check before "v" since "vn" starts with 'v')
**   "v"  - Vertex positions
**   "f"  - Faces (triangles)
**
** After counting, allocates arrays for faces, vertices, and normals.
** Resets counters to 0 and rewinds the stream for pass 2.
*/
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

/*
** set_object_values -- Handle FILE and MATERIAL attributes for an OBJECT block.
**
** FILE handling tries two paths:
**   1. The path as given in the scene file (could be absolute or relative to CWD).
**   2. Relative to the scene file's directory (constructed via dirname).
** This fallback makes scene files portable -- OBJ files in the same directory
** as the scene file will be found regardless of the working directory.
**
** After opening the file, it runs the two-pass OBJ loader (get_quantities
** then read_obj) and closes the stream.
*/
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

/*
** init_object -- Zero-initialize a new mesh object.
**
** All arrays start as NULL and all counters start at 0. The material
** defaults to index 0 (the DEFAULT hot-pink material).
*/
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

/*
** get_object_attributes -- Read all lines of an OBJECT block.
**
** Allocates a new t_object, initializes it, then reads tab-delimited
** attribute lines (FILE and MATERIAL) until a blank line terminates the block.
*/
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
