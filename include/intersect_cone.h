/*
** intersect_cone.h -- Data structures for cone intersection and normal
** computation.
**
** t_intersect_cone holds the intermediate values needed to set up the
** quadratic equation for ray-cone intersection. The cone's half-angle
** creates a mix of cos^2 and sin^2 terms that weight the radial and
** axial components differently from a cylinder.
**
** t_cone_normal holds intermediate values for Rodrigues' rotation, which
** tilts the radial normal by the cone's half-angle to produce the correct
** surface normal on the cone's sloped surface.
*/

#ifndef INTERSECT_CONE_H
# define INTERSECT_CONE_H

# include "rt.h"

/*
** Intermediate state for ray-cone quadratic intersection.
**   quad  - quadratic coefficients (a, b, c) and discriminant
**   cos2  - cos^2(half_angle), weights the radial component
**   sin2  - sin^2(half_angle), weights the axial component
**   v_va  - dot(ray_dir, cone_axis): axial component of ray direction
**   dp_va - dot(dist, cone_axis): axial component of origin offset
**   dist  - vector from cone apex to ray origin
**   a_v   - ray direction component perpendicular to cone axis
**   c_v   - origin offset component perpendicular to cone axis
*/
typedef struct	s_intersect_cone
{
	t_quad		quad;
	double		cos2;
	double		sin2;
	double		v_va;
	double		dp_va;
	t_vector	dist;
	t_vector	a_v;
	t_vector	c_v;
}				t_intersect_cone;

/*
** Intermediate state for cone normal via Rodrigues' rotation.
**   pro      - projection of (hit - apex) onto the cone axis
**   normal   - initial radial normal (perpendicular to axis)
**   rot      - rotation axis (perpendicular to both axis and radial)
**   p_par    - component of normal parallel to rotation axis
**   p_orth   - component of normal orthogonal to rotation axis
**   nnor_orth - rotated orthogonal component (tilted by half-angle)
*/
typedef struct	s_cone_normal
{
	t_vector	pro;
	t_vector	normal;
	t_vector	rot;
	t_vector	p_par;
	t_vector	p_orth;
	t_vector	nnor_orth;
}				t_cone_normal;

#endif
