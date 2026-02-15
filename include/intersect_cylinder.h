/*
** intersect_cylinder.h -- Data structure for ray-cylinder intersection.
**
** t_int_cylinder holds the intermediate values needed to set up the
** quadratic equation. The key idea is projecting the ray into the 2D
** plane perpendicular to the cylinder axis, reducing the problem to
** a ray-circle intersection.
*/

#ifndef INTERSECT_CYLINDER_H
# define INTERSECT_CYLINDER_H

# include "rt.h"

/*
** Intermediate state for ray-cylinder quadratic intersection.
**   quad - quadratic coefficients (a, b, c) and discriminant
**   dist - vector from cylinder center to ray origin (O - C)
**   a_v  - ray direction projected perpendicular to the cylinder axis
**   c_v  - origin offset projected perpendicular to the cylinder axis
*/
typedef struct	s_int_cylinder
{
	t_quad		quad;
	t_vector	dist;
	t_vector	a_v;
	t_vector	c_v;
}				t_int_cylinder;

#endif
