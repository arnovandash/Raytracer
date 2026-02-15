/*
** read_vector.c -- Parse 3D vector values from scene file strings.
**
** Provides two parsing functions for reading 3D vectors from scene files:
**
**   get_vector      - Parses "x y z" into a t_vector as-is. Used for
**                     positions (LOC) where the actual coordinates matter.
**
**   get_unit_vector - Parses "x y z" and normalizes the result to unit
**                     length. Used for directions (DIR, NORMAL) where only
**                     the orientation matters, not the magnitude.
**                     Normalization divides each component by the vector's
**                     length: v_unit = v / |v|, ensuring |v_unit| = 1.
**                     This is important because many intersection and
**                     shading formulas assume unit-length direction vectors
**                     (e.g., dot(N, L) directly gives the cosine of the
**                     angle between them only if both are unit vectors).
**
** Both functions expect exactly 3 space-separated values and will
** call err() if the input format is wrong.
*/

#include "rt.h"

/*
** get_vector -- Parse 3 space-separated doubles into a t_vector.
**
** Parameters:
**   e      - Environment (used only for error reporting).
**   values - Split string with exactly 3 words: x, y, z components.
**
** Returns the raw (unnormalized) vector.
*/
t_vector	get_vector(t_env *e, t_split_string values)
{
	if (values.words != 3)
		err(FILE_FORMAT_ERROR, "Invalid vector format", e);
	return ((t_vector){
		atof(values.strings[0]),
		atof(values.strings[1]),
		atof(values.strings[2])});
}

/*
** get_unit_vector -- Parse 3 doubles into a normalized (unit-length) vector.
**
** Calls vunit() to normalize: result = v / sqrt(v.x^2 + v.y^2 + v.z^2).
** This guarantees |result| = 1.0, which is required by intersection
** algorithms that rely on dot products for geometric calculations.
*/
t_vector	get_unit_vector(t_env *e, t_split_string values)
{
	if (values.words != 3)
		err(FILE_FORMAT_ERROR, "Invalid vector format", e);
	return (vunit((t_vector){
		atof(values.strings[0]),
		atof(values.strings[1]),
		atof(values.strings[2])}));
}
