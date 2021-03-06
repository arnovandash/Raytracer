/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   intersect_box.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adippena <angusdippenaar@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/08/11 20:12:04 by adippena          #+#    #+#             */
/*   Updated: 2016/08/26 19:52:16 by adippena         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"
#include "intersect_object.h"

int				intersect_box(t_ray *r, t_vector box[2])
{
	t_intersect_box		b;

	b.inv_dir = (t_vector){1.0 / r->dir.x, 1.0 / r->dir.y, 1.0 / r->dir.z};
	b.sign[0] = (b.inv_dir.x < 0.0);
	b.sign[1] = (b.inv_dir.y < 0.0);
	b.sign[2] = (b.inv_dir.z < 0.0);
	b.min.x = (box[b.sign[0]].x - r->loc.x) * b.inv_dir.x;
	b.max.x = (box[1 - b.sign[0]].x - r->loc.x) * b.inv_dir.x;
	b.min.y = (box[b.sign[1]].y - r->loc.y) * b.inv_dir.y;
	b.max.y = (box[1 - b.sign[1]].y - r->loc.y) * b.inv_dir.y;
	if (b.min.x > b.max.y || b.min.y > b.max.x)
		return (0);
	if (b.min.y > b.min.x)
		b.min.x = b.min.y;
	if (b.max.y < b.min.x)
		b.max.x = b.max.y;
	b.min.z = (box[b.sign[2]].z - r->loc.z) * b.inv_dir.z;
	b.max.z = (box[1 - b.sign[2]].z - r->loc.z) * b.inv_dir.z;
	if (b.min.x > b.max.x || b.min.x > b.max.x)
		return (0);
	return (1);
}
