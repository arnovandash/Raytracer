/*
** copy_env.c -- Thread-safe environment cloning for multithreaded rendering.
**
** Each render thread needs its own t_env to track per-thread state (e.g.,
** the nearest intersection found so far). This function heap-allocates a
** new t_env and copies all fields via struct assignment.
**
** Shared data (primitive list, object list, lights, materials) is safe to
** share because render threads only READ the scene -- they never modify it.
** The only per-thread mutable state is p_hit (pointer to the closest
** primitive hit by the current ray), which is NULLed so each thread starts
** with a clean hit state.
*/

#include "rt.h"

/*
** Clone the environment for a worker thread.
** Struct assignment copies all fields including pointers (shallow copy).
** p_hit is reset to NULL since each thread independently tracks its
** nearest intersection during ray traversal.
*/
t_env	*copy_env(t_env *e)
{
	t_env	*res;

	res = (t_env *)malloc(sizeof(t_env));
	*res = *e;
	res->p_hit = NULL;
	return (res);
}
