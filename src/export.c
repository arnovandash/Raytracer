/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adippena <angusdippenaar@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/08/31 20:35:15 by adippena          #+#    #+#             */
/*   Updated: 2016/09/04 12:12:13 by adippena         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

char	*itoa(int n)
{
	char	*str;
	int		len;

	len = snprintf(NULL, 0, "%d", n);
	str = malloc(len + 1);
	snprintf(str, len + 1, "%d", n);
	return (str);
}

static void		write_image(t_env *e, int fd)
{
	size_t		total;
	size_t		index;
	uint32_t	px;
	uint32_t	px_rgb;

	total = e->x * e->y;
	index = 0;
	while (index != total)
	{
		px = e->px[index++];
		px_rgb = ((px & 0xFF) << 16) | (px & 0xFF00) | ((px & 0xFF0000) >> 16);
		write(fd, &px_rgb, 3);
	}
}

void			export(t_env *e)
{
	int		fd;
	char	*temp;

	fputs("Exporting rendered image... ", stdout);
	temp = NULL;
	asprintf(&temp, "%s_%ld.ppm", e->file_name, time(NULL));
	if ((fd = open(temp, O_WRONLY | O_TRUNC | O_CREAT, 0666)) == -1)
		err(FILE_OPEN_ERROR, "Could not export rendered image", e);
	strdel(&temp);
	fprintf(fdopen(fd, "w"), "%s\n", "P6\n# Exported by the best RT project ever!");
	temp = itoa(e->x);
	fputs(temp, fdopen(fd, "w"));
	fputs(" ", fdopen(fd, "w"));
	strdel(&temp);
	temp = itoa(e->y);
	fprintf(fdopen(fd, "w"), "%s\n", temp);
	fprintf(fdopen(fd, "w"), "%s\n", "255");
	write_image(e, fd);
	close(fd);
	fputs("Done\n", stdout);
}
