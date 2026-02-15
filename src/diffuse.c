/*
** diffuse.c — Blinn-Phong reflection model for surface shading
**
** Implements per-pixel lighting using the Blinn-Phong model, an efficient
** approximation of the classical Phong reflection model. For each surface
** point, contributions from ALL lights in the scene are accumulated.
**
** The Blinn-Phong model computes two terms per light:
**
**   Diffuse (Lambert's cosine law):
**     I_d = Kd * max(0, N . L)
**     Light intensity is proportional to the cosine of the angle between
**     the surface normal (N) and the direction to the light (L). Surfaces
**     facing the light are bright; surfaces at grazing angles are dim.
**
**   Specular (Blinn's halfway-vector approximation):
**     I_s = Ks * max(0, N . H)^50
**     H = normalize(V + L)  — the halfway vector between view and light
**     This produces a bright highlight where the surface acts like a mirror
**     between the viewer and the light. The exponent (50) controls
**     sharpness: higher = tighter, shinier highlights.
**
** Light attenuation uses a smooth inverse-square falloff:
**     attenuation = lumens * (half / (half + d^2))
**   where `lumens` is the light's brightness and `half` is the distance at
**   which intensity drops to 50%. This avoids the singularity at d=0 that
**   pure 1/d^2 falloff would cause.
**
** Shadow integration: before computing a light's contribution, a shadow ray
** is cast. The shadow factor (0.0 = fully lit, 1.0 = fully blocked) scales
** the light contribution via (1 - shadow). Partially transparent objects
** produce partial shadows.
**
** After accumulating all lights, each RGB channel is clamped to [0, 1].
*/

#include "diffuse.h"

/*
** diffuse_colour — Compute and accumulate one light's contribution.
**
** Parameters (via t_diffuse *d):
**   d->p     — the surface hit point
**   d->n     — the surface normal at the hit point
**   d->mat   — the material (diffuse color, specular color, intensities)
**   d->light — the current light source being evaluated
**   d->colour — running RGB accumulator (as a vector, using x/y/z for r/g/b)
**
** The function first checks if the point is in shadow. If fully shadowed
** (shadow >= 1.0), it skips all computation for this light.
*/

static void		diffuse_colour(t_env *e, t_diffuse *d)
{
	t_vector	temp_colour;
	double		shadow;

	shadow = in_shadow(e, d->light);
	if (shadow < 1.0)
	{
		/* L = unit vector from hit point toward light source */
		d->l = vsub(d->light->loc, d->p);
		d->dist = vnormalize(d->l);
		d->l = vdiv(d->l, d->dist);
		/* V = unit vector from hit point toward camera (viewer) */
		d->v = vunit(vsub(e->ray.loc, d->p));
		/* H = halfway vector = normalize(V + L), the Blinn optimization */
		/* Instead of reflecting L about N (expensive), H bisects V and L */
		d->h = vunit(vadd(d->v, d->l));
		/* Attenuation: lumens * half / (half + d^2) — smooth inverse-square */
		d->intensity = d->light->lm *
			(d->light->half / (d->light->half + d->dist * d->dist));
		/* Diffuse term: Kd * intensity * max(0, N.L) — Lambert's cosine law */
		d->ld = vmult(vmult(colour_to_vector(d->mat->diff),
			d->mat->diff.intensity), d->intensity * MAX(0, vdot(d->n, d->l)));
		/* Specular term: Ks * intensity * max(0, N.H)^50 */
		/* ipow50 computes x^50 efficiently via repeated squaring */
		d->ls = vmult(vmult(colour_to_vector(d->mat->spec),
			d->mat->spec.intensity), d->intensity *
			ipow50(MAX(0, vdot(d->n, d->h))));
		/* Sum diffuse and specular, scale by shadow visibility */
		temp_colour = vadd(d->ld, d->ls);
		temp_colour = vmult(temp_colour, 1.0 - shadow);
		/* Modulate by the light's own color (allows colored lights) */
		temp_colour = (t_vector){
			temp_colour.x * d->light->colour.r,
			temp_colour.y * d->light->colour.g,
			temp_colour.z * d->light->colour.b};
		/* Accumulate this light's contribution into the running total */
		d->colour = vadd(d->colour, temp_colour);
	}
}

/*
** prim_diffuse — Shade a hit point on a standalone primitive (sphere, plane,
** cylinder, cone, disk, hemisphere). Iterates over all lights, accumulating
** their contributions, then clamps each channel to [0, 1].
*/

t_colour		prim_diffuse(t_env *e)
{
	t_diffuse	d;
	size_t		i;

	d.mat = e->material[e->p_hit->material];
	/* Compute the world-space hit point: ray_origin + t * ray_direction */
	d.p = vadd(e->ray.loc, vmult(e->ray.dir, e->t));
	d.n = get_normal(e, d.p);
	d.colour = (t_vector){0.0, 0.0, 0.0};
	d.intensity = 1.0;
	i = e->lights;
	while (i--)
	{
		d.light = e->light[i];
		diffuse_colour(e, &d);
	}
	/* Clamp RGB to [0, 1] — multiple lights can push values above 1.0 */
	d.colour.x = (d.colour.x > 1.0) ? 1.0 : d.colour.x;
	d.colour.y = (d.colour.y > 1.0) ? 1.0 : d.colour.y;
	d.colour.z = (d.colour.z > 1.0) ? 1.0 : d.colour.z;
	return ((t_colour){d.colour.x, d.colour.y, d.colour.z, d.intensity});
}

/*
** face_diffuse — Shade a hit point on a mesh triangle face (from OBJ model).
** Same logic as prim_diffuse, but retrieves the material from the parent
** t_object and uses the pre-computed face normal instead of calling
** get_normal().
*/

t_colour		face_diffuse(t_env *e)
{
	t_diffuse	d;
	size_t		i;

	d.mat = e->material[e->object_hit->material];
	d.p = vadd(e->ray.loc, vmult(e->ray.dir, e->t));
	/* Mesh faces store a pre-computed normal; no need for get_normal() */
	d.n = *e->o_hit->n;
	d.colour = (t_vector){0.0, 0.0, 0.0};
	d.intensity = 1.0;
	i = e->lights;
	while (i--)
	{
		d.light = e->light[i];
		diffuse_colour(e, &d);
	}
	d.colour.x = (d.colour.x > 1.0) ? 1.0 : d.colour.x;
	d.colour.y = (d.colour.y > 1.0) ? 1.0 : d.colour.y;
	d.colour.z = (d.colour.z > 1.0) ? 1.0 : d.colour.z;
	return ((t_colour){d.colour.x, d.colour.y, d.colour.z, d.intensity});
}
