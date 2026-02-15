/*
** strtrim.c -- Whitespace trimming utility.
**
** Strips leading and trailing whitespace (spaces, tabs, newlines) from a
** string. Returns a new heap-allocated copy with the trimmed content.
** Replaces the former libft ft_strtrim function.
**
** Example: strtrim("  \thello world\n  ") -> "hello world"
*/

#include "rt.h"

/*
** Trim leading and trailing whitespace from string 's'.
** Finds the first and last non-whitespace positions, then copies that
** range into a new calloc'd string. Returns an empty string if the
** input is all whitespace.
*/
char	*strtrim(char const *s)
{
	char	*new_str;
	size_t	start;
	size_t	end;
	size_t	i;

	i = 0;
	start = 0;
	if ((end = strlen(s)))
	{
		while (s[start] == ' ' || s[start] == '\n' || s[start] == '\t')
			start++;
		while (s[end - 1] == ' ' || s[end - 1] == '\n' || s[end - 1] == '\t')
			end--;
	}
	new_str = calloc((end - start) + 1, sizeof(char));
	while (start < end)
		new_str[i++] = s[start++];
	return (new_str);
}
