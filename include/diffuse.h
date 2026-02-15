/*
** diffuse.h — Blinn-Phong shading intermediate state
**
** The t_diffuse struct holds all intermediate values needed during
** Blinn-Phong shading of a single surface point. It is populated
** incrementally as each light source is evaluated, accumulating the
** total illumination in the `colour` field.
**
** The key vectors (all originating from the surface hit point):
**
**        N (normal)
**        |
**        |   H (halfway)
**        |  /
**        | /
**   -----P----------
**       /|\
**      / | \
**     V  |  L
**  (eye) | (light)
**
**   N — surface normal at hit point
**   L — unit direction toward the light source
**   V — unit direction toward the camera/viewer
**   H — halfway vector = normalize(V + L), used for specular highlights
*/

#ifndef DIFFUSE_H
# define DIFFUSE_H

# include "rt.h"

typedef struct	s_diffuse
{
	t_material	*mat;		/* Material of the surface being shaded           */
	t_light		*light;		/* Current light source being evaluated            */
	t_vector	colour;		/* Accumulated RGB (uses x,y,z as r,g,b channels) */
	t_vector	n;			/* Surface normal at hit point                     */
	t_vector	p;			/* World-space position of the hit point           */
	t_vector	l;			/* Unit direction from hit point to light           */
	t_vector	h;			/* Halfway vector: normalize(v + l)                */
	t_vector	v;			/* Unit direction from hit point to camera          */
	t_vector	ld;			/* Diffuse light contribution for current light    */
	t_vector	ls;			/* Specular light contribution for current light   */
	double		intensity;	/* Computed light attenuation at this point         */
	double		dist;		/* Distance from hit point to current light         */
}				t_diffuse;

#endif
