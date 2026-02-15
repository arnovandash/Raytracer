/*
** reflect.c -- Mirror reflection via recursive ray tracing.
**
** When a ray hits a reflective surface, we need to compute the reflected
** ray direction and recursively trace it into the scene to find what color
** is "seen" in the mirror.
**
** Reflection formula (vector form):
**   R = 2(N . V)N - V
**
** Where:
**   N = surface normal at the hit point (unit vector)
**   V = unit vector from hit point toward the camera/ray origin
**   R = reflected direction (automatically unit length if N and V are unit)
**
** Geometrically: V is mirrored across N. The component of V along N is
** doubled and the tangential component reverses, producing a "bounce."
**
** Recursion depth is tracked to prevent infinite reflection corridors
** (e.g., two mirrors facing each other). The maximum depth is set by
** the scene's MAXDEPTH parameter.
*/

#include "rt.h"

/*
** set_reflect_ray -- Compute the origin and direction of a reflection ray.
**
** Input:
**   e       -- The environment of the incoming ray (has the intersection
**              distance e->t and the original ray direction/origin).
**   reflect -- The environment to populate with the reflected ray.
**
** Steps:
**   1. Compute the hit point: origin + direction * t
**   2. V = unit vector from hit point back to ray origin (toward camera)
**   3. N = surface normal at the hit point
**   4. R = 2(N . V)N - V   (the reflection formula)
**
** The reflected ray's origin is the hit point itself. Its direction is R.
** Note: this function is also called as a fallback from refract.c when
** total internal reflection occurs.
*/
void		set_reflect_ray(t_env *e, t_env *reflect)
{
	t_vector	v;
	t_vector	n;

	/* Hit point = ray origin + ray direction * intersection distance */
	reflect->ray.loc = vadd(e->ray.loc, vmult(e->ray.dir, e->t));
	/* V = unit vector from hit point toward ray origin (incoming direction) */
	v = vunit(vsub(e->ray.loc, reflect->ray.loc));
	/* N = outward-facing surface normal at the hit point */
	n = get_normal(e, reflect->ray.loc);
	/* R = 2(N.V)N - V: reflect V across normal N */
	reflect->ray.dir = vsub(vmult(n, (vdot(n, v) * 2)), v);
}

/*
** reflect -- Trace a reflection ray and return the color seen.
**
** Creates a stack copy of the environment (no heap allocation needed),
** sets up the reflected ray, traces it through the scene, and recursively
** shades whatever it hits via find_colour_struct.
**
** Parameters:
**   e     -- Current environment (the ray that just hit a reflective surface)
**   depth -- Current recursion depth (incremented before the recursive call)
**
** Returns: The color seen in the reflection. If nothing is hit, returns
**          the background color (handled by find_colour_struct).
*/
t_colour	reflect(t_env *e, int depth)
{
	t_env		refl;
	t_colour	colour;

	++g_tls_stats.rays;
	++g_tls_stats.reflection_rays;
	colour = (t_colour){0.0, 0.0, 0.0, 0.0};
	refl = *e;
	refl.p_hit = NULL;
	set_reflect_ray(e, &refl);
	intersect_scene(&refl);
	colour = find_colour_struct(&refl, depth + 1);
	return (colour);
}
