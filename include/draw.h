/*
** draw.h -- Data structures for chunk-based multithreaded rendering.
**
** The renderer divides the image into 64x64 pixel tiles ("chunks"). Each
** chunk is rendered by its own pthread, operating on an independent copy
** of the environment (t_env). This tile-based approach avoids lock
** contention entirely: threads never share mutable state during rendering.
**
** t_chunk   -- Per-thread work unit: holds the tile bounds, a pointer to
**              the pixel buffer, and a private copy of the scene environment.
** t_make_chunks -- Bookkeeping for spawning all chunk threads: tracks
**              pthread IDs and the grid position of the next tile to create.
*/

#ifndef DRAW_H
# define DRAW_H

# include "rt.h"
# include <sys/time.h>

/*
** t_chunk: Represents a single 64x64 pixel tile assigned to one thread.
**
** e     -- Thread-private copy of the full environment (scene, camera, etc.).
**          Each thread gets its own copy so no synchronization is needed.
** d     -- SDL_Rect defining the tile: (x, y) is the top-left corner in
**          pixel coords, (w, h) is the tile size (usually 64x64, smaller
**          at image edges).
** px    -- Pointer to the shared pixel buffer (the SDL surface's pixels).
**          Each thread writes only to its own tile region, so no conflicts.
** pixel -- (Unused/legacy field.)
** stopx -- Right edge of this tile, clamped to image width.
** stopy -- Bottom edge of this tile, clamped to image height.
** x     -- Current x pixel position during iteration within the tile.
*/
typedef struct	s_chunk
{
	t_env			*e;
	SDL_Rect		d;
	uint32_t		*px;
	size_t			pixel;
	int				stopx;
	int				stopy;
	int				x;
}				t_chunk;

/*
** t_make_chunks: State for the chunk-spawning loop.
**
** tids    -- Total number of tiles (and therefore threads) to spawn.
**            Computed as ceil(width/64) * ceil(height/64).
** thread  -- Index of the next thread to create / join.
** chunk_x -- Current tile column index in the grid (0-based).
** chunk_y -- Current tile row index in the grid (0-based).
** tid     -- Dynamically allocated array of pthread_t IDs, one per tile.
** c       -- Temporary pointer to the current t_chunk being constructed.
*/
typedef struct	s_make_chunks
{
	size_t			tids;
	size_t			thread;
	size_t			chunk_x;
	size_t			chunk_y;
	pthread_t		*tid;
	t_chunk			*c;
}				t_make_chunks;

#endif
