/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adippena <angusdippenaar@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/07/08 20:00:35 by adippena          #+#    #+#             */
/*   Updated: 2016/08/15 17:13:51 by rojones          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

int		main(int ac, char **av)
{
	t_env	e;

	nullify_pointers(&e);
	if (ac != 2)
		err(USAGE_ERROR, NULL, &e);
	e.file_name = ft_strdup(av[1]);
	ft_printf("name %s\n", e.file_name);
	init_env(&e);
	draw(&e, (SDL_Rect){0, 0, WIN_X, WIN_Y});
	event_loop(&e);
	return (0);
}
