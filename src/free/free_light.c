/*
** free_light.c -- Deallocate the array of light sources.
**
** Frees each individual t_light struct, then the pointer array itself.
** Iterates backwards using a decrementing counter (common C idiom for
** cleanup loops that avoids underflow issues with size_t).
*/

#include "rt.h"

void	free_light(t_light **light, size_t num_light)
{
	if (light)
	{
		while (num_light--)
		{
			if (light[num_light])
				free(light[num_light]);
			light[num_light] = NULL;
		}
		free(light);
		light = NULL;
	}
}
