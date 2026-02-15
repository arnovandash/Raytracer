/*
** shadow.c — Shadow ray casting with transparent shadow support
**
** Determines how much light from a given light source reaches a surface point.
** A "shadow ray" is cast from the hit point toward the light. If any opaque
** object lies between the point and the light, the point is in shadow.
**
** Transparent shadows: Rather than a binary lit/unlit test, this implementation
** tracks a transmittance value that starts at 1.0 (fully lit). Each
** intersected object reduces transmittance multiplicatively by its refract
** coefficient. For example, two objects with refract=0.5 each reduce
** transmittance to 0.5 * 0.5 = 0.25. An opaque object (refract=0) immediately
** drops transmittance to 0, producing a full shadow.
**
** Return value convention:
**   0.0 = fully lit (no obstruction)
**   1.0 = fully shadowed (opaque object blocks the light)
**   0.0 < x < 1.0 = partial shadow from semi-transparent objects
**
** The function returns (1.0 - transmittance), so the caller can use it as:
**   contribution *= (1.0 - shadow)
**
** Optimization: if transmittance drops below EPSILON (effectively zero),
** we return early with full shadow rather than testing remaining objects.
**
** Both standalone primitives and mesh objects (with bounding-box culling)
** are tested for intersection with the shadow ray.
*/

#include "rt.h"
#include "in_shadow.h"

/*
** init — Prepare the shadow ray and iteration state.
** The shadow ray originates at the surface hit point and points toward the
** light source. The distance to the light is stored so we only consider
** intersections closer than the light (objects behind the light don't
** cast shadows toward this light).
*/

static void	init(t_in_shadow *var, t_env *e, t_light *light)
{
	var->t = INFINITY;
	/* Shadow ray origin = hit point along the primary ray */
	var->ray.loc = vadd(e->ray.loc, vmult(e->ray.dir, e->t));
	/* Direction from hit point to light (unnormalized) */
	var->ray.dir = vsub(light->loc, var->ray.loc);
	/* vnormalize returns the length and stores it; then we make dir unit-length */
	var->distance = vnormalize(var->ray.dir);
	var->ray.dir = vdiv(var->ray.dir, var->distance);
	var->prim = e->prims;
	var->object = e->objects;
}

/*
** in_shadow — Test whether a surface point is occluded from a light source.
**
** Parameters:
**   e     — environment with scene geometry and the current ray/hit state
**   light — the light source to test visibility against
**
** Returns: shadow factor in [0.0, 1.0] (see file-level comment for semantics)
*/

double		in_shadow(t_env *e, t_light *light)
{
	t_in_shadow	var;
	double		transmit;
	double		t_test;

	++g_tls_stats.rays;
	++g_tls_stats.shadow_rays;
	init(&var, e, light);
	transmit = 1.0;
	/* Test shadow ray against all standalone primitives */
	while (var.prim--)
	{
		t_test = var.distance;
		/* Only count intersections closer than the light source */
		if (intersect_prim(e, &var.ray, var.prim, &t_test) && t_test <
				var.distance)
		{
			/* Reduce transmittance by the object's transparency (refract) */
			transmit *= e->material[e->prim[var.prim]->material]->refract;
			/* Early exit: fully opaque shadow, no need to test more objects */
			if (transmit < EPSILON)
				return (1.0);
		}
	}
	/* Test shadow ray against mesh objects (OBJ models) */
	while (var.object--)
	{
		var.o = e->object[var.object];
		/* Bounding box test: skip mesh entirely if ray misses its AABB */
		if (intersect_box(&var.ray, var.o->box))
		{
			var.face = var.o->faces;
			/* Test individual triangles within the mesh */
			while (var.face--)
				if (intersect_triangle(&var.ray, var.o->face[var.face],
						&t_test) && t_test < var.distance)
				{
					transmit *= e->material[var.o->material]->refract;
					if (transmit < EPSILON)
						return (1.0);
					/* One face hit is enough for this mesh — break to next object */
					break ;
				}
		}
	}
	/* Convert transmittance to shadow factor: 1.0 - transmit */
	return (1.0 - transmit);
}
