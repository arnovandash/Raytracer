/*
** find_colour_struct.c — Recursive color composition for raytracing
**
** This file is the "color integrator" of the raytracer. After a ray hits a
** surface, it must determine the final pixel color by blending three sources:
**
**   1. Surface color  — computed by Blinn-Phong diffuse/specular shading
**   2. Reflection color — from a recursive ray bounced off the surface
**   3. Refraction color — from a recursive ray transmitted through the surface
**
** Blending uses linear interpolation (lerp):
**   final = surface * (1 - weight) + effect * weight
**
** For example, a material with reflect=0.3 produces:
**   pixel = 70% surface shading + 30% reflected scene color
**
** Refraction is applied first (modifying the surface color), then reflection
** is applied to the result. This ordering means a glass-like material with
** both reflect=0.1 and refract=0.9 will be mostly transparent with a slight
** surface reflection — physically plausible for glass.
**
** The recursion terminates when depth reaches e->maxdepth, preventing
** infinite bounces between parallel mirrors.
**
** hit_type distinguishes between standalone primitives (sphere, plane, etc.)
** and mesh triangle faces loaded from OBJ files, since they store their
** material reference in different structs (t_prim vs t_object).
*/

#include "rt.h"

/*
** find_colour — Entry point for primary rays (depth 0).
** Computes the final pixel color as a packed 0xRRGGBB uint32_t.
**
** The color channels (r, g, b) are stored as doubles in [0.0, 1.0] during
** computation, then scaled to [0, 255] integers and bit-packed:
**   red   -> bits 23..16  (shifted left 16)
**   green -> bits 15..8   (shifted left 8)
**   blue  -> bits 7..0    (no shift)
*/

uint32_t	find_colour(t_env *e)
{
	t_colour	c;
	t_colour	l;
	t_colour	r;
	t_material	*mat;

	/* Compute diffuse+specular surface shading (Blinn-Phong) */
	c = (e->hit_type == FACE) ? face_diffuse(e) : prim_diffuse(e);
	/* Look up material from the appropriate struct based on hit type */
	mat = (e->hit_type == FACE) ?
		e->material[e->object_hit->material] :
		e->material[e->p_hit->material];
	/* Trace a reflection ray if the material is reflective */
	l = mat->reflect > 0.0 ? reflect(e, 1) : (t_colour){0.0, 0.0, 0.0, 0.0};
	/* Blend in refraction: lerp between surface color and refracted color */
	if (mat->refract > 0.0)
	{
		r = refract(e, 1, c);
		c.r = (c.r * (1 - mat->refract)) + (r.r * mat->refract);
		c.g = (c.g * (1 - mat->refract)) + (r.g * mat->refract);
		c.b = (c.b * (1 - mat->refract)) + (r.b * mat->refract);
	}
	/* Blend reflection with the (possibly refracted) surface color, */
	/* then pack RGB channels into a single uint32_t for the pixel buffer */
	return ((uint32_t)(
	(int)(((c.r * (1 - mat->reflect)) + (l.r * mat->reflect)) * 255.0) << 16 |
	(int)(((c.g * (1 - mat->reflect)) + (l.g * mat->reflect)) * 255.0) << 8 |
	(int)(((c.b * (1 - mat->reflect)) + (l.b * mat->reflect)) * 255.0)));
}

/*
** find_base_colour — Returns only diffuse/specular shading, no recursion.
** Used for preview or simplified rendering modes. Returns medium grey
** (0x7F7F7F) if the ray missed all geometry (hit_type == 0).
*/

uint32_t	find_base_colour(t_env *e)
{
	t_colour	c;

	if (!e->hit_type)
		return (0x7F7F7F);
	c = (e->hit_type == FACE) ? face_diffuse(e) : prim_diffuse(e);
	return ((uint32_t)(
	(unsigned int)(c.r * 255.0) << 16 |
	(unsigned int)(c.g * 255.0) << 8 |
	(unsigned int)(c.b * 255.0)));
}

/*
** find_colour_struct — Recursive version that returns a t_colour struct
** instead of a packed integer. Called by reflect() and refract() to get
** the color from secondary (bounced/transmitted) rays.
**
** This is the heart of recursive raytracing:
**   reflect() traces a new ray -> calls find_colour_struct() for that ray
**   refract() traces a new ray -> calls find_colour_struct() for that ray
** Each recursive call increments depth until maxdepth is reached.
**
** Returns mid-grey {0.5, 0.5, 0.5} for rays that miss all geometry,
** simulating a neutral background/sky.
*/

t_colour	find_colour_struct(t_env *e, int depth)
{
	t_colour	l;
	t_colour	r;
	t_colour	temp_c;
	t_material	*mat;

	/* Default reflection contribution is black (no reflection) */
	l = (t_colour){0.0, 0.0, 0.0, 1.0};
	if (!e->hit_type)
		return ((t_colour){0.5, 0.5, 0.5, 1.0});
	temp_c = (e->hit_type == FACE) ? face_diffuse(e) : prim_diffuse(e);
	mat = (e->hit_type == FACE) ?
		e->material[e->object_hit->material] :
		e->material[e->p_hit->material];
	/* Only recurse for reflection if we haven't exceeded max bounce depth */
	if (depth < e->maxdepth && mat->reflect > 0.0)
		l = reflect(e, depth + 1);
	/* Blend refraction into surface color */
	if (mat->refract > 0.0)
	{
		r = refract(e, depth + 1, temp_c);
		temp_c.r = (temp_c.r * (1 - mat->refract)) + (r.r * mat->refract);
		temp_c.g = (temp_c.g * (1 - mat->refract)) + (r.g * mat->refract);
		temp_c.b = (temp_c.b * (1 - mat->refract)) + (r.b * mat->refract);
	}
	/* Blend reflection into the final color: lerp(surface, reflected, weight) */
	temp_c.r = ((temp_c.r * (1 - mat->reflect)) + (l.r * mat->reflect));
	temp_c.g = ((temp_c.g * (1 - mat->reflect)) + (l.g * mat->reflect));
	temp_c.b = ((temp_c.b * (1 - mat->reflect)) + (l.b * mat->reflect));
	return (temp_c);
}
