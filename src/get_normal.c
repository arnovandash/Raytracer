/*
** get_normal.c -- Surface normal computation for each primitive type.
**
** After a ray hits a surface, we need the outward-facing normal vector at
** the hit point for lighting calculations (diffuse shading, specular
** highlights, reflections, refractions). Each primitive type has its own
** geometric formula for the normal.
**
** The general contract: given a hit point on a surface, return a unit-length
** normal vector pointing outward. If the ray hit from inside the object
** (ray.inter == 2), the normal is flipped so it faces the incoming ray.
*/

#include "diffuse.h"
#include "intersect_cone.h"

/*
** Cone normal computation using Rodrigues' rotation formula.
**
** A cone's surface normal is more complex than other primitives because the
** surface tilts by the cone's half-angle relative to the radial direction.
**
** Algorithm:
**   1. Project (hit - apex) onto the cone axis to get the axial component.
**   2. Subtract the projection to get the radial direction (perpendicular
**      to the axis, pointing outward from the axis to the hit point).
**   3. Determine a rotation axis perpendicular to both the cone axis and
**      the radial direction. The direction depends on which side of the
**      apex the hit is on.
**   4. Decompose the radial normal into components parallel and orthogonal
**      to the rotation axis.
**   5. Apply Rodrigues' rotation to tilt the orthogonal component by the
**      cone's half-angle:
**        N_rotated = cos(angle) * N_orth + sin(angle) * (rot x N_orth)
**   6. Recombine with the parallel component to get the final normal.
**
** This effectively "tilts" the cylinder-like radial normal by the cone's
** half-angle so it lies tangent to the cone's sloped surface.
*/
static t_vector	get_con_normal(t_env *e, t_vector ray)
{
	t_cone_normal	cn;

	/* Step 1: project (hit - apex) onto the cone axis */
	cn.pro = vproject(vsub(ray, e->p_hit->loc), e->p_hit->dir);
	/* Step 2: radial normal = (hit - apex) minus its axial component */
	cn.normal = vunit(vsub(vsub(ray, e->p_hit->loc), cn.pro));
	/* Step 3: rotation axis via cross product; direction flips based on
	** which side of the apex the projection falls on */
	cn.rot = (vcomp(vadd(cn.pro, e->p_hit->loc), e->p_hit->loc) >= 0) ?
		vunit(vcross(e->p_hit->dir, cn.normal)) :
		vunit(vcross(cn.normal, e->p_hit->dir));
	/* Steps 4-5: Rodrigues' rotation -- decompose normal, rotate the
	** orthogonal part by the cone half-angle */
	cn.p_par = vproject(cn.normal, cn.rot);
	cn.p_orth = vsub(cn.normal, cn.p_par);
	cn.nnor_orth = vadd(vmult(cn.p_orth, e->p_hit->cos_angle),
			vmult(vcross(cn.rot, cn.p_orth), e->p_hit->sin_angle));
	/* Step 6: recombine parallel + rotated orthogonal */
	cn.normal = vunit(vadd(cn.nnor_orth, cn.p_par));
	return (cn.normal);
}

/*
** get_normal -- Dispatch to the appropriate normal formula based on
** the type of primitive that was hit.
**
** Parameters:
**   e   - environment containing hit info (p_hit, o_hit, hit_type, ray)
**   ray - the 3D hit point on the surface
**
** Returns: unit normal vector pointing outward from the surface.
**
** Special cases:
**   - For mesh faces (FACE), the precomputed face normal is returned,
**     flipped if the ray approaches from behind.
**   - For planes/disks, the stored normal is returned, flipped if
**     the ray hits the back face (dot(normal, ray_dir) > 0).
**   - If ray.inter == 2, the ray originated inside the object, so
**     the normal is negated to point inward (toward the ray origin),
**     which is needed for correct refraction calculations.
*/
t_vector		get_normal(t_env *e, t_vector ray)
{
	t_vector	normal;

	normal = (t_vector){0.0, 0.0, 1.0};
	if (e->hit_type == FACE)
		/* Mesh face: use precomputed face normal, flip to face the ray */
		return ((vdot(*(e->o_hit->n), e->ray.dir) < 0.0) ?
			vunit(*e->o_hit->n) :
			vunit(vneg(*e->o_hit->n)));
	else if (e->p_hit->type == PRIM_SPHERE ||
		e->p_hit->type == PRIM_HEMI_SPHERE)
		/* Sphere: normal = normalize(hit_point - center) / radius */
		normal = (vunit(vdiv(vsub(ray, e->p_hit->loc), e->p_hit->radius)));
	else if (e->p_hit->type == PRIM_PLANE || e->p_hit->type == PRIM_DISK)
		/* Plane/disk: use stored normal, flip to face the ray */
		return ((vdot(e->p_hit->normal, e->ray.dir) < 0.0) ?
			vunit(e->p_hit->normal) :
			vunit(vneg(e->p_hit->normal)));
	else if (e->p_hit->type == PRIM_CYLINDER)
		/* Cylinder: subtract axial component to get the radial direction.
		** N = normalize( (P - C) - project(P - C, axis) ) */
		normal = (vunit(vsub(vsub(ray, e->p_hit->loc),
			vproject(vsub(ray, e->p_hit->loc), e->p_hit->dir))));
	else if (e->p_hit->type == PRIM_CONE)
		normal = (vunit(get_con_normal(e, ray)));
	/* If the ray hit from inside (inter == 2), flip the normal inward */
	if (e->ray.inter == 2)
		normal = vneg(normal);
	return (normal);
}
