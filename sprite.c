/*
** sprite.c
**
** Gestion des sprites
**
** Rôle:
** - initialisation et libération des sprites.
*/

# include "sprite.h"

void InitSprite(struct s_lib_sprite *libsprite)
{
   libsprite->n_sprite=0;
   libsprite->sprites=NULL;
}

void FreeSprite(struct s_lib_sprite *libsprite)
{
     unsigned int i, anim;

     for(i=0;i<libsprite->n_sprite;i++)
     {
        SDL_FreeSurface(libsprite->sprites[i].surf);        

        for(anim=0;anim<libsprite->sprites[i].n_anim;anim++)
           free(libsprite->sprites[i].anim[anim].frame);
        free(libsprite->sprites[i].anim);
     }
     free(libsprite->sprites);
     libsprite->n_sprite=0;
     
}

void HandleSprites(int ispaused, struct s_lib_sprite *libsprite, struct s_map *map)
{
     int i, oldx, oldy, anim;

     /* centre la vue sur le joueur */
     if(libsprite->sprites[0].x<(800/2))
        map->offsetX=0;     
     else
     {
        map->offsetX=libsprite->sprites[0].x-(800/2);
        if(map->offsetX>map->width*TILE_WIDTH-800) map->offsetX=map->width*TILE_WIDTH-800;
     }

     if(libsprite->sprites[0].y<(600/2))
        map->offsetY=0;     
     else
     {
        map->offsetY=libsprite->sprites[0].y-(600/2);
        if(map->offsetY>map->height*TILE_HEIGHT-600) map->offsetY=map->height*TILE_HEIGHT-600;
     }
        
     /* gestion du joueur */
     HandleSprite(&libsprite->sprites[0], map);        
     
     /* gestion des pnj */
     for(i=1;i<libsprite->n_sprite;i++)
     {
        if( (!ispaused) && (!libsprite->sprites[i].ispaused_anim))
        {
           oldx=libsprite->sprites[i].x, oldy=libsprite->sprites[i].y;
        
        /* on se fait déplacer le pnj en continuant dans sa direction... */
           switch(libsprite->sprites[i].cur_anim)
           {
              case ANIMATION_DROITE:
                 SpriteMove(
                    &libsprite->sprites[i],
                    libsprite->sprites[i].x+1,
                    libsprite->sprites[i].y,
                    map);
                 break;
              case ANIMATION_GAUCHE:
                 if(libsprite->sprites[i].x>0)
                    SpriteMove(
                       &libsprite->sprites[i],
                       libsprite->sprites[i].x-1,
                       libsprite->sprites[i].y,
                       map);
                 break;
              case ANIMATION_HAUT:
                 if(libsprite->sprites[i].y>0)
                    SpriteMove(
                       &libsprite->sprites[i],
                       libsprite->sprites[i].x,
                       libsprite->sprites[i].y-1,
                       map);
                 break;
                 case ANIMATION_BAS:
                 SpriteMove(
                    &libsprite->sprites[i],
                    libsprite->sprites[i].x,
                    libsprite->sprites[i].y+1,
                    map);
                 break;
           }
           if( (oldx==libsprite->sprites[i].x) && (oldy==libsprite->sprites[i].y))
           {
              do 
                 anim=rand()%(libsprite->sprites[i].n_anim);
              while (anim==libsprite->sprites[i].cur_anim);

              SpriteChangeAnim(&libsprite->sprites[i], anim, 0);
           }
        } /*! !libsprite->sprites[i].ispaused_anim */
        HandleSprite(&libsprite->sprites[i], map);
     }
}

void HandleSprite(struct s_sprite *sprite, struct s_map *map)
{
     SDL_Rect src,dest;
     
     if(!sprite->ispaused_anim)
        if(SDL_GetTicks()-sprite->anim[sprite->cur_anim].lastfrmupdate>sprite->anim[sprite->cur_anim].delaybetweenframes)
        {
           sprite->anim[sprite->cur_anim].cur_frame = (sprite->anim[sprite->cur_anim].cur_frame+1)%(sprite->anim[sprite->cur_anim].n_frame);
           sprite->anim[sprite->cur_anim].lastfrmupdate=SDL_GetTicks();
        }
     
     src.x = sprite->anim[sprite->cur_anim].frame[sprite->anim[sprite->cur_anim].cur_frame].x;
     src.y = sprite->anim[sprite->cur_anim].frame[sprite->anim[sprite->cur_anim].cur_frame].y;
     src.w = sprite->width;
     src.h = sprite->height;
     
     dest.x = sprite->x-map->offsetX;
     dest.y = sprite->y-map->offsetY;
     dest.w = sprite->width;
     dest.h = sprite->height;
     
     if(dest.x<-dest.w) return;
     if(dest.y<-dest.h) return;
     
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
     
     SDL_BlitSurface(sprite->surf,&src,SDL_GetVideoSurface(),&dest);
}

void SpriteMove(struct s_sprite *sprite, unsigned int x, unsigned int y, struct s_map *map)
{
     unsigned int hgx,hgy, hdx,hdy, bgx,bgy, bdx,bdy;
     
     /* calcul du tile sur lequel est situé chaque bord du sprite */
     hgx = (x+sprite->offsetG)/TILE_WIDTH;
     hgy = (y+sprite->offsetH)/TILE_HEIGHT;
     
     hdx = (x+sprite->offsetG+TILE_WIDTH-sprite->offsetD)/TILE_WIDTH;
     hdy = (y+sprite->offsetH)/TILE_HEIGHT;
     
     bgx = (x+sprite->offsetG)/TILE_WIDTH;
     bgy = (y+sprite->offsetH+TILE_HEIGHT-sprite->offsetB)/TILE_HEIGHT;
     
     bdx = (x+sprite->offsetG+TILE_WIDTH-sprite->offsetD)/TILE_WIDTH;
     bdy = (y+sprite->offsetH+TILE_HEIGHT-sprite->offsetB)/TILE_HEIGHT;
     
     if (  (map->chipset[GET_TILE(hgx,hgy,map)].collision==0)
        && (map->chipset[GET_TILE(hdx,hdy,map)].collision==0)
        && (map->chipset[GET_TILE(bgx,bgy,map)].collision==0)
        && (map->chipset[GET_TILE(bdx,bdy,map)].collision==0) )
     {
        if(x>map->width*TILE_WIDTH-sprite->width) x=map->width*TILE_WIDTH-sprite->width;
        if(y>map->height*TILE_HEIGHT-sprite->height) y=map->height*TILE_HEIGHT-sprite->height;

        sprite->x=x;
        sprite->y=y;
        sprite->ispaused_anim=0;
     } else {
        sprite->ispaused_anim=1;
        sprite->anim[sprite->cur_anim].cur_frame=0;
     }
}

void SpriteChangeAnim(struct s_sprite *sprite, unsigned int anim, unsigned char ispaused)
{
   sprite->cur_anim=anim;
   sprite->ispaused_anim=ispaused;
   sprite->anim[sprite->cur_anim].cur_frame=0;
   sprite->anim[sprite->cur_anim].lastfrmupdate=SDL_GetTicks();
}

void SpriteEvent(SDL_Event *event, struct s_map *map, struct s_lib_sprite *libsprite)
{
     if(!event) return;
     
     libsprite->sprites[0].ispaused_anim=1;
     
     switch(event->type)
     {
      case SDL_KEYDOWN:
	    if(event->key.state==SDL_PRESSED)
	    {
          if(event->key.keysym.sym==SDLK_RIGHT)
          {
             if(libsprite->sprites[0].cur_anim!=ANIMATION_DROITE)
                SpriteChangeAnim(&libsprite->sprites[0], ANIMATION_DROITE, 0);
             else
                libsprite->sprites[0].ispaused_anim=0;
                
             SpriteMove(&libsprite->sprites[0], libsprite->sprites[0].x+1, libsprite->sprites[0].y,map);
          }
          else if(event->key.keysym.sym==SDLK_LEFT)
          {
             if(libsprite->sprites[0].cur_anim!=ANIMATION_GAUCHE)
                SpriteChangeAnim(&libsprite->sprites[0], ANIMATION_GAUCHE, 0);
             else
                libsprite->sprites[0].ispaused_anim=0;
                
             if(libsprite->sprites[0].x>0)
                SpriteMove(&libsprite->sprites[0], libsprite->sprites[0].x-1, libsprite->sprites[0].y,map);
          }
          else if(event->key.keysym.sym==SDLK_DOWN)
          {
             if(libsprite->sprites[0].cur_anim!=ANIMATION_BAS)
                SpriteChangeAnim(&libsprite->sprites[0], ANIMATION_BAS, 0);
             else
                libsprite->sprites[0].ispaused_anim=0;
                
             SpriteMove(&libsprite->sprites[0], libsprite->sprites[0].x, libsprite->sprites[0].y+1,map);
          }
          else if(event->key.keysym.sym==SDLK_UP)
          {               
             if(libsprite->sprites[0].cur_anim!=ANIMATION_HAUT)
                SpriteChangeAnim(&libsprite->sprites[0], ANIMATION_HAUT, 0);
             else
                libsprite->sprites[0].ispaused_anim=0;
                
             if(libsprite->sprites[0].y>0)
                SpriteMove(&libsprite->sprites[0], libsprite->sprites[0].x, libsprite->sprites[0].y-1,map);
          }
        }
      break;
     }
}


void LoadSprite(struct s_lib_sprite *libsprite, char *filename)
{
     unsigned int i, anim, frm;
     char buffer[256];
     FILE *f;
     
     if(!libsprite) return;
     f=fopen(filename,"rb");
     if(!f) return;
     
     fread(&libsprite->n_sprite, sizeof(unsigned int), 1, f);
     libsprite->sprites=malloc(libsprite->n_sprite*sizeof(struct s_sprite));
     for(i=0;i<libsprite->n_sprite;i++)
     {
        fread(&libsprite->sprites[i].x, sizeof(unsigned int), 1, f);
        fread(&libsprite->sprites[i].y, sizeof(unsigned int), 1, f);
        fread(&libsprite->sprites[i].width, sizeof(unsigned int), 1, f);
        fread(&libsprite->sprites[i].height, sizeof(unsigned int), 1, f);
        fread(&libsprite->sprites[i].n_anim, sizeof(unsigned int), 1, f);
        fread(&libsprite->sprites[i].cur_anim, sizeof(unsigned int), 1, f);
        fread(&libsprite->sprites[i].ispaused_anim, sizeof(unsigned char), 1, f);
                                       
        fread(&libsprite->sprites[i].offsetG, sizeof(unsigned int), 1, f);
        fread(&libsprite->sprites[i].offsetD, sizeof(unsigned int), 1, f);
        fread(&libsprite->sprites[i].offsetH, sizeof(unsigned int), 1, f);
        fread(&libsprite->sprites[i].offsetB, sizeof(unsigned int), 1, f);
        
        memset(&buffer,0,sizeof(buffer));
        fread(&buffer,255,sizeof(char),f);
        libsprite->sprites[i].surf=SDL_LoadBMP(buffer);
        SDL_SetColorKey(libsprite->sprites[i].surf, SDL_SRCCOLORKEY, SDL_MapRGB(libsprite->sprites[i].surf->format,0,0,0));
        
        libsprite->sprites[i].anim=malloc(libsprite->sprites[i].n_anim*sizeof(struct s_anim));
        for(anim=0;anim<libsprite->sprites[i].n_anim;anim++)
        {
           fread(&libsprite->sprites[i].anim[anim].n_frame, sizeof(unsigned int), 1, f);
           fread(&libsprite->sprites[i].anim[anim].cur_frame, sizeof(unsigned int), 1, f);
           fread(&libsprite->sprites[i].anim[anim].lastfrmupdate, sizeof(unsigned int), 1, f);
           fread(&libsprite->sprites[i].anim[anim].delaybetweenframes, sizeof(unsigned int), 1, f);
           
           libsprite->sprites[i].anim[anim].frame=malloc(libsprite->sprites[i].anim[anim].n_frame*sizeof(struct s_frame));
           for(frm=0;frm<libsprite->sprites[i].anim[anim].n_frame;frm++)
           {
              fread(&libsprite->sprites[i].anim[anim].frame[frm].x, sizeof(unsigned int), 1, f);
              fread(&libsprite->sprites[i].anim[anim].frame[frm].y, sizeof(unsigned int), 1, f);
           }
        }   
     }
     
     fclose(f);
}

void SaveSprite(struct s_lib_sprite *libsprite, char *charset, char *filename)
{
     unsigned int i, anim, frm;
     char buffer[256];
     FILE *f;
     
     if(!libsprite) return;
     f=fopen(filename,"wb");
     if(!f) return;
     
     fwrite(&libsprite->n_sprite, sizeof(unsigned int), 1, f);
     for(i=0;i<libsprite->n_sprite;i++)
     {
        fwrite(&libsprite->sprites[i].x, sizeof(unsigned int), 1, f);
        fwrite(&libsprite->sprites[i].y, sizeof(unsigned int), 1, f);
        fwrite(&libsprite->sprites[i].width, sizeof(unsigned int), 1, f);
        fwrite(&libsprite->sprites[i].height, sizeof(unsigned int), 1, f);
        fwrite(&libsprite->sprites[i].n_anim, sizeof(unsigned int), 1, f);
        fwrite(&libsprite->sprites[i].cur_anim, sizeof(unsigned int), 1, f);
        fwrite(&libsprite->sprites[i].ispaused_anim, sizeof(unsigned char), 1, f);
                                       
        fwrite(&libsprite->sprites[i].offsetG, sizeof(unsigned int), 1, f);
        fwrite(&libsprite->sprites[i].offsetD, sizeof(unsigned int), 1, f);
        fwrite(&libsprite->sprites[i].offsetH, sizeof(unsigned int), 1, f);
        fwrite(&libsprite->sprites[i].offsetB, sizeof(unsigned int), 1, f);
        
if(i==2)
{
        memset(&buffer,0,sizeof(buffer));
        strcpy(buffer,"sprites/coffre.bmp");
        fwrite(&buffer,255,sizeof(char),f);
} else {
        memset(&buffer,0,sizeof(buffer));
        strcpy(buffer,charset);
        fwrite(&buffer,255,sizeof(char),f);
}

        for(anim=0;anim<libsprite->sprites[i].n_anim;anim++)
        {
           fwrite(&libsprite->sprites[i].anim[anim].n_frame, sizeof(unsigned int), 1, f);
           fwrite(&libsprite->sprites[i].anim[anim].cur_frame, sizeof(unsigned int), 1, f);
           fwrite(&libsprite->sprites[i].anim[anim].lastfrmupdate, sizeof(unsigned int), 1, f);
           fwrite(&libsprite->sprites[i].anim[anim].delaybetweenframes, sizeof(unsigned int), 1, f);
           
           for(frm=0;frm<libsprite->sprites[i].anim[anim].n_frame;frm++)
           {
              fwrite(&libsprite->sprites[i].anim[anim].frame[frm].x, sizeof(unsigned int), 1, f);
              fwrite(&libsprite->sprites[i].anim[anim].frame[frm].y, sizeof(unsigned int), 1, f);
           }
        }   
     }
     
     fclose(f);
}


void SpriteStaticData(struct s_lib_sprite *libsprite)
{
     libsprite->n_sprite = 2;
     libsprite->sprites=malloc(libsprite->n_sprite*sizeof(struct s_sprite));
     
     /* SPRITE 0: player */
     libsprite->sprites[0].x=256;
     libsprite->sprites[0].y=86;
     
     libsprite->sprites[0].offsetG=4;
     libsprite->sprites[0].offsetD=15;
     libsprite->sprites[0].offsetH=20;
     libsprite->sprites[0].offsetB=10;
     
     libsprite->sprites[0].surf=SDL_LoadBMP("sprites/player.bmp");
     SDL_SetColorKey(libsprite->sprites[0].surf, SDL_SRCCOLORKEY, SDL_MapRGB(libsprite->sprites[0].surf->format,0,0,0));
     libsprite->sprites[0].width=24;
     libsprite->sprites[0].height=32;
     libsprite->sprites[0].n_anim=4;
     libsprite->sprites[0].cur_anim=ANIMATION_BAS;
     libsprite->sprites[0].ispaused_anim=1;
     libsprite->sprites[0].anim=malloc(libsprite->sprites[0].n_anim*sizeof(struct s_anim));

     /* ANIM 0: haut */
     libsprite->sprites[0].anim[0].cur_frame=0;
     libsprite->sprites[0].anim[0].n_frame=3;
     libsprite->sprites[0].anim[0].lastfrmupdate=SDL_GetTicks();
     libsprite->sprites[0].anim[0].delaybetweenframes = 100;
     libsprite->sprites[0].anim[0].frame=malloc(libsprite->sprites[0].anim[0].n_frame*sizeof(struct s_frame));
     
     libsprite->sprites[0].anim[0].frame[0].x=144;
     libsprite->sprites[0].anim[0].frame[0].y=0;
     
     libsprite->sprites[0].anim[0].frame[1].x=168;
     libsprite->sprites[0].anim[0].frame[1].y=0;
     
     libsprite->sprites[0].anim[0].frame[2].x=192;
     libsprite->sprites[0].anim[0].frame[2].y=0;
     
     /* ANIM 1: droite */
     libsprite->sprites[0].anim[1].cur_frame=0;
     libsprite->sprites[0].anim[1].n_frame=3;
     libsprite->sprites[0].anim[1].lastfrmupdate=SDL_GetTicks();
     libsprite->sprites[0].anim[1].delaybetweenframes = 100;
     libsprite->sprites[0].anim[1].frame=malloc(libsprite->sprites[0].anim[1].n_frame*sizeof(struct s_frame));
     
     libsprite->sprites[0].anim[1].frame[0].x=144;
     libsprite->sprites[0].anim[1].frame[0].y=32;
     
     libsprite->sprites[0].anim[1].frame[1].x=168;
     libsprite->sprites[0].anim[1].frame[1].y=32;
     
     libsprite->sprites[0].anim[1].frame[2].x=192;
     libsprite->sprites[0].anim[1].frame[2].y=32;
     
     /* ANIM 2: gauche */
     libsprite->sprites[0].anim[2].cur_frame=0;
     libsprite->sprites[0].anim[2].n_frame=3;
     libsprite->sprites[0].anim[2].lastfrmupdate=SDL_GetTicks();
     libsprite->sprites[0].anim[2].delaybetweenframes = 100;
     libsprite->sprites[0].anim[2].frame=malloc(libsprite->sprites[0].anim[2].n_frame*sizeof(struct s_frame));
     
     libsprite->sprites[0].anim[2].frame[0].x=144;
     libsprite->sprites[0].anim[2].frame[0].y=64;
     
     libsprite->sprites[0].anim[2].frame[1].x=168;
     libsprite->sprites[0].anim[2].frame[1].y=64;
     
     libsprite->sprites[0].anim[2].frame[2].x=192;
     libsprite->sprites[0].anim[2].frame[2].y=64;
     
     /* ANIM 3: bas */
     libsprite->sprites[0].anim[3].cur_frame=0;
     libsprite->sprites[0].anim[3].n_frame=3;
     libsprite->sprites[0].anim[3].lastfrmupdate=SDL_GetTicks();
     libsprite->sprites[0].anim[3].delaybetweenframes = 100;
     libsprite->sprites[0].anim[3].frame=malloc(libsprite->sprites[0].anim[3].n_frame*sizeof(struct s_frame));
     
     libsprite->sprites[0].anim[3].frame[0].x=144;
     libsprite->sprites[0].anim[3].frame[0].y=96;
     
     libsprite->sprites[0].anim[3].frame[1].x=168;
     libsprite->sprites[0].anim[3].frame[1].y=96;
     
     libsprite->sprites[0].anim[3].frame[2].x=192;
     libsprite->sprites[0].anim[3].frame[2].y=96;
     
     
     /* SPRITE 1: pnj */
     libsprite->sprites[1].x=100;
     libsprite->sprites[1].y=128;
     
     libsprite->sprites[1].offsetG=4;
     libsprite->sprites[1].offsetD=15;
     libsprite->sprites[1].offsetH=20;
     libsprite->sprites[1].offsetB=10;
     
     libsprite->sprites[1].surf=SDL_LoadBMP("sprites/player.bmp");
     SDL_SetColorKey(libsprite->sprites[1].surf, SDL_SRCCOLORKEY, SDL_MapRGB(libsprite->sprites[0].surf->format,0,0,0));
     libsprite->sprites[1].width=24;
     libsprite->sprites[1].height=32;
     libsprite->sprites[1].n_anim=4;
     libsprite->sprites[1].cur_anim=ANIMATION_BAS;
     libsprite->sprites[1].ispaused_anim=0;
     libsprite->sprites[1].anim=malloc(libsprite->sprites[0].n_anim*sizeof(struct s_anim));

     /* ANIM 0: haut */
     libsprite->sprites[1].anim[0].cur_frame=0;
     libsprite->sprites[1].anim[0].n_frame=3;
     libsprite->sprites[1].anim[0].lastfrmupdate=SDL_GetTicks();
     libsprite->sprites[1].anim[0].delaybetweenframes = 100;
     libsprite->sprites[1].anim[0].frame=malloc(libsprite->sprites[0].anim[0].n_frame*sizeof(struct s_frame));
     
     libsprite->sprites[1].anim[0].frame[0].x=0;
     libsprite->sprites[1].anim[0].frame[0].y=0;
     
     libsprite->sprites[1].anim[0].frame[1].x=24;
     libsprite->sprites[1].anim[0].frame[1].y=0;
     
     libsprite->sprites[1].anim[0].frame[2].x=48;
     libsprite->sprites[1].anim[0].frame[2].y=0;
     
     /* ANIM 1: droite */
     libsprite->sprites[1].anim[1].cur_frame=0;
     libsprite->sprites[1].anim[1].n_frame=3;
     libsprite->sprites[1].anim[1].lastfrmupdate=SDL_GetTicks();
     libsprite->sprites[1].anim[1].delaybetweenframes = 100;
     libsprite->sprites[1].anim[1].frame=malloc(libsprite->sprites[0].anim[1].n_frame*sizeof(struct s_frame));
     
     libsprite->sprites[1].anim[1].frame[0].x=0;
     libsprite->sprites[1].anim[1].frame[0].y=32;
     
     libsprite->sprites[1].anim[1].frame[1].x=24;
     libsprite->sprites[1].anim[1].frame[1].y=32;
     
     libsprite->sprites[1].anim[1].frame[2].x=48;
     libsprite->sprites[1].anim[1].frame[2].y=32;
     
     /* ANIM 2: gauche */
     libsprite->sprites[1].anim[2].cur_frame=0;
     libsprite->sprites[1].anim[2].n_frame=3;
     libsprite->sprites[1].anim[2].lastfrmupdate=SDL_GetTicks();
     libsprite->sprites[1].anim[2].delaybetweenframes = 100;
     libsprite->sprites[1].anim[2].frame=malloc(libsprite->sprites[0].anim[2].n_frame*sizeof(struct s_frame));
     
     libsprite->sprites[1].anim[2].frame[0].x=0;
     libsprite->sprites[1].anim[2].frame[0].y=64;
     
     libsprite->sprites[1].anim[2].frame[1].x=24;
     libsprite->sprites[1].anim[2].frame[1].y=64;
     
     libsprite->sprites[1].anim[2].frame[2].x=48;
     libsprite->sprites[1].anim[2].frame[2].y=64;
     
     /* ANIM 3: bas */
     libsprite->sprites[1].anim[3].cur_frame=0;
     libsprite->sprites[1].anim[3].n_frame=3;
     libsprite->sprites[1].anim[3].lastfrmupdate=SDL_GetTicks();
     libsprite->sprites[1].anim[3].delaybetweenframes = 100;
     libsprite->sprites[1].anim[3].frame=malloc(libsprite->sprites[0].anim[3].n_frame*sizeof(struct s_frame));
     
     libsprite->sprites[1].anim[3].frame[0].x=0;
     libsprite->sprites[1].anim[3].frame[0].y=96;
     
     libsprite->sprites[1].anim[3].frame[1].x=24;
     libsprite->sprites[1].anim[3].frame[1].y=96;
     
     libsprite->sprites[1].anim[3].frame[2].x=48;
     libsprite->sprites[1].anim[3].frame[2].y=96;
     
}
