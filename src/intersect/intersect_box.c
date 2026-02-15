/*
** intersect_box.c -- Axis-Aligned Bounding Box (AABB) intersection using
** the slab method.
**
** An AABB is defined by two corner points: box[0] = min corner and
** box[1] = max corner. The box can be thought of as the intersection of
** three pairs of parallel planes (slabs), one pair per axis.
**
** The slab method works as follows:
**   For each axis (x, y, z):
**     - Compute t_enter = distance to the near slab
**     - Compute t_exit  = distance to the far slab
**   The ray is inside the box when it is inside ALL three slabs
**   simultaneously, so:
**     - Overall t_enter = max(t_enter_x, t_enter_y, t_enter_z)
**     - Overall t_exit  = min(t_exit_x, t_exit_y, t_exit_z)
**   If t_enter > t_exit, the ray misses the box.
**
** The sign[] array handles the case where the ray direction is negative
** along an axis, which swaps which face is "near" vs "far".
**
** This function is used as a fast pre-check before testing individual
** triangles in a mesh object. It is much cheaper to test a box than
** to test hundreds of triangles, so rays that miss the bounding box
** skip the mesh entirely.
*/

#include "rt.h"
#include "intersect_object.h"

/*
** intersect_box -- Test a ray against an axis-aligned bounding box.
**
** Parameters:
**   r      - ray (origin + direction)
**   box[2] - AABB corners: box[0] = min, box[1] = max
**
** Returns: 0 = miss, 1 = ray intersects the box.
** Note: does not compute the exact t value since this is only used
** as a culling test, not for finding the actual hit point.
*/
int				intersect_box(t_ray *r, t_vector box[2])
{
	t_intersect_box		b;

	/* Precompute inverse direction to replace divisions with multiplies */
	b.inv_dir = (t_vector){1.0 / r->dir.x, 1.0 / r->dir.y, 1.0 / r->dir.z};
	/* sign[i] = 1 if ray goes in -i direction; selects which face is near */
	b.sign[0] = (b.inv_dir.x < 0.0);
	b.sign[1] = (b.inv_dir.y < 0.0);
	b.sign[2] = (b.inv_dir.z < 0.0);
	/* X-axis slab entry and exit */
	b.min.x = (box[b.sign[0]].x - r->loc.x) * b.inv_dir.x;
	b.max.x = (box[1 - b.sign[0]].x - r->loc.x) * b.inv_dir.x;
	/* Y-axis slab entry and exit */
	b.min.y = (box[b.sign[1]].y - r->loc.y) * b.inv_dir.y;
	b.max.y = (box[1 - b.sign[1]].y - r->loc.y) * b.inv_dir.y;
	/* Early rejection: X and Y slabs don't overlap */
	if (b.min.x > b.max.y || b.min.y > b.max.x)
		return (0);
	/* Narrow the interval: take the tighter of the X and Y bounds */
	if (b.min.y > b.min.x)
		b.min.x = b.min.y;
	if (b.max.y < b.min.x)
		b.max.x = b.max.y;
	/* Z-axis slab entry and exit */
	b.min.z = (box[b.sign[2]].z - r->loc.z) * b.inv_dir.z;
	b.max.z = (box[1 - b.sign[2]].z - r->loc.z) * b.inv_dir.z;
	/* Final rejection: combined XY interval vs Z slab */
	if (b.min.x > b.max.z || b.min.z > b.max.x)
		return (0);
	return (1);
}
