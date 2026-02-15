/*
** export.c -- PPM image export.
**
** Exports the current rendered image as a PPM P6 (binary) file.
** PPM P6 format: "P6\n<width> <height>\n255\n" header followed by raw
** RGB triplets (3 bytes per pixel, no padding).
**
** The internal pixel format is SDL's 32-bit layout where bytes are
** stored in memory as B, G, R, A (little-endian 0xAARRGGBB). PPM P6
** expects R, G, B byte order. The write_image function swaps the R and
** B channels via bit manipulation before writing 3 bytes per pixel.
**
** Output filename: <scene_name>_<unix_timestamp>.ppm
*/

#include "rt.h"

/* Convert an integer to a heap-allocated decimal string via snprintf. */
char	*itoa(int n)
{
	char	*str;
	int		len;

	len = snprintf(NULL, 0, "%d", n);
	str = malloc(len + 1);
	snprintf(str, len + 1, "%d", n);
	return (str);
}

/*
** Write raw pixel data in PPM P6 format.
** For each pixel, swap R and B channels to convert from SDL's in-memory
** 0x00BBGGRR (little-endian) to PPM's R,G,B byte order:
**   - Bits [7:0]   (B in SDL) moved to bits [23:16] -> becomes R position
**   - Bits [15:8]  (G) stays in place
**   - Bits [23:16] (R in SDL) moved to bits [7:0]   -> becomes B position
** Only 3 bytes are written per pixel (the alpha byte is skipped).
*/
static void		write_image(t_env *e, int fd)
{
	size_t		total;
	size_t		index;
	uint32_t	px;
	uint32_t	px_rgb;

	total = e->x * e->y;
	index = 0;
	while (index != total)
	{
		px = e->px[index++];
		px_rgb = ((px & 0xFF) << 16) | (px & 0xFF00) | ((px & 0xFF0000) >> 16);
		write(fd, &px_rgb, 3);
	}
}

/*
** Export the rendered image to a PPM file.
** Generates a unique filename using the scene name and current unix
** timestamp. Writes the P6 header (magic number, comment, dimensions,
** max color value) then the raw pixel data.
*/
void			export(t_env *e)
{
	int		fd;
	char	*temp;

	fputs("Exporting rendered image... ", stdout);
	temp = NULL;
	asprintf(&temp, "%s_%ld.ppm", e->file_name, time(NULL));
	if ((fd = open(temp, O_WRONLY | O_TRUNC | O_CREAT, 0666)) == -1)
		err(FILE_OPEN_ERROR, "Could not export rendered image", e);
	strdel(&temp);
	fprintf(fdopen(fd, "w"), "%s\n", "P6\n# Exported by the best RT project ever!");
	temp = itoa(e->x);
	fputs(temp, fdopen(fd, "w"));
	fputs(" ", fdopen(fd, "w"));
	strdel(&temp);
	temp = itoa(e->y);
	fprintf(fdopen(fd, "w"), "%s\n", temp);
	fprintf(fdopen(fd, "w"), "%s\n", "255");
	write_image(e, fd);
	close(fd);
	fputs("Done\n", stdout);
}
