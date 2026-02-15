/*
** in_shadow.h — Shadow ray testing state
**
** The t_in_shadow struct holds the state for a single shadow ray test.
** A shadow ray is cast from a surface hit point toward a light source to
** determine if any geometry blocks the light. The struct tracks the ray
** itself, the distance to the light (to ignore intersections beyond it),
** and iteration counters for looping through scene primitives and mesh
** objects.
*/

#ifndef IN_SHADOW_H
# define IN_SHADOW_H

typedef struct	s_in_shadow
{
	t_ray		ray;		/* Shadow ray: origin at hit point, dir toward light */
	double		t;			/* Nearest intersection distance found so far        */
	double		distance;	/* Distance from hit point to light source            */
	size_t		prim;		/* Loop counter / index for standalone primitives     */
	size_t		face;		/* Loop counter / index for mesh triangle faces       */
	size_t		object;		/* Loop counter / index for mesh objects               */
	t_object	*o;			/* Pointer to the current mesh object being tested    */
}				t_in_shadow;

#endif
