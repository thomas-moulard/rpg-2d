/*
** gui.h
**
** Gestion du GUI, des menus, des dialogues...
**
** Rôle:
** - dialogue
** - menus
** - GUI (barre de vie par exemple)
*/

#ifndef GUI_H
# define GUI_H

# include <FMOD/fmod.h>
# include <SDL/SDL.h>
# include <SDL/SDL_ttf.h>

# include <stdarg.h>

# include "map.h"
# include "sprite.h"
# include "player.h"

struct s_gui
{
   TTF_Font *font;
   SDL_Surface *surf;
   SDL_Surface *menu;
};

void InitGui(struct s_gui *gui);
void FreeGui(struct s_gui *gui);
void DrawDialog(struct s_gui *gui, char *dialog, SDL_Color color);

int DrawInput(struct s_map *map, struct s_lib_sprite *libsprite,
              struct s_gui *gui, SDL_Color color, char *dialog, int n_input, ...);
int DrawShopAchat(struct s_map *map, struct s_lib_sprite *libsprite, struct s_player *player,
                   struct s_gui *gui, SDL_Color color, char *dialog,
									 int n_item, unsigned int *itemId, unsigned int *coutItem);
int DrawShopVente(struct s_map *map, struct s_lib_sprite *libsprite, struct s_player *player,
                   struct s_gui *gui, SDL_Color color, char *dialog, unsigned int *prixVente);

int Menu(struct s_player *player, struct s_gui *gui);
void MenuBlitLeftPanel(struct s_gui *gui, struct s_player *player, unsigned char selection);
int MenuInventaire(struct s_gui *gui, struct s_player *player, int selectiong, FSOUND_SAMPLE *son);
void MenuBlitTopPanel(struct s_gui *gui, unsigned char selection);
int MenuEquiper(struct s_gui *gui, struct s_player *player, int selectiong, FSOUND_SAMPLE *son);

#endif /* !GUI_H */
