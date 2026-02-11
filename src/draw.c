/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   draw.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adippena <angusdippenaar@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/07/10 14:00:07 by adippena          #+#    #+#             */
/*   Updated: 2016/09/04 17:47:57 by adippena         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "draw.h"
#include <stdio.h>

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

static void		*draw_chunk(void *q)
{
	t_chunk		*c;
	uint32_t	*px;
	uint32_t	seed;

	c = (t_chunk *)q;
	seed = (uint32_t)(c->d.x * 7919 + c->d.y * 104729 + 1);
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

static void		p_join(t_env *e, SDL_Surface *img, t_make_chunks *m)
{
	while (m->thread--)
	{
		pthread_join(m->tid[m->thread], NULL);
		SDL_BlitSurface(img, NULL, e->win_img, NULL);
		SDL_UpdateWindowSurface(e->win);
	}
}

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

static void		render(t_env *e, SDL_Rect d)
{
	setup_camera_plane(e);
	make_chunks(e, &d, e->img);
}

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
