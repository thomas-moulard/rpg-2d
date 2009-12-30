/*
** fight.c
**
** Système de combat tour par tour.
**
*/
#include "fight.h"

int DoFight(
  struct s_event_param_battle *param,
  struct s_player             *player,
  struct s_lib_attaque        *libattaque,
  struct s_lib_sprite         *libsprite,
  struct s_gui                *gui)
{
   SDL_Surface *apres;
   int         res;
               
  /* Fondu */
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  apres = SDL_CreateRGBSurface(SDL_SWSURFACE, 800, 600, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
#else
  apres = SDL_CreateRGBSurface(SDL_SWSURFACE, 800, 600, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
#endif
  SDL_FillRect(apres, NULL, SDL_MapRGB(apres->format,0,0,0));

  if(FonduEnchaine(SDL_GetVideoSurface(),apres,param->fondu,1000,5))
    return 1;
  SDL_FreeSurface(apres);

  res=FightMenu(param, player, libattaque, gui);
   
  return res;
}

void BlitFight(struct s_event_param_battle *param,
               struct s_player             *player,
               struct s_lib_attaque        *libattaque,
               SDL_Surface                 *fond,
               int                         *attaque,
               int                         *cible,
               int                         *tour) /* 0=HUMAIN | 1 =IA */
{
  /* affichage fond */
  SDL_BlitSurface(fond, NULL, SDL_GetVideoSurface(), NULL);
  /* affichage joueurs */
  BlitJoueurs(player);
  /* affichage monstres */
  BlitMonstres(&param->monstres, param->n_monstre);
  
  /* affichage attaque en cours */
  if((*attaque<0)||(*cible<0)) return; /* quitte s'il n'y a pas d'attaque en cours! */

  if(*tour)
  {
    if(BlitAnimation(
      libattaque,
      *attaque,
      PosXJoueur(*cible,player->joueurs[*cible].surf->w)+player->joueurs[*cible].surf->w/2-(libattaque->attaques[*attaque].tailleX)/2,
      PosYJoueur(*cible,player->joueurs[*cible].surf->h)+player->joueurs[*cible].surf->h/2-(libattaque->attaques[*attaque].tailleY)/2)<0)
    {
      *tour=0;
      *attaque=-1;
      *cible=-1;
    }
  } else {
    if(BlitAnimation(
      libattaque,
      *attaque,
      PosXMonstre(*cible,param->monstres[*cible].surf->w)+param->monstres[*cible].surf->w/2-(libattaque->attaques[*attaque].tailleX)/2,
      PosYMonstre(*cible,param->monstres[*cible].surf->h)+param->monstres[*cible].surf->h/2-(libattaque->attaques[*attaque].tailleY)/2)<0)
    {
      *tour=1;
      *attaque=-1;
      *cible=-1;
    }
   }
}

int FightMenu(struct s_event_param_battle *param,
              struct s_player *player,
              struct s_lib_attaque *libattaque,
              struct s_gui *gui)
{
         unsigned int i;
         SDL_Rect     src, dest;
  static SDL_Surface  *surf_gui = NULL,
                      *menu[4]  = {NULL,NULL,NULL,NULL};
         SDL_Surface  *tmp = NULL,
                      *fond = NULL;
         int          selection = 0,
                      res       = 0,
                      isdone    = 0,
                      attaque_j = -1,
                      cible     = -1,
                      tour      = 0,
                      old_hp[param->n_monstre],
                      monstre_attaquant;
         SDL_Color    color={255,255,255};
         SDL_Event    event;
         char         buffer[255];
                      
  if(!surf_gui) surf_gui = SDL_LoadBMP("img/combat.bmp");
  fond = SDL_LoadBMP(param->fond);
  
  if(!menu[0]) menu[0]=TTF_RenderText_Blended(gui->font,"Attaquer",color);
  if(!menu[1]) menu[1]=TTF_RenderText_Blended(gui->font,"Magie",color);
  if(!menu[2]) menu[2]=TTF_RenderText_Blended(gui->font,"Objet",color);
  if(!menu[3]) menu[3]=TTF_RenderText_Blended(gui->font,"Fuir",color);
  
  for(i=0;i<param->n_monstre;i++)
    old_hp[i]=param->monstres[i].hp;
  
  while(!isdone)
  {
    while(SDL_PollEvent(&event))
       switch(event.type)
       {
         case SDL_KEYUP:
           if((event.key.keysym.sym==SDLK_UP)&&(!tour)&&(attaque_j==-1))
             if(selection>0)
               selection--;
           if((event.key.keysym.sym==SDLK_DOWN)&&(!tour)&&(attaque_j==-1))
             if(selection<4-1)
               selection++;

           if((event.key.keysym.sym==SDLK_RETURN)&&(!tour)&&(attaque_j==-1))
             switch(selection)
             {
               case 0: /* attaquer */
                 if(player->joueurs[0].mp< (libattaque->attaques[player->joueurs[0].attaques[0]].coutPM))
                   break;
                   
                 attaque_j=player->joueurs[0].attaques[0];
                 cible=-1;
                 for(i=0;i<param->n_monstre;i++)
                   if(param->monstres[i].hp>0)
                     cible=i;
                 if(cible==-1)
                 {
                   isdone=1;
                   continue;
                 }
                 if(libattaque->attaques[attaque_j].son)
                   FSOUND_PlaySound(FSOUND_FREE,libattaque->attaques[attaque_j].son);
                 if(rand()%(50-(param->monstres[cible].precision-player->joueurs[0].precision))==1)
                   BlitDegats(
                     "Raté!",
                     PosXMonstre(cible,param->monstres[cible].surf->w)+param->monstres[cible].surf->w/2-(libattaque->attaques[attaque_j].tailleX)/2,
                     16+PosYMonstre(cible,param->monstres[cible].surf->h)+param->monstres[cible].surf->h/2-(libattaque->attaques[attaque_j].tailleY)/2,
                     1,
                     gui);
                 else
                 {
                   if(param->monstres[cible].hp > libattaque->attaques[attaque_j].degats)
                     param->monstres[cible].hp -= libattaque->attaques[attaque_j].degats;
                   else
                     param->monstres[cible].hp = 0;
                   player->joueurs[0].mp -= libattaque->attaques[attaque_j].coutPM;

                   sprintf(buffer,"%d",libattaque->attaques[attaque_j].degats);
                   BlitDegats(
                     buffer,
                     PosXMonstre(cible,param->monstres[cible].surf->w)+param->monstres[cible].surf->w/2-(libattaque->attaques[attaque_j].tailleX)/2,
                     16+PosYMonstre(cible,param->monstres[cible].surf->h)+param->monstres[cible].surf->h/2-(libattaque->attaques[attaque_j].tailleY)/2,
                     1,
                     gui);
                 }
                 break;
                 
               case 1: /* magie    */
                 if(player->joueurs[0].mp<(libattaque->attaques[player->joueurs[0].attaques[1]].coutPM))
                   break;

                 attaque_j=player->joueurs[0].attaques[1];
                 cible=-1;
                 for(i=0;i<param->n_monstre;i++)
                   if(param->monstres[i].hp>0)
                     cible=i;
                 if(cible==-1)
                 {
                   isdone=1;
                   continue;
                 }

                 if(libattaque->attaques[attaque_j].son)
                   FSOUND_PlaySound(FSOUND_FREE,libattaque->attaques[attaque_j].son);
                 if(rand()%(50-(param->monstres[cible].precision-player->joueurs[0].precision))==1)
                   BlitDegats(
                     "Raté!",
                     PosXMonstre(cible,param->monstres[cible].surf->w)+param->monstres[cible].surf->w/2-(libattaque->attaques[attaque_j].tailleX)/2,
                     16+PosYMonstre(cible,param->monstres[cible].surf->h)+param->monstres[cible].surf->h/2-(libattaque->attaques[attaque_j].tailleY)/2,
                     1,
                     gui);
                 else
                 {
                   if(param->monstres[cible].hp > libattaque->attaques[attaque_j].degats)
                     param->monstres[cible].hp -= libattaque->attaques[attaque_j].degats;
                   else
                     param->monstres[cible].hp = 0;
                   player->joueurs[0].mp -= libattaque->attaques[attaque_j].coutPM;

                   sprintf(buffer,"%d",libattaque->attaques[attaque_j].degats);
                   BlitDegats(
                     buffer,
                     PosXMonstre(cible,param->monstres[cible].surf->w)+param->monstres[cible].surf->w/2-(libattaque->attaques[attaque_j].tailleX)/2,
                     16+PosYMonstre(cible,param->monstres[cible].surf->h)+param->monstres[cible].surf->h/2-(libattaque->attaques[attaque_j].tailleY)/2,
                     1,
                     gui);
                 }
                 break;
               case 2: /* objets   */
                 break;
               case 3: /* fuir     */
                 if(rand()%(100-(player->joueurs[0].vitesse-param->monstres[0].vitesse))==1)
                   isdone=1;
                 tour=1;
                 break;
               default:
                 break;
             }
           break;
         case SDL_QUIT:
           res=isdone=1;
           break;
      }
      
    /* Si c'est à l'ia de jouer... elle joue! */
    if((tour)&&(attaque_j==-1))
    {
      /* on vérifie si l'ia est en état de combattre */
      isdone=1;
      for(i=0;i<param->n_monstre;i++)
        if(param->monstres[i].hp>0)
        {
          isdone=0;
          break;
        }
      if(isdone==1) continue;

      cible=-1;
      for(i=0;i<param->n_monstre;i++)
        if(param->monstres[i].hp>0)
          attaque_j=param->monstres[i].attaques[0], monstre_attaquant=i;
      for(i=0;i<MAX_JOUEURS;i++)
        if((player->joueurs[i].joueurActif)&&(player->joueurs[i].hp>0))
          cible=i;
      if(cible==-1)
      {
        isdone=1;
        continue;
      }

      if(libattaque->attaques[attaque_j].son)
        FSOUND_PlaySound(FSOUND_FREE,libattaque->attaques[attaque_j].son);
      if(rand()%(50-(player->joueurs[cible].precision-param->monstres[monstre_attaquant].precision))==1)
        BlitDegats(
          "Raté!",
          PosXJoueur(cible,player->joueurs[cible].surf->w)+player->joueurs[cible].surf->w/2-(libattaque->attaques[attaque_j].tailleX)/2,
          16+PosYJoueur(cible,player->joueurs[cible].surf->h)+player->joueurs[cible].surf->h/2-(libattaque->attaques[attaque_j].tailleY)/2,
          1,
          gui);
        else
        {
          if(player->joueurs[cible].hp > libattaque->attaques[attaque_j].degats)
            player->joueurs[cible].hp -= libattaque->attaques[attaque_j].degats;
          else
            player->joueurs[cible].hp = 0;
          param->monstres[monstre_attaquant].mp -= libattaque->attaques[attaque_j].coutPM;

          sprintf(buffer,"%d",libattaque->attaques[attaque_j].degats);
          BlitDegats(
            buffer,
            PosXJoueur(cible,player->joueurs[cible].surf->w)+player->joueurs[cible].surf->w/2-(libattaque->attaques[attaque_j].tailleX)/2,
            16+PosYJoueur(cible,player->joueurs[cible].surf->h)+player->joueurs[cible].surf->h/2-(libattaque->attaques[attaque_j].tailleY)/2,
            1,
            gui);
        }
    }
    
    /* Vérifications de fin de partie */
    if((!isdone)&&(attaque_j<0))
    {
      isdone=1;
      for(i=0;i<param->n_monstre;i++)
        if(param->monstres[i].hp>0)
        {
          isdone=0;
          break;
        }
      if(isdone==1) continue;
      isdone=1;
      for(i=0;i<MAX_JOUEURS;i++)
        if((player->joueurs[i].joueurActif)&&(player->joueurs[i].hp>0))
        {
          isdone=0;
          break;
        }
    }
    
    /* Affichage du combat */
    BlitFight(param, player, libattaque, fond, &attaque_j, &cible, &tour);

    BlitDegats(NULL,0,0,0,gui); /* continue l'affichage des dégâts courants */

    /* affichage du menu principal */
    dest.x=0, dest.y=400, dest.w=surf_gui->w, dest.h=surf_gui->h;
    SDL_BlitSurface(surf_gui, NULL, SDL_GetVideoSurface(), &dest);
    for(i=0;i<4;i++)
    {
      dest.x=40, dest.y=430+32*i, dest.w=menu[i]->w, dest.h=menu[i]->h;
      SDL_BlitSurface(menu[i], NULL, SDL_GetVideoSurface(), &dest);
    }
    src.x=0, src.y=300, src.w=80, src.h=32;
    dest.x=35, dest.y=430+32*selection-8, dest.w=80, dest.h=32;
    SDL_SetAlpha(gui->surf, SDL_SRCALPHA, 150);
    SDL_BlitSurface(gui->surf, &src, SDL_GetVideoSurface(), &dest);
    
    /* Affichage des pv */
    for(i=0;i<MAX_JOUEURS;i++)
      if(player->joueurs[i].joueurActif)
      {
        sprintf(
          buffer,
          "Joueur %d - PV: %d/%d",
          i+1,
          player->joueurs[i].hp,
          player->joueurs[i].hpmax);
        if(tmp)
        {
          SDL_FreeSurface(tmp);
          tmp=NULL;
        }
        tmp=TTF_RenderText_Blended(gui->font, buffer, color);
        
        dest.x=300, dest.y=450+i*32, dest.w=tmp->w, dest.h=tmp->h;
        SDL_BlitSurface(tmp, NULL, SDL_GetVideoSurface(), &dest);
      }
    for(i=0;i<param->n_monstre;i++)
    {
      sprintf(
        buffer,
        "Monstre %d - PV: %d/%d",
        i+1,
        param->monstres[i].hp,
        param->monstres[i].hpmax);
      if(tmp)
      {
        SDL_FreeSurface(tmp);
        tmp=NULL;
      }
      tmp=TTF_RenderText_Blended(gui->font, buffer, color);
      
      dest.x=500, dest.y=450+i*32, dest.w=tmp->w, dest.h=tmp->h;
      SDL_BlitSurface(tmp, NULL, SDL_GetVideoSurface(), &dest);
    }
    
    SDL_Flip(SDL_GetVideoSurface());
  }
  
  for(i=0;i<MAX_JOUEURS;i++)
    if((player->joueurs[i].joueurActif)&&(player->joueurs[i].hp!=0))
    {
      isdone=0;
      break;
    }
  if(isdone) GameOver(); /* Le joueur a perdu */
  if(res) return res; /* si le joueur quitte l'appli, pas de victoire! */
  
  /* Le joueur a gagné! */
  player->argent += 100;
  
  DrawDialog(gui, "Bravo, vous avez gagné un combat!\n\n\n\nVous obtenez:\n - 100 pieces d'or", color);
  
  /* restauration des hp des monstre... */
  for(i=0;i<param->n_monstre;i++)
    param->monstres[i].hp=old_hp[i];
  
  SDL_FreeSurface(fond);
  if(tmp) SDL_FreeSurface(tmp);
  return res;
}

void BlitJoueurs(struct s_player *player)
{
  unsigned int i;
  SDL_Rect     dest;
  
  for(i=0;i<MAX_JOUEURS;i++)
    if((player->joueurs[i].joueurActif)&&(player->joueurs[i].hp>0))
    {
      dest.w=player->joueurs[i].surf->w;
      dest.h=player->joueurs[i].surf->h;
      dest.x=PosXJoueur(i,player->joueurs[i].surf->w);
      dest.y=PosYJoueur(i,player->joueurs[i].surf->h);
      SDL_BlitSurface(player->joueurs[i].surf,NULL,SDL_GetVideoSurface(),&dest);
    }
}

void BlitMonstres(struct s_monstre **monstres, int n_monstre)
{
  unsigned int i;
  SDL_Rect     dest;
  dest.x=700;
  dest.y=100;
  for(i=0;i<n_monstre;i++)
    if(monstres[i]->hp>0)
    {
      dest.w=monstres[i]->surf->w;
      dest.h=monstres[i]->surf->h;
      dest.x=PosXMonstre(i,monstres[i]->surf->w);
      dest.y=PosYMonstre(i,monstres[i]->surf->h);

      SDL_BlitSurface((*monstres[i]).surf,NULL,SDL_GetVideoSurface(),&dest);
    }
}

int BlitAnimation(struct s_lib_attaque *libattaque, int attaque, int x, int y)
{
  static int         cur_img     = 0;
  static int         cur_attaque = 0;
  static int         istime      = 0;
         SDL_Rect    src,
                     dest;
  
  if(attaque!=cur_attaque)
  {
    cur_img=0;
    cur_attaque=attaque;
    istime=0;
  } else {
    if(attaque>=0)
      if(SDL_GetTicks()-istime>(libattaque->attaques[cur_attaque].duree/GetNbImgAttaque(libattaque->attaques[cur_attaque])))
      {
        istime=SDL_GetTicks();
        cur_img++;
      }
  }

  if(attaque<0) return 0;

  if(cur_img>=GetNbImgAttaque(libattaque->attaques[cur_attaque]))
  {
    cur_attaque=-1;
    cur_img=0;
    return -1;
  }

  src.x=(cur_img*libattaque->attaques[cur_attaque].tailleX)%libattaque->attaques[cur_attaque].surf->w;
  src.y=(cur_img*libattaque->attaques[cur_attaque].tailleX)/libattaque->attaques[cur_attaque].surf->w;
  src.w=libattaque->attaques[cur_attaque].tailleX;
  src.h=libattaque->attaques[cur_attaque].tailleY;

  dest.x=x, dest.y=y;
  dest.w=libattaque->attaques[cur_attaque].tailleX;
  dest.h=libattaque->attaques[cur_attaque].tailleY;
  
  SDL_BlitSurface(
    libattaque->attaques[cur_attaque].surf,&src,
    SDL_GetVideoSurface(),&dest);
    
  return cur_img;
}

void BlitDegats(char *string, int nx, int ny, int reinit, struct s_gui *gui)
{
  static unsigned char  alpha;
  static int            x,y;
  static SDL_Surface *surf     = NULL;
         SDL_Rect    dest;
         SDL_Color   color={255,255,255};
  
  if(reinit)
  {
    alpha=255, x=nx, y=ny;
    if(surf)
    {
      SDL_FreeSurface(surf);
      surf=NULL;
    }
    surf=TTF_RenderText_Blended(gui->font, string, color);
  } else {
    if(alpha>5)
      alpha-=5;
    else
      alpha=0;
  }
    
  if(alpha==0) return;
  if(!surf) return;
  
  dest.x=x, dest.y=y;
  dest.w=surf->w;
  dest.h=surf->h;
  SDL_SetAlpha(surf, SDL_SRCALPHA, alpha);
  SDL_BlitSurface(surf, NULL, SDL_GetVideoSurface(), &dest);
}
