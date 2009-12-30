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
  SDL_Event event;
  struct s_map map;
  int isdone;
    
  if(Init()>0)
     return 1;


  RandomizeMap(&map,50,50,"tiles/mchip2.bmp");

  SDL_EnableKeyRepeat(100, 10);

  isdone=0;
  while (!isdone)
  {
    /* Lecture des évènements dans la queue d'évènements */
    while (SDL_PollEvent (&event))
    {
      MapEvent(&event, &map, 185);
      
      switch (event.type)
      {
      case SDL_KEYDOWN:
	    if(event.key.state==SDL_PRESSED)
	    {
          if(event.key.keysym.sym==SDLK_ESCAPE)
	        isdone = 1;
	      else if (event.key.keysym.sym==SDLK_F1)
	        SDL_SaveBMP(SDL_GetVideoSurface(),"screenshot.bmp");

	   }
      break;
      case SDL_QUIT:
        isdone = 1;
	  break;
      }
    }

    HandleMap(&map, 185);
    
    SDL_Flip(SDL_GetVideoSurface());
  }
  
  FreeMap(&map);
  return 0;
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
    if(SDL_Init(SDL_INIT_VIDEO)==-1)
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
    SDL_WM_SetCaption("RPG 2D :: Editeur de cartes - Games Creators Network (http://www.games-creators.org)","RPG 2d");
    
    /* Initialisation de FMOD */
    if (FSOUND_GetVersion() < FMOD_VERSION)
    {
        fprintf(stderr, "Mauvaise version de la librairie. Vous devriez utiliser la version %.02f\n", FMOD_VERSION);
        exit(1);
    }
    
    /*
        INITIALIZE
    */
    if (!FSOUND_Init(32000, 64, 0))
    {
        fprintf(stderr,"Erreur à l'initialisation de FMOD: %s\n", FMOD_ErrorString(FSOUND_GetError()));
        exit(1);
    }
   
    return 0;
}

/*
** Libération du programme
*/
void Free()
{
    SDL_Quit();
}
