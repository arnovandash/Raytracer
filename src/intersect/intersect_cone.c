/*
** intersect_cone.c -- Ray-cone intersection using the quadratic formula.
**
** A cone is defined by an apex C, an axis direction A, and a half-angle
** theta. A point P is on the cone surface if the angle between (P - C)
** and the axis A equals theta. Algebraically:
**
**   [(P - C) . A]^2 = |P - C|^2 * cos^2(theta)
**
** This can be rearranged to distinguish radial and axial components:
**   cos^2(theta) * |perp_component|^2 - sin^2(theta) * |axial_component|^2 = 0
**
** Substituting the ray P(t) = O + t*D and expanding gives a quadratic
** at^2 + bt + c = 0 where the coefficients mix cos^2 and sin^2 terms.
**
** The key difference from a cylinder: the cylinder has radius constant
** along the axis, while the cone's effective radius grows linearly with
** distance from the apex. The sin^2 terms capture this linear growth.
**
** Height limiting works identically to the cylinder (see intersect_cylinder.c).
*/

#include "intersect_cone.h"

/*
** check_lim -- Verify a hit point lies within the cone's height bounds.
** Same logic as the cylinder: dot product tests against two cap planes.
*/
static int	check_lim(t_ray *r, t_prim *o, double *t, double *t_test)
{
	t_vector	p1;
	t_vector	p2;
	t_vector	ph;

	ph = vadd(r->loc, vmult(r->dir, *t_test));
	p1 = vadd(o->loc, vmult(o->dir, o->limit));
	p2 = vadd(o->loc, vmult(o->dir, -o->limit));
	if (vdot(o->dir, vsub(ph, p1)) <= 0 && vdot(o->dir, vsub(ph, p2)) >= 0)
	{
		*t = *t_test;
		return (1);
	}
	return (0);
}

/*
** check_t -- Accept a candidate t, applying height limit if applicable.
** A limit of -1 means the cone extends infinitely in both directions.
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
** Returns 1 for front hit, 2 for inside hit.
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
** intersect_cone -- Test a ray against a (possibly height-limited) cone.
**
** Parameters:
**   r - ray (origin + direction)
**   o - cone primitive (apex, axis, cos/sin of half-angle, limit)
**   t - output: ray parameter at intersection
**
** Returns: 0 = miss, 1 = front hit, 2 = hit from inside.
**
** The quadratic coefficients differ from the cylinder by mixing cos^2 and
** sin^2 terms. Where the cylinder has:
**   a = |D_perp|^2
** the cone has:
**   a = cos^2 * |D_perp|^2 - sin^2 * (D . A)^2
** The sin^2 subtractive term accounts for the cone flaring outward.
*/
int			intersect_cone(t_ray *r, t_prim *o, double *t)
{
	t_intersect_cone	c;

	c.dist = vsub(r->loc, o->loc);
	/* Dot products of ray direction and origin-offset with the cone axis */
	c.v_va = vdot(r->dir, o->dir);
	c.dp_va = vdot(c.dist, o->dir);
	/* Perpendicular components (same as cylinder) */
	c.a_v = vsub(r->dir, vmult(o->dir, c.v_va));
	c.c_v = vsub(c.dist, vmult(o->dir, c.dp_va));
	c.cos2 = o->cos_angle * o->cos_angle;
	c.sin2 = o->sin_angle * o->sin_angle;
	/* Quadratic coefficients: cos^2 * radial^2 - sin^2 * axial^2 */
	c.quad.a = c.cos2 * vdot(c.a_v, c.a_v) -
		c.sin2 * c.v_va * c.v_va;
	c.quad.b = 2.0 * c.cos2 * vdot(c.a_v, c.c_v) -
		2.0 * c.sin2 * c.v_va * c.dp_va;
	c.quad.c = c.cos2 * vdot(c.c_v, c.c_v) - c.sin2 * c.dp_va * c.dp_va;
	c.quad.discr = c.quad.b * c.quad.b - 4.0 * c.quad.a * c.quad.c;
	if (c.quad.discr < EPSILON)
		return (0);
	return (find_t(&c.quad, t, o, r));
}
