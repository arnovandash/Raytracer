/*
** write_coord.c -- Write a 3D vector as space-separated coordinates
**
** Utility function used by all save_*.c files to write t_vector values
** in the "x y z" format expected by the scene file parser.
*/

#include "rt.h"

/*
** Writes a 3D vector to a file descriptor as "x y z\n".
** Used for LOC, DIR, NORMAL, UP, and any other vector field in the scene file.
*/
void	write_coord(t_vector v, int fd)
{
	dprintf(fd, "%f %f %f\n", v.x, v.y, v.z);
}
