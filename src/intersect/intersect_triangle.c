/*
** intersect_triangle.c -- Moller-Trumbore ray-triangle intersection.
**
** Reference: https://en.wikipedia.org/wiki/Moller-Trumbore_intersection_algorithm
**
** This is one of the most efficient ray-triangle algorithms. It avoids
** precomputing the triangle's plane equation by solving for the intersection
** point and barycentric coordinates simultaneously.
**
** Given triangle vertices v0, v1, v2 and ray P(t) = O + t*D:
**
**   Any point in the triangle can be written as:
**     P = (1 - u - v) * v0 + u * v1 + v * v2
**   where u >= 0, v >= 0, and u + v <= 1 (barycentric coordinates).
**
**   Setting this equal to the ray equation and solving the 3x3 linear system
**   using Cramer's rule yields:
**
**   [t, u, v]^T = (1 / det) * [Q . e2, T . P, D . Q]^T
**
**   where:
**     e1 = v1 - v0, e2 = v2 - v0      (triangle edge vectors)
**     P  = D x e2                       (cross product for determinant)
**     det = e1 . P                      (scalar triple product)
**     T  = O - v0                       (vector from v0 to ray origin)
**     Q  = T x e1                       (second cross product)
**
**   If det is near zero, the ray is parallel to the triangle plane.
**   The barycentric coordinates (u, v) are checked at each step for
**   early rejection -- this is what makes the algorithm fast.
*/

#include "intersect_triangle.h"

/*
** intersect_triangle -- Test a ray against a triangle face.
**
** Parameters:
**   r - ray (origin + direction)
**   f - triangle face (three vertex pointers + face normal pointer)
**   t - output: ray parameter at intersection
**
** Returns: 0 = miss, 1 = hit.
*/
int		intersect_triangle(t_ray *r, t_face *f, double *t)
{
	t_intersect_triangle	it;

	it.edge1 = vsub(*f->v1, *f->v0);
	it.edge2 = vsub(*f->v2, *f->v0);
	/* P = D x e2, used in both the determinant and the u coordinate */
	it.p = vcross(r->dir, it.edge2);
	/* Determinant = e1 . P = scalar triple product [D, e1, e2] */
	it.d = vdot(it.edge1, it.p);
	/* If determinant is near zero, ray is parallel to the triangle */
	if (it.d > -EPSILON && it.d < EPSILON)
		return (0);
	it.inverse_d = 1.0 / it.d;
	/* T = vector from v0 to ray origin */
	it.dist = vsub(r->loc, *f->v0);
	/* First barycentric coordinate: u = (T . P) / det */
	it.u = vdot(it.dist, it.p) * it.inverse_d;
	if (it.u < 0.0 || it.u > 1.0)
		return (0);
	/* Q = T x e1, used for both v and t */
	it.q = vcross(it.dist, it.edge1);
	/* Second barycentric coordinate: v = (D . Q) / det */
	it.v = vdot(r->dir, it.q) * it.inverse_d;
	/* Point is inside triangle only if u >= 0, v >= 0, u + v <= 1 */
	if (it.v < 0.0 || it.u + it.v > 1.0)
		return (0);
	/* Ray parameter: t = (e2 . Q) / det */
	it.t0 = vdot(it.edge2, it.q) * it.inverse_d;
	if (it.t0 > EPSILON)
	{
		*t = it.t0;
		return (1);
	}
	return (0);
}
