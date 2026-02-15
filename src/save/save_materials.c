/*
** save_materials.c -- Serialize materials to scene file
**
** Materials define surface appearance: diffuse color, specular highlights,
** reflection, refraction, and index of refraction (IOR). Each material has
** a name so primitives can reference it.
**
** Note: The loop starts at index 1, skipping the DEFAULT material at index 0.
** The default material is implicitly present and not written to the file.
*/

#include "rt.h"

/*
** Converts a color to hex RRGGBB format, followed by the intensity value.
** Material colors include an intensity component (unlike light colors)
** which controls how strongly the color contributes to shading.
*/
static void	write_colour(t_colour *col, int fd)
{
	dprintf(fd, "%02X%02X%02X %f\n",
		(int)(col->r * 255.0),
		(int)(col->g * 255.0),
		(int)(col->b * 255.0),
		col->intensity);
}

/*
** Writes a single MATERIAL block.
** REFLECT/REFRACT are [0.0, 1.0] coefficients controlling how much light
** is reflected vs. refracted. IOR (index of refraction) determines the
** bending angle when light passes through the surface (e.g., glass = 1.5).
*/
static void	save_mat(t_material *mat, int fd)
{
	dprintf(fd, "\n\tMATERIAL\n");
	dprintf(fd, "\t\tNAME\t\t%s\n", mat->name);
	dprintf(fd, "\t\tDIFFUSE\t\t");
	write_colour(&mat->diff, fd);
	dprintf(fd, "\t\tSPECULAR\t");
	write_colour(&mat->spec, fd);
	dprintf(fd, "\t\tREFLECT\t\t%f\n", mat->reflect);
	dprintf(fd, "\t\tREFRACT\t\t%f\n", mat->refract);
	dprintf(fd, "\t\tIOR\t\t\t%f\n", mat->ior);
}

/*
** Iterates over all materials and serializes each one.
** Starts at index 1 to skip the built-in DEFAULT material (index 0),
** which is always present and not stored in the scene file.
*/
void		save_materials(t_material **material, size_t materials, int fd)
{
	size_t	i;

	i = 1;
	while (i < materials)
	{
		save_mat(material[i], fd);
		i++;
	}
}
