/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shadow.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adippena <angusdippenaar@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/07/16 23:47:45 by adippena          #+#    #+#             */
/*   Updated: 2016/08/25 13:01:26 by adippena         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"
#include "in_shadow.h"

static void	init(t_in_shadow *var, t_env *e, t_light *light)
{
	var->t = INFINITY;
	var->ray.loc = vadd(e->ray.loc, vmult(e->ray.dir, e->t));
	var->ray.dir = vsub(light->loc, var->ray.loc);
	var->distance = vnormalize(var->ray.dir);
	var->ray.dir = vdiv(var->ray.dir, var->distance);
	var->prim = e->prims;
	var->object = e->objects;
}

double		in_shadow(t_env *e, t_light *light)
{
	t_in_shadow	var;
	double		transmit;
	double		t_test;

	++g_tls_stats.rays;
	++g_tls_stats.shadow_rays;
	init(&var, e, light);
	transmit = 1.0;
	while (var.prim--)
	{
		t_test = var.distance;
		if (intersect_prim(e, &var.ray, var.prim, &t_test) && t_test <
				var.distance)
		{
			transmit *= e->material[e->prim[var.prim]->material]->refract;
			if (transmit < EPSILON)
				return (1.0);
		}
	}
	while (var.object--)
	{
		var.o = e->object[var.object];
		if (intersect_box(&var.ray, var.o->box))
		{
			var.face = var.o->faces;
			while (var.face--)
				if (intersect_triangle(&var.ray, var.o->face[var.face],
						&t_test) && t_test < var.distance)
				{
					transmit *= e->material[var.o->material]->refract;
					if (transmit < EPSILON)
						return (1.0);
					break ;
				}
		}
	}
	return (1.0 - transmit);
}
