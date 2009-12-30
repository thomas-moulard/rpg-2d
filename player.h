/*
** player.h
**
** Gestion du joueur
**
** Rôle:
** - gestion des stats relatives aux joueurs
** - gestion de l'inventaire
*/
#ifndef PLAYER_H
# define PLAYER_H

# include <FMOD/fmod.h>
# include <SDL/SDL.h>
# include <stdlib.h>
# include <string.h>

# define GetNbImgAttaque(x) (((x).surf->w/(x).tailleX)*((x).surf->h/(x).tailleY))

# define MAX_JOUEURS 4
# define MAX_ITEMS   200

enum e_type_attaque {
  ATTAQUE_NORMAL=0,
  ATTAQUE_MAGIQUE,
  ATTAQUE_COMBO
};

struct s_attaque {
  char                *nom;
  enum e_type_attaque type;
  unsigned int        duree;
  unsigned int        degats;
  unsigned int        tailleX;
  unsigned int        tailleY;
  unsigned int        coutPM;
  FSOUND_SAMPLE       *son;
  SDL_Surface         *surf;
  SDL_Surface         *icone;
};

struct s_lib_attaque {
  unsigned int n_attaque;
  struct s_attaque *attaques;
};


enum e_statut_joueur
{
   STATUT_NORMAL=0,
   STATUT_POISON,
   STATUT_SOMMEIL
};

struct s_stat_player
{
   char joueurActif;
   
   unsigned int level;
   enum e_statut_joueur statut;
   
   unsigned int hp, hpmax;
   unsigned int mp, mpmax;
   
   unsigned int vitesse;
   
   unsigned int force;
   unsigned int dexterite;
   unsigned int precision;
   unsigned int intelligence;
   
   int equipement_tete;
   int equipement_corps;
   int equipement_arme;
   
   unsigned int n_attaques;
   unsigned int *attaques;
   
   SDL_Surface *surf;
};

enum e_item_type {
   ITEM_OBJET=0,
   ITEM_ARMURE_TETE,
   ITEM_ARMURE_CORPS,
   ITEM_ARME,
   ITEM_QUETE
};

struct s_item
{
   char             *nom;
   SDL_Surface      *icone;
   enum e_item_type type;
   FSOUND_SAMPLE    *son;

   unsigned char quantite;
};

struct s_player
{
   struct s_stat_player joueurs[MAX_JOUEURS];
   struct s_item        inventaire[MAX_ITEMS];
   
   unsigned int         argent;
};

struct s_monstre
{
   unsigned int hp, hpmax;
   unsigned int mp, mpmax;

   unsigned int vitesse;

   unsigned int force;
   unsigned int dexterite;
   unsigned int precision;
   unsigned int intelligence;
   
   unsigned int n_attaques;
   unsigned int *attaques;

   SDL_Surface *surf;
};


void InitPlayer(struct s_player *player);
void InitStatPlayer(struct s_stat_player *stat);
void InitItem(struct s_item *item);
void InitInventaire(struct s_player *player);

void FreePlayer(struct s_player *player);

void DoItem(struct s_player *player, unsigned int itemId, FSOUND_SAMPLE *son);


void InitLibAttaque(struct s_lib_attaque *lib_attaque);
void FreeLibAttaque(struct s_lib_attaque *lib_attaque);
void ChargeAttaques(struct s_lib_attaque *lib_attaque);

void InitMonstre(struct s_monstre *monstre);
void FreeMonstre(struct s_monstre *monstre);

/* Suppr. warning */
//extern _CRTIMP char* __cdecl	strdup (const char*) __MINGW_ATTRIB_MALLOC;

# endif /* !PLAYER_H */
