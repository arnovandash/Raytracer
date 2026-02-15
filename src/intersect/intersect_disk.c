/*
** intersect_disk.c -- Ray-disk intersection (bounded plane).
**
** A disk is a circular region of a plane, defined by a center point,
** a normal vector, and a radius. The intersection algorithm is:
**
**   1. Intersect the ray with the infinite plane (same math as plane).
**   2. If the plane hit point exists, check whether it lies within the
**      disk's radius from the center.
**
** The distance check uses the magnitude of (hit_point - center). If this
** distance exceeds the radius, the ray hit the plane outside the disk.
*/

#include "rt.h"

/*
** intersect_disk -- Test a ray against a disk (bounded circular plane).
**
** Parameters:
**   r - ray (origin + direction)
**   o - disk primitive (center, normal, radius)
**   t - output: ray parameter at intersection
**
** Returns: 0 = miss, 1 = hit within the disk radius.
*/
int		intersect_disk(t_ray *r, t_prim *o, double *t)
{
	t_vector	point;
	double		denominator;
	double		numerator;
	double		t0;

	/* Step 1: intersect with the infinite plane containing this disk */
	if ((denominator = vdot(r->dir, o->normal)) == 0)
		return (0);
	numerator = vdot(o->loc, o->normal) - vdot(r->loc, o->normal);
	t0 = numerator / denominator;
	if (t0 > EPSILON)
	{
		/* Step 2: check if the plane hit point is within the disk radius */
		point = vadd(r->loc, vmult(r->dir, t0));
		if (vnormalize(vsub(point, o->loc)) <= o->radius)
		{
			*t = t0;
			return (1);
		}
		return (0);
	}
	return (0);
}
