/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   save.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rojones <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/08/15 11:59:15 by rojones           #+#    #+#             */
/*   Updated: 2016/09/03 16:50:56 by adippena         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

static void	save_camra(t_camera *cam, int fd)
{
	dprintf(fd, "\tCAMERA\n");
	dprintf(fd, "\t\tLOC\t\t\t");
	write_coord(cam->loc, fd);
	dprintf(fd, "\t\tDIR\t\t\t");
	write_coord(cam->dir, fd);
	dprintf(fd, "\t\tUP\t\t\t");
	write_coord(cam->up, fd);
	dprintf(fd, "\t\tAPERTURE\t%lf\n", cam->a);
}

static void	save_render(t_env *e, int fd)
{
	dprintf(fd, "\tRENDER\t\t%zu %zu\n", e->x, e->y);
	dprintf(fd, "\tSUPER\t\t%zu\n", e->super);
}

void		save(t_env *e)
{
	int		fd;

	printf("Saving file... ");
	if ((fd = open(e->file_name, O_WRONLY | O_TRUNC)) == -1)
		err(FILE_OPEN_ERROR, "Could not save the file", e);
	dprintf(fd, "# SCENE RT\n");
	dprintf(fd, " \tMAXDEPTH\t%d\n", e->maxdepth);
	save_render(e, fd);
	dprintf(fd, "\n");
	save_camra(&e->camera, fd);
	save_lights(e->light, e->lights, fd);
	save_materials(e->material, e->materials, fd);
	save_prims(e->prim, e->material, e->prims, fd);
	save_objects(e->object, e->objects, e->material, fd);
	close(fd);
	printf("Done\n");
}
