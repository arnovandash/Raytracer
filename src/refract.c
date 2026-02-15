/*
** refract.c -- Refraction (transparency) via Snell's law.
**
** When a ray hits a transparent surface (e.g., glass), it bends according
** to Snell's law:  n1 * sin(theta1) = n2 * sin(theta2)
** where n1 and n2 are the indices of refraction (IOR) of the two media.
**
** The refracted ray direction is computed as:
**   T = eta * D + (eta * cos_i - cos_t) * N
** where:
**   eta   = n1 / n2  (ratio of refractive indices)
**   D     = incoming ray direction (unit vector)
**   N     = surface normal (pointing outward)
**   cos_i = dot(D, N)  (cosine of incident angle)
**   cos_t = sqrt(1 - eta^2 * (1 - cos_i^2))  (cosine of refracted angle)
**
** TOTAL INTERNAL REFLECTION:
**   When the term under the square root (called "factor" in the code)
**   becomes negative, no refracted ray exists -- all light is reflected.
**   This happens when light travels from a denser medium to a less dense
**   one at a steep angle (e.g., looking up through water at a sharp angle).
**   In this case, we fall back to set_reflect_ray from reflect.c.
**
** INSIDE/OUTSIDE TRACKING:
**   The RAY_INSIDE flag tracks whether the ray is currently inside an
**   object. When entering (outside -> inside), the IOR ratio is 1/n
**   (air to material). When exiting (inside -> outside), it is n/1
**   (material to air). The normal is flipped when exiting so the math
**   remains consistent.
**
** There are separate functions for primitives and mesh objects because
** they store their material index in different struct fields (p_hit->material
** vs object_hit->material), but the math is identical.
*/

#include "rt.h"

/*
** refract_prim -- Compute the refracted ray direction for a primitive hit.
**
** Parameters:
**   e       -- Environment with the incoming ray and hit information.
**   refract -- Environment to receive the refracted ray direction.
**   n       -- Surface normal (may have been flipped if ray is inside).
**
** Returns 1 if refraction succeeds, 0 if total internal reflection occurs.
**
** The sign of cos = dot(ray_dir, normal) determines inside/outside:
**   cos > 0: ray and normal point in the same half-space -> ray is INSIDE
**            the object, exiting. Use IOR directly (material -> air).
**   cos < 0: ray enters the object. Use 1/IOR (air -> material).
**
** The factor (discriminant) = 1 - (1 - cos^2) * eta^2
** This is cos^2(theta_t). If <= 0, total internal reflection occurs.
*/
static int	refract_prim(t_env *e, t_env *refract, t_vector n)
{
	double	cos;
	double	factor;
	double	index;

	cos = vdot(e->ray.dir, n);
	if (cos > 0.0)
	{
		/* Ray is exiting the object: eta = IOR (material to air, n_mat/1) */
		index = e->material[e->p_hit->material]->ior;
		factor = 1.0 - (1.0 - cos * cos) * index * index;
		if (factor <= 0.0)
			return (0);
		/* Refracted direction: positive sqrt because ray exits */
		factor = -cos * index + sqrt(factor);
	}
	else
	{
		/* Ray is entering the object: eta = 1/IOR (air to material) */
		index = 1.0 / e->material[e->p_hit->material]->ior;
		factor = 1.0 - (1.0 - cos * cos) * index * index;
		if (factor <= 0.0)
			return (0);
		/* Refracted direction: negative sqrt because ray enters */
		factor = -cos * index - sqrt(factor);
	}
	/* T = eta * D + factor * N */
	refract->ray.dir = vadd(vmult(e->ray.dir, index), vmult(n, factor));
	return (1);
}

/*
** refract_obj -- Compute the refracted ray direction for a mesh object hit.
**
** Identical math to refract_prim, but reads the material IOR from
** e->object_hit->material instead of e->p_hit->material. Mesh objects
** (loaded from OBJ files) use the object_hit pointer for their material.
*/
static int	refract_obj(t_env *e, t_env *refract, t_vector n)
{
	double	cos;
	double	factor;
	double	index;

	cos = vdot(e->ray.dir, n);
	if (cos > 0.0)
	{
		index = e->material[e->object_hit->material]->ior;
		factor = 1.0 - (1.0 - cos * cos) * index * index;
		if (factor <= 0.0)
			return (0);
		factor = -cos * index + sqrt(factor);
	}
	else
	{
		index = 1.0 / e->material[e->object_hit->material]->ior;
		factor = 1.0 - (1.0 - cos * cos) * index * index;
		if (factor <= 0.0)
			return (0);
		factor = -cos * index - sqrt(factor);
	}
	refract->ray.dir = vadd(vmult(e->ray.dir, index), vmult(n, factor));
	return (1);
}

/*
** set_refract_ray_object -- Set up a refraction ray for a mesh object hit.
**
** Computes the hit point, gets the surface normal, and determines whether
** the ray is entering or exiting the object:
**
**   - If RAY_INSIDE is set: the ray is currently inside the object and is
**     exiting. Flip the normal (so it points inward, matching the ray's
**     side), attempt refraction. If successful, clear RAY_INSIDE. If total
**     internal reflection occurs, fall back to set_reflect_ray.
**
**   - If RAY_INSIDE is not set: the ray is entering the object. Use the
**     normal as-is, attempt refraction. If successful, set RAY_INSIDE.
**     If total internal reflection, fall back to reflection.
*/
static void	set_refract_ray_object(t_env *e, t_env *refract)
{
	t_vector	n;

	refract->ray.loc = vadd(e->ray.loc, vmult(e->ray.dir, e->t));
	n = get_normal(e, refract->ray.loc);
	if (refract->flags & RAY_INSIDE)
	{
		/* Exiting: flip normal to point inward (same side as ray) */
		n = vunit(vneg(n));
		if (refract_obj(e, refract, n))
			refract->flags &= ~RAY_INSIDE;
		else
			set_reflect_ray(e, refract);
	}
	else
	{
		/* Entering: normal already points outward */
		if (refract_obj(e, refract, n))
			refract->flags |= RAY_INSIDE;
		else
			set_reflect_ray(e, refract);
	}
}

/*
** set_refract_ray_prim -- Set up a refraction ray for a primitive hit.
**
** Same logic as set_refract_ray_object but calls refract_prim (which reads
** the material from p_hit instead of object_hit).
*/
static void	set_refract_ray_prim(t_env *e, t_env *refract)
{
	t_vector	n;

	refract->ray.loc = vadd(e->ray.loc, vmult(e->ray.dir, e->t));
	n = get_normal(e, refract->ray.loc);
	if (refract->flags & RAY_INSIDE)
	{
		n = vunit(vneg(n));
		if (refract_prim(e, refract, n))
			refract->flags &= ~RAY_INSIDE;
		else
			set_reflect_ray(e, refract);
	}
	else
	{
		if (refract_prim(e, refract, n))
			refract->flags |= RAY_INSIDE;
		else
			set_reflect_ray(e, refract);
	}
}

/*
** refract -- Trace a refraction ray and return the color seen through it.
**
** Creates a stack copy of the environment, computes the refracted (or
** totally-internally-reflected) ray, traces it through the scene, and
** recursively shades the result.
**
** Parameters:
**   e      -- Current environment (ray that hit a transparent surface)
**   depth  -- Current recursion depth (checked against e->maxdepth)
**   colour -- Fallback color returned if depth limit is exceeded
**
** Returns: The color seen through the transparent surface, or the fallback
**          color if the recursion depth limit has been reached.
**
** The hit_type field (FACE or PRIMITIVE) determines which refraction path
** to use, since mesh faces and geometric primitives store materials
** differently.
*/
t_colour	refract(t_env *e, int depth, t_colour colour)
{
	t_env		refract_env;

	++g_tls_stats.rays;
	++g_tls_stats.refraction_rays;
	if (depth > e->maxdepth)
		return (colour);
	refract_env = *e;
	refract_env.p_hit = NULL;
	if (e->hit_type == FACE)
	{
		set_refract_ray_object(e, &refract_env);
		intersect_scene(&refract_env);
		colour = find_colour_struct(&refract_env, depth);
	}
	else if (e->hit_type == PRIMITIVE)
	{
		set_refract_ray_prim(e, &refract_env);
		intersect_scene(&refract_env);
		colour = find_colour_struct(&refract_env, depth);
	}
	return (colour);
}
