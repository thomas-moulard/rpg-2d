/*
** map.c
**
** Gestion des cartes.
**
*/
#include "map.h"

void InitMap(struct s_map *map)
{
     map->n_tile=map->width=map->height=map->offsetX=map->offsetY=0;
     map->chipset=NULL;
     map->surf_chipset=NULL;
     map->bg_sound=NULL;
}

void RandomizeMap(struct s_map *map, int width, int height, char *chipset)
{
     int i,x,y;

     InitMap(map);

     map->surf_chipset=SDL_LoadBMP(chipset);
     SDL_SetColorKey(map->surf_chipset, SDL_SRCCOLORKEY, SDL_MapRGB(map->surf_chipset->format,8,33,82));
     
     map->n_tile=(map->surf_chipset->w/TILE_WIDTH)*(map->surf_chipset->h/TILE_HEIGHT);
     map->chipset=calloc((map->n_tile),sizeof(struct s_chipset));

     x=y=0;

     for(i=0;i<map->n_tile;i++)
     {
                               
        map->chipset[i].x=x;
        map->chipset[i].y=y;
        map->chipset[i].collision=0;
        map->chipset[i].nextTile=0;
        
        x += TILE_WIDTH;
        if(x>(map->surf_chipset->w-TILE_WIDTH))
        {
           x=0;
           y += TILE_HEIGHT;
        }
     }
   
     map->width = width;
     map->height = height;
     
     map->data = malloc(map->width*map->height*sizeof(unsigned int));
     
     for(x=0;x<map->width;x++)
        for(y=0;y<map->height;y++)
           map->data[x+y*map->width]=rand()%(map->n_tile);
}

void HandleMap(struct s_map *map)
{
   int x,y, tx,ty;
   SDL_Rect src, dest;
   
   x=-(map->offsetX%TILE_WIDTH);
   y=-(map->offsetY%TILE_HEIGHT);
   tx=map->offsetX/TILE_WIDTH;
   ty=map->offsetY/TILE_HEIGHT;
   
   while(y<600)
   {
      x=-(map->offsetX%TILE_WIDTH);
      tx=map->offsetX/TILE_WIDTH;
      while(x<800)
      {
         src.x=map->chipset[GET_TILE(tx,ty,map)].x;
         src.y=map->chipset[GET_TILE(tx,ty,map)].y;
         dest.x=x;
         dest.y=y;
         src.w=dest.w=TILE_WIDTH;
         src.h=dest.h=TILE_HEIGHT;
         
         if(dest.x<0)
         {
            src.x-=dest.x;
            src.w+=dest.x;
            dest.w+=dest.x;
            dest.x=0;
         }
         if(dest.y<0)
         {
            src.y-=dest.y;
            src.h+=dest.y;
            dest.h+=dest.h;
            dest.y=0;
         } 
         
         SDL_BlitSurface(map->surf_chipset,&src,SDL_GetVideoSurface(),&dest);
         
         x+=TILE_WIDTH;
         tx++;
      }
      y+=TILE_HEIGHT;
      ty++;
    }
}

void MapEvent(SDL_Event *event, struct s_map *map)
{
     if(!event) return;
}

void FreeMap(struct s_map *map)
{
    SDL_FreeSurface(map->surf_chipset);
    free(map->data);
    free(map->chipset);
    FMUSIC_StopSong(map->bg_sound);
    FMUSIC_FreeSong(map->bg_sound);
}


void LoadMap(struct s_map *map, char *filename)
{
     FILE *f;
     int i,x,y;
     char buffer[256];
     
     if(!map) return;
     f=fopen(filename,"rb");
     if(!f) return;

     /* BG SOUND */  
     memset(&buffer,0,sizeof(buffer));
     fread(&buffer,255,sizeof(char),f);
     map->bg_sound = FMUSIC_LoadSong(buffer);
     if (!map->bg_sound)
         fprintf(stderr,"Erreur lors du chargement de la carte: %s\n", FMOD_ErrorString(FSOUND_GetError()));

     /* CHIPSET */
     memset(&buffer,0,sizeof(buffer));
     fread(&buffer,255,sizeof(char),f);
     map->surf_chipset=SDL_LoadBMP(buffer);
     SDL_SetColorKey(map->surf_chipset, SDL_SRCCOLORKEY, SDL_MapRGB(map->surf_chipset->format,8,33,82));

     fread(&map->n_tile,1,sizeof(unsigned int),f);
     fread(&map->width,1,sizeof(unsigned int),f);
     fread(&map->height,1,sizeof(unsigned int),f);
     fread(&map->offsetX,1,sizeof(unsigned int),f);
     fread(&map->offsetY,1,sizeof(unsigned int),f);

     map->chipset=malloc((map->n_tile)*sizeof(struct s_chipset));

     for(i=0;i<map->n_tile;i++)
     {
        fread(&map->chipset[i].x, 1, sizeof(unsigned int),f);
        fread(&map->chipset[i].y, 1, sizeof(unsigned int),f);
        fread(&map->chipset[i].collision, 1, sizeof(unsigned char),f);
        fread(&map->chipset[i].nextTile, 1, sizeof(unsigned int),f);
     }

     map->data = calloc(map->width*map->height,sizeof(unsigned int));

     for(x=0;x<map->width;x++)
        for(y=0;y<map->height;y++)
        {
           fread(&GET_TILE(x,y,map), 1, sizeof(unsigned int),f);
           if(GET_TILE(x,y,map)>=map->n_tile)
           {
              fprintf(stderr,"Carte corrompue! (%d,%d)\n",x,y);
              GET_TILE(x,y,map)=0;
           }
        }
     fclose(f);

     FMUSIC_PlaySong(map->bg_sound);
}

void SaveMap(struct s_map *map, char *filename, char *bg_sound, char *chipset)
{
     FILE *f;
     int i,x,y;
     char buffer[256];
     
     if(!map) return;
     f=fopen(filename,"wb");
     if(!f) return;
   
     memset(&buffer,0,sizeof(buffer));
     strcpy(buffer,bg_sound);
     fwrite(&buffer,255,sizeof(char),f);
     memset(&buffer,0,sizeof(buffer));
     strcpy(buffer,chipset);
     fwrite(&buffer,255,sizeof(char),f);

     fwrite(&map->n_tile,1,sizeof(unsigned int),f);
     fwrite(&map->width,1,sizeof(unsigned int),f);
     fwrite(&map->height,1,sizeof(unsigned int),f);
     fwrite(&map->offsetX,1,sizeof(unsigned int),f);
     fwrite(&map->offsetY,1,sizeof(unsigned int),f);

     for(i=0;i<map->n_tile;i++)
     {
        fwrite(&map->chipset[i].x, 1, sizeof(unsigned int),f);
        fwrite(&map->chipset[i].y, 1, sizeof(unsigned int),f);
        fwrite(&map->chipset[i].collision, 1, sizeof(unsigned char),f);
        fwrite(&map->chipset[i].nextTile, 1, sizeof(unsigned int),f);
     }

     for(x=0;x<map->width;x++)
        for(y=0;y<map->height;y++)
           fwrite(&GET_TILE(x,y,map), 1, sizeof(unsigned int),f);
     fclose(f);
}
