#include "lotr.h"
#include "character.h"
#include "init.h"
#include "map.h"
#include "object.h"
#include "spot.h"
#include "game.h"
#include "utils.h"

#include <SDL.h>

// game.c
extern int game_maps[][4];
// map.c
#define TILESIZE 8
extern int map_width;
extern int map_height;
// graphics.c
extern Uint8 *main_screen;
extern int screen_width;
extern int screen_height;
extern int window_w;
extern int window_h;
// lotr_sdl.c
extern SDL_Color *active_palette;

int
main(void)
{
    int i;
#ifndef TTT
    int max_map = 13;
#else
    int max_map = 15;
#endif

    int largetilesize = 4 * 4 * TILESIZE;
#if PIXEL_PRECISE
    int border_size = 16;
#else
    int border_size = 17;
#endif

    map_width = MAP_WIDTH;                      // large tiles
    map_height = MAP_HEIGHT;                    // large tiles
    screen_width = map_width * largetilesize + border_size;   // pixels
    screen_height = map_height * largetilesize + border_size; // pixels
    lotr_init();
    window_w = screen_width;
    window_h = screen_height;

    for (i = 0; i <= max_map; ++i) {
        char name[64];
        SDL_Surface *surf;

        if (!game_maps[i][0])
            continue;

        game_load_map(i);

        sprintf(name, "map%02d.bmp", i);

        map_display(0, 0);

        surf = SDL_CreateRGBSurfaceFrom(main_screen,
            screen_width, screen_height, 8, screen_width, 0, 0, 0, 0);
        SDL_SetPalette(surf, SDL_LOGPAL, active_palette, 0, 256);

        SDL_SaveBMP(surf, name);
        SDL_FreeSurface(surf);
    }

    return 0;
}
