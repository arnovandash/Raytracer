/*
** error.c -- Error handling and cleanup.
**
** Provides centralized error reporting and resource cleanup:
**   - err(): Builds a descriptive error message from an error code and
**     the originating function name. Prints to stdout with ANSI red color.
**     Error codes <= 15 are system errors (uses perror to append errno
**     description). Error codes >= 16 are format/usage errors (uses puts).
**   - exit_rt(): Frees all dynamically allocated resources (SDL surfaces,
**     window, scene data) and exits cleanly.
**   - strjoin(): Simple heap-allocated string concatenation helper,
**     replacing the former libft ft_strjoin function.
*/

#include "rt.h"

/*
** Concatenate two strings into a new heap-allocated string.
** Caller is responsible for freeing the result.
*/
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

/*
** Clean shutdown: free all resources in reverse order of allocation.
** Skips cleanup for USAGE_ERROR since nothing was allocated yet.
** Always calls SDL_Quit() to properly shut down the SDL subsystem.
*/
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

/*
** Report an error and exit. Constructs a message from the error code
** and function name. ANSI escape "\e[1;91m" sets bold bright red text,
** "\e[0m" resets. System errors (code <= 15) use perror() to append
** the errno string (e.g., "Permission denied"). Format errors use puts().
** USAGE_ERROR (32) is a static string -- not heap-allocated, so not freed.
*/
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
	/* Only free heap-allocated error strings (codes < 32). */
	if (error_no < 32)
		strdel(&error);
	exit_rt(e, error_no);
}
