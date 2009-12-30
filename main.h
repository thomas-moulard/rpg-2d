/*
** main.h
**
** Fichier principal.
**
** Rôle:
** - fonction main()
*/

#ifndef MAIN_H
# define MAIN_H

# include <SDL/SDL.h>
# include <SDL/SDL_ttf.h>

# include <FMOD/fmod.h>
# include <FMOD/fmod_errors.h>

# include <stdio.h>
# include <stdlib.h>

# include "map.h"
# include "sprite.h"
# include "event.h"
# include "gui.h"
# include "player.h"


void GameOver();

int Init();
void Free();



#endif /* !MAIN_H */
