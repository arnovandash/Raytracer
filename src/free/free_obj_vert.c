/*
** free_obj_vert.c -- Generic void** array deallocator.
**
** Used to free arrays of heap-allocated items where the element type
** varies (vertices as t_vector**, normals as t_vector**, faces as
** t_face**). By accepting void**, the same function handles all three
** without code duplication.
*/

#include "rt.h"

void	free_obj_vert(void **v, size_t num_v)
{
	if (v)
	{
		while (num_v--)
		{
			if (v[num_v])
				free(v[num_v]);
			v[num_v] = NULL;
		}
		free(v);
		v = NULL;
	}
}
