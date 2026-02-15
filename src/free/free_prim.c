/*
** free_prim.c -- Deallocate the array of geometric primitives.
**
** Takes a triple pointer (t_prim ***) so that it can NULL the caller's
** pointer after freeing, preventing use-after-free. This pattern is used
** because C passes pointers by value -- to modify the caller's pointer,
** we need a pointer to that pointer (and the caller already stores a
** double pointer t_prim **prim, so we pass &prim which is t_prim ***).
*/

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
