/*
** fight.h
**
** Système de combat tour par tour.
**
*/

#ifndef FIGHT_H_
# define FIGHT_H_

# include <SDL/SDL.h>

# include "sprite.h"
# include "player.h"
# include "event.h"
# include "fondu.h"
# include "main.h"

# define PosXMonstre(n,sx) 800-((sx)/4)-(((n)+1)*(sx))
# define PosYMonstre(n,sy) 400-((sy)/4)-(((n)+1)*(sy))
# define PosXJoueur(n,sx)  ((sx)/4)+((n)*(sx))
# define PosYJoueur(n,sy)  400-((sy)/4)-(((n)+1)*(sy))

int DoFight(
  struct s_event_param_battle *param,
  struct s_player             *player,
  struct s_lib_attaque        *libattaque,
  struct s_lib_sprite         *libsprite,
  struct s_gui                *gui);
  
int FightMenu(struct s_event_param_battle *param,
              struct s_player *player,
              struct s_lib_attaque *libattaque,
              struct s_gui *gui);
              
void BlitFight(struct s_event_param_battle *param,
               struct s_player             *player,
               struct s_lib_attaque        *libattaque,
               SDL_Surface                 *fond,
               int                         *attaque,
               int                         *cible,
               int                         *tour);
              
void BlitJoueurs(struct s_player *player);
void BlitMonstres(struct s_monstre **monstres, int n_monstre);
int BlitAnimation(struct s_lib_attaque *libattaque, int attaque, int x, int y);
void BlitDegats(char *string, int nx, int ny, int reinit, struct s_gui *gui);
  

#endif /* !FIGHT_H_ */
