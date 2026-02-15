/*
** draw.c -- Rendering orchestrator: multithreaded tile-based ray tracing.
**
** This file is the heart of the rendering pipeline. It divides the image
** into 64x64 pixel tiles ("chunks"), spawns one pthread per tile, and
** each thread independently traces rays through its assigned pixels.
**
** Key concepts implemented here:
**
** 1. CHUNK-BASED MULTITHREADING
**    Each tile gets a full copy of the environment (copy_env), so threads
**    never contend on shared mutable state. The only synchronization is
**    at the end when each thread atomically adds its local statistics to
**    the global counters.
**
** 2. XORshift32 PRNG
**    A minimal, fast pseudorandom number generator used for jittering
**    sample positions during supersampling. The seed is deterministic
**    based on tile position, so renders are reproducible.
**
** 3. STOCHASTIC SUPERSAMPLING (ANTI-ALIASING)
**    When e->super > 1, multiple rays are cast per pixel with random
**    sub-pixel offsets in [0, 1). The resulting colors are averaged.
**    This smooths out jagged edges and produces softer shadows/reflections.
**
** 4. GRAB MODE (KEY_G)
**    A fast interactive preview mode that uses flat shading (find_base_colour)
**    instead of full lighting/reflection. Useful for positioning the camera.
**
** 5. THREAD-LOCAL STATISTICS
**    Each thread accumulates ray counts in g_tls_stats (_Thread_local),
**    then atomically merges them into g_stats at thread exit. This avoids
**    per-ray atomic operations that would destroy performance.
*/

#include "draw.h"
#include <stdio.h>

/*
** xorshift32 -- Fast 32-bit pseudorandom number generator.
**
** The XORshift family of PRNGs (Marsaglia, 2003) produces decent
** randomness with only three XOR-shift operations and no multiplies.
** Period is 2^32 - 1. The state must never be zero.
**
** Used here to generate sub-pixel jitter offsets for supersampling.
** We only need "good enough" randomness -- not cryptographic quality.
*/
static uint32_t	xorshift32(uint32_t *state)
{
	uint32_t	x;

	x = *state;
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 5;
	*state = x;
	return (x);
}

/*
** trace_pixel -- Cast a single primary ray through pixel (x, y) and shade it.
**
** Steps:
**   1. Compute the ray direction from camera through pixel (x, y) on the
**      image plane (get_ray_dir).
**   2. Find the nearest intersection with any object (intersect_scene).
**   3. If an object was hit:
**      - Normal mode: compute full shading with find_colour (diffuse,
**        specular, reflections, refractions).
**      - Grab mode (KEY_G): use find_base_colour for flat/unlit shading.
**      - If s_bool is set on the hit primitive, it is a "selection boolean"
**        object used only for grab mode, so it also falls through to
**        find_base_colour.
**   4. If nothing was hit, find_base_colour returns the background color.
**
** Returns: 0xRRGGBB packed color as uint32_t.
*/
static uint32_t	trace_pixel(t_chunk *c, double x, double y)
{
	++g_tls_stats.rays;
	++g_tls_stats.primary_rays;
	c->e->p_hit = NULL;
	get_ray_dir(c->e, x, y);
	intersect_scene(c->e);
	return ((c->e->p_hit && !c->e->p_hit->s_bool &&
		!(c->e->flags & KEY_G)) ?
		find_colour(c->e) : find_base_colour(c->e));
}

/*
** supersample -- Cast multiple jittered rays per pixel and average them.
**
** For each of the e->super samples:
**   1. Generate a random sub-pixel offset in [0, 1) for both x and y
**      using xorshift32. The mask 0xFFFF gives 16 bits of randomness,
**      divided by 65536.0 to normalize to [0, 1).
**   2. Trace a ray through (px + jitter_x, py + jitter_y).
**   3. Accumulate the R, G, B channels separately (extracted from the
**      packed 0xRRGGBB uint32_t).
**
** Finally, divide each channel by the sample count and re-pack into
** a single uint32_t color.
**
** This is stochastic (random) supersampling, as opposed to grid-based.
** Stochastic sampling trades structured aliasing artifacts for
** less objectionable noise, and converges well with few samples.
*/
static uint32_t	supersample(t_chunk *c, double px, double py, uint32_t *seed)
{
	double		r;
	double		g;
	double		b;
	size_t		i;
	uint32_t	col;
	double		inv;

	r = 0;
	g = 0;
	b = 0;
	i = c->e->super;
	while (i--)
	{
		col = trace_pixel(c,
				px + (double)(xorshift32(seed) & 0xFFFF) / 65536.0,
				py + (double)(xorshift32(seed) & 0xFFFF) / 65536.0);
		r += (col >> 16) & 0xFF;
		g += (col >> 8) & 0xFF;
		b += col & 0xFF;
	}
	inv = 1.0 / c->e->super;
	return (((uint32_t)(r * inv) << 16) |
		((uint32_t)(g * inv) << 8) |
		(uint32_t)(b * inv));
}

/*
** draw_chunk -- Thread entry point: renders all pixels in one 64x64 tile.
**
** Each thread receives a t_chunk* with its own copy of the environment
** and the bounding rectangle of its tile.
**
** The PRNG seed is derived deterministically from the tile's (x, y)
** position using two primes (7919, 104729), so the same tile always
** produces the same jitter pattern. This makes renders reproducible
** even though threads may execute in any order.
**
** After rendering all pixels, the thread atomically merges its local
** ray-count statistics into the global g_stats, then cleans up.
*/
static void		*draw_chunk(void *q)
{
	t_chunk		*c;
	uint32_t	*px;
	uint32_t	seed;

	c = (t_chunk *)q;
	/* Deterministic seed from tile position for reproducible jitter */
	seed = (uint32_t)(c->d.x * 7919 + c->d.y * 104729 + 1);
	/* Clamp tile edges to image bounds (handles partial tiles at edges) */
	c->stopx = MIN(c->d.x + c->d.w, (int)c->e->x);
	c->stopy = MIN(c->d.y + c->d.h, (int)c->e->y);
	while (c->d.y < c->stopy)
	{
		c->x = c->d.x;
		px = &c->px[c->d.y * c->e->x + c->d.x];
		while (c->x < c->stopx)
		{
			if (c->e->super > 1)
				*px++ = supersample(c, (double)c->x,
						(double)c->d.y, &seed);
			else
				*px++ = trace_pixel(c, (double)c->x,
						(double)c->d.y);
			++c->x;
		}
		++c->d.y;
	}
	/* Merge thread-local stats into global counters atomically */
	atomic_fetch_add(&g_stats.rays, g_tls_stats.rays);
	atomic_fetch_add(&g_stats.primary_rays, g_tls_stats.primary_rays);
	atomic_fetch_add(&g_stats.reflection_rays, g_tls_stats.reflection_rays);
	atomic_fetch_add(&g_stats.refraction_rays, g_tls_stats.refraction_rays);
	atomic_fetch_add(&g_stats.shadow_rays, g_tls_stats.shadow_rays);
	atomic_fetch_add(&g_stats.intersection_tests, g_tls_stats.intersection_tests);
	memset(&g_tls_stats, 0, sizeof(t_thread_stats));
	free(c->e);
	free(c);
	pthread_exit(0);
}

/*
** p_join -- Wait for all rendering threads to complete.
**
** As each thread finishes, blit the rendered surface to the window and
** update the display. This provides progressive rendering feedback:
** the user sees tiles appearing one by one as threads complete.
*/
static void		p_join(t_env *e, SDL_Surface *img, t_make_chunks *m)
{
	while (m->thread--)
	{
		pthread_join(m->tid[m->thread], NULL);
		SDL_BlitSurface(img, NULL, e->win_img, NULL);
		SDL_UpdateWindowSurface(e->win);
	}
}

/*
** make_chunks -- Divide the image into 64x64 tiles and spawn one thread each.
**
** The image (of size d->w x d->h) is partitioned into a grid of 64x64 tiles.
** Tiles at the right and bottom edges may be smaller than 64x64 (handled by
** clamping in draw_chunk).
**
** For each tile:
**   1. Allocate a t_chunk with the tile bounds.
**   2. Copy the entire environment (copy_env) so the thread has private state.
**   3. Point px at the SDL surface pixel buffer (threads write to disjoint
**      regions, so no locking is needed).
**   4. Spawn a pthread running draw_chunk.
**
** After all threads are spawned, p_join waits for them all to finish.
*/
static void		make_chunks(t_env *e, SDL_Rect *d, SDL_Surface *img)
{
	t_make_chunks	m;

	m.tids = ceil((double)d->w / 64.0) * ceil((double)d->h / 64.0);
	g_stats.threads = m.tids;
	m.tid = (pthread_t *)malloc(sizeof(pthread_t) * m.tids);
	m.thread = 0;
	m.chunk_y = 0;
	while (m.chunk_y * 64 < (size_t)d->h)
	{
		m.chunk_x = 0;
		while (m.chunk_x * 64 < (size_t)d->w)
		{
			m.c = (t_chunk *)malloc(sizeof(t_chunk));
			m.c->px = (uint32_t *)img->pixels;
			m.c->e = copy_env(e);
			m.c->d = (SDL_Rect){m.chunk_x * 64, m.chunk_y * 64, 64, 64};
			pthread_create(&m.tid[m.thread++], NULL, draw_chunk, (void *)m.c);
			++m.chunk_x;
		}
		++m.chunk_y;
	}
	p_join(e, img, &m);
	free(m.tid);
}

/*
** render -- Set up the camera and launch multithreaded rendering.
*/
static void		render(t_env *e, SDL_Rect d)
{
	setup_camera_plane(e);
	make_chunks(e, &d, e->img);
}

/*
** draw -- Top-level render call. Renders a full frame and prints statistics.
**
** In normal mode:
**   1. Dims the previous frame (half_bytes halves every byte in the surface,
**      creating a fade effect so the user sees something while waiting).
**   2. Renders the new frame with full lighting, reflections, and refractions.
**   3. Prints render time and per-category ray counts.
**
** In grab mode (KEY_G):
**   Renders with flat shading only (no dimming, no stats) for fast
**   interactive camera positioning.
*/
void			draw(t_env *e, SDL_Rect d)
{
	struct timeval	tv;
	struct timeval	tv2;
	size_t			sec;

	if (!(e->flags & KEY_G))
	{
		half_bytes(e->img);
		SDL_UpdateWindowSurface(e->win);
		gettimeofday(&tv, NULL);
		render(e, d);
		gettimeofday(&tv2, NULL);
		sec = tv2.tv_sec * 1000000 - tv.tv_sec * 1000000;
		sec += tv2.tv_usec - tv.tv_usec;
		printf("Frame drawn in %zu.%06zu seconds\n", sec / 1000000,
			sec % 1000000);
		printf("Rendered using %zu threads\n", atomic_load(&g_stats.threads));
		printf("Total rays: %zu\n", atomic_load(&g_stats.rays));
		printf("Primary rays: %zu\n", atomic_load(&g_stats.primary_rays));
		printf("Reflection rays: %zu\n", atomic_load(&g_stats.reflection_rays));
		printf("Refraction rays: %zu\n", atomic_load(&g_stats.refraction_rays));
		printf("Shadow rays: %zu\n", atomic_load(&g_stats.shadow_rays));
		printf("Intersection tests: %zu\n", atomic_load(&g_stats.intersection_tests));
	}
	else
		render(e, d);
}
