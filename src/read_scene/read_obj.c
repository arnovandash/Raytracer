/*
** read_obj.c -- Wavefront OBJ file parser (pass 2: data reading).
**
** Wavefront OBJ is a widely-used text-based 3D model format. This parser
** handles the subset needed for the raytracer:
**
**   v  x y z       - Vertex position (3 floats)
**   vn x y z       - Vertex normal (3 floats, used for smooth shading)
**   f  v1//n1 v2//n2 v3//n3  - Triangular face referencing vertex and
**                               normal indices (1-based in the OBJ spec)
**
** Lines starting with '#' (comments) or 's' (smoothing groups) are skipped.
** Only triangular faces (exactly 3 vertices) are supported.
**
** After all geometry is loaded, an axis-aligned bounding box (AABB) is
** computed from the vertex positions. During rendering, a ray is first
** tested against this bounding box before checking individual triangles.
** This is essential for performance -- a mesh with 10,000 faces would
** require 10,000 intersection tests per ray without this early-out.
**
** OBJ face format note:
** The "v//vn" format (vertex//normal, no texture coordinate) is parsed
** by finding the last '/' in each face token and reading the normal index
** after it. Vertex indices are read directly with atoi on the token.
** Both are converted from 1-based (OBJ convention) to 0-based (C arrays)
** by subtracting 1.
*/

#include "rt.h"

/*
** make_box -- Compute the axis-aligned bounding box (AABB) for a mesh.
**
** Iterates through all vertices to find the minimum and maximum x, y, z
** coordinates. The result is stored as two corner points:
**   box[0] = (min_x, min_y, min_z)  -- the "near" corner
**   box[1] = (max_x, max_y, max_z)  -- the "far" corner
**
** An AABB is the tightest box with faces aligned to the coordinate axes
** that fully contains the mesh. Ray-AABB intersection is very fast
** (just 6 comparisons after some arithmetic), making it ideal as a
** first-pass culling test before expensive per-triangle checks.
*/
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

/*
** read_vertex -- Parse an OBJ "v" line and store the vertex position.
**
** OBJ format: "v x y z" where x, y, z are floating-point coordinates.
** values->strings[0] is "v", and [1],[2],[3] are x, y, z.
*/
static void	read_vertex(t_object *o, t_split_string *values)
{
	o->v[o->verticies] = (t_vector *)malloc(sizeof(t_vector));
	o->v[o->verticies]->x = atof(values->strings[1]);
	o->v[o->verticies]->y = atof(values->strings[2]);
	o->v[o->verticies]->z = atof(values->strings[3]);
	++o->verticies;
}

/*
** read_vnormal -- Parse an OBJ "vn" line and store the vertex normal.
**
** OBJ format: "vn x y z" where x, y, z are the normal vector components.
** Normals are typically unit vectors but the OBJ spec does not require it.
*/
static void	read_vnormal(t_object *o, t_split_string *values)
{
	o->vn[o->vnormals] = (t_vector *)malloc(sizeof(t_vector));
	o->vn[o->vnormals]->x = atof(values->strings[1]);
	o->vn[o->vnormals]->y = atof(values->strings[2]);
	o->vn[o->vnormals]->z = atof(values->strings[3]);
	++o->vnormals;
}

/*
** read_face -- Parse an OBJ "f" line and create a triangle face.
**
** OBJ format: "f v1//n1 v2//n2 v3//n3" where v and n are 1-based indices
** into the vertex and normal arrays respectively.
**
** The face stores pointers (not copies) to the vertex and normal data,
** so multiple faces sharing a vertex point to the same t_vector in memory.
** This saves memory and ensures consistency.
**
** Index extraction:
**   - Vertex index: atoi(token) gives the number before the first "/".
**   - Normal index: strrchr(token, '/') + 1 finds the number after the
**     last "/", handling both "v//vn" and "v/vt/vn" formats.
**   - Both are decremented by 1 to convert from OBJ's 1-based indexing
**     to C's 0-based array indexing.
*/
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

/*
** read_obj -- Pass 2 of OBJ loading: read vertex, normal, and face data.
**
** Reads every line of the OBJ file, splitting on spaces. Lines are
** dispatched based on their first token:
**   "v"  (with 4 words) -> read_vertex
**   "vn" (with 4 words) -> read_vnormal
**   "f"  (with 4 words) -> read_face (triangles only; quads are not supported)
**
** After all data is loaded, make_box() computes the AABB for ray culling.
**
** Note: The condition (line[0] != '#' || line[0] != 's') is always true
** due to the logical OR -- this is a minor bug that has no practical effect
** since comment and smoothing-group lines don't match any of the strcmp
** checks inside.
*/
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
