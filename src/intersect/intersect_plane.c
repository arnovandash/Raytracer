/*
** intersect_plane.c -- Ray-plane intersection via dot product.
**
** A plane is defined by a point P0 (loc) and a normal vector N. Any point
** P on the plane satisfies: (P - P0) . N = 0, or equivalently P . N = P0 . N.
**
** Substituting the ray equation P(t) = O + t*D:
**   (O + t*D) . N = P0 . N
**   t * (D . N) = (P0 . N) - (O . N)
**   t = ((P0 - O) . N) / (D . N)
**
** If D . N == 0, the ray is parallel to the plane (no intersection).
** If t <= EPSILON, the intersection is behind the ray origin.
*/

#include "rt.h"

/*
** intersect_plane -- Test a ray against an infinite plane.
**
** Parameters:
**   r - ray (origin + direction)
**   o - plane primitive (point on plane + normal)
**   t - output: ray parameter at intersection
**
** Returns: 0 = miss (parallel or behind), 1 = hit.
*/
int		intersect_plane(t_ray *r, t_prim *o, double *t)
{
	double	denominator;
	double	numerator;
	double	t0;

	/* D . N: if zero, ray is parallel to the plane */
	if ((denominator = vdot(r->dir, o->normal)) == 0)
		return (0);
	/* (P0 . N) - (O . N) = (P0 - O) . N */
	numerator = vdot(o->loc, o->normal) - vdot(r->loc, o->normal);
	t0 = numerator / denominator;
	if (t0 > EPSILON)
	{
		*t = t0;
		return (1);
	}
	return (0);
}
