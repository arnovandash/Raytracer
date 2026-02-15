/*
** save_prims.c -- Serialize geometric primitives to scene file
**
** Each primitive type has different attributes that need to be written:
**   - Sphere/Hemisphere/Cylinder: LOC + RADIUS
**   - Cone: LOC + DIR + ANGLE (stored in radians, converted to degrees)
**   - Cylinder: LOC + DIR + LIMIT (length cap)
**   - Plane/Disk: LOC + NORMAL
**   - Hemisphere: LOC + DIR (orientation of the open face)
**   - Triangle: LOC only (vertices stored elsewhere)
**
** The save is split into save_prim() for the first half of attributes
** and save_prim2() for the remainder, due to 42-school function length limits.
*/

#include "rt.h"

/*
** Converts an integer primitive type ID to its string name for the scene file.
** These string names match what the parser expects when reading scenes back.
*/
static void	put_type(int type, int fd)
{
	char *temp;

	temp = NULL;
	if (type == PRIM_SPHERE)
		temp = "sphere";
	else if (type == PRIM_PLANE)
		temp = "plane";
	else if (type == PRIM_HEMI_SPHERE)
		temp = "hemi_sphere";
	else if (type == PRIM_CONE)
		temp = "cone";
	else if (type == PRIM_CYLINDER)
		temp = "cylinder";
	else if (type == PRIM_DISK)
		temp = "disk";
	else if (type == PRIM_TRIANGLE)
		temp = "triangle";
	dprintf(fd, "%s\n", temp);
}

/*
** Writes the second half of primitive attributes: RADIUS, ANGLE, MATERIAL.
** ANGLE is converted from radians (internal) to degrees (scene file) using
** the formula: degrees = (180 / pi) * radians.
*/
static void	save_prim2(const t_prim *prim, t_material **mat, const int fd)
{
	if (prim->type == PRIM_SPHERE || prim->type == PRIM_CYLINDER ||
			prim->type == PRIM_HEMI_SPHERE)
		dprintf(fd, "\t\tRADIUS\t\t%f\n", prim->radius);
	if (prim->type == PRIM_CONE)
		dprintf(fd, "\t\tANGLE\t\t%f\n", (180 / M_PI) * prim->angle);
	dprintf(fd, "\t\tMATERIAL\t%s\n", mat[prim->material]->name);
}

/*
** Writes the first half of a PRIMITIVE block: TYPE, LOC, and type-specific
** directional attributes (DIR for oriented primitives, NORMAL for planes,
** LIMIT for capped primitives like cylinders and cones).
*/
static void	save_prim(const t_prim *prim, t_material **mat, const int fd)
{
	dprintf(fd, "\n\tPRIMITIVE\n");
	dprintf(fd, "\t\tTYPE\t\t");
	put_type(prim->type, fd);
	dprintf(fd, "\t\tLOC\t\t\t");
	write_coord(prim->loc, fd);
	if (prim->type == PRIM_HEMI_SPHERE)
	{
		dprintf(fd, "\t\tDIR\t\t\t");
		write_coord(prim->dir, fd);
	}
	if (prim->type == PRIM_CONE || prim->type == PRIM_CYLINDER)
	{
		dprintf(fd, "\t\tDIR\t\t\t");
		write_coord(prim->dir, fd);
		dprintf(fd, "\t\tLIMIT\t\t%f\n", prim->limit);
	}
	if (prim->type == PRIM_PLANE || prim->type == PRIM_DISK)
	{
		dprintf(fd, "\t\tNORMAL\t\t");
		write_coord(prim->normal, fd);
	}
	save_prim2(prim, mat, fd);
}

/*
** Iterates over all primitives in the scene and serializes each one.
*/
void		save_prims(t_prim **prim, t_material **mat, size_t prims, int fd)
{
	size_t	i;

	i = 0;
	while (i < prims)
	{
		save_prim(prim[i], mat, fd);
		i++;
	}
}
