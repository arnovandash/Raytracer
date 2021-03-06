/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_keys.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adippena <angusdippenaar@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/08/29 20:08:58 by adippena          #+#    #+#             */
/*   Updated: 2016/09/02 22:06:21 by adippena         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

void		reset_keys(t_env *e)
{
	e->flags = (e->flags & ~(KEY_G | KEY_S | KEY_R)) | KEY_X | KEY_Y | KEY_Z;
	SDL_SetRelativeMouseMode(0);
}
