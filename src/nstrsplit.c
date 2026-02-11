/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nstrsplit.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adippena <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/05/15 11:30:36 by adippena          #+#    #+#             */
/*   Updated: 2016/07/16 17:00:35 by adippena         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

static char		*strsub(char const *s, unsigned int start, size_t len)
{
	char	*str;
	size_t	i;

	i = 0;
	if (s == NULL)
		return (NULL);
	str = (char *)malloc(len + 1);
	if (str == NULL)
		return (NULL);
	while (i < len && s[i])
	{
		str[i] = s[start + i];
		i++;
	}
	while (i < len)
	{
		str[i] = '\0';
		i++;
	}
	str[i] = '\0';
	return (str);
}

static int		count_words(const char *s, char c)
{
	size_t	words;

	words = 0;
	while (*s)
	{
		while (*s && *s == c)
			s++;
		if (*s)
		{
			words++;
			while (*s && *s != c)
				s++;
		}
	}
	return (words);
}

static char		*next_word(const char *s, char c, size_t *len)
{
	*len = 0;
	while (*s && *s == c)
		s++;
	while (s[*len] && s[*len] != c)
		(*len)++;
	return ((char *)s);
}

t_split_string	nstrsplit(char const *s, char c)
{
	t_split_string	split;
	size_t			wordl;
	size_t			i;

	i = 0;
	split.words = count_words(s, c);
	split.strings = (char **)malloc(sizeof(char *) * split.words);
	if (split.strings == NULL)
	{
		split.words = -1;
		return (split);
	}
	while (i < split.words)
	{
		s = next_word(s, c, &wordl);
		split.strings[i] = strsub(s, 0, wordl);
		if (split.strings[i] == NULL)
		{
			split.words = -1;
			return (split);
		}
		s += wordl;
		i++;
	}
	return (split);
}

void		free_split(t_split_string *split)
{
	size_t	i;

	i = 0;
	while (i < split->words)
	{
		free(split->strings[i]);
		split->strings[i] = NULL;
		i++;
	}
	if (split->strings)
		free(split->strings);
	split->words = 0;
}
