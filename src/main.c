/*
** main.c -- Raytracer entry point.
**
** Declares global performance counters for thread-safe statistics tracking:
** - g_stats: atomic counters shared across all threads (safe for concurrent
**   increments from rendering threads).
** - g_tls_stats: thread-local counters that each thread accumulates into
**   privately, avoiding atomic overhead on hot paths.
**
** Program flow: validate arguments -> store scene filename -> init_env()
** (parse scene file + create SDL window) -> draw() (render the initial frame)
** -> event_loop() (interactive SDL event handling for camera movement, etc.).
*/

#include "rt.h"

/* Global atomic counters -- incremented by all render threads concurrently. */
t_stats					g_stats;

/* Thread-local stats -- each pthread gets its own copy, no locking needed. */
_Thread_local t_thread_stats	g_tls_stats;

int		main(int ac, char **av)
{
	t_env	e;

	memset(&g_stats, 0, sizeof(t_stats));
	if (ac != 2)
		err(USAGE_ERROR, NULL, &e);
	e.file_name = strdup(av[1]);
	init_env(&e);
	/* Render the full image (region covers entire window). */
	draw(&e, (SDL_Rect){0, 0, e.x, e.y});
	/* Enter the interactive event loop -- never returns (exits via exit_rt). */
	event_loop(&e);
	return (0);
}
