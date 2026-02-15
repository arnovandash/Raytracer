/*
** half_bytes.c — Fast pixel buffer dimming for interactive rendering
**
** Halves the brightness of every pixel in an SDL surface by right-shifting
** each byte by 1 (equivalent to integer division by 2). This creates a
** visual fade/ghosting effect: the previous frame dims before the next
** frame's pixels are drawn on top, giving the user visual feedback during
** interactive camera movement or progressive rendering.
**
** The operation works directly on the raw pixel buffer at the byte level.
** Since each color channel (R, G, B, and any padding/alpha byte) occupies
** one byte, shifting right by 1 independently halves each channel without
** any cross-channel interference.
**
** Performance note: this processes every byte in the surface (h * pitch),
** where pitch is the number of bytes per row (may include padding). The
** loop runs backward from the last byte to the first.
*/

#include "rt.h"

/*
** half_bytes — Dim all pixels in the SDL surface to 50% brightness.
** Operates on the raw byte array: each byte >>= 1 divides that channel by 2.
*/

void		half_bytes(SDL_Surface *s)
{
	size_t			index;
	unsigned char	*px;

	/* Total bytes in the pixel buffer: rows * bytes-per-row */
	index = s->h * s->pitch;
	px = (unsigned char *)(s->pixels + index - 1);
	while (index--)
		*px-- >>= 1;
}
