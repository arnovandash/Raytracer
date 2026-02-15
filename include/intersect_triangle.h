/*
** intersect_triangle.h -- Data structure for Moller-Trumbore ray-triangle
** intersection algorithm.
**
** The struct holds all intermediate values computed during the algorithm,
** avoiding repeated cross/dot product computations and enabling the
** step-by-step early-rejection checks on barycentric coordinates.
*/

#ifndef INTERSECT_TRIANGLE_H
# define INTERSECT_TRIANGLE_H

# include "rt.h"

/*
** Intermediate state for Moller-Trumbore intersection.
**   edge1, edge2 - triangle edge vectors (v1-v0, v2-v0)
**   p            - cross(ray_dir, edge2), used for determinant and u
**   q            - cross(dist, edge1), used for v and t
**   dist         - vector from v0 to ray origin (T in the algorithm)
**   d            - determinant (scalar triple product of [D, e1, e2])
**   inverse_d    - 1/determinant, precomputed to avoid repeated division
**   u, v         - barycentric coordinates of the hit point
**   t0           - ray parameter at intersection
*/
typedef struct	s_intersect_triangle
{
	t_vector	edge1;
	t_vector	edge2;
	t_vector	p;
	t_vector	q;
	t_vector	dist;
	double		d;
	double		inverse_d;
	double		u;
	double		v;
	double		t0;
}				t_intersect_triangle;

#endif
