/*
** fondu.h
**
** Gestion des fondus enchaînés.
**
*/
#ifndef FONDU_H_
# define FONDU_H_

# include <SDL/SDL.h>

int FonduEnchaine(
  SDL_Surface *avant,
  SDL_Surface *apres,
  char *file_transition,
  unsigned int duree,
  unsigned int vitesse);

#endif /* !FONDU_H_ */
