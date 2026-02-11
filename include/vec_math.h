#ifndef VEC_MATH_H
# define VEC_MATH_H

# include <math.h>

static inline t_vector	vadd(t_vector v1, t_vector v2)
{
	return ((t_vector){v1.x + v2.x, v1.y + v2.y, v1.z + v2.z});
}

static inline t_vector	vsub(t_vector v1, t_vector v2)
{
	return ((t_vector){v1.x - v2.x, v1.y - v2.y, v1.z - v2.z});
}

static inline t_vector	vmult(t_vector v, double s)
{
	return ((t_vector){v.x * s, v.y * s, v.z * s});
}

static inline t_vector	vdiv(t_vector v, double d)
{
	return ((t_vector){v.x / d, v.y / d, v.z / d});
}

static inline double	vdot(t_vector v1, t_vector v2)
{
	return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
}

static inline t_vector	vcross(t_vector v1, t_vector v2)
{
	return ((t_vector){
		v1.y * v2.z - v1.z * v2.y,
		v1.z * v2.x - v1.x * v2.z,
		v1.x * v2.y - v1.y * v2.x});
}

static inline double	vnormalize(t_vector v)
{
	return (sqrt(v.x * v.x + v.y * v.y + v.z * v.z));
}

static inline t_vector	vunit(t_vector v)
{
	return (vdiv(v, vnormalize(v)));
}

static inline t_vector	vproject(t_vector a, t_vector b)
{
	return (vmult(b, (vdot(a, b) / vdot(b, b))));
}

static inline int	vcomp(t_vector v1, t_vector v2)
{
	if (v1.x == v2.x && v1.y == v2.y && v1.z == v2.z)
		return (0);
	else if (v1.x >= v2.x && v1.y >= v2.y && v1.z >= v2.z)
		return (1);
	else if (v1.x <= v2.x && v1.y <= v2.y && v1.z <= v2.z)
		return (-1);
	return (-2);
}

static inline t_vector	colour_to_vector(t_colour colour)
{
	return ((t_vector){colour.r, colour.g, colour.b});
}

static inline t_vector	vrotx(t_vector v, double angle)
{
	return ((t_vector){
		v.x,
		v.y * cos(angle) - v.z * sin(angle),
		v.y * sin(angle) + v.z * cos(angle)});
}

static inline t_vector	vroty(t_vector v, double angle)
{
	return ((t_vector){
		v.x * cos(angle) + v.z * sin(angle),
		v.y,
		v.z * cos(angle) + v.x * sin(angle)});
}

static inline t_vector	vrotz(t_vector v, double angle)
{
	return ((t_vector){
		v.x * cos(angle) - v.y * sin(angle),
		v.x * sin(angle) - v.y * cos(angle),
		v.z});
}

static inline t_vector	vrot(t_vector v, double angle)
{
	t_vector	res;

	res = vrotx(v, angle);
	res = vroty(res, angle);
	res = vrotz(res, angle);
	return (res);
}

static inline t_vector	vneg(t_vector v)
{
	return ((t_vector){-v.x, -v.y, -v.z});
}

static inline double	ipow50(double x)
{
	double	x2;
	double	x4;
	double	x8;
	double	x16;
	double	x32;

	x2 = x * x;
	x4 = x2 * x2;
	x8 = x4 * x4;
	x16 = x8 * x8;
	x32 = x16 * x16;
	return (x32 * x16 * x2);
}

#endif
