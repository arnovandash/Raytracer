/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_vector.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adippena <angusdippenaar@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/08/05 15:53:16 by adippena          #+#    #+#             */
/*   Updated: 2016/09/04 15:09:59 by adippena         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

t_vector	get_vector(t_env *e, t_split_string values)
{
	if (values.words != 3)
		err(FILE_FORMAT_ERROR, "Invalid vector format", e);
	return ((t_vector){
		atof(values.strings[0]),
		atof(values.strings[1]),
		atof(values.strings[2])});
}

t_vector	get_unit_vector(t_env *e, t_split_string values)
{
	if (values.words != 3)
		err(FILE_FORMAT_ERROR, "Invalid vector format", e);
	return (vunit((t_vector){
		atof(values.strings[0]),
		atof(values.strings[1]),
		atof(values.strings[2])}));
}
