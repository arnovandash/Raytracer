/*
** intersect_object.c -- Mesh object intersection via brute-force triangle
** testing.
**
** A mesh object (loaded from an OBJ file) consists of an array of triangle
** faces. This function tests the ray against every triangle in the mesh
** and tracks the nearest hit.
**
** This is called only after the ray has already passed the bounding box
** test (see intersect_box.c), so we know the ray at least enters the
** volume containing the mesh. The brute-force approach tests all faces
** rather than using a spatial acceleration structure (BVH/KD-tree),
** which keeps the code simple at the cost of O(n) per ray per mesh.
**
** When a hit is found, the environment's o_hit (face pointer),
** object_hit (mesh pointer), and hit_type are updated so that the
** shading pipeline can access the face normal and the object's material.
*/

#include "rt.h"

/*
** intersect_object -- Test a ray against all triangles in a mesh object.
**
** Parameters:
**   e - environment (contains the ray and stores the nearest hit)
**   o - mesh object (array of face pointers + face count)
**   t - scratch variable for individual triangle hit distances
**
** Returns: 1 if any triangle was hit, 0 otherwise.
** Side effect: updates e->t, e->o_hit, e->object_hit, e->hit_type
** whenever a closer triangle is found.
*/
int		intersect_object(t_env *e, t_object *o, double *t)
{
	size_t	face;
	int		hit;

	face = o->faces;
	hit = 0;
	while (face--)
	{
		++g_tls_stats.intersection_tests;
		if (intersect_triangle(&e->ray, o->face[face], t) && *t < e->t)
		{
			e->t = *t;
			e->o_hit = o->face[face];
			e->object_hit = o;
			e->hit_type = FACE;
			hit = 1;
		}
	}
	return (hit);
}
