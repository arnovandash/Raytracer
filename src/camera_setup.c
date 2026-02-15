/*
** camera_setup.c -- Constructs the camera's view plane and generates rays.
**
** A virtual camera in a raytracer needs to map 2D pixel coordinates to 3D
** ray directions. This is done by constructing an "image plane" -- a
** rectangle in 3D world space positioned in front of the camera. Each pixel
** corresponds to a point on this plane, and the primary ray goes from the
** camera position through that point.
**
** The image plane is defined by an orthonormal basis (u, v, n):
**   n = backward direction (from look-at toward camera position)
**   u = right direction   (perpendicular to up and n)
**   v = true up direction (perpendicular to n and u)
**
** The plane dimensions simulate a 35mm film camera: the half-height is
** derived from 18mm (half of 36mm film height) scaled by ARBITRARY_NUMBER
** (the focal distance), and the width follows from the aspect ratio.
*/

#include "draw.h"

/*
** setup_camera_plane -- Build the camera's orthonormal basis and image plane.
**
** Constructs the camera coordinate system and computes the lower-left corner
** of the image plane plus the per-pixel step sizes.
**
** Math:
**   h = half-height of image plane = 18mm * focal_dist / 35mm
**       (simulates a 35mm film camera; 18mm is half the 36mm film height)
**   w = half-width = h * aspect_ratio
**
**   n = normalize(camera_pos - look_at)    -- view direction (reversed)
**   u = normalize(up x n)                  -- camera right axis
**   v = normalize(n x u)                   -- camera true up axis
**
**   center = camera_pos - n * focal_dist   -- center of image plane
**   camera.l = center - u*w/2 + v*h/2      -- lower-left corner
**       (+ v*h/2 because v points up but screen y increases downward)
**
**   stepx = w / image_width_pixels         -- world-space width per pixel
**   stepy = h / image_height_pixels        -- world-space height per pixel
*/
void		setup_camera_plane(t_env *e)
{
	t_vector	n;
	t_vector	c;
	double		w;
	double		h;

	/* Image plane half-height from 35mm film equivalent */
	h = 18.0 * ARBITRARY_NUMBER / 35.0;
	/* Half-width from aspect ratio */
	w = h * (double)e->x / (double)e->y;
	/* n: unit vector from look-at to camera (reverse view direction) */
	n = vunit(vsub(e->camera.loc, e->camera.dir));
	/* u: right vector via cross product of world-up and n */
	e->camera.u = vunit(vcross(e->camera.up, n));
	/* v: true up vector, orthogonal to both n and u */
	e->camera.v = vunit(vcross(n, e->camera.u));
	/* Center of image plane: camera position shifted forward by focal dist */
	c = vsub(e->camera.loc, vmult(n, ARBITRARY_NUMBER));
	/*
	** Lower-left corner of image plane:
	** Start at center, go left by half-width, up by half-height.
	** Note: +v goes up in world space, but pixel y=0 is the top row,
	** so get_ray_dir subtracts v*y*stepy to move downward per scanline.
	*/
	e->camera.l = vadd(vsub(c,
		vmult(e->camera.u, w / 2.0)),
		vmult(e->camera.v, h / 2.0));
	/* World-space size of one pixel */
	e->camera.stepx = w / (double)e->x;
	e->camera.stepy = h / (double)e->y;
}

/*
** get_ray_dir -- Compute the primary ray for pixel (x, y).
**
** Maps pixel coordinates to a point on the image plane, then creates a
** unit direction vector from the camera position through that point.
**
** The image plane point for pixel (x, y) is:
**   P = camera.l + u * (x * stepx) - v * (y * stepy)
**
** Starting from the lower-left corner (camera.l):
**   - Move right by x pixels worth of u
**   - Move down by y pixels worth of v (subtract because v points up)
**
** The ray direction is then: normalize(P - camera_pos)
** The ray origin is the camera position itself.
**
** IOR is set to 1.0 (air) and o_in to NULL because the primary ray
** starts outside all objects.
*/
void		get_ray_dir(t_env *e, double x, double y)
{
	e->ray.dir = vunit(vsub(vsub(
		vadd(e->camera.l, vmult(e->camera.u, x * e->camera.stepx)),
		vmult(e->camera.v, y * e->camera.stepy)), e->camera.loc));
	e->ray.loc = e->camera.loc;
	e->ray.o_in = NULL;
	e->ray.ior = 1;
}
