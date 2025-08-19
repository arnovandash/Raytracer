/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rt.h                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adippena <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/07/26 14:47:44 by adippena          #+#    #+#             */
/*   Updated: 2016/08/27 11:03:27 by adippena         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RT_H
# define RT_H

#include <stddef.h>

typedef struct	s_split_string
{
	char	**strings;
	size_t	words;
}				t_split_string;

#define strdel(s) do { free(s); s = NULL; } while (0)

t_split_string	nstrsplit(char const *s, char c);
void			free_split(t_split_string *split);
char			*strtrim(char const *s);
# include "SDL.h"
# include <errno.h>
# include <float.h>
# include <pthread.h>
# include <stdatomic.h>
# include <string.h>
# include <stdlib.h>
# include <ctype.h>

# include "defines.h"
# include "structs.h"
# include "prototypes.h"

extern t_stats	g_stats;

#endif
