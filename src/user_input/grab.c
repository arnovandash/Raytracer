/*
** grab.c -- Blender-style object manipulation (grab/move and selection)
**
** "Grab" mode allows the user to move selected primitives by dragging the
** mouse. The workflow mirrors Blender:
**   1. Select one or more primitives (left-click or A for all)
**   2. Press G to enter grab mode (mouse becomes relative/hidden)
**   3. Move the mouse to translate objects along enabled axes
**   4. Optionally press X/Y/Z to constrain to a single axis
**   5. Left-click or press G again to confirm; right-click to cancel
**
** Mouse-to-world mapping:
**   - Horizontal mouse motion (xrel) maps to world X axis
**   - Vertical mouse motion (yrel) maps to world Z axis
**   - Scroll wheel maps to world Y axis (depth, into/out of screen)
**
** The scale factor 0.015 converts pixel deltas to world units, providing
** fine-grained control. Scroll wheel uses a larger factor (0.5) since
** wheel ticks are coarser than pixel movements.
*/

#include "rt.h"

/*
** Scroll wheel handler: moves selected objects along the Y axis (depth).
** Only active when the Y axis flag is set (not constrained away from Y).
** The negative sign means scrolling "up" (positive wheel.y) moves objects
** toward the camera (decreasing Y in the scene's coordinate system).
** SDL_FlushEvent prevents scroll event buildup during rendering.
*/
void	m_wheel(t_env *e, SDL_Event *event)
{
	size_t index;

	if (e->s_num)
	{
		index = e->prims;
		while (index--)
			if ((e->flags & KEY_Y) && e->prim[index]->s_bool)
				e->prim[index]->loc.y -= (double)event->wheel.y * 0.5;
		SDL_FlushEvent(SDL_MOUSEWHEEL);
		draw(e, (SDL_Rect){0, 0, e->x, e->y});
	}
}

/*
** Moves all selected primitives based on mouse motion delta.
** Only the axes whose flags are set in e->flags are affected, allowing
** axis-constrained movement (e.g., press X to move only along X).
**
** The mapping is:
**   mouse xrel -> world X (positive xrel = move right = +X)
**   mouse yrel -> world Z (positive yrel = move down on screen = -Z)
**
** The 0.015 scale factor provides fine control: ~67 pixels of mouse
** movement equals 1 world unit.
**
** SDL_FlushEvent discards queued motion events so movement doesn't
** accumulate while the scene is re-rendering.
*/
void	grab(t_env *e, SDL_Event *event)
{
	size_t	index;

	SDL_SetRelativeMouseMode(1);
	if (e->s_num)
	{
		index = e->prims;
		while (index--)
			if (e->prim[index]->s_bool)
			{
				if (e->flags & KEY_X)
					e->prim[index]->loc.x += (double)event->motion.xrel * 0.015;
				if (e->flags & KEY_Z)
					e->prim[index]->loc.z -= (double)event->motion.yrel * 0.015;
			}
		SDL_FlushEvent(SDL_MOUSEMOTION);
		draw(e, (SDL_Rect){0, 0, e->x, e->y});
	}
}

/*
** Clears the selection state on all primitives and resets the selection count.
** Only iterates through primitives if at least one is selected (optimization).
*/
void	deselect_all(t_env *e)
{
	size_t	index;

	if (e->s_num)
	{
		index = e->prims;
		while (index--)
			e->prim[index]->s_bool = 0;
	}
	e->s_num = 0;
}

/*
** Marks every primitive as selected and backs up all positions for undo.
** The loc_bak snapshot allows right-click during grab to revert all objects
** to where they were before the grab started.
*/
void	select_all(t_env *e)
{
	size_t	index;

	index = e->prims;
	while (index--)
	{
		e->prim[index]->s_bool = 1;
		e->prim[index]->loc_bak = e->prim[index]->loc;
	}
	e->s_num = e->prims;
}
