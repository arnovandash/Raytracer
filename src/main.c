/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adippena <angusdippenaar@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/07/08 20:00:35 by adippena          #+#    #+#             */
/*   Updated: 2016/08/29 17:13:46 by adippena         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

t_stats					g_stats;
_Thread_local t_thread_stats	g_tls_stats;

int		main(int ac, char **av)
{
	t_env	e;

	memset(&g_stats, 0, sizeof(t_stats));
	if (ac != 2)
		err(USAGE_ERROR, NULL, &e);
	e.file_name = strdup(av[1]);
	init_env(&e);
	draw(&e, (SDL_Rect){0, 0, e.x, e.y});
	event_loop(&e);
	return (0);
}
