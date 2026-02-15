/*
** defines.h -- Constants, macros, and ID definitions for the raytracer.
**
** Organized into sections:
**   1. Utility macros (MIN/MAX)
**   2. Floating-point tolerance (EPSILON)
**   3. Camera field-of-view constant (ARBITRARY_NUMBER)
**   4. Primitive type IDs (for dispatching intersection routines)
**   5. Hit type IDs (primitive vs. mesh face)
**   6. Error codes (system errors < 16, format errors >= 16, usage = 32)
**   7. Key/mode flag bitmasks (each flag occupies one unique bit position)
*/

#ifndef DEFINES_H
# define DEFINES_H

/* Standard min/max macros. */
# define MAX(a, b)			((a > b) ? a : b)
# define MIN(a, b)			((a < b) ? a : b)

/*
** EPSILON: floating-point comparison tolerance.
** In ray tracing, when a ray hits a surface and spawns a secondary ray
** (shadow, reflection, refraction), the new ray origin must be offset
** slightly off the surface. Without EPSILON, floating-point imprecision
** can cause the secondary ray to immediately re-intersect the same
** surface (shadow acne, reflection artifacts). All intersection tests
** reject hits closer than EPSILON.
*/
# define EPSILON			0.000001

/*
** ARBITRARY_NUMBER: distance from camera to the virtual image plane.
** This empirically chosen value (~2.175) produces a field of view
** roughly equivalent to a 35mm camera lens. Increasing it narrows the
** FOV (zoom in); decreasing it widens the FOV (wide-angle).
*/
# define ARBITRARY_NUMBER	2.175

/*
** Primitive type IDs.
** Each geometric primitive type has a unique integer ID used to dispatch
** to the correct intersection routine (intersect_sphere, intersect_plane,
** etc.) via a switch/if-else chain in the intersection code.
*/
# define PRIM_SPHERE		1
# define PRIM_PLANE			2
# define PRIM_CONE			3
# define PRIM_CYLINDER		4
# define PRIM_TRIANGLE		5
# define PRIM_DISK			6
# define PRIM_HEMI_SPHERE	7

/*
** Hit type IDs.
** After an intersection is found, these indicate whether the closest hit
** was a geometric primitive (PRIMITIVE) or a triangle face from an OBJ
** mesh object (FACE). This determines which shading path to follow.
*/
# define PRIMITIVE			1
# define FACE				2

/*
** Error codes.
** Codes 0-15: system errors (passed to perror() which appends errno info).
** Codes 16-31: scene file format errors (printed with puts()).
** Code 32: command-line usage error (static string, not heap-allocated).
*/
# define FILE_OPEN_ERROR	0
# define MALLOC_ERROR		1
# define FREE_ERROR			2
# define FILE_FORMAT_ERROR	16
# define USAGE_ERROR		32

/*
** Key and mode flag bitmasks.
** Stored in e->flags as a bitmask -- multiple flags can be active at once.
** Each flag is a unique power of 2 (1 << N) so they occupy distinct bit
** positions and can be combined with bitwise OR and tested with bitwise AND.
**
** Example: e->flags |= KEY_G;          // enable grab mode
**          if (e->flags & KEY_G) ...    // test if grab mode is active
**          e->flags &= ~KEY_G;         // disable grab mode
*/
# define KEY_SHIFT			(1 << 0)
# define KEY_G				(1 << 1)
# define KEY_R				(1 << 2)
# define KEY_X				(1 << 3)
# define KEY_Y				(1 << 4)
# define KEY_Z				(1 << 5)
# define KEY_W				(1 << 6)
# define KEY_S				(1 << 7)
# define KEY_A				(1 << 8)
# define KEY_D				(1 << 9)
# define KEY_CTRL			(1 << 10)
# define KEY_SPACE			(1 << 11)
# define KEY_MID_CLICK		(1 << 12)

/*
** RAY_INSIDE: flag indicating the current ray is traveling inside a
** refractive object (e.g., inside a glass sphere). Used to correctly
** compute refraction direction and determine IOR transitions.
*/
# define RAY_INSIDE			(1 << 13)

#endif
