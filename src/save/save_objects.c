/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   save_objects.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rojones <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/08/15 15:50:56 by rojones           #+#    #+#             */
/*   Updated: 2016/09/04 12:03:08 by adippena         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

static void	save_object(t_object *obj, t_material **mat, int fd)
{
	dprintf(fd, "\n\tOBJECT\n");
	dprintf(fd, "\t\tFILE\t\t%s\n", obj->name);
	dprintf(fd, "\t\tMATERIAL\t%s\n", mat[obj->material]->name);
}

void		save_objects(t_object **o, size_t objects, t_material **m, int fd)
{
	size_t	i;

	i = 0;
	while (i < objects)
		save_object(o[i++], m, fd);
}
