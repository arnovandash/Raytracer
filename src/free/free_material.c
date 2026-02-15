/*
** free_material.c -- Deallocate the array of materials.
**
** Each material owns a heap-allocated name string that must be freed
** before the material struct itself. Frees name -> struct -> array.
*/

#include "rt.h"

void	free_material(t_material **material, size_t num_mat)
{
	if (material)
	{
		while (num_mat--)
		{
			free(material[num_mat]->name);
			material[num_mat]->name = NULL;
			if (material[num_mat])
				free(material[num_mat]);
			material[num_mat] = NULL;
		}
		free(material);
		material = NULL;
	}
}
