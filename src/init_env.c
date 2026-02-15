/*
** init_env.c -- Two-phase environment initialization.
**
** Phase 1 (nullify_pointers): Zeroes all pointer fields in t_env to prevent
** dangling references if an error occurs before they are assigned. Sets
** sensible defaults: 1600x900 resolution, maxdepth=1 (no recursion by
** default), camera at (0, -10, 0) looking at the origin with Z-up.
**
** Phase 2 (init_env): Parses the scene file, then creates the SDL window
** and two rendering surfaces:
**   - img: the main render target (pixels written by worker threads)
**   - dof: accumulation buffer for depth-of-field multi-sampling
** Both surfaces use 32-bit pixels; their pixel data is cast to uint32_t*
** for direct 0xAARRGGBB access without SDL pixel-format conversion.
*/

#include "rt.h"

/*
** Set default camera: positioned 10 units back along -Y axis, looking at
** the origin. The 'up' vector is +Z (convention used throughout this
** engine). Aperture (a) of 0.1 controls depth-of-field blur radius.
*/
static void		init_camera(t_env *e)
{
	e->camera.loc = (t_vector){0.0, -10.0, 0.0};
	e->camera.dir = (t_vector){0.0, 0.0, 0.0};
	e->camera.up = (t_vector){0.0, 0.0, 1.0};
	e->camera.a = 0.1;
}

/* Reset all performance counters to zero before a render pass. */
static void		init_stats(void)
{
	g_stats.rays = 0;
	g_stats.primary_rays = 0;
	g_stats.reflection_rays = 0;
	g_stats.refraction_rays = 0;
	g_stats.shadow_rays = 0;
	g_stats.intersection_tests = 0;
	g_stats.threads = 0;
}

/*
** Set numeric defaults. Notable values:
** - t = INFINITY: no intersection found yet (any real hit will be closer)
** - maxdepth = 1: only primary rays by default (scene file can override)
** - super = 0: no depth-of-field supersampling by default
*/
static void		initials(t_env *e)
{
	init_stats();
	e->s_num = 0;
	e->prims = 0;
	e->objects = 0;
	e->lights = 0;
	e->materials = 0;
	e->hit_type = 0;
	e->prims = 0;
	e->objects = 0;
	e->lights = 0;
	e->materials = 0;
	e->t = INFINITY;
	e->maxdepth = 1;
	e->x = 1600;
	e->y = 900;
	e->flags = 0;
	e->super = 0;
}

/* NULL all pointers so cleanup functions can safely check before freeing. */
static void		nulls(t_env *e)
{
	e->win = NULL;
	e->img = NULL;
	e->px = NULL;
	e->p_hit = NULL;
	e->prim = NULL;
	e->o_hit = NULL;
	e->object_hit = NULL;
	e->object = NULL;
	e->light = NULL;
	e->material = NULL;
	e->p_hit = NULL;
}

/* Phase 1: safe defaults + NULL pointers + default camera. */
void			nullify_pointers(t_env *e)
{
	initials(e);
	nulls(e);
	init_camera(e);
}

/*
** Phase 2: full initialization.
** After parsing the scene file, create the SDL window and two 32-bit
** surfaces. memset clears pixel buffers to black (0x00000000).
*/
void			init_env(t_env *e)
{
	nullify_pointers(e);
	read_scene(e->file_name, e);
	e->win = SDL_CreateWindow(e->file_name, SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED, e->x, e->y, SDL_WINDOW_SHOWN);
	e->win_img = SDL_GetWindowSurface(e->win);
	e->img = SDL_CreateRGBSurface(0, e->x, e->y, 32, 0, 0, 0, 0);
	e->dof = SDL_CreateRGBSurface(0, e->x, e->y, 32, 0, 0, 0, 0);
	/* Cast pixel data to uint32_t* for direct 32-bit ARGB access. */
	e->px = (uint32_t *)e->img->pixels;
	e->dx = (uint32_t *)e->dof->pixels;
	memset(e->px, 0, (e->x * 4) * e->y);
	memset(e->dx, 0, (e->x * 4) * e->y);
	SDL_UpdateWindowSurface(e->win);
}
