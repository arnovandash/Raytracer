/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adippena <angusdippenaar@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/07/08 19:58:53 by adippena          #+#    #+#             */
/*   Updated: 2016/09/03 15:52:31 by adippena         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

char	*strjoin(char const *s1, char const *s2)
{
	char	*new;
	size_t	len1;
	size_t	len2;

	len1 = strlen(s1);
	len2 = strlen(s2);
	new = (char *)malloc(len1 + len2 + 1);
	if (new == NULL)
		return (NULL);
	memcpy(new, s1, len1);
	memcpy(new + len1, s2, len2);
	new[len1 + len2] = '\0';
	return (new);
}

void	exit_rt(t_env *e, int code)
{
	if (code != USAGE_ERROR)
	{
		if (e->file_name)
			free(e->file_name);
		if (e->img)
			SDL_FreeSurface(e->img);
		if (e->dof)
			SDL_FreeSurface(e->dof);
		if (e->win)
			SDL_DestroyWindow(e->win);
		free_light(e->light, e->lights);
		free_material(e->material, e->materials);
		free_object(e->object, e->objects);
		free_prim(&e->prim, e->prims);
	}
	SDL_Quit();
	exit(0);
}

void	err(int error_no, char *function, t_env *e)
{
	char	*error;

	fputs("\e[1;91mERROR:\e[0m ", stdout);
	if (error_no == FILE_OPEN_ERROR)
		error = strjoin(function, ": File open error");
	else if (error_no == MALLOC_ERROR)
		error = strjoin(function, ": Malloc error");
	else if (error_no == FREE_ERROR)
		error = strjoin(function, ": Free error");
	else if (error_no == FILE_FORMAT_ERROR)
		error = strjoin(function, ": Invalid file format");
	else if (error_no == USAGE_ERROR)
		error = "Invalid Usage\n    ./RT [SCENE FILE]";
	if (error_no > 15)
		puts(error);
	else
		perror(error);
	if (error_no < 32)
		strdel(&error);
	exit_rt(e, error_no);
}
