/*
** main.c
**
** Fichier principal.
**
** Rôle:
** - initialisation et libération des librairies.
** - fonction main()
*/

#include "main.h"


int main(int argc, char **argv)
{
  SDL_Event             event;
  struct s_player       player;
  struct s_map          map;
  struct s_lib_sprite   libsprite;
  struct s_lib_event    libevent;
  struct s_gui          gui;
  struct s_lib_attaque  libattaque;
  
  int                   isdone;
    
  if(Init()>0)
     return 1;

InitGui(&gui);

InitPlayer(&player);

InitLibAttaque(&libattaque);
ChargeAttaques(&libattaque);

LoadSprite(&libsprite,"map/01.sprite");
/*
InitSprite(&libsprite);
SpriteStaticData(&libsprite);
SaveSprite(&libsprite,"sprites/player.bmp","map/01.sprite");
*/


LoadMap(&map, "map/01.map");
/*
RandomizeMap(&map,100,100,"tiles/mchip0.bmp");
SaveMap(&map, "map/save.map", "musique/Opening1.mid", "tiles/mchip0.bmp");
*/


LoadEvent(&libevent,&map,&libsprite,"map/01.event");
/*
InitLibEvent(&libevent,&map,&libsprite);
EventStaticData(&libevent,&map,&libsprite);
SaveEvent(&libevent,&map,&libsprite,"son/tornade.wav","map/01.event");
*/

  SDL_EnableKeyRepeat(10,10);
  isdone=0;
  while (!isdone)
  {
    /* Lecture des évènements dans la queue d'évènements */
    while (SDL_PollEvent (&event))
    {
      MapEvent(&event, &map);
      SpriteEvent(&event, &map, &libsprite);
      Event(&event, &libevent, &map, &libsprite, &player, &libattaque, &gui);

      switch (event.type)
      {
      case SDL_KEYUP:
	    if(event.key.state==SDL_RELEASED)
	    {
          if(event.key.keysym.sym==SDLK_ESCAPE)
	        isdone = Menu(&player, &gui);
	      else if (event.key.keysym.sym==SDLK_F1)
	        SDL_SaveBMP(SDL_GetVideoSurface(),"screenshot.bmp");

	   }
      break;
      case SDL_QUIT:
        isdone = 1;
	  break;
      }
    }

    HandleMap(&map);
    HandleSprites(0, &libsprite, &map);
    HandleEvent(&libevent, &map, &libsprite, &player, &libattaque, &gui);
    SDL_Flip(SDL_GetVideoSurface());
  }
  
  FreeLibEvent(&libevent, &map, &libsprite);    
  FreeMap(&map);
  FreeSprite(&libsprite);
  FreeLibAttaque(&libattaque);
  FreePlayer(&player);
  FreeGui(&gui);
  
  return 0;
}

/*
** Le jeu est fini!
*/
void GameOver()
{
  SDL_Event     event;
  SDL_Surface   *surf;
  FMUSIC_MODULE *bg_son;
  
  surf=SDL_LoadBMP("img/001-Gameover01.bmp");
  bg_son = FMUSIC_LoadSong("musique/006-Defeat02.mid");
  FMUSIC_SetLooping(bg_son,0);
  FMUSIC_PlaySong(bg_son);

  while(1)
  {
    while(SDL_PollEvent(&event))
    {
      if(event.type==SDL_QUIT)
      {
        FMUSIC_StopSong(bg_son);
        FMUSIC_FreeSong(bg_son);
        exit(0);
      }
    }
    SDL_BlitSurface(surf, NULL, SDL_GetVideoSurface(), NULL);
    SDL_Flip(SDL_GetVideoSurface());
  }
}

/*
** Initialisation du programme
*/
int Init()
{
    SDL_Surface *screen;
    
    /* Appel de Free lors de la fermeture du programme */
    atexit(Free);
    
    /* Initialisation de SDL */
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER)==-1)
    {
       perror("Impossible d'initialiser SDL.\n");
       return 1;
    }

    /* Initialisation de la fenêtre */
    screen = SDL_SetVideoMode(800, 600, 32, SDL_HWSURFACE|SDL_DOUBLEBUF);
    if(screen==NULL)
    {
       perror("Erreur lors de la création de la fenêtre.\n");
       return 1;
    }
     
    /* Change le titre de la fenêtre et de l'icône */
    SDL_WM_SetCaption("RPG 2D - Games Creators Network (http://www.games-creators.org)","RPG 2d");
    
    /* Initialisation de SDL_ttf */
    if(TTF_Init()==-1) {
       fprintf(stderr,"Erreur lors de l'initialisation de SDL_TTF: %s\n", TTF_GetError());
       return 1;
    }

    
    /* Initialisation de FMOD */
    if (FSOUND_GetVersion() < FMOD_VERSION)
    {
        fprintf(stderr, "Mauvaise version de la librairie. Vous devriez utiliser la version %.02f\n", FMOD_VERSION);
        exit(1);
    }
    
    /*
        INITIALIZE
    */

    FSOUND_SetOutput(FSOUND_OUTPUT_ALSA);


    if (!FSOUND_Init(32000, 64, 0))
    {
        fprintf(stderr,"Erreur à l'initialisation de FMOD: %s\n", FMOD_ErrorString(FSOUND_GetError()));
        exit(1);
    }
    FSOUND_SetLoopMode(FSOUND_ALL,FSOUND_LOOP_OFF);
   
    return 0;
}

/*
** Libération du programme
*/
void Free()
{
    TTF_Quit();
    SDL_Quit();
}
