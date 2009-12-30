/*
** map.h
**
** Gestion des cartes.
**
*/

#ifndef MAP_H
# define MAP_H

# define TILE_WIDTH  32
# define TILE_HEIGHT 32

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <SDL/SDL.h>
# include <FMOD/fmod.h>
# include <FMOD/fmod_errors.h>

# define GET_TILE(x,y,map) map->data[x+y*map->width]

struct s_chipset
{
  unsigned int x;
  unsigned int y;
  
  unsigned char collision;
  
  unsigned int nextTile;
};

struct s_map
{
   unsigned int n_tile;
   struct s_chipset *chipset;
   SDL_Surface *surf_chipset;

   unsigned int width;
   unsigned int height;
   unsigned int *data;
   
   unsigned int offsetX;
   unsigned int offsetY;
   
   FMUSIC_MODULE *bg_sound;
};


void InitMap(struct s_map *map);
void RandomizeMap(struct s_map *map, int width, int height, char *chipset);
void HandleMap(struct s_map *map);
void MapEvent(SDL_Event *event, struct s_map *map);

void FreeMap(struct s_map *map);
void LoadMap(struct s_map *map, char *filename);
void SaveMap(struct s_map *map, char *filename, char *bg_sound, char *chipset);



#endif /* !MAP_H */
