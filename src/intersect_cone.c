/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   intersect_cone.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adippena <angusdippenaar@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/07/16 09:02:46 by adippena          #+#    #+#             */
/*   Updated: 2016/07/16 15:46:03 by adippena         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "intersect_cone.h"

static int	find_t(double a, double b, double discr, double *t)
{
	double	sqrt_discr;
	double	t0;
	double	t1;

	sqrt_discr = sqrt(discr);
	t0 = (-b + sqrt_discr) / (2.0 * a);
	t1 = (-b - sqrt_discr) / (2.0 * a);
	if (t0 > t1)
		t0 = t1;
	if (t0 > 0.000001 && t0 < *t)
	{
		*t = t0;
		return (1);
	}
	return (0);
}

int			intersect_cone(t_ray *r, t_object *o, double *t)
{
	t_intersect_cone	c;

	c.dist = vector_sub(r->loc, o->loc);
	c.v_va = vector_dot(r->dir, o->dir);
	c.dp_va = vector_dot(c.dist, o->dir);
	c.a_v = vector_sub(r->dir, vector_mult(o->dir, c.v_va));
	c.c_v = vector_sub(c.dist, vector_mult(o->dir, c.dp_va));
	c.cos2 = cos(o->angle) * cos(o->angle);
	c.sin2 = sin(o->angle) * sin(o->angle);
	c.a = c.cos2 * vector_dot(c.a_v, c.a_v) -
		c.sin2 * c.v_va * c.v_va;
	c.b = 2.0 * c.cos2 * vector_dot(c.a_v, c.c_v) -
		2.0 * c.sin2 * c.v_va * c.dp_va;
	c.c = c.cos2 * vector_dot(c.c_v, c.c_v) - c.sin2 * c.dp_va * c.dp_va;
	c.d = c.b * c.b - 4.0 * c.a * c.c;
	if (c.d < 0.000001)
		return (0);
	return (find_t(c.a, c.b, c.d, t));
}
