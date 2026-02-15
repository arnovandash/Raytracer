/*
** loop.c -- SDL event loop and input dispatch.
**
** Runs an infinite event loop at approximately 60 fps (16 ms delay per
** iteration). Polls SDL events and dispatches them to the appropriate
** handlers:
**   - Keyboard: Escape exits, D redraws, E exports PPM, S saves scene
**   - Mouse: left/right click for selection, middle-click for camera
**     rotation mode, scroll wheel for grab-mode depth adjustment
**   - Camera: middle-click + WASD for camera fly-through movement,
**     middle-click + mouse motion for camera rotation
**   - Grab mode: G key enables object dragging with mouse motion
**
** The program has two keyboard modes:
**   1. Normal mode (key_press): object manipulation keys
**   2. Middle-click mode (mkey_press): WASD camera movement
**
** Note: "while (42)" is a 42-school convention -- it is simply an
** always-true condition (equivalent to "while (1)").
*/

#include "rt.h"

/*
** Handle key-down events. Escape always exits. D/E/S only work when
** not in middle-click camera mode (to avoid accidental triggers while
** flying the camera). When objects are loaded (e->objects != 0), key
** handling for movement/selection is disabled.
*/
static void	event_keydown(t_env *e, SDL_Keycode key)
{
	if (key == SDLK_ESCAPE)
		exit_rt(e, 0);
	else if (key == SDLK_d && !(e->flags & KEY_MID_CLICK))
		draw(e, (SDL_Rect){0, 0, e->x, e->y});
	else if (key == SDLK_e && !(e->flags & KEY_MID_CLICK))
		export(e);
	else if (key == SDLK_s && !(e->flags & KEY_MID_CLICK))
		save(e);
	else if (!e->objects && e->flags & KEY_MID_CLICK)
		mkey_press(e, key);
	else if (!e->objects && !(e->flags & KEY_MID_CLICK))
		key_press(e, key);
}

/*
** Poll and dispatch all pending SDL events.
** Multiple event types may be queued per frame -- process them all.
** Mouse motion for camera rotation is only processed while middle-click
** is held. Grab mode (KEY_G) handles mouse motion and scroll separately.
*/
static void	event_poll(t_env *e)
{
	SDL_Event	event;

	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
			exit_rt(e, 0);
		else if (event.type == SDL_WINDOWEVENT_EXPOSED)
			SDL_UpdateWindowSurface(e->win);
		else if (event.type == SDL_KEYDOWN && event.key.repeat == 0)
			event_keydown(e, event.key.keysym.sym);
		else if (event.type == SDL_KEYUP && event.key.repeat == 0)
			key_release(e, event.key.keysym.sym);
		else if (event.type == SDL_MOUSEBUTTONUP && !e->objects)
			click_release(e, event);
		else if (event.type == SDL_MOUSEBUTTONDOWN && !e->objects)
			mouse_click(e, event.button.button);
		else if (e->flags & KEY_MID_CLICK && !e->s_num)
			(event.type == SDL_MOUSEMOTION) ? cam_rot(e, event) : 0;
		/* Camera movement is applied every frame while middle-click held. */
		(e->flags & KEY_MID_CLICK) ? cam_move(e) : 0;
		if (e->flags & KEY_G && event.type == SDL_MOUSEMOTION)
			grab(e, &event);
		if (e->flags & KEY_G && event.type == SDL_MOUSEWHEEL)
			m_wheel(e, &event);
	}
}

/*
** Main event loop. Runs forever (exits via exit_rt from within handlers).
** SDL_Delay(16) yields ~60 fps to avoid busy-waiting.
*/
void		event_loop(t_env *e)
{
	while (42)
	{
		event_poll(e);
		SDL_Delay(16);
	}
}
