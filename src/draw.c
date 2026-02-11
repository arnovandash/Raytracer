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

static void		*draw_chunk(void *q)
{
	t_chunk		*c;
	uint32_t	*px;

	c = (t_chunk *)q;
	c->stopx = MIN(c->d.x + c->d.w, (int)c->e->x);
	c->stopy = MIN(c->d.y + c->d.h, (int)c->e->y);
	while (c->d.y < c->stopy)
	{
		c->x = c->d.x;
		px = &c->px[c->d.y * c->e->x + c->d.x];
		while (c->x < c->stopx)
		{
			++g_tls_stats.rays;
			++g_tls_stats.primary_rays;
			c->e->p_hit = NULL;
			get_ray_dir(c->e, (double)c->x, (double)c->d.y);
			intersect_scene(c->e);
			*px++ = (c->e->p_hit && !c->e->p_hit->s_bool &&
				!(c->e->flags & KEY_G)) ?
				find_colour(c->e) : find_base_colour(c->e);
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
	t_render	r;

	setup_camera_plane(e);
	make_chunks(e, &d, e->img);
	if (!(e->s_num) && e->super > 1)
	{
		r.cam_origin = e->camera.loc;
		r.i = 0;
		r.angle = 0;
		r.angle_step = 2 * M_PI / (e->super - 1);
		SDL_BlitSurface(e->img, NULL, e->dof, NULL);
		while (r.i++ != e->super)
		{
			printf("Rendering frame %zu of %zu\n", r.i, e->super);
			r.angle += r.angle_step;
			e->camera.loc = vadd(r.cam_origin, (t_vector)
				{cos(r.angle) * e->camera.a, 0.0, sin(r.angle) * e->camera.a});
			setup_camera_plane(e);
			make_chunks(e, &d, e->dof);
			blend(e->img, e->dof);
		}
		e->camera.loc = r.cam_origin;
		SDL_BlitSurface(e->img, NULL, e->win_img, NULL);
		SDL_UpdateWindowSurface(e->win);
	}
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
