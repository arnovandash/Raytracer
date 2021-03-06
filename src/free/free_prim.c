/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_prim.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rojones <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/08/17 07:47:18 by rojones           #+#    #+#             */
/*   Updated: 2016/08/22 21:36:56 by adippena         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

void	free_prim(t_prim ***prim, size_t num_prim)
{
	if (*prim)
	{
		while (num_prim--)
		{
			if (prim[0][num_prim])
				free(prim[0][num_prim]);
		}
		free(*prim);
		*prim = NULL;
	}
}
