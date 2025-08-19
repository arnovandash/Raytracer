/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   light_values.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adippena <angusdippenaar@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/07/09 12:30:08 by adippena          #+#    #+#             */
/*   Updated: 2016/09/04 15:05:38 by adippena         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

static void	set_light_values(t_env *e, char *pt1, char *pt2)
{
	t_split_string	values;

	values = nstrsplit(pt2, ' ');
	if (!strcmp(pt1, "LOC"))
		e->light[e->lights]->loc = get_vector(e, values);
	else if (!strcmp(pt1, "COLOUR"))
		e->light[e->lights]->colour = get_colour(e, values);
	else if (!strcmp(pt1, "INTENSITY"))
		e->light[e->lights]->lm = atof(values.strings[0]) * 3.415;
	else if (!strcmp(pt1, "HALF"))
		e->light[e->lights]->half = atof(values.strings[0]);
	free_split(&values);
}

static void	init_light(t_light *l)
{
	l->loc = (t_vector){0.0, 0.0, 5.0};
	l->colour = (t_colour){1.0, 1.0, 1.0, 1.0};
	l->lm = 3.415;
	l->half = 0.0;
}

void		get_light_attributes(t_env *e, FILE *stream)
{
	t_split_string	attr;
	char			*line = NULL;
	size_t			len = 0;

	attr.words = 0;
	e->light[e->lights] = (t_light *)malloc(sizeof(t_light));
	init_light(e->light[e->lights]);
	while (getline(&line, &len, stream) != -1)
	{
		if (line[0] == '\0' || line[0] == '\n')
			break ;
		attr = nstrsplit(line, '\t');
		if (attr.words < 2)
			err(FILE_FORMAT_ERROR, "Light attributes", e);
		set_light_values(e, attr.strings[0], attr.strings[1]);
		free_split(&attr);
	}
	free(line);
	++e->lights;
}
