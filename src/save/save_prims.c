/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   save_prims.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rojones <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/08/15 15:05:07 by rojones           #+#    #+#             */
/*   Updated: 2016/09/03 16:56:04 by adippena         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

static void	put_type(int type, int fd)
{
	char *temp;

	temp = NULL;
	if (type == PRIM_SPHERE)
		temp = "sphere";
	else if (type == PRIM_PLANE)
		temp = "plane";
	else if (type == PRIM_HEMI_SPHERE)
		temp = "hemi_sphere";
	else if (type == PRIM_CONE)
		temp = "cone";
	else if (type == PRIM_CYLINDER)
		temp = "cylinder";
	else if (type == PRIM_DISK)
		temp = "disk";
	else if (type == PRIM_TRIANGLE)
		temp = "triangle";
	dprintf(fd, "%s\n", temp);
}

static void	save_prim2(const t_prim *prim, t_material **mat, const int fd)
{
	if (prim->type == PRIM_SPHERE || prim->type == PRIM_CYLINDER ||
			prim->type == PRIM_HEMI_SPHERE)
		dprintf(fd, "\t\tRADIUS\t\t%f\n", prim->radius);
	if (prim->type == PRIM_CONE)
		dprintf(fd, "\t\tANGLE\t\t%f\n", (180 / M_PI) * prim->angle);
	dprintf(fd, "\t\tMATERIAL\t%s\n", mat[prim->material]->name);
}

static void	save_prim(const t_prim *prim, t_material **mat, const int fd)
{
	dprintf(fd, "\n\tPRIMITIVE\n");
	dprintf(fd, "\t\tTYPE\t\t");
	put_type(prim->type, fd);
	dprintf(fd, "\t\tLOC\t\t\t");
	write_coord(prim->loc, fd);
	if (prim->type == PRIM_HEMI_SPHERE)
	{
		dprintf(fd, "\t\tDIR\t\t\t");
		write_coord(prim->dir, fd);
	}
	if (prim->type == PRIM_CONE || prim->type == PRIM_CYLINDER)
	{
		dprintf(fd, "\t\tDIR\t\t\t");
		write_coord(prim->dir, fd);
		dprintf(fd, "\t\tLIMIT\t\t%f\n", prim->limit);
	}
	if (prim->type == PRIM_PLANE || prim->type == PRIM_DISK)
	{
		dprintf(fd, "\t\tNORMAL\t\t");
		write_coord(prim->normal, fd);
	}
	save_prim2(prim, mat, fd);
}

void		save_prims(t_prim **prim, t_material **mat, size_t prims, int fd)
{
	size_t	i;

	i = 0;
	while (i < prims)
	{
		save_prim(prim[i], mat, fd);
		i++;
	}
}
