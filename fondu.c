/*
** fondu.c
**
** Gestion des fondus enchaînés.
**
*/
#include "fondu.h"

int FonduEnchaine(
  SDL_Surface *avant,
  SDL_Surface *apres,
  char *file_transition,
  unsigned int duree,
  unsigned int vitesse)
{
  SDL_Event event;
  SDL_Surface *copie, *transition;
  unsigned int rmask, gmask, bmask, amask,
               time,
               i;
  Uint32       temp,
               *pt, *pta; /* pointeurs vers les surfaces */
  Uint8        r,g,b,a;
  Uint8        comp;
  Uint8 v;

  /* Quelques vérifications pour éviter les principales
     sources de segfault */
  if((!avant)||(!apres)) return 0;
  if(avant->w!=apres->w) return 0;
  if(avant->h!=apres->h) return 0;

  /* On copie la surface "avant" afin de pouvoir traviller
     dessus sans crainte */
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  rmask = 0xff000000;
  gmask = 0x00ff0000;
  bmask = 0x0000ff00;
  amask = 0x000000ff;
#else
  rmask = 0x000000ff;
  gmask = 0x0000ff00;
  bmask = 0x00ff0000;
  amask = 0xff000000;
#endif
  copie = SDL_CreateRGBSurface(SDL_SWSURFACE, 800, 600, 32,
                                   rmask, gmask, bmask, amask);
  SDL_BlitSurface(avant, NULL, copie, NULL);

  /* Chargement de la transition */
  transition = SDL_LoadBMP(file_transition);
  transition = SDL_ConvertSurface(transition,copie->format,SDL_SWSURFACE|SDL_SRCALPHA);
  if(!transition)
  {
    SDL_FreeSurface(copie);
    return 0;
  }

  /* On fixe la transparence de la surface */
  SDL_SetAlpha(copie, SDL_SRCALPHA, SDL_ALPHA_OPAQUE);

  /* Début de la boucle */
  time=SDL_GetTicks();
  v=vitesse;
  while((SDL_GetTicks()-time)<duree)
  {

    while(SDL_PollEvent(&event))
    {
      switch(event.type)
      {
        case SDL_QUIT:
          SDL_FreeSurface(transition);
          SDL_FreeSurface(copie);
          return 1;
          break;
      }
    }

    /* Affiche fond */
    SDL_BlitSurface(apres, NULL, SDL_GetVideoSurface(), NULL);

    /* Mise à jour de la couche alpha */
    SDL_LockSurface(copie);
    SDL_LockSurface(transition);

    pt=((Uint32*)copie->pixels);
    pta=((Uint32*)transition->pixels);

    for(i=0;i<(copie->w*copie->h);i++)
    {
      SDL_GetRGBA(pt[i],copie->format,&r,&g,&b,&a);

      /* je ne récupère qu'une composante... c'est du niveau de gris! */
      temp=pta[i]&transition->format->Rmask;
      temp=temp>>transition->format->Rshift;
      temp=temp<<transition->format->Rloss;
      comp=(Uint8)temp;

      if(comp<v) comp=v;
      if(a>(comp/v))
        a-=(comp/v==0)?1:(comp/v);
      else
        a=0;

      pt[i]=SDL_MapRGBA(copie->format,r,g,b,a);
    }
    SDL_UnlockSurface(transition);
    SDL_UnlockSurface(copie);
    
    /* On augmente petit à petit la vitesse afin d'éviter
       les fins de transition trop longs */
    if(v>1)
      v--;

    /* On affiche la deuxième surface */
    SDL_BlitSurface(copie, NULL, SDL_GetVideoSurface(), NULL);

    /* Inversion des tampons */
    SDL_Flip(SDL_GetVideoSurface());
  }

  SDL_FreeSurface(transition);
  SDL_FreeSurface(copie);

  return 0;
}
