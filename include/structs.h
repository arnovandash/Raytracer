/*
** structs.h -- All data structures for the raytracer.
**
** This header defines every struct used in the engine. The central design
** revolves around t_env, the master environment struct that holds ALL
** state: the SDL window, pixel buffers, camera, scene geometry (primitives
** and mesh objects), lights, materials, and render settings.
**
** Naming convention: all structs use the t_ prefix (42-school norm).
*/

#ifndef STRUCTS_H
# define STRUCTS_H

/*
** t_vector -- 3D point or direction vector.
** Used interchangeably for positions (points in space) and directions
** (unit vectors, ray directions, normals). The distinction is contextual.
** All coordinates are doubles for floating-point precision in ray math.
*/
typedef struct	s_vector
{
	double	x;
	double	y;
	double	z;
}				t_vector;

/*
** t_colour -- RGB color with intensity weight.
** Components r, g, b are in the range [0.0, 1.0].
** 'intensity' acts as a weighting factor for the material: it controls
** how strongly this color contributes (e.g., a material's diffuse color
** might have intensity 0.8, meaning 80% diffuse contribution).
*/
typedef struct	s_colour
{
	double	r;
	double	g;
	double	b;
	double	intensity;
}				t_colour;

/*
** t_material -- Surface material properties.
** Controls how a surface interacts with light:
**   - name:    identifier string (referenced by primitives in scene file)
**   - reflect: mirror reflectivity [0.0 = matte, 1.0 = perfect mirror]
**   - refract: transparency [0.0 = opaque, 1.0 = fully transparent]
**   - ior:     index of refraction (e.g., air=1.0, glass=1.5, water=1.33).
**              Controls how much light bends when entering the material.
**   - diff:    diffuse color and its intensity weight
**   - spec:    specular highlight color and its intensity weight
*/
typedef struct	s_material
{
	char		*name;
	double		reflect;
	double		refract;
	double		ior;
	t_colour	diff;
	t_colour	spec;
}				t_material;

/*
** t_face -- A single triangle face from an OBJ mesh.
** Stores pointers to three vertices (v0, v1, v2) and one face normal (n).
** Vertices point into the parent t_object's vertex array, so they are
** shared (not duplicated) across faces that reference the same vertex.
*/
typedef struct	s_face
{
	t_vector	*v0;
	t_vector	*v1;
	t_vector	*v2;
	t_vector	*n;
}				t_face;

/*
** t_object -- A mesh object loaded from an OBJ file.
**   - name:      object name from the OBJ file
**   - face:      array of triangle face pointers
**   - faces:     number of faces
**   - material:  index into the global materials array
**   - v:         array of vertex position pointers
**   - verticies: number of vertices (note: original spelling preserved)
**   - vn:        array of vertex normal pointers
**   - vnormals:  number of vertex normals
**   - box[2]:    axis-aligned bounding box (AABB) as two corner points
**                [0] = min corner, [1] = max corner. Used for fast
**                rejection -- if a ray misses the box, skip all faces.
*/
typedef struct	s_object
{
	char		*name;
	t_face		**face;
	size_t		faces;
	size_t		material;
	t_vector	**v;
	size_t		verticies;
	t_vector	**vn;
	size_t		vnormals;
	t_vector	box[2];
}				t_object;

/*
** t_prim -- A geometric primitive (sphere, plane, cylinder, cone, etc.).
**   - loc:       position (center for sphere, point-on-plane for plane, etc.)
**   - dir:       axis direction (used for cylinders, cones, disks)
**   - normal:    surface normal (for planes; computed at hit point for others)
**   - loc_bak:   backup of original position (for grab/undo operations)
**   - type:      primitive type ID (PRIM_SPHERE, PRIM_PLANE, etc. from defines.h)
**   - s_bool:    selection state (non-zero = selected for grab mode)
**   - material:  index into the global materials array
**   - radius:    sphere/cylinder/cone radius
**   - angle:     cone half-angle in radians
**   - cos_angle: precomputed cos(angle) -- avoids recomputing during intersection
**   - sin_angle: precomputed sin(angle)
**   - limit:     height limit for finite cylinders/cones (INFINITY if unlimited)
*/
typedef struct	s_prim
{
	t_vector	loc;
	t_vector	dir;
	t_vector	normal;
	t_vector	loc_bak;
	int			type;
	int			s_bool;
	size_t		material;
	double		radius;
	double		angle;
	double		cos_angle;
	double		sin_angle;
	double		limit;
}				t_prim;

/*
** t_ray -- A ray for tracing through the scene.
**   - inter: intersection result flag:
**            0 = no intersection (miss)
**            1 = hit front face (entering object)
**            2 = hit from inside (exiting object, relevant for refraction)
**   - ior:   index of refraction of the medium the ray is currently in
**            (starts at 1.0 for air; changes when entering/exiting glass etc.)
**   - loc:   ray origin point
**   - dir:   ray direction vector (should be unit length)
**   - o_in:  pointer to the mesh object the ray is currently inside of
**            (NULL if in open air). Used for refraction tracking.
*/
typedef struct	s_ray
{
	int			inter;
	double		ior;
	t_vector	loc;
	t_vector	dir;
	t_object	*o_in;
}				t_ray;

/*
** t_camera -- Virtual camera defining the viewpoint.
**   - loc:   camera position in world space
**   - dir:   look-at target point (NOT a direction vector)
**   - up:    world up vector (typically 0,0,1)
**   - u:     camera right vector (part of orthonormal basis)
**   - v:     camera up vector (part of orthonormal basis, may differ from 'up')
**   - l:     lower-left corner of the image plane in world space
**   - stepx: horizontal distance between adjacent pixels on the image plane
**   - stepy: vertical distance between adjacent pixels on the image plane
**   - a:     aperture radius for depth-of-field effect (0 = pinhole camera)
*/
typedef struct	s_camera
{
	t_vector	loc;
	t_vector	dir;
	t_vector	up;
	t_vector	u;
	t_vector	v;
	t_vector	l;
	double		stepx;
	double		stepy;
	double		a;
}				t_camera;

/*
** t_light -- A point light source.
**   - loc:    position in world space
**   - colour: light color (r,g,b each in [0,1])
**   - lm:     lumens -- brightness/intensity of the light
**   - half:   half-distance for attenuation falloff. At this distance
**             from the light, intensity drops to 50%. Implements inverse
**             square-like falloff for realistic lighting.
*/
typedef struct	s_light
{
	t_vector	loc;
	t_colour	colour;
	double		lm;
	double		half;
}				t_light;

/*
** t_stats -- Global performance counters using C11 atomics.
** Incremented by multiple render threads concurrently. _Atomic ensures
** thread-safe updates without explicit mutexes (hardware atomic operations).
** Useful for profiling: how many rays were cast, how many intersection
** tests performed, etc.
*/
typedef struct	s_stats
{
	_Atomic size_t	rays;
	_Atomic size_t	primary_rays;
	_Atomic size_t	reflection_rays;
	_Atomic size_t	refraction_rays;
	_Atomic size_t	shadow_rays;
	_Atomic size_t	intersection_tests;
	_Atomic size_t	threads;
}				t_stats;

/*
** t_thread_stats -- Per-thread performance counters (non-atomic).
** Each thread accumulates stats locally in _Thread_local storage,
** avoiding the overhead of atomic operations on every single ray.
** These can be aggregated into g_stats at the end of a render pass.
*/
typedef struct	s_thread_stats
{
	size_t	rays;
	size_t	primary_rays;
	size_t	reflection_rays;
	size_t	refraction_rays;
	size_t	shadow_rays;
	size_t	intersection_tests;
}				t_thread_stats;

/*
** t_env -- Master environment struct holding ALL application state.
**
** SDL state:
**   - win:      SDL window handle
**   - win_img:  SDL surface bound to the window (for blitting)
**   - img:      offscreen render target surface (32-bit ARGB)
**   - dof:      depth-of-field accumulation surface
**   - px:       direct pointer to img's pixel data as uint32_t array
**   - dx:       direct pointer to dof's pixel data as uint32_t array
**
** Scene data (arrays of pointers, dynamically allocated):
**   - prim/prims:         geometric primitives and count
**   - object/objects:     OBJ mesh objects and count
**   - light/lights:       light sources and count
**   - material/materials: materials and count
**
** Current ray state (per-thread via copy_env):
**   - ray:        the current ray being traced
**   - t:          nearest intersection distance (starts at INFINITY)
**   - p_hit:      pointer to the closest primitive hit
**   - o_hit:      pointer to the closest mesh face hit
**   - object_hit: pointer to the mesh object owning o_hit
**   - hit_type:   whether the hit was a primitive (PRIMITIVE) or face (FACE)
**   - s_num:      selected primitive index (for grab mode)
**
** Render settings:
**   - maxdepth:  maximum recursion depth for reflection/refraction rays
**   - super:     number of depth-of-field supersamples (0 = disabled)
**   - flags:     bitmask of active key/mode flags (KEY_G, KEY_MID_CLICK, etc.)
**   - x, y:      window/image dimensions in pixels
*/
typedef struct	s_env
{
	SDL_Window		*win;
	SDL_Surface		*win_img;
	SDL_Surface		*img;
	SDL_Surface		*dof;
	uint32_t		*px;
	uint32_t		*dx;
	char			*file_name;
	t_ray			ray;
	t_camera		camera;
	t_prim			*p_hit;
	size_t			s_num;
	size_t			hit_type;
	t_prim			**prim;
	size_t			prims;
	t_face			*o_hit;
	t_object		*object_hit;
	t_object		**object;
	size_t			objects;
	t_light			**light;
	size_t			lights;
	t_material		**material;
	size_t			materials;
	double			t;
	int				maxdepth;
	size_t			super;
	size_t			flags;
	size_t			x;
	size_t			y;
}				t_env;

/*
** t_quad -- Coefficients for a quadratic equation: ax^2 + bx + c = 0.
** Used in ray-sphere, ray-cylinder, and ray-cone intersection tests.
** The discriminant (discr = b^2 - 4ac) determines the number of
** intersections: negative = miss, zero = tangent, positive = two hits.
*/
typedef struct	s_quadratic
{
	double	a;
	double	b;
	double	c;
	double	discr;
}				t_quad;

#endif
