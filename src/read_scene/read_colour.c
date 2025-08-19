/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_colour.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adippena <angusdippenaar@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/08/05 15:54:07 by adippena          #+#    #+#             */
/*   Updated: 2016/09/04 15:08:53 by adippena         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

static double	to_range(double value, double min, double max)
{
	value = (value < min) ? min : value;
	value = (value > max) ? max : value;
	return (value);
}

t_colour		get_colour(t_env *e, t_split_string values)
{
	t_colour	res;
	char		*temp;

	res = (t_colour){0.0, 0.0, 0.0, 1.0};
	if (values.words == 1 || values.words == 2)
	{
		if (strlen(values.strings[0]) != 6)
			err(FILE_FORMAT_ERROR, "Colour has invalid format", e);
		temp = calloc(3, sizeof(char));
		temp = strncpy(temp, values.strings[0], 2);
		res.r = (int)strtol(temp, NULL, 16) / 255.0;
		temp = strncpy(temp, values.strings[0] + 2, 2);
		res.g = (int)strtol(temp, NULL, 16) / 255.0;
		temp = strncpy(temp, values.strings[0] + 4, 2);
		res.b = (int)strtol(temp, NULL, 16) / 255.0;
		free(temp);
	}
	if (values.words == 2)
		res.intensity = to_range(atof(values.strings[1]), 0.0, 1.0);
	if (values.words > 2)
		err(FILE_FORMAT_ERROR, "Invalid colour", e);
	return (res);
}
