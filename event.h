/*
** event.h
**
** Gère le système d'événements
**
** ATTENTION: à initialiser après la carte et les sprites.
**            à libérer avant la carte et les sprites.
**
** Rôle:
** - initialise et libère les événements
** - gère le déclenchement et le déroulenement des événements
*/

#ifndef EVENT_H
# define EVENT_H

# include <stdlib.h>
# include <string.h>

# include <SDL/SDL.h>
# include <FMOD/fmod.h>

# include "gui.h"
# include "map.h"
# include "sprite.h"
# include "player.h"


# define GET_MAPEVENT(x,y,map,event) event->map_event[(x)+(y)*map->width]
# define RND_EVENT_SUCCESS(event) ( ((event).proba==100) || ( (rand()%101) < (event).proba) )
# define IS_NEAR(x1,x2,y1,y2,dist_max) ( (abs((x1)-(x2))<dist_max) && (abs((y1)-(y2))<dist_max) )


enum e_event_type {
     EVENT_NULL=0,
     EVENT_TELEPORT,
     EVENT_DIALOG,
     EVENT_BATTLE,
     EVENT_SHOP,
     EVENT_INN,
     EVENT_CHANGETILE
};

enum e_event_action {
     EVENT_ACTION_AUTO=0,
     EVENT_ACTION_ONKEYPRESS
};

struct s_event
{
   enum e_event_type type;
   enum e_event_action onaction;
   unsigned char proba;
   
   void *param;
   
   signed int player_anim;

   FSOUND_SAMPLE * sound;
   
   unsigned char is_unique;
   
   struct s_event *next;
};

struct s_lib_event {
   struct s_event global_event;
   struct s_event *map_event;
   struct s_event *sprite_event;

};

/* Paramètres des événements */
struct s_event_param_teleport {
   char   *filename;
   unsigned int startX,startY;
};

struct s_event_param_dialog {
   char   *dialog;
};

struct s_event_param_battle {
  int              n_monstre;
  char             *fond;
  char             *fondu;
  struct s_monstre *monstres;
};

struct s_event_param_shop {
	 unsigned int n_item;
	 unsigned int *itemId;
	 unsigned int *coutItem;
	 unsigned int *prixVente;
};

struct s_event_param_inn {
   unsigned int cout;
};

struct s_event_param_changetile {
   unsigned int x,y;
   unsigned int newtile;
};
/* fin paramètres */

void InitLibEvent(struct s_lib_event *libevent, struct s_map *map, struct s_lib_sprite *libsprite);
void InitEvent(struct s_event *event);

void FreeLibEvent(struct s_lib_event *libevent, struct s_map *map, struct s_lib_sprite *libsprite);
void FreeEvent(struct s_event *event);

void EventStaticData(struct s_lib_event *libevent, struct s_map *map, struct s_lib_sprite *libsprite);

void HandleEvent(struct s_lib_event   *libevent,
                 struct s_map         *map,
                 struct s_lib_sprite  *libsprite,
                 struct s_player      *player,
                 struct s_lib_attaque *libattaque,
                 struct s_gui         *gui);
void Event(SDL_Event            *sdlevent,
           struct s_lib_event   *libevent,
           struct s_map         *map,
           struct s_lib_sprite  *libsprite,
           struct s_player      *player,
           struct s_lib_attaque *libattaque,
           struct s_gui         *gui);
int DoEvent(struct s_event       *event,
            struct s_lib_event   *libevent,
            struct s_map         *map,
            struct s_lib_sprite  *libsprite,
            struct s_player      *player,
            struct s_lib_attaque *libattaque,
            struct s_gui         *gui);

void LoadEvent(struct s_lib_event *libevent, struct s_map *map, struct s_lib_sprite *libsprite, char *filename);
void ReadEvent(FILE *f, struct s_event *event);
void SaveEvent(struct s_lib_event *libevent, struct s_map *map, struct s_lib_sprite *libsprite, char *bg_sound, char *filename);
void WriteEvent(FILE *f, struct s_event *event, char *bg_sound);


/* Code spécifique à chaque événement */
void DoEventTeleport(struct s_event *event, struct s_lib_event *libevent, struct s_map *map, struct s_lib_sprite *libsprite);
void DoEventDialog(struct s_event *event, struct s_gui *gui);
void DoEventBattle(struct s_event *event, struct s_map *map, struct s_player *player, struct s_lib_attaque *libattaque, struct s_lib_sprite *libsprite, struct s_gui *gui);

void DoEventShop(struct s_event *event, struct s_map *map, struct s_lib_sprite *libsprite, struct s_player *player, struct s_gui *gui);
void DoEventInn(struct s_event *event, struct s_map *map, struct s_lib_sprite *libsprite, struct s_player *player, struct s_gui *gui);

void DoEventChangetile(struct s_event *event, struct s_map *map);


/* Débug */
void DEBUG_PrintLibEvent(struct s_lib_event *libevent, struct s_map *map, struct s_lib_sprite *libsprite);
void DEBUG_PrintEvent(struct s_event *event);


/* Suppr. warning */
//extern _CRTIMP char* __cdecl	strdup (const char*) __MINGW_ATTRIB_MALLOC;
extern int DoFight(
  struct s_event_param_battle *param,
  struct s_player             *player,
  struct s_lib_attaque        *libattaque,
  struct s_lib_sprite         *libsprite,
  struct s_gui                *gui);

#endif /* !EVENT_H */
