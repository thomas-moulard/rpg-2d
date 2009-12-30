/*
** sprite.h
**
** Gestion des sprites
**
** Rôle:
** - initialisation et libération des sprites.
*/

#ifndef SPRITE_H
# define SPRITE_H

# include <stdlib.h>
# include <SDL/SDL.h>

# include "map.h"

# define ANIMATION_HAUT   0
# define ANIMATION_DROITE 1
# define ANIMATION_GAUCHE 3
# define ANIMATION_BAS    2



struct s_lib_sprite
{
   unsigned int n_sprite;
   struct s_sprite *sprites;
};

struct s_frame
{
   unsigned int x,y;
};

struct s_anim
{
   unsigned int n_frame;
   unsigned int cur_frame;
   unsigned int lastfrmupdate;
   unsigned int delaybetweenframes;
   struct s_frame *frame;
};

struct s_sprite
{
   unsigned int x,y;
   
   unsigned int width, height;
   
   unsigned int n_anim;
   unsigned int cur_anim;
   unsigned char ispaused_anim;
   struct s_anim *anim;
   
   unsigned int offsetG, offsetD, offsetH, offsetB;
   
   SDL_Surface *surf;
};

void InitSprite(struct s_lib_sprite *libsprite);
void FreeSprite(struct s_lib_sprite *libsprite);
void HandleSprites(int ispaused, struct s_lib_sprite *libsprite, struct s_map *map);
void HandleSprite(struct s_sprite *sprite, struct s_map *map);

void SpriteChangeAnim(struct s_sprite *sprite, unsigned int anim, unsigned char ispaused);

void SpriteEvent(SDL_Event *event, struct s_map *map, struct s_lib_sprite *libsprite);

void SpriteMove(struct s_sprite *sprite, unsigned int x, unsigned int y, struct s_map *map);

void LoadSprite(struct s_lib_sprite *libsprite, char *filename);
void SaveSprite(struct s_lib_sprite *libsprite, char *charset, char *filename);
void SpriteStaticData(struct s_lib_sprite *libsprite);

#endif /* !SPRITE_H_ */
