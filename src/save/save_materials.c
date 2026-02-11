/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   save_materials.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rojones <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/08/15 14:39:54 by rojones           #+#    #+#             */
/*   Updated: 2016/09/03 16:55:20 by adippena         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

static void	write_colour(t_colour *col, int fd)
{
	dprintf(fd, "%02X%02X%02X %f\n",
		(int)(col->r * 255.0),
		(int)(col->g * 255.0),
		(int)(col->b * 255.0),
		col->intensity);
}

static void	save_mat(t_material *mat, int fd)
{
	dprintf(fd, "\n\tMATERIAL\n");
	dprintf(fd, "\t\tNAME\t\t%s\n", mat->name);
	dprintf(fd, "\t\tDIFFUSE\t\t");
	write_colour(&mat->diff, fd);
	dprintf(fd, "\t\tSPECULAR\t");
	write_colour(&mat->spec, fd);
	dprintf(fd, "\t\tREFLECT\t\t%f\n", mat->reflect);
	dprintf(fd, "\t\tREFRACT\t\t%f\n", mat->refract);
	dprintf(fd, "\t\tIOR\t\t\t%f\n", mat->ior);
}

void		save_materials(t_material **material, size_t materials, int fd)
{
	size_t	i;

	i = 1;
	while (i < materials)
	{
		save_mat(material[i], fd);
		i++;
	}
}
