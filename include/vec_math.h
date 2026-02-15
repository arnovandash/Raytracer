/*
** vec_math.h -- Inline 3D vector math library.
**
** All functions are declared static inline for zero-overhead calls (the
** compiler inlines them at every call site, avoiding function-call overhead
** on the hot rendering path where millions of vector operations occur).
**
** Mathematical conventions:
**   - Vectors represent both 3D points and directions (no distinction).
**   - Dot product (vdot) measures alignment: positive = same direction,
**     zero = perpendicular, negative = opposing. Used in lighting, normals,
**     and intersection math.
**   - Cross product (vcross) produces a vector perpendicular to both inputs.
**     Used for computing surface normals and building orthonormal bases.
**
** NOTE: vnormalize() is misnamed -- it returns the vector LENGTH (magnitude),
** not a normalized vector. vunit() is the actual normalization function.
**
** NOTE: vroty() has a sign error. The standard Y-axis rotation matrix is:
**     [ cos(a)  0  sin(a)]        but this code uses:
**     [   0     1    0   ]        [cos(a)  0  sin(a)]
**     [-sin(a)  0  cos(a)]        [  0     1    0   ]
**                                  [sin(a)  0  cos(a)]  <- should be -sin(a)
*/

#ifndef VEC_MATH_H
# define VEC_MATH_H

# include <math.h>

/* Vector addition: v1 + v2 (component-wise). */
static inline t_vector	vadd(t_vector v1, t_vector v2)
{
	return ((t_vector){v1.x + v2.x, v1.y + v2.y, v1.z + v2.z});
}

/* Vector subtraction: v1 - v2 (component-wise). */
static inline t_vector	vsub(t_vector v1, t_vector v2)
{
	return ((t_vector){v1.x - v2.x, v1.y - v2.y, v1.z - v2.z});
}

/* Scalar multiplication: v * s (scales each component). */
static inline t_vector	vmult(t_vector v, double s)
{
	return ((t_vector){v.x * s, v.y * s, v.z * s});
}

/* Scalar division: v / d (divides each component). */
static inline t_vector	vdiv(t_vector v, double d)
{
	return ((t_vector){v.x / d, v.y / d, v.z / d});
}

/*
** Dot product: v1 . v2 = v1.x*v2.x + v1.y*v2.y + v1.z*v2.z
** Returns a scalar measuring how aligned two vectors are.
** Used extensively in lighting calculations (Lambert's cosine law),
** intersection tests, and reflection/refraction formulas.
*/
static inline double	vdot(t_vector v1, t_vector v2)
{
	return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
}

/*
** Cross product: v1 x v2
** Returns a vector perpendicular to both v1 and v2 (right-hand rule).
** Magnitude equals |v1|*|v2|*sin(angle). Used for computing surface
** normals from triangle edges and building camera orthonormal bases.
*/
static inline t_vector	vcross(t_vector v1, t_vector v2)
{
	return ((t_vector){
		v1.y * v2.z - v1.z * v2.y,
		v1.z * v2.x - v1.x * v2.z,
		v1.x * v2.y - v1.y * v2.x});
}

/*
** Vector length (magnitude): |v| = sqrt(x^2 + y^2 + z^2)
** MISNAMED: this computes the length, not normalization. See vunit().
** Uses the Pythagorean theorem extended to 3D.
*/
static inline double	vnormalize(t_vector v)
{
	return (sqrt(v.x * v.x + v.y * v.y + v.z * v.z));
}

/*
** Unit vector (normalize): v / |v|
** Returns a vector with the same direction but length = 1.
** Essential for direction vectors in ray tracing -- many formulas
** assume unit-length input (dot product = cosine of angle, etc.).
*/
static inline t_vector	vunit(t_vector v)
{
	return (vdiv(v, vnormalize(v)));
}

/*
** Vector projection: project vector 'a' onto vector 'b'.
** Formula: proj_b(a) = b * (a.b / b.b)
** Returns the component of 'a' that lies along 'b'.
*/
static inline t_vector	vproject(t_vector a, t_vector b)
{
	return (vmult(b, (vdot(a, b) / vdot(b, b))));
}

/*
** Component-wise vector comparison.
** Returns:  0 if equal, 1 if v1 >= v2 in all components,
**          -1 if v1 <= v2 in all components, -2 otherwise (mixed).
** Used for AABB (axis-aligned bounding box) containment tests.
*/
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

/* Convert a t_colour (r,g,b) to a t_vector (x,y,z) for arithmetic. */
static inline t_vector	colour_to_vector(t_colour colour)
{
	return ((t_vector){colour.r, colour.g, colour.b});
}

/*
** Rotation around the X axis by 'angle' radians.
** Matrix: [1    0       0    ]
**         [0  cos(a) -sin(a) ]
**         [0  sin(a)  cos(a) ]
*/
static inline t_vector	vrotx(t_vector v, double angle)
{
	return ((t_vector){
		v.x,
		v.y * cos(angle) - v.z * sin(angle),
		v.y * sin(angle) + v.z * cos(angle)});
}

/*
** Rotation around the Y axis by 'angle' radians.
** NOTE: Non-standard sign -- the z component uses +sin instead of -sin.
** Standard matrix would be: [ cos(a)  0  sin(a)]
**                            [   0     1    0   ]
**                            [-sin(a)  0  cos(a)]
*/
static inline t_vector	vroty(t_vector v, double angle)
{
	return ((t_vector){
		v.x * cos(angle) + v.z * sin(angle),
		v.y,
		v.z * cos(angle) + v.x * sin(angle)});
}

/*
** Rotation around the Z axis by 'angle' radians.
** Matrix: [cos(a) -sin(a)  0]
**         [sin(a)  cos(a)  0]  <- NOTE: code uses -cos, not +cos for v.y
**         [  0       0     1]
*/
static inline t_vector	vrotz(t_vector v, double angle)
{
	return ((t_vector){
		v.x * cos(angle) - v.y * sin(angle),
		v.x * sin(angle) - v.y * cos(angle),
		v.z});
}

/*
** Apply rotation around all three axes sequentially (X, then Y, then Z).
** Note: this is NOT a proper Euler rotation -- the result depends on the
** order of application, and using the same angle for all three axes is
** unusual. This function is used for simple interactive rotation only.
*/
static inline t_vector	vrot(t_vector v, double angle)
{
	t_vector	res;

	res = vrotx(v, angle);
	res = vroty(res, angle);
	res = vrotz(res, angle);
	return (res);
}

/* Vector negation: returns -v (flips direction). */
static inline t_vector	vneg(t_vector v)
{
	return ((t_vector){-v.x, -v.y, -v.z});
}

/*
** Fast integer power: x^50 via repeated squaring.
** Decomposition: 50 = 32 + 16 + 2, so x^50 = x^32 * x^16 * x^2.
** Only 6 multiplications instead of 49 with naive iteration.
** Used for specular highlights in Blinn-Phong shading -- the high
** exponent (50) creates a tight, bright specular spot.
*/
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
