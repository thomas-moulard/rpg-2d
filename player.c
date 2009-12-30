/*
** player.c
**
** Gestion du joueur
**
** Rôle:
** - gestion des stats relatives aux joueurs
** - gestion de l'inventaire
*/
# include "player.h"

void InitPlayer(struct s_player *player)
{
  int i;
  for(i=0;i<MAX_JOUEURS;i++)
    InitStatPlayer(&player->joueurs[i]);
  InitInventaire(player);

  /* Init joueur 0 */
  player->joueurs[0].joueurActif=1;
  player->joueurs[0].hp=80; /* FIXME: en attendant d'implanter les combats... */
  player->joueurs[0].hpmax=100;
  player->joueurs[0].mp=player->joueurs[0].mpmax=20;
  
  player->joueurs[0].vitesse=3;
   
  player->joueurs[0].force=1;
  player->joueurs[0].dexterite=1;
  player->joueurs[0].precision=1;
  player->joueurs[0].intelligence=1;
  player->joueurs[0].surf=SDL_LoadBMP("battlers/1.bmp");
  SDL_SetColorKey(player->joueurs[0].surf, SDL_SRCCOLORKEY,
     SDL_MapRGB(player->joueurs[0].surf->format,255,255,255));
  player->joueurs[0].equipement_tete=1;
  player->joueurs[0].equipement_corps=2;
  player->joueurs[0].equipement_arme=4;
  
  /* Init joueur */
  player->argent=100;
}

void InitStatPlayer(struct s_stat_player *stat)
{
   stat->joueurActif=0;
   
   stat->level=1;
   stat->statut=STATUT_NORMAL;
   
   stat->hp=stat->hpmax=0;
   stat->mp=stat->mpmax=0;
   
   stat->vitesse=0;
   
   stat->force=0;
   stat->dexterite=0;
   stat->precision=0;
   stat->intelligence=0;
   
   stat->equipement_tete=-1;
   stat->equipement_corps=-1;
   stat->equipement_arme=-1;
   
   stat->surf=NULL;
   
   /* affectation des attaques de base */
   stat->n_attaques=2;
   stat->attaques=malloc(2*sizeof(int));
   stat->attaques[0]=0;
   stat->attaques[1]=1;
}

void InitItem(struct s_item *item)
{
   item->nom=NULL;
   item->icone=NULL;
   item->type=ITEM_OBJET;
   item->son=NULL;
   item->quantite=0;
}

void InitInventaire(struct s_player *player)
{
  int i;
  for(i=0;i<MAX_ITEMS;i++)
    InitItem(&player->inventaire[i]);
    
  /* Item 0: potion de santé */
  player->inventaire[0].nom=strdup("Petite potion de santé");
  player->inventaire[0].icone=SDL_LoadBMP("icons/potion.bmp");
  SDL_SetColorKey(player->inventaire[0].icone, SDL_SRCCOLORKEY,
     SDL_MapRGB(player->inventaire[0].icone->format,239,243,255));
  player->inventaire[0].type=ITEM_OBJET;
  player->inventaire[0].son=FSOUND_Sample_Load(FSOUND_FREE,"son/heal.wav",FSOUND_NORMAL,0,0);
  player->inventaire[0].quantite=5;

  /* Item 1: casque */  
  player->inventaire[1].nom=strdup("Casque en fer");
  player->inventaire[1].icone=SDL_LoadBMP("icons/casque.bmp");
  SDL_SetColorKey(player->inventaire[1].icone, SDL_SRCCOLORKEY,
     SDL_MapRGB(player->inventaire[1].icone->format,239,243,255));
  player->inventaire[1].type=ITEM_ARMURE_TETE;
  player->inventaire[1].son=NULL;
  player->inventaire[1].quantite=1;
  
  /* Item 2: armure */  
  player->inventaire[2].nom=strdup("Armure moyenne");
  player->inventaire[2].icone=SDL_LoadBMP("icons/armure.bmp");
  SDL_SetColorKey(player->inventaire[2].icone, SDL_SRCCOLORKEY,
     SDL_MapRGB(player->inventaire[2].icone->format,239,243,255));
  player->inventaire[2].type=ITEM_ARMURE_CORPS;
  player->inventaire[2].son=NULL;
  player->inventaire[2].quantite=1;
  
  /* Item 3: bouclier */  
  player->inventaire[3].nom=strdup("Bouclier en fer");
  player->inventaire[3].icone=SDL_LoadBMP("icons/bouclier.bmp");
  SDL_SetColorKey(player->inventaire[3].icone, SDL_SRCCOLORKEY,
     SDL_MapRGB(player->inventaire[3].icone->format,239,243,255));
  player->inventaire[3].type=ITEM_ARME;
  player->inventaire[3].son=NULL;
  player->inventaire[3].quantite=1;
  
  /* Item 4: épée */  
  player->inventaire[4].nom=strdup("Epée de mauvaise qualité");
  player->inventaire[4].icone=SDL_LoadBMP("icons/epee.bmp");
  SDL_SetColorKey(player->inventaire[4].icone, SDL_SRCCOLORKEY,
     SDL_MapRGB(player->inventaire[4].icone->format,239,243,255));
  player->inventaire[4].type=ITEM_ARME;
  player->inventaire[4].son=NULL;
  player->inventaire[4].quantite=1;
  
  /* Item 5: arc */  
  player->inventaire[5].nom=strdup("Arc court");
  player->inventaire[5].icone=SDL_LoadBMP("icons/arc.bmp");
  SDL_SetColorKey(player->inventaire[5].icone, SDL_SRCCOLORKEY,
     SDL_MapRGB(player->inventaire[5].icone->format,239,243,255));
  player->inventaire[5].type=ITEM_ARME;
  player->inventaire[5].son=NULL;
  player->inventaire[5].quantite=1;
}


void FreePlayer(struct s_player *player)
{
  int i;
  /* libération de l'inventaire */
  for(i=0;i<MAX_ITEMS;i++)
  {
    free(player->inventaire[i].nom);
    SDL_FreeSurface(player->inventaire[i].icone);
    FSOUND_Sample_Free(player->inventaire[i].son);
  }
  
  for(i=0;i<MAX_JOUEURS;i++)
    if(player->joueurs[i].joueurActif)
      free(player->joueurs[i].attaques);
  
}


void DoItem(struct s_player *player, unsigned int itemId, FSOUND_SAMPLE *son)
{
     switch(itemId)
     {
        case 0:
           if((player->inventaire[0].quantite>0) && (player->joueurs[0].hp<player->joueurs[0].hpmax))
           {
                                                 
             FSOUND_PlaySound(FSOUND_FREE,  player->inventaire[0].son);
             player->joueurs[0].hp+=20;
             if(player->joueurs[0].hp>player->joueurs[0].hpmax)
               player->joueurs[0].hp=player->joueurs[0].hpmax;
             player->inventaire[0].quantite--;
           }
           break;
     }
}


void InitLibAttaque(struct s_lib_attaque *lib_attaque)
{
  lib_attaque->n_attaque=0;
  lib_attaque->attaques=NULL;
}

void FreeLibAttaque(struct s_lib_attaque *lib_attaque)
{
  lib_attaque->n_attaque=0;
  free(lib_attaque->attaques);
}

void ChargeAttaques(struct s_lib_attaque *lib_attaque)
{
  lib_attaque->n_attaque=2;
  lib_attaque->attaques=malloc(2*sizeof(struct s_attaque));

  /* Attaque 1: attaque de base */
  lib_attaque->attaques[0].nom=strdup("Coup d'épée");
  lib_attaque->attaques[0].type=ATTAQUE_NORMAL;
  lib_attaque->attaques[0].duree=1000;
  lib_attaque->attaques[0].degats=30;
  lib_attaque->attaques[0].tailleX=192;
  lib_attaque->attaques[0].tailleY=192;
  lib_attaque->attaques[0].coutPM=0;
  lib_attaque->attaques[0].son=FSOUND_Sample_Load(FSOUND_FREE,"son/tornade.wav",FSOUND_NORMAL,0,0);
  lib_attaque->attaques[0].surf=SDL_LoadBMP("animations/009-Weapon04.bmp");
  SDL_SetColorKey(
    lib_attaque->attaques[0].surf,
    SDL_SRCCOLORKEY,
    SDL_MapRGB(lib_attaque->attaques[0].surf->format,255,255,255));
  lib_attaque->attaques[0].icone=SDL_LoadBMP("icons/epee.bmp");
  SDL_SetColorKey(
    lib_attaque->attaques[0].icone,
    SDL_SRCCOLORKEY,
    SDL_MapRGB(lib_attaque->attaques[0].icone->format,239,243,255));

  /* Attaque 2: magie de base */
  lib_attaque->attaques[1].nom=strdup("Foudre");
  lib_attaque->attaques[1].type=ATTAQUE_NORMAL;
  lib_attaque->attaques[1].duree=1500;
  lib_attaque->attaques[1].degats=50;
  lib_attaque->attaques[1].tailleX=192;
  lib_attaque->attaques[1].tailleY=192;
  lib_attaque->attaques[1].coutPM=0;
  lib_attaque->attaques[1].son=FSOUND_Sample_Load(FSOUND_FREE,"son/tornade.wav",FSOUND_NORMAL,0,0);
  lib_attaque->attaques[1].surf=SDL_LoadBMP("animations/017-Thunder01.bmp");
  SDL_SetColorKey(
    lib_attaque->attaques[1].surf,
    SDL_SRCCOLORKEY,
    SDL_MapRGB(lib_attaque->attaques[1].surf->format,255,255,255));
  lib_attaque->attaques[1].icone=SDL_LoadBMP("icons/044-Skill01.bmp");
  SDL_SetColorKey(
    lib_attaque->attaques[1].icone,
    SDL_SRCCOLORKEY,
    SDL_MapRGB(lib_attaque->attaques[1].icone->format,239,243,255));
}



void InitMonstre(struct s_monstre *monstre)
{
   monstre->hp=monstre->hpmax=monstre->mp=monstre->mpmax=monstre->vitesse=0;
   monstre->force=monstre->dexterite=monstre->precision=monstre->intelligence=0;
   monstre->n_attaques=0;
   monstre->attaques=NULL;
   monstre->surf=NULL;
}
void FreeMonstre(struct s_monstre *monstre)
{
   monstre->n_attaques=0;
   free(monstre->attaques);
   SDL_FreeSurface(monstre->surf);
   monstre->surf=NULL;
}

