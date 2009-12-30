/*
** event.c
**
** Gère le système d'événements
**
** Rôle:
** - initialise et libère les événements
** - gère le déclenchement et le déroulenement des événements
*/
#include "event.h"

void InitLibEvent(struct s_lib_event *libevent, struct s_map *map, struct s_lib_sprite *libsprite)
{
     unsigned int i;
     
     InitEvent(&libevent->global_event);
     libevent->map_event=calloc(map->width*map->height,sizeof(struct s_event));
     for(i=0;i<map->width*map->height;i++)
        InitEvent(&libevent->map_event[i]);
     libevent->sprite_event=calloc(libsprite->n_sprite,sizeof(struct s_event));
     for(i=0;i<libsprite->n_sprite;i++)
        InitEvent(&libevent->sprite_event[i]);
}

void InitEvent(struct s_event *event)
{
     event->type=EVENT_NULL;
     event->onaction=EVENT_ACTION_AUTO;
     event->proba=0;
     event->param=NULL;
     event->player_anim=-1;
     event->sound=NULL;
     event->is_unique=0;
     event->next=NULL;
}

void FreeLibEvent(struct s_lib_event *libevent, struct s_map *map, struct s_lib_sprite *libsprite)
{
     int i;
     FreeEvent(&libevent->global_event);
     for(i=0;i<map->width*map->height;i++)
        FreeEvent(&libevent->map_event[i]);
     free(libevent->map_event);
     for(i=0;i<libsprite->n_sprite;i++)
        FreeEvent(&libevent->sprite_event[i]);
     free(libevent->sprite_event);
     
     libevent->global_event.next=NULL;
     libevent->map_event=NULL;
     libevent->sprite_event=NULL;
}

void FreeEvent(struct s_event *event)
{
     struct s_event *next;

     if(!event) return;
     event=event->next;
     while(event)
     {
        next=event->next;
        if(event->sound) FSOUND_Sample_Free(event->sound);
        switch(event->type)
        {
           case EVENT_NULL:
                break;
           case EVENT_TELEPORT:
                free(((struct s_event_param_teleport*)event->param)->filename);
                free((struct s_event_param_teleport*)event->param);
                break;
           case EVENT_DIALOG:
                free(((struct s_event_param_dialog*)event->param)->dialog);
                free((struct s_event_param_dialog*)event->param);
                break;
           case EVENT_BATTLE:
                free((struct s_event_param_battle*)event->param);
                break;
           case EVENT_SHOP:
                free(((struct s_event_param_shop*)event->param)->itemId);
                free(((struct s_event_param_shop*)event->param)->coutItem);
                free(((struct s_event_param_shop*)event->param)->prixVente);
                free((struct s_event_param_shop*)event->param);
                break;
           case EVENT_INN:
                free((struct s_event_param_inn*)event->param);
                break;
           case EVENT_CHANGETILE:
                free((struct s_event_param_changetile*)event->param);
                break;
        }
        free(event);
        event=next;
     }
}

void HandleEvent(struct s_lib_event   *libevent,
                 struct s_map         *map,
                 struct s_lib_sprite  *libsprite,
                 struct s_player      *player,
                 struct s_lib_attaque *libattaque,
                 struct s_gui         *gui)
{
     unsigned int tx, ty, i;
     static int istime=0;
     struct s_event *event;

     if(SDL_GetTicks()-istime<1000) return;
     istime=SDL_GetTicks();

     /* Test des événements qui se produisent automatiquement */
     /* GLOBAL */
     event=&libevent->global_event;
     while(event)
     {
        if ((event->onaction==EVENT_ACTION_AUTO) && (RND_EVENT_SUCCESS(*event)))
           if (DoEvent(event, libevent, map, libsprite, player, libattaque, gui))
              return;
        event=event->next;
     }

     /* MAP    */
     tx  = (2*libsprite->sprites[0].x+2*libsprite->sprites[0].offsetG+TILE_WIDTH-libsprite->sprites[0].offsetD)/(2*TILE_WIDTH);
     ty  = (2*libsprite->sprites[0].y+2*libsprite->sprites[0].offsetH+TILE_HEIGHT-libsprite->sprites[0].offsetB)/(2*TILE_HEIGHT);
     
     event=&GET_MAPEVENT(tx,ty,map,libevent);
     while(event)
     {
        if ( (event->onaction==EVENT_ACTION_AUTO) && (RND_EVENT_SUCCESS(*event)) )
           if (DoEvent(event, libevent, map, libsprite, player, libattaque, gui))
             return;
        event=event->next;
     }
     
     /* SPRITE */
     /* inutile de tester 0, c'est le joueur! */
     for(i=1;i<libsprite->n_sprite;i++)
     {
        event=&libevent->sprite_event[i];
        while(event)
        {
           if(IS_NEAR(libsprite->sprites[0].x,libsprite->sprites[i].x,libsprite->sprites[0].y,libsprite->sprites[i].y,TILE_WIDTH/2))
              if ((event->onaction==EVENT_ACTION_AUTO) && (RND_EVENT_SUCCESS(*event)) )
                  if (DoEvent(event, libevent, map, libsprite, player, libattaque, gui))
                    return;
           event=event->next;
        }
     }
}

void Event(SDL_Event            *sdlevent,
           struct s_lib_event   *libevent,
           struct s_map         *map,
           struct s_lib_sprite  *libsprite,
           struct s_player      *player,
           struct s_lib_attaque *libattaque,
           struct s_gui         *gui)
{
     unsigned int tx, ty, i, istime = 0;
     struct s_event *event;
     
     if(SDL_GetTicks()-istime<1000) return;
     istime=SDL_GetTicks();
     
     switch(sdlevent->type)
     {
      case SDL_KEYUP:
	    if(sdlevent->key.state==SDL_RELEASED)
	    {
          if(sdlevent->key.keysym.sym==SDLK_SPACE)
          {
             /* Test des événements qui se produisent quand on appuie sur une touche */
             /* GLOBAL */
             event=&libevent->global_event;
             while(event)
             {
                if ((event->onaction==EVENT_ACTION_ONKEYPRESS) && (RND_EVENT_SUCCESS(*event)))
                   if (DoEvent(event, libevent, map, libsprite, player, libattaque, gui))
                     return;
                event=event->next;
             }

             /* MAP    */
             tx  = (2*libsprite->sprites[0].x+2*libsprite->sprites[0].offsetG+TILE_WIDTH-libsprite->sprites[0].offsetD)/(2*TILE_WIDTH);
             ty  = (2*libsprite->sprites[0].y+2*libsprite->sprites[0].offsetH+TILE_HEIGHT-libsprite->sprites[0].offsetB)/(2*TILE_HEIGHT);
     
             event=&GET_MAPEVENT(tx,ty,map,libevent);

             while(event)
             {
                if ( (event->onaction==EVENT_ACTION_ONKEYPRESS) && (RND_EVENT_SUCCESS(*event)) )
                   if (DoEvent(event, libevent, map, libsprite, player, libattaque, gui))
                     return;
                event=event->next;
             }
     
             /* SPRITE */
             /* inutile de tester 0, c'est le joueur! */
             for(i=1;i<libsprite->n_sprite;i++)
             {
                event=&libevent->sprite_event[i];
                while(event)
                {
                   if(IS_NEAR(libsprite->sprites[0].x,libsprite->sprites[i].x,libsprite->sprites[0].y,libsprite->sprites[i].y,TILE_WIDTH/2))
                      if ((event->onaction==EVENT_ACTION_ONKEYPRESS) && (RND_EVENT_SUCCESS(*event)) )
                         if ( DoEvent(event, libevent, map, libsprite, player, libattaque, gui))
                           return;
                   event=event->next;
                }
             }
          } /* !event->key.state==SDL_PRESSED */
        }
        break;
     }
}

int DoEvent(struct s_event       *event,
            struct s_lib_event   *libevent,
            struct s_map         *map,
            struct s_lib_sprite  *libsprite,
            struct s_player      *player,
            struct s_lib_attaque *libattaque,
            struct s_gui         *gui)
{
     if(!event) return 0;
     if(event->type==EVENT_NULL) return 0;
     
     if(event->sound) FSOUND_PlaySound(FSOUND_FREE,event->sound);
     
     switch(event->type)
     {
        case EVENT_TELEPORT:
           DoEventTeleport(event, libevent, map, libsprite);
           return 1;
        case EVENT_DIALOG:
           DoEventDialog(event, gui);
           break;
        case EVENT_BATTLE:
           DoEventBattle(event, map, player, libattaque, libsprite, gui);
           break;
        case EVENT_SHOP:
           DoEventShop(event, map, libsprite, player, gui);
           break;
        case EVENT_INN:
           DoEventInn(event, map, libsprite, player, gui);
           break;
        case EVENT_CHANGETILE:
           DoEventChangetile(event, map);
           break;
        default:
           break;
     }
     
     if(event->is_unique)
     {
        /* suppr. de l'événement */
        switch(event->type)
        {
           case EVENT_NULL:
                break;
           case EVENT_TELEPORT:
                free(((struct s_event_param_teleport*)event->param)->filename);
                free((struct s_event_param_teleport*)event->param);
                break;
           case EVENT_DIALOG:
                free(((struct s_event_param_dialog*)event->param)->dialog);
                free((struct s_event_param_dialog*)event->param);
                break;
           case EVENT_BATTLE:
                free((struct s_event_param_battle*)event->param);
                break;
           case EVENT_SHOP:
                free(((struct s_event_param_shop*)event->param)->itemId);
                free(((struct s_event_param_shop*)event->param)->coutItem);
                
                free((struct s_event_param_shop*)event->param);
                break;
           case EVENT_INN:
                free((struct s_event_param_inn*)event->param);
                break;
           case EVENT_CHANGETILE:
                free((struct s_event_param_changetile*)event->param);
                break;
        }
        event->type=EVENT_NULL;
        event->onaction=EVENT_ACTION_AUTO;
        event->param=NULL;
     }

   return 0;
}

void LoadEvent(struct s_lib_event *libevent, struct s_map *map, struct s_lib_sprite *libsprite, char *filename)
{
     FILE *f;
     struct s_event *event;
     unsigned int n_event, x, y, i, spr;
     
     InitLibEvent(libevent,map,libsprite);
     
     f=fopen(filename,"rb");
     if(!f) return;

     /* GLOBAL */
     n_event=0;
     fread(&n_event, sizeof(unsigned int), 1, f);

     ReadEvent(f, &libevent->global_event);
     
     event=&libevent->global_event;
     for(i=1;i<n_event;i++)
     {
        event->next=calloc(1,sizeof(struct s_event));
        event->next=event;
        ReadEvent(f, event);
     }
     event->next=NULL;

     /* MAP    */
     for(x=0;x<map->width;x++)
        for(y=0;y<map->height;y++)
        {
           n_event=0;
           fread(&n_event, sizeof(unsigned int), 1, f);
           
           ReadEvent(f, &GET_MAPEVENT(x,y,map,libevent));

           event=&GET_MAPEVENT(x,y,map,libevent);

           for(i=1;i<n_event;i++)
           {
              event->next=calloc(1,sizeof(struct s_event));
              event->next=event;
              
              ReadEvent(f,event);
           }
           event->next=NULL;
        } /* !y<map->height */

     /* SPRITE */
     for(spr=0;spr<libsprite->n_sprite;spr++)
     {
           n_event=0;
           fread(&n_event, sizeof(unsigned int), 1, f);
           
           ReadEvent(f, &libevent->sprite_event[spr]);
           
           event=&libevent->sprite_event[spr];
           for(i=1;i<n_event;i++)
           {
              event->next=calloc(1,sizeof(struct s_event));
              event->next=event;
              ReadEvent(f,event);
           }
           event->next=NULL;
     } /* !spr<libsprite->n_sprite */
     fclose(f);
}

void ReadEvent(FILE *f, struct s_event *event)
{
  char buffer[256];
  register unsigned int i, j;
  
  InitEvent(event);
  
  fread(&event->type, sizeof(enum e_event_type), 1, f);
  if(event->type==EVENT_NULL)
     return;
     
  fread(&event->onaction, sizeof(enum e_event_action), 1, f);
  fread(&event->proba, sizeof(unsigned char), 1, f);
  fread(&event->player_anim, sizeof(unsigned int), 1, f);
  fread(&event->is_unique, sizeof(unsigned char), 1, f);
        
  memset(&buffer,0,sizeof(buffer));
  fread(&buffer,63,sizeof(char),f);
  event->sound=FSOUND_Sample_Load(FSOUND_FREE,buffer,FSOUND_NORMAL,0,0);
        
  switch(event->type)
  {
    case EVENT_NULL:
      break;
    case EVENT_TELEPORT:
      event->param=calloc(1,sizeof(struct s_event_param_teleport));
      memset(&buffer,0,sizeof(buffer));
      fread(&buffer,63,sizeof(char),f);
      if(buffer[0]==0)
        ((struct s_event_param_teleport*)event->param)->filename=NULL;
      else                
        ((struct s_event_param_teleport*)event->param)->filename=strdup(buffer);
                
      fread(&((struct s_event_param_teleport*)event->param)->startX, sizeof(unsigned int), 1, f);
      fread(&((struct s_event_param_teleport*)event->param)->startY, sizeof(unsigned int), 1, f);
      break;
    case EVENT_DIALOG:
      event->param=calloc(1,sizeof(struct s_event_param_dialog));
      memset(&buffer,0,sizeof(buffer));
      fread(&buffer,255,sizeof(char),f);
      ((struct s_event_param_dialog*)event->param)->dialog=strdup(buffer);
      break;
    case EVENT_BATTLE:
      event->param=calloc(1,sizeof(struct s_event_param_battle));
      fread(&((struct s_event_param_battle*)event->param)->n_monstre, sizeof(unsigned int), 1, f);

      memset(&buffer,0,sizeof(buffer));
      fread(&buffer,63,sizeof(char),f);
      if(buffer[0]==0)
        ((struct s_event_param_battle*)event->param)->fond=NULL;
      else
        ((struct s_event_param_battle*)event->param)->fond=strdup(buffer);

      memset(&buffer,0,sizeof(buffer));
      fread(&buffer,63,sizeof(char),f);
      if(buffer[0]==0)
        ((struct s_event_param_battle*)event->param)->fondu=NULL;
      else
        ((struct s_event_param_battle*)event->param)->fondu=strdup(buffer);

      ((struct s_event_param_battle*)event->param)->monstres=
        malloc(((struct s_event_param_battle*)event->param)->n_monstre*sizeof(struct s_monstre));
        
      for(i=0;i<((struct s_event_param_battle*)event->param)->n_monstre;i++)
      {
        fread(&((struct s_event_param_battle*)event->param)->monstres[i].hp,
               sizeof(unsigned int), 1, f);
        fread(&((struct s_event_param_battle*)event->param)->monstres[i].hpmax,
               sizeof(unsigned int), 1, f);
        fread(&((struct s_event_param_battle*)event->param)->monstres[i].mp,
               sizeof(unsigned int), 1, f);
        fread(&((struct s_event_param_battle*)event->param)->monstres[i].mpmax,
               sizeof(unsigned int), 1, f);
        fread(&((struct s_event_param_battle*)event->param)->monstres[i].vitesse,
               sizeof(unsigned int), 1, f);
        fread(&((struct s_event_param_battle*)event->param)->monstres[i].force,
               sizeof(unsigned int), 1, f);
        fread(&((struct s_event_param_battle*)event->param)->monstres[i].dexterite,
               sizeof(unsigned int), 1, f);
        fread(&((struct s_event_param_battle*)event->param)->monstres[i].precision,
               sizeof(unsigned int), 1, f);
        fread(&((struct s_event_param_battle*)event->param)->monstres[i].intelligence,
               sizeof(unsigned int), 1, f);
        fread(&((struct s_event_param_battle*)event->param)->monstres[i].n_attaques,
               sizeof(unsigned int), 1, f);
               
        ((struct s_event_param_battle*)event->param)->monstres[i].attaques=
          malloc(((struct s_event_param_battle*)event->param)->monstres[i].n_attaques*sizeof(unsigned int));
        for(j=0;j<((struct s_event_param_battle*)event->param)->monstres[i].n_attaques;j++)
          fread(&((struct s_event_param_battle*)event->param)->monstres[i].attaques[i],
                 sizeof(unsigned int), 1, f);

        memset(&buffer,0,sizeof(buffer));
        fread(&buffer,63,sizeof(char),f);
        if(buffer[0]==0)
          ((struct s_event_param_battle*)event->param)->monstres[i].surf=NULL;
        else
          ((struct s_event_param_battle*)event->param)->monstres[i].surf=SDL_LoadBMP(buffer);
        SDL_SetColorKey(
          ((struct s_event_param_battle*)event->param)->monstres[i].surf,
          SDL_SRCCOLORKEY,
          SDL_MapRGB(((struct s_event_param_battle*)event->param)->monstres[i].surf->format,255,255,255));
      }
      break;
    case EVENT_SHOP:
      event->param=calloc(1,sizeof(struct s_event_param_shop));
      fread(&((struct s_event_param_shop*)event->param)->n_item, sizeof(unsigned int), 1, f);
      ((struct s_event_param_shop*)event->param)->itemId=malloc(((struct s_event_param_shop*)event->param)->n_item*sizeof(unsigned int));
      ((struct s_event_param_shop*)event->param)->coutItem=malloc(((struct s_event_param_shop*)event->param)->n_item*sizeof(unsigned int));
      for(i=0;i<((struct s_event_param_shop*)event->param)->n_item;i++)
      {
        fread(&((struct s_event_param_shop*)event->param)->itemId[i], sizeof(unsigned int), 1, f);
        fread(&((struct s_event_param_shop*)event->param)->coutItem[i], sizeof(unsigned int), 1, f);
			}
      ((struct s_event_param_shop*)event->param)->prixVente=malloc(MAX_ITEMS*sizeof(unsigned int));
      for(i=0;i<MAX_ITEMS;i++)
        fread(&((struct s_event_param_shop*)event->param)->prixVente[i], sizeof(unsigned int), 1, f);
      break;
    case EVENT_INN:
      event->param=calloc(1,sizeof(struct s_event_param_inn));
      fread(&((struct s_event_param_inn*)event->param)->cout, sizeof(unsigned int), 1, f);
      break;
    case EVENT_CHANGETILE:
      event->param=calloc(1,sizeof(struct s_event_param_changetile));
      fread(&((struct s_event_param_changetile*)event->param)->x, sizeof(unsigned int), 1, f);
      fread(&((struct s_event_param_changetile*)event->param)->y, sizeof(unsigned int), 1, f);
      fread(&((struct s_event_param_changetile*)event->param)->newtile, sizeof(unsigned int), 1, f);
      break;
  }
}


void SaveEvent(struct s_lib_event *libevent, struct s_map *map, struct s_lib_sprite *libsprite, char *bg_sound, char *filename)
{
     FILE *f;
     struct s_event *event;
     unsigned int n_event, x, y, i;
     
     f=fopen(filename,"wb");
     if(!f) return;
     
     /* GLOBAL */
     n_event=0;
     event=&libevent->global_event;
     while(event)
     {
        n_event++;
        event=event->next;
     }
     fwrite(&n_event, sizeof(unsigned int), 1, f);
     
     event=&libevent->global_event;
     while(event)
     {
       WriteEvent(f, event, bg_sound);
       event=event->next;
     }
     
     /* MAP    */
     for(x=0;x<map->width;x++)
        for(y=0;y<map->height;y++)
        {
           n_event=0;
           event=&GET_MAPEVENT(x,y,map,libevent);
           while(event)
           {
              n_event++;
              event=event->next;
           }
           fwrite(&n_event, sizeof(unsigned int), 1, f);
     
           event=&GET_MAPEVENT(x,y,map,libevent);
           while(event)
           {
              WriteEvent(f, event, bg_sound);
              event=event->next;
           }
        } /* !y<map->height */
     /* SPRITE */
     for(i=0;i<libsprite->n_sprite;i++)
     {
        n_event=0;
        event=&libevent->sprite_event[i];
        while(event)
        {
           n_event++;
           event=event->next;
        }

        fwrite(&n_event, sizeof(unsigned int), 1, f);
     
        event=&libevent->sprite_event[i];
        while(event)
        {
          WriteEvent(f, event, bg_sound);
          event=event->next;
         }
     } /* !i<libsprite->n_sprite */
     
     
     free(bg_sound);
     fclose(f);
}

void WriteEvent(FILE *f, struct s_event *event, char *bg_sound)
{
  char buffer[256];
  register unsigned int i, j;
  
  if(!f) return;
  fwrite(&event->type, sizeof(enum e_event_type), 1, f);
  if(event->type==EVENT_NULL) return;
        
  fwrite(&event->onaction, sizeof(enum e_event_action), 1, f);
  fwrite(&event->proba, sizeof(unsigned char), 1, f);
  fwrite(&event->player_anim, sizeof(unsigned int), 1, f);
  fwrite(&event->is_unique, sizeof(unsigned char), 1, f);
        
  memset(&buffer,0,sizeof(buffer));
  strcpy(buffer,bg_sound);
  fwrite(&buffer,63,sizeof(char),f);
  
  switch(event->type)
  {
    case EVENT_NULL:
      break;
    case EVENT_TELEPORT:
      memset(&buffer,0,sizeof(buffer));
      if(((struct s_event_param_teleport*)event->param)->filename!=NULL)
        strcpy(buffer,((struct s_event_param_teleport*)event->param)->filename);
      fwrite(&buffer,63,sizeof(char),f);
                
      fwrite(&((struct s_event_param_teleport*)event->param)->startX, sizeof(unsigned int), 1, f);
      fwrite(&((struct s_event_param_teleport*)event->param)->startY, sizeof(unsigned int), 1, f);
      break;
    case EVENT_DIALOG:
      memset(&buffer,0,sizeof(buffer));
      if(((struct s_event_param_dialog*)event->param)->dialog!=NULL)                
        strcpy(buffer,((struct s_event_param_dialog*)event->param)->dialog);
      fwrite(&buffer,255,sizeof(char),f);
      break;
    case EVENT_BATTLE:
      fwrite(&((struct s_event_param_battle*)event->param)->n_monstre, sizeof(unsigned int), 1, f);
      
      memset(&buffer,0,sizeof(buffer));
      if(((struct s_event_param_battle*)event->param)->fond!=NULL)
        strcpy(buffer,((struct s_event_param_battle*)event->param)->fond);
      fwrite(&buffer,63,sizeof(char),f);

      memset(&buffer,0,sizeof(buffer));
      if(((struct s_event_param_battle*)event->param)->fondu!=NULL)
        strcpy(buffer,((struct s_event_param_battle*)event->param)->fondu);
      fwrite(&buffer,63,sizeof(char),f);

      for(i=0;i<((struct s_event_param_battle*)event->param)->n_monstre;i++)
      {
        fwrite(&((struct s_event_param_battle*)event->param)->monstres[i].hp,
               sizeof(unsigned int), 1, f);
        fwrite(&((struct s_event_param_battle*)event->param)->monstres[i].hpmax,
               sizeof(unsigned int), 1, f);
        fwrite(&((struct s_event_param_battle*)event->param)->monstres[i].mp,
               sizeof(unsigned int), 1, f);
        fwrite(&((struct s_event_param_battle*)event->param)->monstres[i].mpmax,
               sizeof(unsigned int), 1, f);
        fwrite(&((struct s_event_param_battle*)event->param)->monstres[i].vitesse,
               sizeof(unsigned int), 1, f);
        fwrite(&((struct s_event_param_battle*)event->param)->monstres[i].force,
               sizeof(unsigned int), 1, f);
        fwrite(&((struct s_event_param_battle*)event->param)->monstres[i].dexterite,
               sizeof(unsigned int), 1, f);
        fwrite(&((struct s_event_param_battle*)event->param)->monstres[i].precision,
               sizeof(unsigned int), 1, f);
        fwrite(&((struct s_event_param_battle*)event->param)->monstres[i].intelligence,
               sizeof(unsigned int), 1, f);
        fwrite(&((struct s_event_param_battle*)event->param)->monstres[i].n_attaques,
               sizeof(unsigned int), 1, f);
        for(j=0;j<((struct s_event_param_battle*)event->param)->monstres[i].n_attaques;j++)
          fwrite(&((struct s_event_param_battle*)event->param)->monstres[i].attaques[i],
                 sizeof(unsigned int), 1, f);

        memset(&buffer,0,sizeof(buffer));
        strcpy(buffer,"battlers/051-Undead01.bmp");
        fwrite(&buffer,63,sizeof(char),f);
      }
      break;
    case EVENT_SHOP:
      fwrite(&((struct s_event_param_shop*)event->param)->n_item, sizeof(unsigned int), 1, f);
      for(i=0;i<((struct s_event_param_shop*)event->param)->n_item;i++)
      {
        fwrite(&((struct s_event_param_shop*)event->param)->itemId[i], sizeof(unsigned int), 1, f);
        fwrite(&((struct s_event_param_shop*)event->param)->coutItem[i], sizeof(unsigned int), 1, f);
			}
      for(i=0;i<MAX_ITEMS;i++)
        fwrite(&((struct s_event_param_shop*)event->param)->prixVente[i], sizeof(unsigned int), 1, f);
      break;
    case EVENT_INN:
      fwrite(&((struct s_event_param_inn*)event->param)->cout, sizeof(unsigned int), 1, f);
      break;
    case EVENT_CHANGETILE:
      fwrite(&((struct s_event_param_changetile*)event->param)->x, sizeof(unsigned int), 1, f);
      fwrite(&((struct s_event_param_changetile*)event->param)->y, sizeof(unsigned int), 1, f);
      fwrite(&((struct s_event_param_changetile*)event->param)->newtile, sizeof(unsigned int), 1, f);
      break;
  }
}

void EventStaticData(struct s_lib_event *libevent, struct s_map *map, struct s_lib_sprite *libsprite)
{
     GET_MAPEVENT(2,2,map,libevent).type=EVENT_DIALOG;
     GET_MAPEVENT(2,2,map,libevent).onaction=EVENT_ACTION_AUTO;
     GET_MAPEVENT(2,2,map,libevent).proba=100;
     GET_MAPEVENT(2,2,map,libevent).player_anim=-1;
     GET_MAPEVENT(2,2,map,libevent).sound=NULL;
     GET_MAPEVENT(2,2,map,libevent).is_unique=1;
     GET_MAPEVENT(2,2,map,libevent).next=NULL;
     GET_MAPEVENT(2,2,map,libevent).param=calloc(1,sizeof(struct s_event_param_dialog));
     ((struct s_event_param_dialog*)GET_MAPEVENT(2,2,map,libevent).param)->dialog=(char*)strdup("Test de dialogue\nSur plusieurs lignes...");
     
     
     GET_MAPEVENT(2,3,map,libevent).type=EVENT_TELEPORT;
     GET_MAPEVENT(2,3,map,libevent).onaction=EVENT_ACTION_ONKEYPRESS;
     GET_MAPEVENT(2,3,map,libevent).proba=100;
     GET_MAPEVENT(2,3,map,libevent).player_anim=-1;
     GET_MAPEVENT(2,3,map,libevent).sound=FSOUND_Sample_Load(FSOUND_FREE,"son/tornade.wav",FSOUND_NORMAL,0,0);
     GET_MAPEVENT(2,3,map,libevent).is_unique=0;
     GET_MAPEVENT(2,3,map,libevent).next=NULL;
     GET_MAPEVENT(2,3,map,libevent).param=calloc(1,sizeof(struct s_event_param_teleport));
     ((struct s_event_param_teleport*)GET_MAPEVENT(2,3,map,libevent).param)->filename=(char*)strdup("map/empty");
     ((struct s_event_param_teleport*)GET_MAPEVENT(2,3,map,libevent).param)->startX=128;
     ((struct s_event_param_teleport*)GET_MAPEVENT(2,3,map,libevent).param)->startY=128;
     
     libevent->sprite_event[1].type=EVENT_TELEPORT;
     libevent->sprite_event[1].onaction=EVENT_ACTION_AUTO;
     libevent->sprite_event[1].proba=100;
     libevent->sprite_event[1].player_anim=-1;
     libevent->sprite_event[1].sound=FSOUND_Sample_Load(FSOUND_FREE,"son/tornade.wav",FSOUND_NORMAL,0,0);
     libevent->sprite_event[1].is_unique=0;
     libevent->sprite_event[1].next=NULL;
     libevent->sprite_event[1].param=calloc(1,sizeof(struct s_event_param_teleport));
     ((struct s_event_param_teleport*)libevent->sprite_event[1].param)->filename=NULL;
     ((struct s_event_param_teleport*)libevent->sprite_event[1].param)->startX=128;
     ((struct s_event_param_teleport*)libevent->sprite_event[1].param)->startY=128;

}


/* Code spécifique à chaque événement */
void DoEventTeleport(struct s_event *event, struct s_lib_event *libevent, struct s_map *map, struct s_lib_sprite *libsprite)
{
     char *file_map, *file_sprite, *file_event;
     struct s_event_param_teleport *param;
     unsigned int startX, startY;
     
     param=event->param;
     if(param->filename==NULL)
     {
        SpriteMove(&libsprite->sprites[0], param->startX, param->startY, map);
     } else {
        file_map=calloc(strlen(param->filename)+5,sizeof(char));
        strcpy(file_map,param->filename);
        strcat(file_map, ".map");
        file_sprite=calloc(strlen(param->filename)+8,sizeof(char));
        strcpy(file_sprite,param->filename);
        strcat(file_sprite, ".sprite");
        file_event=calloc(strlen(param->filename)+7,sizeof(char));
        strcpy(file_event,param->filename);
        strcat(file_event, ".event");
        
        startX=param->startX, startY=param->startY;
            
        FreeMap(map);
        FreeSprite(libsprite);
        FreeLibEvent(libevent,map,libsprite);
        
        LoadSprite(libsprite, file_sprite);
        
        LoadMap(map, file_map);
        
        LoadEvent(libevent,map,libsprite,file_event);
        
        SpriteMove(&libsprite->sprites[0],startX,startY,map);
        
        free(file_map);
        free(file_sprite);
        free(file_event);
     }
}

void DoEventDialog(struct s_event *event, struct s_gui *gui)
{
     struct s_event_param_dialog *param;
     SDL_Color color={255,255,255};
     param=event->param;
     
     DrawDialog(gui, param->dialog,color);
}

void DoEventBattle(struct s_event *event, struct s_map *map, struct s_player *player, struct s_lib_attaque *libattaque, struct s_lib_sprite *libsprite, struct s_gui *gui)
{
  if(DoFight(event->param, player, libattaque, libsprite, gui))
  {
    FMUSIC_StopSong(map->bg_sound);
    exit(0);
  }
}

void DoEventShop(struct s_event *event, struct s_map *map, struct s_lib_sprite *libsprite, struct s_player *player, struct s_gui *gui)
{
  struct s_event_param_shop *param;
	int res;
  SDL_Color color={255,255,255};
  param=event->param;
     
  while((res=DrawInput(map, libsprite, gui, color, "Bonjour, que puis-je faire pour vous?", 3, "Acheter", "Vendre", "Rien pour le moment"))!=2)
  {
		if(res==0) /* Achat */
		{
      DrawShopAchat(map, libsprite, player, gui, color, "Quel objet souhaitez-vous acquérir?", param->n_item, param->itemId, param->coutItem);
		} else {   /* Vente */
	    while(DrawShopVente(map,libsprite,player,gui,color,"Quel objet souhaitez-vous vendre?", param->prixVente)==2)
  	  {
        HandleMap(map);
        HandleSprites(0, libsprite, map);
	    }
		}
  }
  HandleMap(map);
  HandleSprites(0, libsprite, map);
	DrawDialog(gui, "Merci de votre visite.", color);
  SDL_Delay(100);
}

void DoEventInn(struct s_event *event, struct s_map *map, struct s_lib_sprite *libsprite, struct s_player *player, struct s_gui *gui)
{
  struct s_event_param_inn *param;
  int i;
  SDL_Color color={255,255,255};
  param=event->param;

  if(DrawInput(map, libsprite, gui, color, "Bonjour, que puis-je faire pour vous?", 2, "J'aimerais me reposer", "Rien pour le moment")==0)
  {
    HandleMap(map);
    HandleSprites(0, libsprite, map);
    if(player->argent>=param->cout)
    {
      player->argent-=param->cout;
      for(i=0;i<MAX_JOUEURS;i++)
        player->joueurs[i].hp=player->joueurs[i].hpmax, player->joueurs[i].mp=player->joueurs[i].mpmax;
			DrawDialog(gui, "Vous êtes reposé!\n\n  Merci d'être venu!", color);
    } else
			DrawDialog(gui, "Je suis désolé, mais vous n'avez pas assez d'argent.", color);
  } else {
    HandleMap(map);
    HandleSprites(0, libsprite, map);
 		DrawDialog(gui, "Repassez quand vous le souhaitez.", color);
  }
  
  SDL_Delay(100);
}

void DoEventChangetile(struct s_event *event, struct s_map *map)
{
  struct s_event_param_changetile *param;
  param=event->param;
  map->data[param->x+param->y*map->width]=param->newtile;;
}


void DEBUG_PrintLibEvent(struct s_lib_event *libevent, struct s_map *map, struct s_lib_sprite *libsprite)
{
  unsigned int i;
  
  printf("** Evenements GLOBAUX **\n");
  DEBUG_PrintEvent(&libevent->global_event);
  
  printf("** Evenements CARTES **\n");
  for(i=0;i<map->width*map->height;i++)
    DEBUG_PrintEvent(&libevent->map_event[i]);
    
  printf("** Evenements SPRITES **\n");
  for(i=0;i<libsprite->n_sprite;i++)
    DEBUG_PrintEvent(&libevent->sprite_event[i]);
}

void DEBUG_PrintEvent(struct s_event *event)
{
   printf("---------------------------------------\n");
   printf("Type: %d\n",event->type);
   printf("Action: %d\n",event->onaction);
   printf("Probabilité: %d\n",event->proba);
   
   printf("---   PARAMETRES   --- [%d]\n", (int)event->param);
   printf("--- FIN PARAMETRES --- \n");
   printf("Animation du joueur: %d\n",event->player_anim);

   printf("Son: %d\n", (int)event->sound);
   
   printf("Est unique: %d\n", event->is_unique);
   
   printf("Prochain: %d %s\n", (int)event->next,(event->next==NULL)?"":"NON-NUL");
   printf("---------------------------------------\n");
}
