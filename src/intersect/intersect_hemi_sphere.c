/*
** intersect_hemi_sphere.c -- Ray-hemisphere intersection.
**
** A hemisphere is a sphere cut in half by a plane through its center.
** It is defined by a center, radius, and a direction vector that indicates
** which half of the sphere to keep.
**
** Algorithm:
**   1. Perform a standard ray-sphere intersection (quadratic formula).
**   2. For each candidate hit point, verify it lies on the correct
**      hemisphere by checking: dot(hit_point - center, direction) <= 0.
**      This means the hit point is on the opposite side of the center
**      from the direction vector (the "bowl" side).
**
** If the nearer intersection is on the wrong hemisphere, the farther
** one is tested. This handles rays that graze the hemisphere boundary.
*/

#include "rt.h"

/*
** check_lim -- Verify a hit point lies on the correct hemisphere.
**
** The hemisphere keeps points where dot(dir, P - center) <= 0, meaning
** the point is on the side opposite to the direction vector (forming a
** bowl shape facing away from the direction).
*/
static int	check_lim(t_ray *r, t_prim *o, double *t, double *t_test)
{
	t_vector	ph;

	ph = vadd(r->loc, vmult(r->dir, *t_test));
	if (vdot(o->dir, vsub(ph, o->loc)) <= 0)
	{
		*t = *t_test;
		return (1);
	}
	return (0);
}

/*
** find_t -- Solve the sphere quadratic and pick the nearest root that
** passes the hemisphere check.
**
** Returns: 0 = miss, 1 = front hit, 2 = hit from inside.
*/
static int	find_t(t_quad *quad, double *t, t_prim *o, t_ray *r)
{
	double	sqrt_discr;
	double	t0;
	double	t1;
	double	tc;

	sqrt_discr = sqrt(quad->discr);
	t0 = (-quad->b + sqrt_discr) / (2.0 * quad->a);
	t1 = (-quad->b - sqrt_discr) / (2.0 * quad->a);
	tc = (t0 > t1) ? t1 : t0;
	if (tc > EPSILON)
		if (check_lim(r, o, t, &tc))
			return (1);
	tc = (tc == t1) ? t0 : t1;
	if (tc > EPSILON)
		if (check_lim(r, o, t, &tc))
			return (2);
	return (0);
}

/*
** intersect_hemi_sphere -- Test a ray against a hemisphere.
**
** Parameters:
**   r - ray (origin + direction)
**   o - hemisphere primitive (center, radius, direction for clipping)
**   t - output: ray parameter at intersection
**
** Returns: 0 = miss, 1 = front hit, 2 = hit from inside.
*/
int			intersect_hemi_sphere(t_ray *r, t_prim *o, double *t)
{
	t_quad		quad;
	int			inter;
	t_vector	dist;

	/* Standard sphere quadratic setup */
	dist = vsub(r->loc, o->loc);
	quad.a = vdot(r->dir, r->dir);
	quad.b = 2.0 * vdot(r->dir, dist);
	quad.c = vdot(dist, dist) - (o->radius * o->radius);
	quad.discr = quad.b * quad.b - 4.0 * quad.a * quad.c;
	if (quad.discr < EPSILON)
		return (0);
	/* find_t applies the hemisphere clipping check to each candidate */
	inter = find_t(&quad, t, o, r);
	return (inter);
}
