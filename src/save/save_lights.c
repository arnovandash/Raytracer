/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   save_lights.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rojones <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/08/15 13:52:36 by rojones           #+#    #+#             */
/*   Updated: 2016/09/03 16:55:06 by adippena         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

static void	write_colour(t_colour *col, int fd)
{
	dprintf(fd, "%02X%02X%02X\n",
		(int)(col->r * 255.0),
		(int)(col->g * 255.0),
		(int)(col->b * 255.0));
}

static void	save_light(t_light *light, int fd)
{
	dprintf(fd, "\n\tLIGHT\n");
	dprintf(fd, "\t\tLOC\t\t\t");
	write_coord(light->loc, fd);
	dprintf(fd, "\t\tCOLOUR\t\t");
	write_colour(&light->colour, fd);
	dprintf(fd, "\t\tINTENSITY\t%f\n", light->lm / 3.415);
	dprintf(fd, "\t\tHALF\t\t%f\n", light->half);
}

void		save_lights(t_light **lights, size_t num_light, int fd)
{
	size_t	i;

	i = 0;
	while (i < num_light)
	{
		save_light(lights[i], fd);
		i++;
	}
}
