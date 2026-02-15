/*
** free_object.c -- Deallocate OBJ mesh objects.
**
** Each t_object owns:
**   - A heap-allocated name string
**   - An array of t_face pointers (triangle faces)
**   - An array of t_vector pointers (vertex positions)
**   - An array of t_vector pointers (vertex normals)
** All sub-arrays are freed via free_obj_vert (generic void** freer),
** then the object struct itself, then the top-level array.
*/

#include "rt.h"

void	free_object(t_object **obj, size_t num_obj)
{
	if (obj)
	{
		while (num_obj--)
			if (obj[num_obj])
			{
				free(obj[num_obj]->name);
			obj[num_obj]->name = NULL;
				free_obj_vert((void**)(obj[num_obj]->face),
						obj[num_obj]->faces);
				free_obj_vert((void**)(obj[num_obj]->v),
						obj[num_obj]->verticies);
				free_obj_vert((void**)(obj[num_obj]->vn),
						obj[num_obj]->vnormals);
				free(obj[num_obj]);
				obj[num_obj] = NULL;
			}
		free(obj);
		obj = NULL;
	}
}
