/*
** primitive_values.c -- Parse PRIMITIVE block from scene file.
**
** Primitives are the geometric building blocks of the scene. Each primitive
** is defined by its type and a set of type-specific parameters:
**
**   TYPE     - One of: sphere, plane, cone, cylinder, disk, hemi_sphere.
**              Each type has its own intersection algorithm in src/intersect/.
**   LOC      - Position in world space (center for spheres, point-on for planes).
**   DIR      - Orientation direction (axis for cylinders/cones, ignored for spheres).
**              Automatically normalized to a unit vector.
**   NORMAL   - Surface normal (used by planes, disks). Also normalized.
**   RADIUS   - Size parameter (sphere radius, cylinder/cone base radius,
**              disk radius).
**   ANGLE    - Half-angle for cones, in degrees. Converted to radians internally.
**              cos/sin are precomputed here to avoid recalculating them for
**              every intersection test during rendering.
**   MATERIAL - Name of the material to apply (looked up by name in the
**              material array). Falls back to index 0 (DEFAULT) if not found.
**   LIMIT    - Height limit for cylinders and cones. -1 means infinite
**              (unbounded). When set, the primitive is truncated at this
**              distance along its DIR axis.
**
** Primitive types are identified by integer constants defined in defines.h
** (PRIM_SPHERE, PRIM_PLANE, etc.) rather than strings, for fast comparison
** during the render loop.
*/

#include "rt.h"

/*
** get_type -- Convert a primitive type name string to its integer constant.
**
** Returns -1 if the type name is not recognized, which triggers an error
** in set_primitive_values.
*/
static int		get_type(char *type_str)
{
	int				type;

	type = -1;
	if (!strcmp(type_str, "sphere"))
		type = PRIM_SPHERE;
	else if (!strcmp(type_str, "plane"))
		type = PRIM_PLANE;
	else if (!strcmp(type_str, "hemi_sphere"))
		type = PRIM_HEMI_SPHERE;
	else if (!strcmp(type_str, "cone"))
		type = PRIM_CONE;
	else if (!strcmp(type_str, "cylinder"))
		type = PRIM_CYLINDER;
	else if (!strcmp(type_str, "disk"))
		type = PRIM_DISK;
	return (type);
}

/*
** get_material_number -- Look up a material by name, return its array index.
**
** Searches the material array in reverse order (most recently added first).
** If no match is found, prints a warning to stderr and returns 0 (the
** DEFAULT hot-pink material), ensuring the render can continue even with
** a typo in the scene file.
*/
size_t		get_material_number(t_env *e, char *str)
{
	size_t	material;
	char	*warn;

	material = e->materials;
	while (material--)
		if (!strcmp(e->material[material]->name, str))
			return (material);
	asprintf(&warn, "\e[208m    WARNING: Material name: %s \
		 is not a defined material name\n", str);
	dprintf(2, "%s", warn);
	free(warn);
	return (0);
}

/*
** set_primitive_values -- Assign a parsed key-value pair to the current primitive.
**
** ANGLE conversion: The scene file specifies cone half-angles in degrees
** for human readability. We convert to radians (degrees * pi / 180) and
** precompute cos(angle) and sin(angle). These precomputed trig values are
** used repeatedly in the cone intersection test (intersect_cone.c), which
** needs them to solve the quadratic equation for ray-cone intersection.
** Precomputing avoids calling cos/sin on every intersection test.
**
** DIR and NORMAL are normalized via get_unit_vector() because the
** intersection algorithms assume unit-length direction vectors to
** simplify the math (e.g., dot products directly give cosines).
*/
static void		set_primitive_values(t_env *e, char *pt1, char *pt2)
{
	t_split_string	values;

	values = nstrsplit(pt2, ' ');
	if (!strcmp(pt1, "TYPE"))
		if ((e->prim[e->prims]->type = get_type(values.strings[0])) == -1)
			err(FILE_FORMAT_ERROR, "Unknown primitive type", e);
	if (!strcmp(pt1, "LOC"))
		e->prim[e->prims]->loc = get_vector(e, values);
	else if (!strcmp(pt1, "DIR"))
		e->prim[e->prims]->dir = get_unit_vector(e, values);
	else if (!strcmp(pt1, "NORMAL"))
		e->prim[e->prims]->normal = get_unit_vector(e, values);
	else if (!strcmp(pt1, "RADIUS"))
		e->prim[e->prims]->radius = atof(values.strings[0]);
	else if (!strcmp(pt1, "ANGLE"))
	{
		e->prim[e->prims]->angle = atof(values.strings[0]) * M_PI / 180;
		e->prim[e->prims]->cos_angle = cos(e->prim[e->prims]->angle);
		e->prim[e->prims]->sin_angle = sin(e->prim[e->prims]->angle);
	}
	else if (!strcmp(pt1, "MATERIAL"))
		e->prim[e->prims]->material = get_material_number(e, values.strings[0]);
	else if (!strcmp(pt1, "LIMIT"))
		e->prim[e->prims]->limit = atof(values.strings[0]);
	free_split(&values);
}

/*
** init_primitive -- Set sensible defaults for a new primitive.
**
** Defaults to a unit sphere (radius 1) at the origin with the DEFAULT
** material (index 0). The default cone half-angle is ~30 degrees
** (0.523599 radians). loc_bak stores the original position for use by
** interactive camera transforms. s_bool tracks selection state for the
** UI. limit of -1 means unbounded (no height clipping).
*/
static void		init_primitive(t_prim *p)
{
	p->type = PRIM_SPHERE;
	p->loc = (t_vector){0.0, 0.0, 0.0};
	p->loc_bak = (t_vector){0.0, 0.0, 0.0};
	p->dir = (t_vector){0.0, 0.0, 1.0};
	p->normal = (t_vector){0.0, 0.0, 1.0};
	p->radius = 1.0;
	p->angle = 0.523599;
	p->cos_angle = cos(p->angle);
	p->sin_angle = sin(p->angle);
	p->material = 0;
	p->s_bool = 0;
	p->limit = -1;
}

/*
** get_primitive_attributes -- Read all lines of a PRIMITIVE block.
**
** Allocates a new t_prim, initializes with defaults, then reads
** tab-delimited attribute lines until a blank line terminates the block.
** After parsing, increments the primitive counter.
*/
void get_primitive_attributes(t_env *e, FILE *stream)
{
	t_split_string	attr;
	char			*line = NULL;
	size_t			len = 0;

	attr.words = 0;
	e->prim[e->prims] = (t_prim *)malloc(sizeof(t_prim));
	init_primitive(e->prim[e->prims]);
	while (getline(&line, &len, stream) != -1)
	{
		line[strcspn(line, "\n")] = '\0';
		if (line[0] == '\0')
			break ;
		attr = nstrsplit(line, '\t');
		if (attr.words < 2)
			err(FILE_FORMAT_ERROR, "Primitive attributes", e);
		set_primitive_values(e, attr.strings[0], attr.strings[1]);
		free_split(&attr);
	}
	free(line);
	++e->prims;
}
