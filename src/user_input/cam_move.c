/*
** cam_move.c -- Camera translation and rotation
**
** The camera is defined by two points: loc (position) and dir (look-at target).
** The view direction is the vector from loc to dir. This representation makes
** rotation easy (move dir while keeping loc fixed) and translation easy (move
** both loc and dir by the same offset to preserve the view direction).
**
** Rotation: Mouse motion during middle-click mode rotates the camera by
** adjusting the look-at point. This is an orbiting/panning rotation, not a
** true arcball -- it modifies dir.x and dir.z linearly from mouse deltas.
**
** Translation: WASD/Space/Ctrl move the camera along world-space axes.
** Both loc and dir are moved together so the view direction stays constant.
**
** Coordinate system:
**   X = left/right (A/D keys)
**   Y = forward/backward (S/W keys) -- this is the depth axis
**   Z = up/down (Ctrl/Space keys)
*/

#include "rt.h"

/*
** Rotates the camera by adjusting the look-at point based on mouse motion.
** - Horizontal mouse motion (xrel) rotates around Z axis -> modifies dir.x
** - Vertical mouse motion (yrel) tilts up/down -> modifies dir.z
** The 0.1 factor controls rotation sensitivity (degrees per pixel of motion).
** SDL_FlushEvent discards queued motion events to prevent rotation buildup
** while the scene is being re-rendered.
*/
void			cam_rot(t_env *e, SDL_Event event)
{
	SDL_SetRelativeMouseMode(1);
	e->camera.dir.x += (double)event.motion.xrel * 0.1;
	e->camera.dir.z -= (double)event.motion.yrel * 0.1;
	draw(e, (SDL_Rect){0, 0, e->x, e->y});
	SDL_FlushEvent(SDL_MOUSEMOTION);
}

/*
** Translates camera in the negative direction along world axes.
** Both loc and dir are decremented together so the view direction
** (dir - loc) remains unchanged -- the camera slides without rotating.
**   A key -> -X (move left)
**   S key -> -Y (move backward)
**   Ctrl  -> -Z (move down)
*/
static void		cam_move_minus(t_env *e)
{
	if (e->flags & KEY_A)
	{
		--e->camera.loc.x;
		--e->camera.dir.x;
	}
	if (e->flags & KEY_S)
	{
		--e->camera.loc.y;
		--e->camera.dir.y;
	}
	if (e->flags & KEY_CTRL)
	{
		--e->camera.loc.z;
		--e->camera.dir.z;
	}
}

/*
** Translates camera in the positive direction along world axes.
** Same principle as cam_move_minus: both points move together.
**   D key   -> +X (move right)
**   W key   -> +Y (move forward)
**   Space   -> +Z (move up)
*/
static void		cam_move_plus(t_env *e)
{
	if (e->flags & KEY_D)
	{
		++e->camera.loc.x;
		++e->camera.dir.x;
	}
	if (e->flags & KEY_W)
	{
		++e->camera.loc.y;
		++e->camera.dir.y;
	}
	if (e->flags & KEY_SPACE)
	{
		++e->camera.loc.z;
		++e->camera.dir.z;
	}
}

/*
** Called each frame during camera movement mode. Applies all active
** movement flags (multiple keys can be held simultaneously for diagonal
** movement) and re-renders the scene.
*/
void			cam_move(t_env *e)
{
	cam_move_plus(e);
	cam_move_minus(e);
	draw(e, (SDL_Rect){0, 0, e->x, e->y});
}
