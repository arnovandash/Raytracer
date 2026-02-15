/*
** intersect_scene.c -- Scene-level ray traversal: test a ray against every
** object in the scene and find the nearest intersection.
**
** This is the core of the ray tracing loop. For each ray (primary, shadow,
** reflection, or refraction), this code iterates through all primitives
** and mesh objects to find the closest surface the ray hits.
**
** The intersection result is stored in the environment struct:
**   e->t        - distance to the nearest hit
**   e->p_hit    - pointer to the hit primitive (if any)
**   e->o_hit    - pointer to the hit mesh face (if any)
**   e->hit_type - PRIMITIVE or FACE (to choose normal computation method)
**   e->ray.inter - 1 for front hit, 2 for inside hit
**
** Mesh objects are tested with a bounding box pre-check. If the ray
** misses the bounding box, all triangles in that mesh are skipped.
*/

#include "rt.h"

/*
** intersect_prim -- Dispatch to the correct intersection function based
** on the primitive type. Also increments the per-thread intersection
** test counter for performance monitoring.
**
** Returns: 0 = miss, 1 = front hit, 2 = inside hit.
*/
int			intersect_prim(t_env *e, t_ray *ray, size_t prim, double *t)
{
	++g_tls_stats.intersection_tests;
	if (e->prim[prim]->type == PRIM_SPHERE)
		return (intersect_sphere(ray, e->prim[prim], t));
	if (e->prim[prim]->type == PRIM_HEMI_SPHERE)
		return (intersect_hemi_sphere(ray, e->prim[prim], t));
	if (e->prim[prim]->type == PRIM_PLANE)
		return (intersect_plane(ray, e->prim[prim], t));
	if (e->prim[prim]->type == PRIM_CYLINDER)
		return (intersect_cylinder(ray, e->prim[prim], t));
	if (e->prim[prim]->type == PRIM_CONE)
		return (intersect_cone(ray, e->prim[prim], t));
	if (e->prim[prim]->type == PRIM_DISK)
		return (intersect_disk(ray, e->prim[prim], t));
	return (0);
}

/*
** intersect_scene -- Find the nearest intersection of e->ray with
** all objects in the scene.
**
** Algorithm:
**   1. Initialize t to infinity (no hit yet).
**   2. Test every primitive; if this hit is closer than the current
**      nearest, update e->t and e->p_hit.
**   3. Test every mesh object: first check the bounding box for a
**      quick reject, then test individual triangles via intersect_object().
**
** After this function, if e->p_hit or e->o_hit is non-NULL, the ray
** hit something, and e->t holds the distance.
*/
void		intersect_scene(t_env *e)
{
	int			inter;
	double		t;
	size_t		prim;
	size_t		object;

	e->t = INFINITY;
	e->p_hit = NULL;
	e->o_hit = NULL;
	e->hit_type = 0;
	prim = e->prims;
	object = e->objects;
	/* Test all standalone primitives */
	while (prim--)
		if ((inter = intersect_prim(e, &e->ray, prim, &t)) && t < e->t)
		{
			e->ray.inter = inter;
			e->t = t;
			e->p_hit = e->prim[prim];
			e->hit_type = PRIMITIVE;
		}
	/* Test mesh objects with bounding box culling */
	while (object--)
		if (intersect_box(&e->ray, e->object[object]->box))
			intersect_object(e, e->object[object], &t);
}
