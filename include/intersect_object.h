/*
** intersect_object.h -- Data structure for AABB (bounding box) intersection.
**
** The slab method tests a ray against three pairs of axis-aligned planes.
** This struct holds the precomputed inverse direction (to avoid division
** in the inner loop) and the per-axis entry/exit t values.
*/

#ifndef INTERSECT_OBJECT_H
# define INTERSECT_OBJECT_H

/*
** Intermediate state for AABB slab intersection.
**   inv_dir - reciprocal of each ray direction component (1/Dx, 1/Dy, 1/Dz)
**   sign[3] - 0 or 1 per axis: indicates if the ray travels in the
**             negative direction on that axis, used to select near/far faces
**   min     - entry t values for each axis slab (reused for narrowing)
**   max     - exit t values for each axis slab (reused for narrowing)
*/
typedef struct	s_intersect_box
{
	t_vector	inv_dir;
	int			sign[3];
	t_vector	min;
	t_vector	max;
}				t_intersect_box;

#endif
