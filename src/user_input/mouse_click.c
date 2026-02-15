/*
** mouse_click.c -- Mouse click handling for selection and camera control
**
** Implements a pick-ray selection system: when the user left-clicks, a ray is
** cast from the camera through the clicked pixel into the scene. If it hits a
** primitive, that primitive is selected (highlighted in the render). This is
** the standard approach for 3D object picking in raytracers.
**
** Selection model (Blender-inspired):
**   - Left-click: Replace selection with clicked object
**   - Shift+left-click: Add/remove clicked object from selection (toggle)
**   - Click on empty space: Deselect all
**   - Right-click during grab: Cancel grab, restore original positions
**   - Middle-click: Enter camera rotation mode
**
** Each primitive has:
**   - s_bool: Whether it is currently selected (0 or 1)
**   - loc_bak: Backed-up position for undo (saved when selected)
** The global e->s_num tracks total number of selected primitives.
*/

#include "rt.h"

/*
** Restores all selected primitives to their backed-up positions.
** Called on right-click during grab mode to cancel the move operation.
** This is the "undo" mechanism: loc_bak was saved when the primitive
** was first selected, so this reverts to the pre-grab position.
*/
static void	reset_loc(t_env *e)
{
	size_t	index;

	index = e->prims;
	while (index--)
		if (e->prim[index]->s_bool)
			e->prim[index]->loc = e->prim[index]->loc_bak;
}

/*
** Casts a pick ray through the clicked pixel and selects/deselects the hit.
**
** 1. Get mouse coordinates from SDL
** 2. Build a ray from the camera through that pixel (get_ray_dir)
** 3. Test the ray against all scene objects (intersect_scene)
** 4. If something was hit (p_hit != NULL):
**    - Without Shift: clear existing selection first (exclusive select)
**    - Toggle the hit primitive's selection state
**    - Back up its position for potential undo
** 5. If nothing was hit: deselect everything
*/
static void	click_select(t_env *e)
{
	int	x;
	int	y;

	SDL_GetMouseState(&x, &y);
	get_ray_dir(e, x, y);
	intersect_scene(e);
	if (e->p_hit)
	{
		if (!(e->flags & KEY_SHIFT))
			deselect_all(e);
		if (!e->p_hit->s_bool)
		{
			e->p_hit->s_bool = 1;
			e->p_hit->loc_bak = e->p_hit->loc;
			++e->s_num;
		}
		else
		{
			e->p_hit->s_bool = 0;
			--e->s_num;
		}
	}
	else
		deselect_all(e);
}

/*
** Handles middle mouse button release: exits camera rotation mode.
** Disables SDL relative mouse mode (restores normal cursor) and
** clears the KEY_MID_CLICK flag so the event loop stops routing
** mouse motion to cam_rot().
*/
void		click_release(t_env *e, SDL_Event event)
{
	if (event.button.button == SDL_BUTTON_MIDDLE)
	{
		SDL_SetRelativeMouseMode(0);
		e->flags &= ~KEY_MID_CLICK;
	}
}

/*
** Main mouse click dispatcher.
**
** Left button:  Select object (only when not in grab mode, to avoid
**               accidentally selecting while moving objects)
** Right button: Cancel grab -- restore all selected objects to their
**               backed-up positions (only meaningful during grab mode)
** Middle button: Enter camera rotation mode (KEY_MID_CLICK flag).
**               Disables relative mouse so cursor stays visible.
**
** After any click, reset_keys() clears transient state (grab/scale/rotate)
** and re-enables all axes, then the scene is re-rendered.
*/
void		mouse_click(t_env *e, uint8_t button)
{
	if (button == SDL_BUTTON_LEFT)
	{
		if (!(e->flags & KEY_G))
			click_select(e);
	}
	else if (button == SDL_BUTTON_RIGHT)
	{
		if (e->flags & KEY_G)
			reset_loc(e);
	}
	else if (button == SDL_BUTTON_MIDDLE)
	{
		e->flags |= KEY_MID_CLICK;
		SDL_SetRelativeMouseMode(0);
	}
	reset_keys(e);
	draw(e, (SDL_Rect){0, 0, e->x, e->y});
}
