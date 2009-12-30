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

# include <FMOD/fmod.h>
# include <FMOD/fmod_errors.h>

# include <stdio.h>
# include <stdlib.h>

# include "map.h"

int Init();
void Free();

#endif /* !MAIN_H */
