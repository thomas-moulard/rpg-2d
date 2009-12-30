/*
** gui.c
**
** Gestion du GUI, des menus, des dialogues...
**
** Rôle:
** - dialogue
** - menus
** - GUI (barre de vie par exemple)
*/
# include "gui.h"

void InitGui(struct s_gui *gui)
{
     gui->surf=SDL_LoadBMP("img/gui.bmp");
     SDL_SetColorKey(gui->surf, SDL_SRCCOLORKEY, SDL_MapRGB(gui->surf->format,0,0,0));
     
     gui->menu=SDL_LoadBMP("img/menu.bmp");
     SDL_SetColorKey(gui->menu, SDL_SRCCOLORKEY, SDL_MapRGB(gui->menu->format,0,0,0));


    /* chargement de la police utilisée dans le jeu */
    gui->font=TTF_OpenFont("carolingia.ttf", 16);
    if(!gui->font)
       fprintf(stderr,"TTF_OpenFont: %s\n", TTF_GetError());
}

void FreeGui(struct s_gui *gui)
{
     SDL_FreeSurface(gui->surf);
     gui->surf=NULL;
     SDL_FreeSurface(gui->menu);
     gui->menu=NULL;
     TTF_CloseFont(gui->font);
     gui->font=NULL;
}


void DrawDialog(struct s_gui *gui, char *dialog, SDL_Color color)
{
  SDL_Rect src, dest;
  SDL_Surface *text_surface;
  SDL_Event event;
  int isdone, i;
	unsigned int advance;
  char *tmp;
  
  tmp=calloc(2,sizeof(char));
  
  /* affichage fond */
  src.x=src.y=0;
  src.w=400;
  src.h=150;
     
  dest.x=200;
  dest.y=400;
  dest.w=400;
  dest.h=150;
    
  SDL_SetAlpha(gui->surf,SDL_SRCALPHA,150); 
  SDL_BlitSurface(gui->surf,&src,SDL_GetVideoSurface(),&dest);
  
  /* affichage fond */
  src.x=0;
  src.y=150;
  src.w=400;
  src.h=150;
    
  SDL_SetAlpha(gui->surf,SDL_SRCALPHA,SDL_ALPHA_OPAQUE); 
  SDL_BlitSurface(gui->surf,&src,SDL_GetVideoSurface(),&dest);
    
  dest.y+=(3*TTF_FontLineSkip(gui->font))/4;
	dest.x=205;
	advance=10;
  i=0;
  isdone=0;
  while (!isdone)
  {
    /* Lecture des évènements dans la queue d'évènements */
    while (SDL_PollEvent (&event))
    {      
      switch (event.type)
      {
      case SDL_KEYUP:
	    if(event.key.state==SDL_RELEASED)
	    {
          if(event.key.keysym.sym==SDLK_SPACE)
	        isdone = 1;
	    }
        break;
      case SDL_QUIT:
        isdone = 1;
	  break;
      }
    }
    /* affichage texte */
    if(dialog[i]!=0)
    {
       tmp[0]=dialog[i];
       tmp[1]=0;
       
       if(dialog[i]=='\n')
       {
          dest.x=205;
          dest.y+=TTF_FontLineSkip(gui->font);
       } else {
          dest.x+=advance;
       
          text_surface=TTF_RenderText_Blended(gui->font,tmp,color);
          SDL_BlitSurface(text_surface,NULL,SDL_GetVideoSurface(),&dest);
          advance=text_surface->w+3;
          SDL_FreeSurface(text_surface);
          text_surface=NULL;
       }
       i++;
    }
    
    SDL_Flip(SDL_GetVideoSurface());
  }
  
  free(tmp);
}

int DrawInput(struct s_map *map, struct s_lib_sprite *libsprite,
              struct s_gui *gui, SDL_Color color, char *dialog, int n_input, ...)
{
  SDL_Rect src, dest;
  SDL_Surface *text_surface;
  SDL_Surface *inputs[n_input];
  SDL_Event event;
  int isdone, i, selection, advance;
  char *tmp;
  va_list liste;
  
  tmp=calloc(2,sizeof(char));

  /* Génération choix */  
  va_start(liste, n_input);
  for(i=0;i<n_input;i++)
    inputs[i]=TTF_RenderText_Blended(gui->font,va_arg(liste, char*),color);
  va_end(liste);
  
  isdone=selection=0;
  while (!isdone)
  {
    /* Lecture des évènements dans la queue d'évènements */
    while (SDL_PollEvent (&event))
    {      
      switch (event.type)
      {
      case SDL_KEYUP:
	    if(event.key.state==SDL_RELEASED)
	    {
          if(event.key.keysym.sym==SDLK_UP)
	        if(selection>0)
	          selection--;
          if(event.key.keysym.sym==SDLK_DOWN)
	        if(selection<n_input-1)
	          selection++;
          if(event.key.keysym.sym==SDLK_SPACE)
	        isdone = 1;
	    }
        break;
      case SDL_QUIT:
        isdone = 1;
	      break;
      }
    }
    
    HandleMap(map);
    HandleSprites(0, libsprite, map);
    
    /* affichage fond */
    src.x=src.y=0;
    src.w=dest.w=400;
    src.h=dest.h=150;
    dest.x=200, dest.y=400;
    
    SDL_SetAlpha(gui->surf,SDL_SRCALPHA,150); 
    SDL_BlitSurface(gui->surf,&src,SDL_GetVideoSurface(),&dest);
    
    src.x=0, src.y=150;
    src.w=dest.w=400;
    src.h=dest.h=150;
    dest.x=200, dest.y=400;

    SDL_SetAlpha(gui->surf,SDL_SRCALPHA,SDL_ALPHA_OPAQUE);
    SDL_BlitSurface(gui->surf,&src,SDL_GetVideoSurface(),&dest);
    
    /* affichage texte */
    dest.y+=(3*TTF_FontLineSkip(gui->font))/4;
    dest.x+=5;
    i=0;
    while(dialog[i]!=0)
    {
       tmp[0]=dialog[i];
       tmp[1]=0;
       
       if(dialog[i]=='\n')
       {
          dest.x=205;
          dest.y+=TTF_FontLineSkip(gui->font);
       } else {
          dest.x+=advance;
       
          text_surface=TTF_RenderText_Blended(gui->font,tmp,color);
          SDL_BlitSurface(text_surface,NULL,SDL_GetVideoSurface(),&dest);
          advance=text_surface->w;
          SDL_FreeSurface(text_surface);
          text_surface=NULL;
       }
       i++;
    }
    
    /* affichage des choix */
    dest.x=220;
    dest.y+=10;
    for(i=0;i<n_input;i++)
    {
      dest.y+=(3*TTF_FontLineSkip(gui->font))/2;
      dest.w=inputs[i]->w, dest.h=inputs[i]->h;
      SDL_BlitSurface(inputs[i],NULL,SDL_GetVideoSurface(),&dest);
    }
    
    /* Affichage rectangle de sélection */
    src.x=0;
    src.y=300;
    src.w=dest.w=8;
    src.h=dest.h=32;
    dest.x=210;

    dest.y-=((3*TTF_FontLineSkip(gui->font))/2)*(n_input-selection-1)+8;
    SDL_BlitSurface(gui->surf,&src,SDL_GetVideoSurface(),&dest);
    
    
    SDL_Flip(SDL_GetVideoSurface());
  }
  
  for(i=0;i<n_input;i++)
    SDL_FreeSurface(inputs[i]);
  free(tmp);
  
  return selection;
}

int DrawShopAchat(struct s_map *map, struct s_lib_sprite *libsprite, struct s_player *player,
                   struct s_gui *gui, SDL_Color color, char *dialog,
									 int n_item, unsigned int *itemId, unsigned int *coutItem)
{
  SDL_Rect src, dest;
  SDL_Surface *text_surface;
  SDL_Surface *inputs[n_item];
  SDL_Event event;
  int isdone, i, selection, res;
  char *tmp, buffer[255];


  tmp=calloc(2,sizeof(char));

  /* Génération choix */
  for(i=0;i<n_item;i++)
  {
		memset(&buffer, 0, sizeof(buffer));
		sprintf(buffer, "%s : %d",player->inventaire[itemId[i]].nom, coutItem[i]);
    inputs[i]=TTF_RenderText_Blended(gui->font,buffer,color);
  }


  isdone=selection=res=0;
  while (!isdone)
  {
    /* Lecture des évènements dans la queue d'évènements */
    while (SDL_PollEvent (&event))
    {
      switch (event.type)
      {
      case SDL_KEYUP:
	    if(event.key.state==SDL_RELEASED)
	    {
          if(event.key.keysym.sym==SDLK_UP)
	        if(selection>0)
	          selection--;
          if(event.key.keysym.sym==SDLK_DOWN)
	        if(selection<n_item-1)
	          selection++;
          if(event.key.keysym.sym==SDLK_SPACE)
          {
						if(player->argent>=coutItem[selection])
						{
						  player->argent-=coutItem[selection];
						  player->inventaire[itemId[selection]].quantite++;

              HandleMap(map);
			        HandleSprites(0, libsprite, map);
							DrawDialog(gui, "Merci!", color);
						} else {
              HandleMap(map);
			        HandleSprites(0, libsprite, map);
							DrawDialog(gui, "Vous n'avez pas assez d'argent\npour acheter cet objet.", color);
						}
					}
          if(event.key.keysym.sym==SDLK_ESCAPE)
	          isdone = 1;
	    }
        break;
      case SDL_QUIT:
        res = isdone = 1;
	      break;
      }
    }

    HandleMap(map);
    HandleSprites(0, libsprite, map);

    /* affichage fond */
    src.x=src.y=0;
    src.w=dest.w=400;
    src.h=dest.h=150;
    dest.x=200, dest.y=400;

    SDL_SetAlpha(gui->surf,SDL_SRCALPHA,150);
    SDL_BlitSurface(gui->surf,&src,SDL_GetVideoSurface(),&dest);

    src.x=0, src.y=150;
    src.w=dest.w=400;
    src.h=dest.h=150;
    dest.x=200, dest.y=400;

    SDL_SetAlpha(gui->surf,SDL_SRCALPHA,SDL_ALPHA_OPAQUE);
    SDL_BlitSurface(gui->surf,&src,SDL_GetVideoSurface(),&dest);

    /* affichage texte */
    dest.y+=(3*TTF_FontLineSkip(gui->font))/4;
    dest.x+=5;
    i=0;
    while(dialog[i]!=0)
    {
       tmp[0]=dialog[i];
       tmp[1]=0;

       if(dialog[i]=='\n')
       {
          dest.x=205;
          dest.y+=TTF_FontLineSkip(gui->font);
       } else {
          dest.x+=10;

          text_surface=TTF_RenderText_Blended(gui->font,tmp,color);
          SDL_BlitSurface(text_surface,NULL,SDL_GetVideoSurface(),&dest);
          SDL_FreeSurface(text_surface);
          text_surface=NULL;
       }
       i++;
    }

    /* affichage de l'item sélectioné */
    dest.x=220;
    dest.y+=50;
    SDL_BlitSurface(player->inventaire[itemId[selection]].icone,NULL,SDL_GetVideoSurface(),&dest);
    dest.x+=32;
    SDL_BlitSurface(inputs[selection],NULL,SDL_GetVideoSurface(),&dest);

    SDL_Flip(SDL_GetVideoSurface());
  }

  for(i=0;i<n_item;i++)
    SDL_FreeSurface(inputs[i]);
  free(tmp);

  return res;
}

int DrawShopVente(struct s_map *map, struct s_lib_sprite *libsprite, struct s_player *player,
                   struct s_gui *gui, SDL_Color color, char *dialog, unsigned int *prixVente)
{
  SDL_Rect src, dest;
  SDL_Surface *text_surface;
  SDL_Surface **inputs;
  SDL_Event event;
  int isdone, i, selection, res, item, n_item;
  unsigned int *itemId, *coutItem;
  char *tmp, buffer[255];

  /* Calcul du nombre d'items */
  n_item=0;
	for(i=0;i<MAX_ITEMS;i++)
		if((player->inventaire[i].nom!=NULL) && (player->inventaire[i].quantite>0))
			n_item++;
	if(n_item==0)
	{
    HandleMap(map);
    HandleSprites(0, libsprite, map);
		DrawDialog(gui, "Vous n'avez aucun objet à vendre.", color);
		return 0;
	}
			
	/* Allocation des surfaces */
	inputs=malloc(n_item*sizeof(SDL_Surface*));
	
	/* Création des tableaux d'objets */
	itemId=malloc(n_item*sizeof(unsigned int));
	coutItem=malloc(n_item*sizeof(unsigned int));
	item=0;
	for(i=0;i<MAX_ITEMS;i++)
		if((player->inventaire[i].nom!=NULL) && (player->inventaire[i].quantite>0))
		{
			itemId[item]=i;
			coutItem[item]=prixVente[i];
			item++;
		}

  tmp=calloc(2,sizeof(char));

  /* Génération choix */
  for(i=0;i<n_item;i++)
  {
		memset(&buffer, 0, sizeof(buffer));
		sprintf(buffer, "%s : %d",player->inventaire[itemId[i]].nom, coutItem[i]);
    inputs[i]=TTF_RenderText_Blended(gui->font,buffer,color);
  }

  isdone=selection=res=0;
  while (!isdone)
  {
    /* Lecture des évènements dans la queue d'évènements */
    while (SDL_PollEvent (&event))
    {
      switch (event.type)
      {
      case SDL_KEYUP:
	    if(event.key.state==SDL_RELEASED)
	    {
          if(event.key.keysym.sym==SDLK_UP)
	        if(selection>0)
	          selection--;
          if(event.key.keysym.sym==SDLK_DOWN)
	        if(selection<n_item-1)
	          selection++;
          if(event.key.keysym.sym==SDLK_SPACE)
          {
						  player->argent+=coutItem[selection];
						  player->inventaire[itemId[selection]].quantite--;
						  if(player->inventaire[itemId[selection]].quantite==0)
								isdone = res = 2;

              HandleMap(map);
			        HandleSprites(0, libsprite, map);
							DrawDialog(gui, "Merci!", color);
					}
          if(event.key.keysym.sym==SDLK_ESCAPE)
	          isdone = 1;
	    }
        break;
      case SDL_QUIT:
        res = isdone = 1;
	      break;
      }
    }

    HandleMap(map);
    HandleSprites(0, libsprite, map);

    /* affichage fond */
    src.x=src.y=0;
    src.w=dest.w=400;
    src.h=dest.h=150;
    dest.x=200, dest.y=400;

    SDL_SetAlpha(gui->surf,SDL_SRCALPHA,150);
    SDL_BlitSurface(gui->surf,&src,SDL_GetVideoSurface(),&dest);

    src.x=0, src.y=150;
    src.w=dest.w=400;
    src.h=dest.h=150;
    dest.x=200, dest.y=400;

    SDL_SetAlpha(gui->surf,SDL_SRCALPHA,SDL_ALPHA_OPAQUE);
    SDL_BlitSurface(gui->surf,&src,SDL_GetVideoSurface(),&dest);

    /* affichage texte */
    dest.y+=(3*TTF_FontLineSkip(gui->font))/4;
    dest.x+=5;
    i=0;
    while(dialog[i]!=0)
    {
       tmp[0]=dialog[i];
       tmp[1]=0;

       if(dialog[i]=='\n')
       {
          dest.x=205;
          dest.y+=TTF_FontLineSkip(gui->font);
       } else {
          dest.x+=10;

          text_surface=TTF_RenderText_Blended(gui->font,tmp,color);
          SDL_BlitSurface(text_surface,NULL,SDL_GetVideoSurface(),&dest);
          SDL_FreeSurface(text_surface);
          text_surface=NULL;
       }
       i++;
    }

    /* affichage de l'item sélectioné */
    dest.x=220;
    dest.y+=50;
    SDL_BlitSurface(player->inventaire[itemId[selection]].icone,NULL,SDL_GetVideoSurface(),&dest);
    dest.x+=32;
    SDL_BlitSurface(inputs[selection],NULL,SDL_GetVideoSurface(),&dest);

    SDL_Flip(SDL_GetVideoSurface());
  }

  for(i=0;i<n_item;i++)
    SDL_FreeSurface(inputs[i]);
	free(inputs);
  free(tmp);
  
  free(itemId);
  free(coutItem);
  
  /* L'équipement équipe a-t-il été vendu? */
  for(i=0;i<MAX_JOUEURS;i++)
		 if(player->joueurs[i].joueurActif)
		 {
			 if(player->inventaire[player->joueurs[i].equipement_arme].quantite==0)
         player->joueurs[i].equipement_arme=-1;
			 if(player->inventaire[player->joueurs[i].equipement_tete].quantite==0)
         player->joueurs[i].equipement_tete=-1;
			 if(player->inventaire[player->joueurs[i].equipement_corps].quantite==0)
         player->joueurs[i].equipement_corps=-1;
		 }

  return res;
}


int Menu(struct s_player *player, struct s_gui *gui)
{
  SDL_Event event;
  SDL_Rect src, dest;
  SDL_Color color;
  int isdone, res, selection, i;
  int lastmove;
  SDL_Surface *stathp[MAX_JOUEURS],
              *statmp[MAX_JOUEURS],
              *statforce[MAX_JOUEURS],
              *statdexterite[MAX_JOUEURS],
              *statprecision[MAX_JOUEURS],
              *statintelligence[MAX_JOUEURS];
  FSOUND_SAMPLE *son;
  char tmp[255];
  
  son = FSOUND_Sample_Load(FSOUND_FREE,"son/clic.wav",FSOUND_NORMAL,0,0);
  
  /* Génération txt */
  color.r=color.g=color.b=255;
  for(i=0;i<MAX_JOUEURS;i++)
     if(player->joueurs[i].joueurActif)
     {
       memset(&tmp, 0, sizeof(tmp));
       sprintf(tmp, "Hp : %d / %d",player->joueurs[i].hp,player->joueurs[i].hpmax); 
       stathp[i]=TTF_RenderText_Blended(gui->font,tmp,color);

       memset(&tmp, 0, sizeof(tmp));
       sprintf(tmp, "Mp : %d / %d",player->joueurs[i].mp,player->joueurs[i].mpmax);
       statmp[i]=TTF_RenderText_Blended(gui->font,tmp,color);
       
       memset(&tmp, 0, sizeof(tmp));
       sprintf(tmp, "Force :        %d",player->joueurs[i].force);
       statforce[i]=TTF_RenderText_Blended(gui->font,tmp,color);
       
       memset(&tmp, 0, sizeof(tmp));
       sprintf(tmp, "Dextérité :    %d",player->joueurs[i].dexterite);
       statdexterite[i]=TTF_RenderText_Blended(gui->font,tmp,color);
       
       memset(&tmp, 0, sizeof(tmp));
       sprintf(tmp, "Précision : %d",player->joueurs[i].precision);
       statprecision[i]=TTF_RenderText_Blended(gui->font,tmp,color);
       
       memset(&tmp, 0, sizeof(tmp));
       sprintf(tmp, "Intelligence : %d",player->joueurs[i].intelligence);
       statintelligence[i]=TTF_RenderText_Blended(gui->font,tmp,color);
     }
  
  selection=isdone=res=0;
  lastmove=SDL_GetTicks();
  while (!isdone)
  {
    /* Lecture des évènements dans la queue d'évènements */
    while (SDL_PollEvent (&event))
    {      
      switch (event.type)
      {
      case SDL_KEYUP:
	    if(event.key.state==SDL_RELEASED)
	    {
          if(SDL_GetTicks()-lastmove>200)
          {
             if(event.key.keysym.sym==SDLK_ESCAPE)
             {
                FSOUND_PlaySound(FSOUND_FREE,son);
    	        isdone = 1;
             }

             if(event.key.keysym.sym==SDLK_UP)
             {
	           selection--;
	           FSOUND_PlaySound(FSOUND_FREE,son);
             }
             if(event.key.keysym.sym==SDLK_DOWN)
             {
    	        selection++;
	            FSOUND_PlaySound(FSOUND_FREE,son);
             }
             if(selection<0) selection=3;
             if(selection>3) selection=0;
             
             if(event.key.keysym.sym==SDLK_SPACE)
             {
                FSOUND_PlaySound(FSOUND_FREE,son);
                switch(selection)
                {
                   case 0:
                      /* inventaire */
                      res = isdone = MenuInventaire(gui,player, selection, son);
                      for(i=0;i<MAX_JOUEURS;i++)
                        if(player->joueurs[i].joueurActif)
                        {
                          memset(&tmp, 0, sizeof(tmp));
                          sprintf(tmp, "Hp : %d / %d",player->joueurs[i].hp,player->joueurs[i].hpmax); 
                          stathp[i]=TTF_RenderText_Blended(gui->font,tmp,color);

                          memset(&tmp, 0, sizeof(tmp));
                          sprintf(tmp, "Mp : %d / %d",player->joueurs[i].mp,player->joueurs[i].mpmax);
                          statmp[i]=TTF_RenderText_Blended(gui->font,tmp,color);
                        }
                      break;
                   case 1:
                      /* équiper */
                      res = isdone = MenuEquiper(gui,player, selection, son);
                      break;
                   case 2:
                      /* magie */
                      break;
                   case 3:
                      /* quitter */
                      res = isdone = 1;
                      break;
                   default:
                      break;
                }
             }
             
             lastmove=SDL_GetTicks();
          }
	    }
        break;
      case SDL_QUIT:
        isdone = res = 1;
	  break;
      }
    }

    src.x=src.y=dest.x=dest.y=0;
    src.w=dest.w=800;
    src.h=dest.h=600;
    
    SDL_BlitSurface(gui->menu,&src,SDL_GetVideoSurface(),&dest);
    
    MenuBlitLeftPanel(gui,player,selection);
    
    /* Affichage infos sur le panneau droit */
    for(i=0;i<MAX_JOUEURS;i++)
       if(player->joueurs[i].joueurActif)
       {
          dest.x=220, dest.y=120+i*(player->joueurs[i].surf->h+10);
          dest.w=player->joueurs[i].surf->w;
          dest.h=player->joueurs[i].surf->h;
          SDL_BlitSurface(player->joueurs[i].surf,NULL,SDL_GetVideoSurface(),&dest);
          
          dest.x=220+player->joueurs[i].surf->w+10, dest.y=120+i*(player->joueurs[i].surf->h+10);
          dest.w=stathp[i]->w;
          dest.h=stathp[i]->h;
          SDL_BlitSurface(stathp[i],NULL,SDL_GetVideoSurface(),&dest);
          
          dest.x=450+player->joueurs[i].surf->w+10, dest.y=120+i*(player->joueurs[i].surf->h+10);
          dest.w=statmp[i]->w;
          dest.h=statmp[i]->h;
          SDL_BlitSurface(statmp[i],NULL,SDL_GetVideoSurface(),&dest);
          
          dest.x=220+player->joueurs[i].surf->w+10, dest.y=170+i*(player->joueurs[i].surf->h+10);
          dest.w=statforce[i]->w;
          dest.h=statforce[i]->h;
          SDL_BlitSurface(statforce[i],NULL,SDL_GetVideoSurface(),&dest);
          
          dest.x=450+player->joueurs[i].surf->w+10, dest.y=170+i*(player->joueurs[i].surf->h+10);
          dest.w=statdexterite[i]->w;
          dest.h=statdexterite[i]->h;
          SDL_BlitSurface(statdexterite[i],NULL,SDL_GetVideoSurface(),&dest);
          
          dest.x=220+player->joueurs[i].surf->w+10, dest.y=190+i*(player->joueurs[i].surf->h+10);
          dest.w=statprecision[i]->w;
          dest.h=statprecision[i]->h;
          SDL_BlitSurface(statprecision[i],NULL,SDL_GetVideoSurface(),&dest);
          
          dest.x=450+player->joueurs[i].surf->w+10, dest.y=190+i*(player->joueurs[i].surf->h+10);
          dest.w=statintelligence[i]->w;
          dest.h=statintelligence[i]->h;
          SDL_BlitSurface(statintelligence[i],NULL,SDL_GetVideoSurface(),&dest);
       }

    
    SDL_Flip(SDL_GetVideoSurface());
  }
  
  FSOUND_Sample_Free(son);
  return res;
}

void MenuBlitLeftPanel(struct s_gui *gui, struct s_player *player, unsigned char selection)
{
  static SDL_Surface *inventaire = NULL,
                     *equiper    = NULL,
                     *magie      = NULL,
                     *quitter    = NULL;
  SDL_Surface        *argent;
  char               tmp[255];
  
  static int alpha = 150, alpha_sens = 0;
  SDL_Rect src,dest;
  SDL_Color color={255,255,255};
  
  if(!inventaire) inventaire=TTF_RenderText_Blended(gui->font,"Inventaire",color);
  if(!equiper)    equiper=TTF_RenderText_Blended(gui->font,"Equiper",color);
  if(!magie)      magie=TTF_RenderText_Blended(gui->font,"Magie",color);
  if(!quitter)    quitter=TTF_RenderText_Blended(gui->font,"Quitter",color);
  
  memset(&tmp, 0, sizeof(tmp));
  sprintf(tmp, "Argent: %d ", player->argent);
  argent=TTF_RenderText_Blended(gui->font, tmp,color);
  
  dest.x=32;
  dest.y=32;
  dest.w=inventaire->w;
  dest.h=inventaire->h;
  SDL_BlitSurface(inventaire,NULL,SDL_GetVideoSurface(),&dest);
  
  dest.y=64;
  dest.w=equiper->w;
  dest.h=equiper->h;
  SDL_BlitSurface(equiper,NULL,SDL_GetVideoSurface(),&dest);
    
    
  dest.y=96;
  dest.w=magie->w;
  dest.h=magie->h;
  SDL_BlitSurface(magie,NULL,SDL_GetVideoSurface(),&dest);
    
  dest.y=128;
  dest.w=quitter->w;
  dest.h=quitter->h;
  SDL_BlitSurface(quitter,NULL,SDL_GetVideoSurface(),&dest);


  if(alpha_sens==0)
  {
     alpha++;
     if(alpha>=175) alpha_sens=1;
  } else {
     alpha--;
     if(alpha<=125) alpha_sens=0;
  }

  SDL_SetAlpha(gui->surf,SDL_SRCALPHA,alpha); 
  
  src.x=0;
  src.y=300;
  src.w=dest.w=76;
  src.h=dest.h=32;
  dest.x=28;
  dest.y=24+selection*32;
  SDL_BlitSurface(gui->surf,&src,SDL_GetVideoSurface(),&dest);
  
  /* Panneau gauche bas */
  dest.x=28;
  dest.y=480;
  dest.w=argent->w;
  dest.h=argent->h;
  SDL_BlitSurface(argent,NULL,SDL_GetVideoSurface(),&dest);
  
  SDL_FreeSurface(argent);
}

int MenuInventaire(struct s_gui *gui, struct s_player *player, int selectiong, FSOUND_SAMPLE *son)
{
  SDL_Event event;
  SDL_Rect src, dest;
  SDL_Surface *surf;
  int isdone, res, selection, selitem, item, i, lastmove, n_item;
  char tmp[255];
  SDL_Color color={255,255,255};
  
  isdone=res=selection=selitem=0;
  lastmove=SDL_GetTicks();
  while (!isdone)
  {
    /* Lecture des évènements dans la queue d'évènements */
    while (SDL_PollEvent (&event))
    {      
      switch (event.type)
      {
      case SDL_KEYUP:
	    if(event.key.state==SDL_RELEASED)
	    {
          if(SDL_GetTicks()-lastmove>200)
          {
             if(event.key.keysym.sym==SDLK_ESCAPE)
             {
                FSOUND_PlaySound(FSOUND_FREE,son);
    	        isdone = 1;
             }

             if(event.key.keysym.sym==SDLK_LEFT)
             {
	           selection--;
	           selitem=0;
	           FSOUND_PlaySound(FSOUND_FREE,son);
             }
             if(event.key.keysym.sym==SDLK_RIGHT)
             {
    	       selection++;
	           selitem=0;
	           FSOUND_PlaySound(FSOUND_FREE,son);
             }
             
             if(event.key.keysym.sym==SDLK_UP)
             {
	           selitem--;
	           FSOUND_PlaySound(FSOUND_FREE,son);
             }
             if(event.key.keysym.sym==SDLK_DOWN)
             {
	           selitem++;
	           FSOUND_PlaySound(FSOUND_FREE,son);
             }
             
             if((n_item>0) && (event.key.keysym.sym==SDLK_SPACE))
             {	            
                /* recherche de l'item sélectionné dans le tableau */
                item=i=0;
                while(item<selitem)
                {
                  if((player->inventaire[i].nom!=NULL)
                     && (player->inventaire[i].quantite>0)
                     && (selection==player->inventaire[i].type))
                       item++;
                   i++;
                }
                DoItem(player, i, son);
             }
             
             
             if(selection<0) selection=3;
             if(selection>3) selection=0;
             
             lastmove=SDL_GetTicks();
          }
	    }
        break;
      case SDL_QUIT:
        isdone = res = 1;
	  break;
      }
    }

    src.x=src.y=dest.x=dest.y=0;
    src.w=dest.w=800;
    src.h=dest.h=600;
    
    SDL_BlitSurface(gui->menu,&src,SDL_GetVideoSurface(),&dest);

    /* Affichage partie haute */
    MenuBlitTopPanel(gui,selection);
    /* Affichage partie gauche */
    MenuBlitLeftPanel(gui,player,selectiong);
    
    /* Affichages des items dans la catégorie sélectionnée */
    n_item=0;
    dest.y=100;
    for(i=0;i<MAX_ITEMS;i++)
    {
       if((player->inventaire[i].nom!=NULL)
          && (player->inventaire[i].quantite>0)
          && (
              ((selection==0) && (player->inventaire[i].type==ITEM_OBJET))
              || ((selection==1) && ((player->inventaire[i].type==ITEM_ARMURE_TETE)||(player->inventaire[i].type==ITEM_ARMURE_CORPS)))
              || ((selection==2) && (player->inventaire[i].type==ITEM_ARME))
              || ((selection==3) && (player->inventaire[i].type==ITEM_QUETE))
             )) /* it's a kind of magic... */
       {
          dest.x=220;
          dest.y+=32;
          
          dest.w=player->inventaire[i].icone->w;
          dest.h=player->inventaire[i].icone->h;
          /* Affichage icône */
          SDL_BlitSurface(player->inventaire[i].icone,NULL,SDL_GetVideoSurface(),&dest);
          /* Affichage texte (nom + qtité */
          dest.x=250;
          dest.y+=5;
          memset(&tmp, 0, sizeof(tmp));
          sprintf(tmp, "%s (x %d)", player->inventaire[i].nom, player->inventaire[i].quantite);
          surf=TTF_RenderText_Blended(gui->font,tmp,color);
          SDL_BlitSurface(surf,NULL,SDL_GetVideoSurface(),&dest);
          SDL_FreeSurface(surf);
          dest.y-=5;
          
          n_item++;
       }
    }
    
    /* affichage du rectangle de sélection */
    if(selitem<0) selitem=0;
    if(selitem>=n_item) selitem=n_item-1;
    if(n_item>0)
    {
      src.x=0;
      src.y=300;
      src.w=dest.w=24;
      src.h=dest.h=24;
      dest.x=220;
      dest.y=132+selitem*32;
      SDL_BlitSurface(gui->surf,&src,SDL_GetVideoSurface(),&dest);
    }

    
    SDL_Flip(SDL_GetVideoSurface());
  }
  return res;
}

void MenuBlitTopPanel(struct s_gui *gui, unsigned char selection)
{
  SDL_Rect src, dest;
  static SDL_Surface *objets = NULL,
                     *armures = NULL,
                     *armes = NULL,
                     *quetes = NULL;
  static int alpha = 150, alpha_sens = 0;
  SDL_Color color = {255,255,255};

  if(!objets)  objets=TTF_RenderText_Blended(gui->font,"Objets",color);
  if(!armures) armures=TTF_RenderText_Blended(gui->font,"Armures",color);
  if(!armes)   armes=TTF_RenderText_Blended(gui->font,"Armes",color);
  if(!quetes)  quetes=TTF_RenderText_Blended(gui->font,"Obj. quêtes",color);
    
  /* Affichage type d'objets */
  dest.x=250;
  dest.y=40;
  dest.w=objets->w;
  dest.h=objets->h;
  SDL_BlitSurface(objets,NULL,SDL_GetVideoSurface(),&dest);
    
  dest.x=350;
  dest.w=armures->w;
  dest.h=armures->h;
  SDL_BlitSurface(armures,NULL,SDL_GetVideoSurface(),&dest);
    
  dest.x=450;
  dest.w=armes->w;
  dest.h=armes->h;
  SDL_BlitSurface(armes,NULL,SDL_GetVideoSurface(),&dest);
    
  dest.x=550;
  dest.w=quetes->w;
  dest.h=quetes->h;
  SDL_BlitSurface(quetes,NULL,SDL_GetVideoSurface(),&dest);
    
    
  if(alpha_sens==0)
  {
     alpha++;
     if(alpha>=175) alpha_sens=1;
  } else {
     alpha--;
     if(alpha<=125) alpha_sens=0;
  }

  SDL_SetAlpha(gui->surf,SDL_SRCALPHA,alpha); 
  
  src.x=0;
  src.y=300;
  src.w=dest.w=76;
  src.h=dest.h=32;
  dest.x=245+selection*100;
  dest.y=32;
  SDL_BlitSurface(gui->surf,&src,SDL_GetVideoSurface(),&dest);
}

int MenuEquiper(struct s_gui *gui, struct s_player *player, int selectiong, FSOUND_SAMPLE *son)
{
  SDL_Event event;
  SDL_Rect src, dest;
  SDL_Surface *surf;
  int isdone, res, selection, i, lastmove;
  char tmp[255];
  SDL_Color color={255,255,255};
  
  isdone=res=selection=0;
  lastmove=SDL_GetTicks();
  while (!isdone)
  {
    /* Lecture des évènements dans la queue d'évènements */
    while (SDL_PollEvent (&event))
    {      
      switch (event.type)
      {
      case SDL_KEYUP:
	    if(event.key.state==SDL_RELEASED)
	    {
          if(SDL_GetTicks()-lastmove>200)
          {
             if(event.key.keysym.sym==SDLK_ESCAPE)
             {
                FSOUND_PlaySound(FSOUND_FREE,son);
    	        isdone = 1;
             }

             if(event.key.keysym.sym==SDLK_LEFT)
             {
	           FSOUND_PlaySound(FSOUND_FREE,son);
	           switch(selection)
	           {
                 case 0:
                   if(player->joueurs[0].equipement_tete==-1)
                     i=MAX_ITEMS-1;
                   else
     	             i=player->joueurs[0].equipement_tete;
	               while(i!=player->joueurs[0].equipement_tete)
	               {
                     if((player->inventaire[i].type==ITEM_ARMURE_TETE) && (player->inventaire[i].quantite>0))
                       player->joueurs[0].equipement_tete=i;
                     else {
                       i++;
                       if(i<0)
                         player->joueurs[0].equipement_tete=i=-1;
                     }
                   }
                 break;
                 case 1:
                   if(player->joueurs[0].equipement_corps==-1)
                     i=MAX_ITEMS-1;
                   else
     	             i=player->joueurs[0].equipement_corps-1;
	               while(i!=player->joueurs[0].equipement_corps)
	               {
                     if((player->inventaire[i].type==ITEM_ARMURE_CORPS) && (player->inventaire[i].quantite>0))
                       player->joueurs[0].equipement_corps=i;
                     else {
                       i--;
                       if(i<0)
                         player->joueurs[0].equipement_corps=i=-1;
                     }
                   }
                 break;
                 case 2:
                   if(player->joueurs[0].equipement_arme==-1)
                     i=MAX_ITEMS-1;
                   else
     	             i=player->joueurs[0].equipement_arme-1;
	               while(i!=player->joueurs[0].equipement_arme)
	               {
                     if((player->inventaire[i].type==ITEM_ARME) && (player->inventaire[i].quantite>0))
                       player->joueurs[0].equipement_arme=i;
                     else {
                       i--;
                       if(i<0)
                         player->joueurs[0].equipement_arme=i=-1;
                     }
                   }
                 break;
                 default:
                   break;
               } /* !switch(selection) */
             }
             if(event.key.keysym.sym==SDLK_RIGHT)
             {
	           FSOUND_PlaySound(FSOUND_FREE,son);
	           switch(selection)
	           {
                 case 0:
   	               i=player->joueurs[0].equipement_tete+1;
	               while(i!=player->joueurs[0].equipement_tete)
	               {
                     if((player->inventaire[i].type==ITEM_ARMURE_TETE) && (player->inventaire[i].quantite>0))
                       player->joueurs[0].equipement_tete=i;
                     else {
                       i++;
                       if(i>=MAX_ITEMS)
                         player->joueurs[0].equipement_tete=i=-1;
                     }
                   }
                 break;
                 case 1:
   	               i=player->joueurs[0].equipement_corps+1;
	               while(i!=player->joueurs[0].equipement_corps)
	               {
                     if((player->inventaire[i].type==ITEM_ARMURE_CORPS) && (player->inventaire[i].quantite>0))
                       player->joueurs[0].equipement_corps=i;
                     else {
                       i++;
                       if(i>=MAX_ITEMS)
                         player->joueurs[0].equipement_corps=i=-1;
                     }
                   }
                 break;
                 case 2:
   	               i=player->joueurs[0].equipement_arme+1;
	               while(i!=player->joueurs[0].equipement_arme)
	               {
                     if((player->inventaire[i].type==ITEM_ARME) && (player->inventaire[i].quantite>0))
                       player->joueurs[0].equipement_arme=i;
                     else {
                       i++;
                       if(i>=MAX_ITEMS)
                         player->joueurs[0].equipement_arme=i=-1;
                     }
                   }
                 break;
                 default:
                   break;
               } /* !switch(selection) */
             }
             
             if(event.key.keysym.sym==SDLK_UP)
             {
    	       selection--;
	           FSOUND_PlaySound(FSOUND_FREE,son);
             }
             if(event.key.keysym.sym==SDLK_DOWN)
             {
	           selection++;
	           FSOUND_PlaySound(FSOUND_FREE,son);
             }            
             
             if(selection<0) selection=2;
             if(selection>2) selection=0;
             
             lastmove=SDL_GetTicks();
          }
	    }
        break;
      case SDL_QUIT:
        isdone = res = 1;
	  break;
      }
    }

    src.x=src.y=dest.x=dest.y=0;
    src.w=dest.w=800;
    src.h=dest.h=600;
    
    SDL_BlitSurface(gui->menu,&src,SDL_GetVideoSurface(),&dest);

    /* Affichage partie gauche */
    MenuBlitLeftPanel(gui,player,selectiong);
    
    /* Affichages équipement + items équipe */
    memset(&tmp, 0, sizeof(tmp));
    sprintf(tmp, "Eqp. tête             %s",
      (player->joueurs[0].equipement_tete==-1)
      ?"non-équipé"
      :player->inventaire[player->joueurs[0].equipement_tete].nom
      ); 
    surf=TTF_RenderText_Blended(gui->font,tmp,color);
    
    dest.x=220;
    dest.y=132;
    dest.w=surf->w;
    dest.h=surf->h;
    SDL_BlitSurface(surf,NULL,SDL_GetVideoSurface(),&dest);
    SDL_FreeSurface(surf);
    
    memset(&tmp, 0, sizeof(tmp));
    sprintf(tmp, "Eqp. corps        %s",
      (player->joueurs[0].equipement_corps==-1)
      ?"non-équipé"
      :player->inventaire[player->joueurs[0].equipement_corps].nom
      ); 
    surf=TTF_RenderText_Blended(gui->font,tmp,color);
    dest.y+=32;
    dest.w=surf->w;
    dest.h=surf->h;
    SDL_BlitSurface(surf,NULL,SDL_GetVideoSurface(),&dest);
    SDL_FreeSurface(surf);
    
    memset(&tmp, 0, sizeof(tmp));
    sprintf(tmp, "Arme                  %s",
      (player->joueurs[0].equipement_arme==-1)
      ?"non-équipé"
      :player->inventaire[player->joueurs[0].equipement_arme].nom
      ); 
    surf=TTF_RenderText_Blended(gui->font,tmp,color);
    dest.y+=32;
    dest.w=surf->w;
    dest.h=surf->h;
    SDL_BlitSurface(surf,NULL,SDL_GetVideoSurface(),&dest);
    SDL_FreeSurface(surf);
    
    /* Affichage du rect de sélection */
    src.x=0;
    src.y=300;
    src.w=dest.w=76;
    src.h=dest.h=32;
    dest.x=218;
    dest.y=125+selection*32;
    SDL_BlitSurface(gui->surf,&src,SDL_GetVideoSurface(),&dest);
    
    SDL_Flip(SDL_GetVideoSurface());
  }
  return res;
}
