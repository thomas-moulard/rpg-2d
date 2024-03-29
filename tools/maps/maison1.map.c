#include <stdio.h>
#include <stdlib.h>

# include "../gui.h"
# include "../map.h"
# include "../event.h"
# include "../sprite.h"

/* *****************
** ** CONFIG ZONE **
** ***************** */
/*
** Configurez ce qui suit afin de personnaliser les
** fichiers g�n�r�s.
**/

/*
** Indiquez ici le nom des fichiers qui vont �tre
** g�n�r�s.
**
** Rapple: pour une carte compl�te, utiliser le
**         m�me nom avec des extensions diff�rentes
*/
# define MAPFILE "map/generator.map"
# define SPRITEFILE "map/generator.sprite"
# define EVENTFILE "map/generator.event"

/* Chipset utilis� par la carte */
# define CHIPSET "tiles/mchip0.bmp"
/* Taille de la carte */
# define MAP_SIZE_X 50
# define MAP_SIZE_Y 50
/* Musique de la carte
** <!> r�enregistrer la carte �crase cette donn�e
*/
# define MAP_MUSIQUE "musique/Opening1.mid"

/*
** Modification de la carte "� la main"/
** Lors de l'appel � cette fonction, map
** contient d�j� une carte al�atoire de
** taille MAP_SIZE_X*MAP_SIZE_Y
*/
void StaticMap(struct s_map *map, struct s_lib_sprite *libsprite, struct s_lib_event *libevent)
{
}

/*
** La planche utilis�e par *tous* vos sprites
*/
# define SPRITE_IMG "sprites/player.bmp"

/*
** Votre biblioth�que de sprite doit �tre
** g�n�r�e ici.
*/
void StaticSprite(struct s_map *map, struct s_lib_sprite *libsprite, struct s_lib_event *libevent)
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

     libsprite->sprites[0].surf=SDL_LoadBMP(SPRITE_IMG);
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

      /* SPRITE 1: pnj t�l�port */
     libsprite->sprites[1].x=352;
     libsprite->sprites[1].y=128;

     libsprite->sprites[1].offsetG=4;
     libsprite->sprites[1].offsetD=15;
     libsprite->sprites[1].offsetH=20;
     libsprite->sprites[1].offsetB=10;

     libsprite->sprites[1].surf=SDL_LoadBMP(SPRITE_IMG);
     SDL_SetColorKey(libsprite->sprites[0].surf, SDL_SRCCOLORKEY, SDL_MapRGB(libsprite->sprites[0].surf->format,0,0,0));
     libsprite->sprites[1].width=24;
     libsprite->sprites[1].height=32;
     libsprite->sprites[1].n_anim=4;
     libsprite->sprites[1].cur_anim=ANIMATION_BAS;
     libsprite->sprites[1].ispaused_anim=1;
     libsprite->sprites[1].anim=malloc(libsprite->sprites[0].n_anim*sizeof(struct s_anim));

     /* ANIM 0: haut */
     libsprite->sprites[1].anim[0].cur_frame=0;
     libsprite->sprites[1].anim[0].n_frame=3;
     libsprite->sprites[1].anim[0].lastfrmupdate=SDL_GetTicks();
     libsprite->sprites[1].anim[0].delaybetweenframes = 100;
     libsprite->sprites[1].anim[0].frame=malloc(libsprite->sprites[0].anim[0].n_frame*sizeof(struct s_frame));

     libsprite->sprites[1].anim[0].frame[0].x=0;
     libsprite->sprites[1].anim[0].frame[0].y=128;

     libsprite->sprites[1].anim[0].frame[1].x=24;
     libsprite->sprites[1].anim[0].frame[1].y=128;

     libsprite->sprites[1].anim[0].frame[2].x=48;
     libsprite->sprites[1].anim[0].frame[2].y=128;

     /* ANIM 1: droite */
     libsprite->sprites[1].anim[1].cur_frame=0;
     libsprite->sprites[1].anim[1].n_frame=3;
     libsprite->sprites[1].anim[1].lastfrmupdate=SDL_GetTicks();
     libsprite->sprites[1].anim[1].delaybetweenframes = 100;
     libsprite->sprites[1].anim[1].frame=malloc(libsprite->sprites[0].anim[1].n_frame*sizeof(struct s_frame));

     libsprite->sprites[1].anim[1].frame[0].x=0;
     libsprite->sprites[1].anim[1].frame[0].y=160;

     libsprite->sprites[1].anim[1].frame[1].x=24;
     libsprite->sprites[1].anim[1].frame[1].y=160;

     libsprite->sprites[1].anim[1].frame[2].x=48;
     libsprite->sprites[1].anim[1].frame[2].y=160;

     /* ANIM 2: gauche */
     libsprite->sprites[1].anim[2].cur_frame=0;
     libsprite->sprites[1].anim[2].n_frame=3;
     libsprite->sprites[1].anim[2].lastfrmupdate=SDL_GetTicks();
     libsprite->sprites[1].anim[2].delaybetweenframes = 100;
     libsprite->sprites[1].anim[2].frame=malloc(libsprite->sprites[0].anim[2].n_frame*sizeof(struct s_frame));

     libsprite->sprites[1].anim[2].frame[0].x=0;
     libsprite->sprites[1].anim[2].frame[0].y=192;

     libsprite->sprites[1].anim[2].frame[1].x=24;
     libsprite->sprites[1].anim[2].frame[1].y=192;

     libsprite->sprites[1].anim[2].frame[2].x=48;
     libsprite->sprites[1].anim[2].frame[2].y=192;

     /* ANIM 3: bas */
     libsprite->sprites[1].anim[3].cur_frame=0;
     libsprite->sprites[1].anim[3].n_frame=3;
     libsprite->sprites[1].anim[3].lastfrmupdate=SDL_GetTicks();
     libsprite->sprites[1].anim[3].delaybetweenframes = 100;
     libsprite->sprites[1].anim[3].frame=malloc(libsprite->sprites[0].anim[3].n_frame*sizeof(struct s_frame));

     libsprite->sprites[1].anim[3].frame[0].x=0;
     libsprite->sprites[1].anim[3].frame[0].y=224;

     libsprite->sprites[1].anim[3].frame[1].x=24;
     libsprite->sprites[1].anim[3].frame[1].y=224;

     libsprite->sprites[1].anim[3].frame[2].x=48;
     libsprite->sprites[1].anim[3].frame[2].y=224;
}

# define EVENT_SON "son/tornade.wav"

void StaticEvent(struct s_map *map, struct s_lib_sprite *libsprite, struct s_lib_event *libevent)
{
     unsigned int i;

     GET_MAPEVENT(6,12,map,libevent).type=EVENT_TELEPORT;
     GET_MAPEVENT(6,12,map,libevent).onaction=EVENT_ACTION_AUTO;
     GET_MAPEVENT(6,12,map,libevent).proba=100;
     GET_MAPEVENT(6,12,map,libevent).player_anim=-1;
     GET_MAPEVENT(6,12,map,libevent).sound=FSOUND_Sample_Load(FSOUND_FREE,EVENT_SON,FSOUND_NORMAL,0,0);
     GET_MAPEVENT(6,12,map,libevent).is_unique=0;
     GET_MAPEVENT(6,12,map,libevent).next=NULL;
     GET_MAPEVENT(6,12,map,libevent).param=calloc(1,sizeof(struct s_event_param_teleport));
     ((struct s_event_param_teleport*)GET_MAPEVENT(6,12,map,libevent).param)->filename=(char*)strdup("map/village");
     ((struct s_event_param_teleport*)GET_MAPEVENT(6,12,map,libevent).param)->startX=88*32;
     ((struct s_event_param_teleport*)GET_MAPEVENT(6,12,map,libevent).param)->startY=3*32;
     
     
     GET_MAPEVENT(2,4,map,libevent).type=EVENT_CHANGETILE;
     GET_MAPEVENT(2,4,map,libevent).onaction=EVENT_ACTION_ONKEYPRESS;
     GET_MAPEVENT(2,4,map,libevent).proba=100;
     GET_MAPEVENT(2,4,map,libevent).player_anim=-1;
     GET_MAPEVENT(2,4,map,libevent).sound=FSOUND_Sample_Load(FSOUND_FREE,EVENT_SON,FSOUND_NORMAL,0,0);
     GET_MAPEVENT(2,4,map,libevent).is_unique=1;
     GET_MAPEVENT(2,4,map,libevent).next=NULL;
     GET_MAPEVENT(2,4,map,libevent).param=calloc(1,sizeof(struct s_event_param_changetile));
     ((struct s_event_param_changetile*)GET_MAPEVENT(2,4,map,libevent).param)->x=2;
     ((struct s_event_param_changetile*)GET_MAPEVENT(2,4,map,libevent).param)->y=3;
     ((struct s_event_param_changetile*)GET_MAPEVENT(2,4,map,libevent).param)->newtile=103;




     libevent->sprite_event[1].type=EVENT_SHOP;
     libevent->sprite_event[1].onaction=EVENT_ACTION_ONKEYPRESS;
     libevent->sprite_event[1].proba=100;
     libevent->sprite_event[1].player_anim=-1;
     libevent->sprite_event[1].sound=NULL;
     libevent->sprite_event[1].is_unique=0;
     libevent->sprite_event[1].next=NULL;
     libevent->sprite_event[1].param=calloc(1,sizeof(struct s_event_param_shop));
     ((struct s_event_param_shop*)libevent->sprite_event[1].param)->n_item=3;
     ((struct s_event_param_shop*)libevent->sprite_event[1].param)->itemId=malloc(5*sizeof(unsigned int));
     ((struct s_event_param_shop*)libevent->sprite_event[1].param)->itemId[0]=0;
     ((struct s_event_param_shop*)libevent->sprite_event[1].param)->itemId[1]=1;
     ((struct s_event_param_shop*)libevent->sprite_event[1].param)->itemId[2]=2;
     ((struct s_event_param_shop*)libevent->sprite_event[1].param)->itemId[3]=4;
     ((struct s_event_param_shop*)libevent->sprite_event[1].param)->itemId[4]=5;
     ((struct s_event_param_shop*)libevent->sprite_event[1].param)->coutItem=malloc(5*sizeof(unsigned int));
     ((struct s_event_param_shop*)libevent->sprite_event[1].param)->coutItem[0]=15;
     ((struct s_event_param_shop*)libevent->sprite_event[1].param)->coutItem[1]=50;
     ((struct s_event_param_shop*)libevent->sprite_event[1].param)->coutItem[2]=75;
     ((struct s_event_param_shop*)libevent->sprite_event[1].param)->coutItem[3]=75;
     ((struct s_event_param_shop*)libevent->sprite_event[1].param)->coutItem[4]=75;
     ((struct s_event_param_shop*)libevent->sprite_event[1].param)->coutItem[5]=75;
     ((struct s_event_param_shop*)libevent->sprite_event[1].param)->prixVente=malloc(MAX_ITEMS*sizeof(unsigned int));
     for(i=0;i<MAX_ITEMS;i++)
		   ((struct s_event_param_shop*)libevent->sprite_event[1].param)->prixVente[i]=50+(rand()%20);
}

/* *****************
** ** FIN         **
** ***************** */

int main(int argc, char **argv)
{
  struct s_map        map;
  struct s_lib_sprite libsprite;
  struct s_lib_event  libevent;

  printf("RPG 2D .:. Generateur de fichiers \n");
  printf("Games Creators Network - http://www.games-creators.org/\n");
  printf("Ce generateur permet de generer facilement les fichiers:\n");
  printf("o .event\n");
  printf("o .map\n");
  printf("o .sprite\n\n\n");
  printf("* SPRITES:\n");
  printf("- initialisation\n");
  InitSprite(&libsprite);
  printf("- chargement des donnees statiques\n");
  StaticSprite(&map,&libsprite,&libevent);
  printf("- sauvegarde\n");
  SaveSprite(&libsprite,SPRITE_IMG,SPRITEFILE);

  printf("* MAP:\n");
  printf("- g�n�ration aleatoire\n");
  RandomizeMap(&map,MAP_SIZE_X,MAP_SIZE_Y,CHIPSET);
  StaticMap(&map,&libsprite,&libevent);
  printf("- sauvegarde des donnees\n");
  SaveMap(&map, MAPFILE, MAP_MUSIQUE, CHIPSET);

  printf("* EVENEMENTS:\n");
  printf("- initialisation\n");
  InitLibEvent(&libevent,&map,&libsprite);
  printf("- chargement des donnees statiques\n");
  StaticEvent(&map,&libsprite,&libevent);
  printf("- sauvegarde des donnees\n");
  SaveEvent(&libevent,&map,&libsprite,EVENT_SON,EVENTFILE);

  printf("\n\n\nG�n�ration des donn�es r�alis�e avec succ�s...\n");
  return 0;
}

void GameOver() {} /* Fonction inutile dans le g�n�rateur, �vite les erreurs lors
                      de l'�dition des liens */
