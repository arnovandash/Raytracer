/*
** save_lights.c -- Serialize light sources to scene file
**
** Converts internal light representation back to the scene file format.
** Key conversion: light colors are stored internally as normalized doubles
** [0.0, 1.0] but written as hex RRGGBB strings. Intensity is stored
** internally in lumens but the scene file uses a custom unit, so we divide
** by the conversion factor 3.415 when saving.
*/

#include "rt.h"

/*
** Converts a normalized color (r,g,b each in [0.0, 1.0]) to a hex string.
** Multiplies by 255 to get the 0-255 integer range, then formats as
** zero-padded two-digit hex per channel: "RRGGBB".
*/
static void	write_colour(t_colour *col, int fd)
{
	dprintf(fd, "%02X%02X%02X\n",
		(int)(col->r * 255.0),
		(int)(col->g * 255.0),
		(int)(col->b * 255.0));
}

/*
** Writes a single LIGHT block with all its properties.
** INTENSITY is divided by 3.415 to convert from the internal lumen value
** back to the scene file's intensity unit. HALF controls light falloff
** (the distance at which intensity drops to half).
*/
static void	save_light(t_light *light, int fd)
{
	dprintf(fd, "\n\tLIGHT\n");
	dprintf(fd, "\t\tLOC\t\t\t");
	write_coord(light->loc, fd);
	dprintf(fd, "\t\tCOLOUR\t\t");
	write_colour(&light->colour, fd);
	dprintf(fd, "\t\tINTENSITY\t%f\n", light->lm / 3.415);
	dprintf(fd, "\t\tHALF\t\t%f\n", light->half);
}

/*
** Iterates over all lights in the scene and serializes each one.
*/
void		save_lights(t_light **lights, size_t num_light, int fd)
{
	size_t	i;

	i = 0;
	while (i < num_light)
	{
		save_light(lights[i], fd);
		i++;
	}
}
