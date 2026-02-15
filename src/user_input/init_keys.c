/*
** init_keys.c -- Reset keyboard/mouse state to defaults
**
** Called after completing an operation (grab, click, etc.) to return the
** input system to a clean state. Clears transient mode flags (grab, scale,
** rotate) while preserving persistent state like selection and shift.
** Re-enables all three axes (X|Y|Z) so the next operation is unconstrained.
*/

#include "rt.h"

/*
** Resets transient key flags to their default state.
**
** The bitmask operation:
**   1. (e->flags & ~(KEY_G | KEY_S | KEY_R))  -- clears grab, scale, rotate
**   2. | KEY_X | KEY_Y | KEY_Z                -- enables all three axes
**
** This preserves other flags (KEY_SHIFT, KEY_MID_CLICK, etc.) while ensuring
** the next grab/scale/rotate starts unconstrained on all axes.
** Also releases SDL relative mouse mode to restore the normal cursor.
*/
void		reset_keys(t_env *e)
{
	e->flags = (e->flags & ~(KEY_G | KEY_S | KEY_R)) | KEY_X | KEY_Y | KEY_Z;
	SDL_SetRelativeMouseMode(0);
}
