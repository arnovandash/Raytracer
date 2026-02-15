/*
** rt.h -- Master header for the raytracer.
**
** Includes all standard library headers, SDL2, and project-specific headers.
** Declares global variables (g_stats atomic counters, g_tls_stats thread-local
** counters). Defines helper types (t_split_string for the string splitter)
** and the strdel() macro (safe free-and-NULL, replacing the former libft
** ft_strdel function).
**
** This header is included by every .c file in the project, so it serves as
** the single point of entry for all common dependencies.
*/

#ifndef RT_H
# define RT_H

#include <stddef.h>

/*
** t_split_string -- Result of nstrsplit(): an array of strings and a count.
** Used throughout the scene parser to tokenize lines by delimiter.
*/
typedef struct	s_split_string
{
	char	**strings;
	size_t	words;
}				t_split_string;

/*
** strdel -- Safe free-and-NULL macro.
** Frees the memory pointed to by *s, then sets *s to NULL to prevent
** use-after-free. The do/while(0) wrapper makes it safe in if/else chains.
*/
#define strdel(s) do { free(*(s)); *(s) = NULL; } while (0)

/* Forward declarations for utilities defined in src/nstrsplit.c and src/strtrim.c */
t_split_string	nstrsplit(char const *s, char c);
void			free_split(t_split_string *split);
char			*strtrim(char const *s);

/* --- Standard library and system headers --- */
# include "SDL.h"
# include <errno.h>
# include <fcntl.h>
# include <float.h>
# include <pthread.h>
# include <stdatomic.h>
# include <stdio.h>
# include <string.h>
# include <stdlib.h>
# include <ctype.h>
# include <unistd.h>

/* --- Project headers (order matters: structs before vec_math before prototypes) --- */
# include "defines.h"
# include "structs.h"
# include "vec_math.h"
# include "prototypes.h"

/*
** Global performance counters.
** g_stats: atomic counters safe for concurrent access from render threads.
** g_tls_stats: thread-local counters (each thread gets its own copy via
** _Thread_local, a C11 storage-class specifier).
*/
extern t_stats			g_stats;
extern _Thread_local	t_thread_stats	g_tls_stats;

#endif
