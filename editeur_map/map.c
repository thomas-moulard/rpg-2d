/*
** map.c
**
** Gestion des cartes.
**
*/
#include "map.h"

void InitMap(struct s_map *map)
{
     map->n_tile=map->width=map->height=map->offsetX=map->offsetY=map->chipsetOffsetY=0;
     map->tile_sel=0;
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
        map->chipset[i].collision=1;
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

void HandleMap(struct s_map *map, int left)
{
   int x,y, tx,ty, i;
   SDL_Rect src, dest;
   
   SDL_FillRect(SDL_GetVideoSurface(), NULL, 0);
   
   x=-(map->offsetX%TILE_WIDTH)+left;
   y=-(map->offsetY%TILE_HEIGHT);
   tx=map->offsetX/TILE_WIDTH;
   ty=map->offsetY/TILE_HEIGHT;
   
   while(y<600)
   {
      x=-(map->offsetX%TILE_WIDTH)+left;
      tx=map->offsetX/TILE_WIDTH;
      while(x<800)
      {
         src.x=map->chipset[GET_TILE(tx,ty,map)].x;
         src.y=map->chipset[GET_TILE(tx,ty,map)].y;
         dest.x=x;
         dest.y=y;
         src.w=dest.w=TILE_WIDTH;
         src.h=dest.h=TILE_HEIGHT;
         
         if(dest.x<left)
         {
            src.x-=dest.x-left;
            src.w+=dest.x-left;
            dest.w+=dest.x-left;
            dest.x=left;
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
    
    /* AFFICHAGE DU CHIPSET */
    x=1;
    y=1;
    for(i=map->chipsetOffsetY;i<map->n_tile;i++)
    {
       src.x=map->chipset[i].x;
       src.y=map->chipset[i].y;
       dest.x=x;
       dest.y=y;
       src.w=dest.w=TILE_WIDTH;
       src.h=dest.h=TILE_HEIGHT;
       
       if(i==map->tile_sel)
       {
         dest.x--;
         dest.y--;
         dest.w+=2;
         dest.h+=2;
         SDL_FillRect(SDL_GetVideoSurface(), &dest, 
            (map->chipset[i].collision==0)?
               SDL_MapRGB(SDL_GetVideoSurface()->format, 0,255,0)
             : SDL_MapRGB(SDL_GetVideoSurface()->format, 255,0,0));
         dest.x++;
         dest.y++;
         dest.w-=2;
         dest.h-=2;
       }
         
       SDL_BlitSurface(map->surf_chipset,&src,SDL_GetVideoSurface(),&dest);
       
       dest.x+=TILE_WIDTH-2;
       dest.y+=TILE_HEIGHT-2;
       dest.w=dest.h=2;
       SDL_FillRect(SDL_GetVideoSurface(), &dest, 
            (map->chipset[i].collision==0)?
               SDL_MapRGB(SDL_GetVideoSurface()->format, 0,255,0)
             : SDL_MapRGB(SDL_GetVideoSurface()->format, 255,0,0));
       

       x += TILE_WIDTH+1;
       if(x>(left-TILE_WIDTH-BORDER_SIZE))
       {
          x=1;
          y+=TILE_HEIGHT+1;
       }
    }
}

void MapEvent(SDL_Event *event, struct s_map *map, int left)
{
     FILE *f;
     int i,j, ntileparcol;
     if(!event) return;
     
     ntileparcol=(left-BORDER_SIZE)/(TILE_WIDTH+1);
     
     switch(event->type)
     {
      case SDL_KEYDOWN:
	    if(event->key.state==SDL_PRESSED)
	    {
          if(event->key.keysym.sym==SDLK_RIGHT)
          {
             if (map->offsetX<(map->width*TILE_WIDTH-800)) map->offsetX++;
          }
          else if(event->key.keysym.sym==SDLK_LEFT)
          {
             if (map->offsetX!=0) map->offsetX--;
          }
          else if(event->key.keysym.sym==SDLK_DOWN)
          {
             if (map->offsetY<(map->height*TILE_HEIGHT-600)) map->offsetY++;
          }
          else if(event->key.keysym.sym==SDLK_UP)
          {
             if (map->offsetY!=0) map->offsetY--;
          }
          /* CHIPSET */
          else if(event->key.keysym.sym==SDLK_PAGEUP)
          {
             if (map->chipsetOffsetY>=ntileparcol) map->chipsetOffsetY-=ntileparcol;
          }
          else if(event->key.keysym.sym==SDLK_PAGEDOWN)
          {
             if (map->chipsetOffsetY<map->n_tile-ntileparcol) map->chipsetOffsetY+=ntileparcol;
          }
          /* FONCTION SPECIALES */
          else if(event->key.keysym.sym==SDLK_f) /* FILL */
          {
             for(i=0;i<map->width;i++)
                for(j=0;j<map->height;j++)
                   GET_TILE(i,j,map)=map->tile_sel;
          }
          else if(event->key.keysym.sym==SDLK_s) /* SAVE */
          {
               SaveMap(map, "map/save.map", "musique/Opening1.mid", "tiles/mchip2.bmp");
          }
          else if(event->key.keysym.sym==SDLK_l) /* LOAD */
          {
               f=fopen("map/save.map","r");
               if(f)
               {
                  fclose(f);
                  FreeMap(map);
                  InitMap(map);
                  LoadMap(map, "map/save.map");
               }
          }
        }
      break;
      
      case SDL_MOUSEBUTTONDOWN:
           if(event->button.button==SDL_BUTTON_LEFT)
           {
              if(event->button.x<=(left-BORDER_SIZE))
              {
                map->tile_sel=map->chipsetOffsetY+(event->button.x/(TILE_WIDTH+1))+ntileparcol*(event->button.y/(TILE_HEIGHT+1));
                if(map->tile_sel>=map->n_tile) map->tile_sel=map->n_tile-1;
              } else if(event->button.x>=left) {
                 i=map->offsetX+event->button.x-left;
                 j=map->offsetY+event->button.y;
                 i /= TILE_WIDTH;
                 j /= TILE_HEIGHT;
                 
                 if(i>=map->width) i=map->width-1;
                 if(j>=map->height) j=map->height-1;
                 GET_TILE(i,j,map)=map->tile_sel;
                 
                 /* on appuie sur CTRL */
                 if((SDL_GetModState() & KMOD_CTRL) || (SDL_GetModState() & KMOD_ALT))
                 {
                    if(i<(map->width-1)) GET_TILE(i+1,j,map)=map->tile_sel;
                    if(j>0) GET_TILE(i,j-1,map)=map->tile_sel;
                    if((j>0) && (i<(map->width-1))) GET_TILE(i+1,j-1,map)=map->tile_sel;
                    
                    if(SDL_GetModState() & KMOD_ALT)
                    {
                       if(i>0) GET_TILE(i-1,j,map)=map->tile_sel;
                       if(j<(map->height-1)) GET_TILE(i,j+1,map)=map->tile_sel;

                       if((i>0) && (j<(map->height-1))) GET_TILE(i-1,j+1,map)=map->tile_sel;
                       if((i>0) && (j>0)) GET_TILE(i-1,j-1,map)=map->tile_sel;
                       if((j<(map->height-1)) && (i<(map->width-1))) GET_TILE(i+1,j+1,map)=map->tile_sel;
                    }
                 }
              }
           } else if(event->button.button==SDL_BUTTON_RIGHT)
           {
              if(event->button.x<=(left-BORDER_SIZE))
              {
               i=map->chipsetOffsetY+(event->button.x/(TILE_WIDTH+1))+ntileparcol*(event->button.y/(TILE_HEIGHT+1));
               if(i>=map->n_tile) i=map->n_tile-1;
               map->chipset[i].collision=(map->chipset[i].collision+1)%2;
              }
           }
      break;
     }
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
     
     InitMap(map);

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
