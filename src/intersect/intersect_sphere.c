/*
** intersect_sphere.c -- Ray-sphere intersection using the quadratic formula.
**
** The sphere is defined by a center C and radius r. A point P lies on the
** sphere if |P - C|^2 = r^2. Substituting the parametric ray equation
** P(t) = O + t*D into the sphere equation gives a quadratic in t:
**
**   a*t^2 + b*t + c = 0
**
** where:
**   a = D . D           (always positive for a non-degenerate ray)
**   b = 2 * D . (O - C)
**   c = (O - C) . (O - C) - r^2
**
** The discriminant (b^2 - 4ac) determines the intersection count:
**   < 0  -->  ray misses the sphere entirely
**   = 0  -->  ray is tangent (one intersection)
**   > 0  -->  ray passes through (two intersections)
**
** We pick the nearest positive t as the visible intersection. If the
** nearest t is negative but the farther t is positive, the ray origin
** is inside the sphere (return code 2, used for refraction).
*/

#include "rt.h"

/*
** find_t -- Given quadratic coefficients, solve for the nearest positive t.
**
** Returns:
**   0 - both roots are behind the ray (no visible intersection)
**   1 - nearest root is in front (normal front-face hit)
**   2 - nearest root is behind but farther root is in front (ray is inside
**       the sphere, hitting the back wall from within)
*/
static int	find_t(double a, double b, double discr, double *t)
{
	double	sqrt_discr;
	double	t0;
	double	t1;
	double	tc;

	sqrt_discr = sqrt(discr);
	/* Quadratic formula: two roots */
	t0 = (-b + sqrt_discr) / (2.0 * a);
	t1 = (-b - sqrt_discr) / (2.0 * a);
	/* Pick the smaller (nearer) root first */
	tc = (t0 > t1) ? t1 : t0;
	if (tc > EPSILON)
	{
		*t = tc;
		return (1);
	}
	else
	{
		/* Nearer root is behind us; try the farther root */
		tc = (tc == t1) ? t0 : t1;
		if (tc > EPSILON)
		{
			*t = tc;
			return (2);
		}
	}
	return (0);
}

/*
** intersect_sphere -- Test a ray against a sphere primitive.
**
** Parameters:
**   r - ray with origin (loc) and direction (dir)
**   o - sphere primitive with center (loc) and radius
**   t - output: distance along the ray to the hit point
**
** Returns: 0 = miss, 1 = front hit, 2 = hit from inside.
*/
int			intersect_sphere(t_ray *r, t_prim *o, double *t)
{
	double		a;
	double		b;
	double		c;
	double		discr;
	t_vector	dist;

	dist = vsub(r->loc, o->loc);
	a = vdot(r->dir, r->dir);
	b = 2.0 * vdot(r->dir, dist);
	c = vdot(dist, dist) - (o->radius * o->radius);
	discr = b * b - 4.0 * a * c;
	if (discr < EPSILON)
		return (0);
	return (find_t(a, b, discr, t));
}
