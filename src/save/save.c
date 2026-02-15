/*
** save.c -- Scene serialization orchestrator
**
** Writes the entire scene state back to the original scene file in the same
** tab-indented declarative format that the parser reads. This allows the user
** to interactively modify a scene (move objects, adjust camera) and then
** persist those changes.
**
** The file is opened with O_TRUNC to clear existing contents before writing.
** Write order matches the parser's expected format:
**   1. Header comment (# SCENE RT)
**   2. Global settings: MAXDEPTH, RENDER, SUPER
**   3. CAMERA block
**   4. LIGHT blocks
**   5. MATERIAL blocks
**   6. PRIMITIVE blocks
**   7. OBJECT blocks (OBJ mesh references)
**
** All output uses dprintf() to write directly to the file descriptor,
** avoiding the need for intermediate string buffers.
*/

#include "rt.h"

/*
** Serializes the camera block: position (LOC), look-at target (DIR),
** up vector (UP), and aperture size for depth-of-field effects.
*/
static void	save_camra(t_camera *cam, int fd)
{
	dprintf(fd, "\tCAMERA\n");
	dprintf(fd, "\t\tLOC\t\t\t");
	write_coord(cam->loc, fd);
	dprintf(fd, "\t\tDIR\t\t\t");
	write_coord(cam->dir, fd);
	dprintf(fd, "\t\tUP\t\t\t");
	write_coord(cam->up, fd);
	dprintf(fd, "\t\tAPERTURE\t%lf\n", cam->a);
}

/*
** Writes render resolution (width height) and supersampling level.
** SUPER controls depth-of-field sample count for anti-aliasing.
*/
static void	save_render(t_env *e, int fd)
{
	dprintf(fd, "\tRENDER\t\t%zu %zu\n", e->x, e->y);
	dprintf(fd, "\tSUPER\t\t%zu\n", e->super);
}

/*
** Main save entry point. Opens the original scene file (truncating it),
** writes all scene data in order, and closes the file.
** Delegates to specialized functions for each block type.
*/
void		save(t_env *e)
{
	int		fd;

	printf("Saving file... ");
	if ((fd = open(e->file_name, O_WRONLY | O_TRUNC)) == -1)
		err(FILE_OPEN_ERROR, "Could not save the file", e);
	dprintf(fd, "# SCENE RT\n");
	dprintf(fd, "\tMAXDEPTH\t%d\n", e->maxdepth);
	save_render(e, fd);
	dprintf(fd, "\n");
	save_camra(&e->camera, fd);
	save_lights(e->light, e->lights, fd);
	save_materials(e->material, e->materials, fd);
	save_prims(e->prim, e->material, e->prims, fd);
	save_objects(e->object, e->objects, e->material, fd);
	close(fd);
	printf("Done\n");
}
