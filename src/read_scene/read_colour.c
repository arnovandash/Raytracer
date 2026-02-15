/*
** read_colour.c -- Hex color string parser.
**
** Converts scene file color specifications from human-readable hex format
** "RRGGBB" (like HTML colors) into the internal t_colour struct with
** normalized [0,1] double-precision RGB channels.
**
** Format: "RRGGBB [intensity]"
**   - RRGGBB is a 6-character hexadecimal string (e.g., "FF8000" = orange).
**   - The optional intensity value (0.0 to 1.0) controls the strength of
**     the color component (diffuse intensity, specular intensity, etc.).
**     Defaults to 1.0 if omitted.
**
** Conversion process:
**   1. Extract 2-character hex substrings for R, G, B.
**   2. Parse each with strtol (base 16) to get integer 0-255.
**   3. Divide by 255.0 to normalize to the [0,1] range used internally
**      by the shading calculations.
**
** Normalized colors simplify the rendering math -- multiplying two colors
** is just component-wise multiplication of values in [0,1], and clamping
** the final result to [0,1] maps directly to the 0-255 output range.
*/

#include "rt.h"

/*
** to_range -- Clamp a value to the interval [min, max].
*/
static double	to_range(double value, double min, double max)
{
	value = (value < min) ? min : value;
	value = (value > max) ? max : value;
	return (value);
}

/*
** get_colour -- Parse a hex color string with optional intensity.
**
** Parameters:
**   e      - Environment (used only for error reporting).
**   values - Split string: values.strings[0] = "RRGGBB",
**            values.strings[1] = intensity (optional).
**
** Returns a t_colour with r, g, b in [0,1] and intensity in [0,1].
**
** The parsing uses a 3-byte temp buffer, copying 2 hex chars at a time
** and converting with strtol. The temp buffer is null-terminated by calloc.
*/
t_colour		get_colour(t_env *e, t_split_string values)
{
	t_colour	res;
	char		*temp;

	res = (t_colour){0.0, 0.0, 0.0, 1.0};
	if (values.words == 1 || values.words == 2)
	{
		if (strlen(values.strings[0]) != 6)
			err(FILE_FORMAT_ERROR, "Colour has invalid format", e);
		temp = calloc(3, sizeof(char));
		/* Extract red channel: first 2 hex digits */
		temp = strncpy(temp, values.strings[0], 2);
		res.r = (int)strtol(temp, NULL, 16) / 255.0;
		/* Extract green channel: middle 2 hex digits */
		temp = strncpy(temp, values.strings[0] + 2, 2);
		res.g = (int)strtol(temp, NULL, 16) / 255.0;
		/* Extract blue channel: last 2 hex digits */
		temp = strncpy(temp, values.strings[0] + 4, 2);
		res.b = (int)strtol(temp, NULL, 16) / 255.0;
		free(temp);
	}
	if (values.words == 2)
		res.intensity = to_range(atof(values.strings[1]), 0.0, 1.0);
	if (values.words > 2)
		err(FILE_FORMAT_ERROR, "Invalid colour", e);
	return (res);
}
