/*
** intersect_cylinder.c -- Ray-cylinder intersection by reducing 3D to 2D.
**
** An infinite cylinder is defined by a center point C, an axis direction A,
** and a radius r. A point P is on the cylinder surface if the perpendicular
** distance from P to the axis line equals r.
**
** Strategy: project away the axis component so the 3D problem becomes a 2D
** circle intersection in the plane perpendicular to the axis.
**
** For ray P(t) = O + t*D, define:
**   dist = O - C                     (vector from cylinder center to ray origin)
**   a_v  = D - (D . A)*A             (ray direction component perpendicular to axis)
**   c_v  = dist - (dist . A)*A       (origin-to-center component perp to axis)
**
** Then the quadratic in t is:
**   a = a_v . a_v
**   b = 2 * (a_v . c_v)
**   c = c_v . c_v - r^2
**
** This is equivalent to the 2D ray-circle test in the projected plane.
**
** Height limiting: if limit > 0, the hit point's projection onto the axis
** must fall within [-limit, +limit] of the center.
*/

#include "intersect_cylinder.h"

/*
** check_lim -- Verify a hit point lies within the cylinder's height bounds.
**
** Computes two cap points (center +/- limit * axis) and checks that the
** hit point is "between" them using dot product sign tests. The hit point
** must be on the axis-side of p2 (bottom cap) and not past p1 (top cap).
*/
static int	check_lim(t_ray *r, t_prim *o, double *t, double *t_test)
{
	t_vector	p1;
	t_vector	p2;
	t_vector	ph;

	ph = vadd(r->loc, vmult(r->dir, *t_test));
	p1 = vadd(o->loc, vmult(o->dir, o->limit));
	p2 = vadd(o->loc, vmult(o->dir, -o->limit));
	/* dot(axis, ph - p1) <= 0: hit is not beyond the top cap
	** dot(axis, ph - p2) >= 0: hit is not beyond the bottom cap */
	if (vdot(o->dir, vsub(ph, p1)) <= 0 && vdot(o->dir, vsub(ph, p2)) >= 0)
	{
		*t = *t_test;
		return (1);
	}
	return (0);
}

/*
** check_t -- Accept a candidate t value, applying height limit if set.
** A limit of -1 means the cylinder is infinite (no height restriction).
*/
static int	check_t(t_ray *r, t_prim *o, double *t, double *t_test)
{
	if (o->limit != -1)
	{
		if (check_lim(r, o, t, t_test))
			return (1);
	}
	else
	{
		*t = *t_test;
		return (1);
	}
	return (0);
}

/*
** find_t -- Solve the quadratic and pick the nearest valid positive t.
**
** Tries the nearer root first. If it fails (behind camera or outside height
** limits), tries the farther root. Return 1 for front hit, 2 for inside hit.
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
		if (check_t(r, o, t, &tc))
			return (1);
	tc = (tc == t1) ? t0 : t1;
	if (tc > EPSILON)
		if (check_t(r, o, t, &tc))
			return (2);
	return (0);
}

/*
** intersect_cylinder -- Test a ray against a (possibly height-limited) cylinder.
**
** Parameters:
**   r - ray with origin and direction
**   o - cylinder primitive (center, axis, radius, limit)
**   t - output: ray parameter at intersection
**
** Returns: 0 = miss, 1 = front hit, 2 = hit from inside.
*/
int			intersect_cylinder(t_ray *r, t_prim *o, double *t)
{
	t_int_cylinder	c;

	c.dist = vsub(r->loc, o->loc);
	/* Remove the axis-parallel component from the ray direction */
	c.a_v = vsub(r->dir, vmult(o->dir, vdot(r->dir, o->dir)));
	/* Remove the axis-parallel component from the origin offset */
	c.c_v = vsub(c.dist, (vmult(o->dir, vdot(c.dist, o->dir))));
	/* Now solve the 2D circle intersection in the projected plane */
	c.quad.a = vdot(c.a_v, c.a_v);
	c.quad.b = 2.0 * vdot(c.a_v, c.c_v);
	c.quad.c = vdot(c.c_v, c.c_v) - (o->radius * o->radius);
	c.quad.discr = c.quad.b * c.quad.b - 4.0 * c.quad.a * c.quad.c;
	if (c.quad.discr < EPSILON)
		return (0);
	return (find_t(&c.quad, t, o, r));
}
