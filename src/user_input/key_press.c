/*
** key_press.c -- Keyboard input handler using bitmask state machine
**
** The renderer uses a bitmask (e->flags) to track which keys are currently
** held down. Each key maps to a single bit (defined in defines.h as KEY_*).
** This approach allows checking multiple simultaneous key states efficiently
** with bitwise AND, and combining states with bitwise OR.
**
** Two separate handlers exist because key behavior differs by mode:
**   - key_press()  : Default mode -- grab, select, axis constraints
**   - mkey_press() : Middle-click camera mode -- WASD/Space/Ctrl movement
**
** Design note: S key has dual meaning depending on mode. In default mode,
** it toggles the "scale" flag (XOR). In camera mode, it means "move backward."
*/

#include "rt.h"

/*
** Clears the corresponding flag bit when a key is released.
** Uses bitwise AND with the complement (~) to zero out exactly one bit
** while leaving all other bits unchanged.
*/
void		key_release(t_env *e, SDL_Keycode key)
{
	if (key == SDLK_LSHIFT)
		e->flags &= ~KEY_SHIFT;
	else if (key == SDLK_LCTRL)
		e->flags &= ~KEY_CTRL;
	else if (key == SDLK_SPACE)
		e->flags &= ~KEY_SPACE;
	else if (key == SDLK_w)
		e->flags &= ~KEY_W;
	else if (key == SDLK_s)
		e->flags &= ~KEY_S;
	else if (key == SDLK_a)
		e->flags &= ~KEY_A;
	else if (key == SDLK_d)
		e->flags &= ~KEY_D;
}

/*
** Toggles between selecting all and deselecting all primitives.
** If any primitives are currently selected (s_num > 0), deselect all;
** otherwise, select all. Re-renders the scene to show selection highlighting.
*/
static void	key_press_a(t_env *e)
{
	if (e->s_num)
	{
		fputs("Deselecting all primitives\n", stdout);
		deselect_all(e);
	}
	else
	{
		fputs("Selecting all primitives\n", stdout);
		select_all(e);
	}
	draw(e, (SDL_Rect){0, 0, e->x, e->y});
}

/*
** Axis constraint toggle for grab/scale/rotate operations.
**
** Without Shift: Pressing X constrains movement to X axis only
**   -> Sets KEY_X, clears KEY_Y and KEY_Z
**
** With Shift: Pressing Shift+X constrains to all axes EXCEPT X (i.e. Y+Z)
**   -> Clears KEY_X, sets KEY_Y and KEY_Z
**
** This mirrors Blender's axis constraint behavior where Shift+axis means
** "lock that axis, move freely on the other two."
*/
static void	key_press_xyz(t_env *e, SDL_Keycode key)
{
	if (key == SDLK_x)
		e->flags = (e->flags & KEY_SHIFT) ?
			(e->flags & ~(KEY_X)) | KEY_Y | KEY_Z :
			(e->flags & ~(KEY_Y | KEY_Z)) | KEY_X;
	else if (key == SDLK_y)
		e->flags = (e->flags & KEY_SHIFT) ?
			(e->flags & ~(KEY_Y)) | KEY_X | KEY_Z :
			(e->flags & ~(KEY_X | KEY_Z)) | KEY_Y;
	else if (key == SDLK_z)
		e->flags = (e->flags & KEY_SHIFT) ?
			(e->flags & ~(KEY_Z)) | KEY_X | KEY_Y :
			(e->flags & ~(KEY_X | KEY_Y)) | KEY_Z;
}

/*
** Main key press handler (default mode, not camera movement mode).
**
** G key: Enters "grab" mode (Blender-style object manipulation). Only works
**   if at least one primitive is selected. Enables SDL relative mouse mode
**   so mouse deltas drive object movement instead of cursor position.
**
** A key: Select/deselect all (only when not in grab mode to avoid conflict).
**
** S key: Toggles scale flag (XOR ^= so pressing again turns it off).
** R key: Toggles rotate flag (same XOR toggle pattern).
**
** X/Y/Z keys: Delegated to key_press_xyz for axis constraint logic.
*/
void		key_press(t_env *e, SDL_Keycode key)
{
	if (key == SDLK_LSHIFT)
		e->flags |= KEY_SHIFT;
	else if (e->s_num && key == SDLK_g)
	{
		e->flags |= KEY_G;
		SDL_SetRelativeMouseMode(1);
		draw(e, (SDL_Rect){0, 0, e->x, e->y});
	}
	else if (!(e->flags & KEY_G) && key == SDLK_a)
		key_press_a(e);
	else if (key == SDLK_s)
		e->flags ^= KEY_S;
	else if (key == SDLK_r)
		e->flags ^= KEY_R;
	else
		key_press_xyz(e, key);
}

/*
** Camera movement key handler -- only called during middle-click camera mode.
** Sets flag bits for continuous movement keys (WASD/Space/Ctrl).
** These flags are polled each frame by cam_move() to translate the camera.
**
** Key mapping (world-space axes):
**   W/S = +Y/-Y (forward/backward)
**   A/D = -X/+X (left/right)
**   Space/Ctrl = +Z/-Z (up/down)
*/
void		mkey_press(t_env *e, SDL_Keycode key)
{
	if (key == SDLK_LCTRL)
		e->flags |= KEY_CTRL;
	else if (key == SDLK_SPACE)
		e->flags |= KEY_SPACE;
	else if (key == SDLK_w)
		e->flags |= KEY_W;
	else if (key == SDLK_s)
		e->flags |= KEY_S;
	else if (key == SDLK_a)
		e->flags |= KEY_A;
	else if (key == SDLK_d)
		e->flags |= KEY_D;
}
