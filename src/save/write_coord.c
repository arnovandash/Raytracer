/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   write_coord.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rojones <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/08/15 13:51:06 by rojones           #+#    #+#             */
/*   Updated: 2016/09/03 16:56:11 by adippena         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

void	write_coord(t_vector v, int fd)
{
	char	temp[40];

	sprintf(temp, "%f %f %f\n", v.x, v.y, v.z);
	ft_putstr_fd(temp, fd);
}
