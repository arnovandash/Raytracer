/*
** nstrsplit.c -- Custom string splitting utility.
**
** Splits a string by a single delimiter character into an array of
** substrings. Replaces the former libft ft_nstrsplit function.
**
** The result is returned as a t_split_string containing the array of
** heap-allocated substring copies and the word count. Consecutive
** delimiters are treated as a single separator (no empty strings).
**
** Example: nstrsplit("  hello   world  ", ' ')
**   -> { strings: ["hello", "world"], words: 2 }
*/

#include "rt.h"

/*
** Extract a substring of 'len' characters starting at position 'start'.
** Returns a new heap-allocated string. Pads with null bytes if the
** source string is shorter than 'len'.
*/
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

/*
** Count the number of delimiter-separated tokens in the string.
** Skips runs of delimiters, then counts each non-delimiter run as one word.
*/
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

/*
** Advance past any leading delimiters and return a pointer to the start
** of the next word. Sets *len to the number of non-delimiter characters
** in that word.
*/
static char		*next_word(const char *s, char c, size_t *len)
{
	*len = 0;
	while (*s && *s == c)
		s++;
	while (s[*len] && s[*len] != c)
		(*len)++;
	return ((char *)s);
}

/*
** Split string 's' by delimiter 'c' into an array of substrings.
** Returns a t_split_string with the array and word count.
** On allocation failure, words is set to (size_t)-1 as an error signal.
*/
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

/* Free all strings in the split result and the array itself. */
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
