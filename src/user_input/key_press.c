/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   key_press.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arnovan- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/08/12 15:44:46 by arnovan-          #+#    #+#             */
/*   Updated: 2016/09/02 22:27:15 by adippena         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

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
