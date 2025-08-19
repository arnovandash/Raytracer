/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera_values.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adippena <angusdippenaar@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/07/09 12:24:39 by adippena          #+#    #+#             */
/*   Updated: 2016/09/03 15:28:41 by adippena         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

static void	set_camera_values(t_env *e, char *pt1, char *pt2)
{
	t_split_string	values;

	values = nstrsplit(pt2, ' ');
	if (!strcmp(pt1, "LOC"))
		e->camera.loc = get_vector(e, values);
	else if (!strcmp(pt1, "DIR"))
		e->camera.dir = get_vector(e, values);
	else if (!strcmp(pt1, "UP"))
		e->camera.up = get_vector(e, values);
	else if (!strcmp(pt1, "APERTURE"))
		e->camera.a = atof(pt2);
	free_split(&values);
}

void		get_camera_attributes(t_env *e, FILE *stream)
{
	t_split_string	attr;
	char			*line = NULL;
	size_t			len = 0;

	attr.words = 0;
	while (getline(&line, &len, stream) != -1)
	{
		if (line[0] == '\0' || line[0] == '\n')
			break ;
		attr = nstrsplit(line, '\t');
		if (attr.words < 2)
			err(FILE_FORMAT_ERROR, "Camera attributes", e);
		set_camera_values(e, &attr.strings[0][0], &attr.strings[1][0]);
		free_split(&attr);
	}
	free(line);
}
