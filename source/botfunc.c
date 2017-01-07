#include <stdlib.h>
#include <ctype.h>
#define _BOT_SOURCE
  #include "prototypes.h"
#undef _BOT_SOURCE

/* Default Variables For The Bot */
#define BOTNAME	  "Bob"
#define BOTRECAP  "~CBB~CTo~CWb"
#define BOTDESC   "is a bot."
#define BOTSITE   "bob.some-new-talker.com"
#define BOTINPHR  "enters the room"
#define BOTOUTPHR "leaves the room"
#define BOTROOM   "Bot_Closet"
#define BOTIP     "127.0.0.1"
#define BOTLEVEL  1

/* Moe's Poor Attempt At A Bot :P */

void create_bot(void)
{
UR_OBJECT user,create_user();
int i=0;

printf("|> Initializing Bot: %s...\n",BOTNAME);
if(botuser!=NULL) {
	if (!strcasecmp(botuser->name,BOTNAME)) { 
		printf("|> Bot Is Already Online!\n");
		return;
		}
	printf("|> Bot User Isn't Null, but name doesn't match BOTNAME\n");
	return;
	}
if ((user=create_user())==NULL) {
	printf("|> Unable to initialize bot: %s...\n",BOTNAME);
	return;
	}
/* Bot String Vars */
strncpy(user->name,BOTNAME,USER_NAME_LEN);
strncpy(user->recap,BOTRECAP,USER_RECAP_LEN);
strncpy(user->desc,BOTDESC,USER_DESC_LEN);
strcpy(user->site,BOTSITE);
strcpy(user->site_port,BOTIP);
strcpy(user->in_phrase,"wanders into the room.");
strcpy(user->out_phrase,"leaves the room");
strcpy(user->pass,"AGviwjfF234"); /* Something fake, as it's not needed      */
strcpy(user->last_site,BOTSITE);  /* but it will get written to a userfile   */
/* Bot Ints */                    /* So we don't want an easy pw for someone */
user->type=BOT_TYPE;              /* To be able to login as the bot user...  */
user->socket=-1;                  /* <--  Do Not Touch!! */
user->port=port[1];
user->level=BOTLEVEL;
user->vis=1;
user->hidden=0;
user->invis=0;
user->room=room_first;
user->prompt=0;
user->prompt+=MALE;
/* initialise user structure */
user->ignuser[0]='\0';
user->afk_mesg[0]='\0';
user->page_file[0]='\0';
user->mail_to[0]='\0';
user->inpstr_old[0]='\0';
user->buff[0]='\0';
user->buffpos=0;
user->filepos=0;
user->read_mail=time(0);
user->invite_room=NULL;
user->command_mode=0;
user->ignall=0;
user->ignall_store=0;
user->chkrev=0;
user->ignore=0;
user->muzzled=0;
user->gaged=0;
user->remote_com=-1;
user->last_input=time(0);
user->last_login=time(0);
user->last_login_len=0;
user->total_login=0;
user->prompt=0;
user->colour=0;
user->charmode_echo=0;
user->misc_op=0;
user->edit_op=0;
user->edit_line=0;
user->charcnt=0;
user->warned=0;
user->accreq=0;
user->afk=0;
user->revline=0;
user->clone_hear=CLONE_HEAR_ALL;
user->malloc_start=NULL;
user->malloc_end=NULL;
user->owner=NULL;
user->pop=NULL;
for(i=0;i<REVTELL_LINES;++i) user->revbuff[i][0]='\0';
user->samesite_all_store=0;
user->samesite_check_store[0]='\0';
user->wrap=0;
user->mashed=0;
user->invis=0;
user->age=34;
user->whostyle=1;
user->last_room=1;
strcpy(user->email,"bot@moenuts.com");
strcpy(user->homepage,"http://www.moenuts.com");
user->array[0]='\0';
user->rules=0;
user->news=0;
user->twin=0;
user->tlose=0;
user->tdraw=0;
user->first=0;
user->shackled=0;
user->shackle_level=0;
user->default_wrap=0;
user->hidden=0;
user->high_ascii=0;
user->start_script=0;
user->fmail=0;
user->temp_level=0;
strcpy(user->married,"Single");
strcpy(user->birthday,"07/10/2004");
user->bank_balance=8192;
user->bank_update=1;
user->bank_temp=0;
strcpy(user->level_alias,"Bot");
strcpy(user->icq,"00000000");
strcpy(user->login_room,room_first->name);
strcpy(user->roomname,BOTROOM);
strcpy(user->roomtopic,"Welcome To My Room!");
user->roombanned=0;
user->cpot=0;
user->cwager=0;
user->inpstr[0]='\0';
user->hang_word[0]='\0';
user->hang_guess[0]='\0';
user->hang_word_show[0]='\0';
user->hang_stage=-1;
user->confirm=0;
user->hideroom=0;
user->fakeage[0]='\0';
user->branded_by[0]='\0';
user->brand_desc[0]='\0';
user->callared_by[0]='\0';
user->callared_desc[0]='\0';
user->callared=0;
user->predesc[0]='\0';
user->bdsm_type=0;
user->bdsm_life_type=0;
user->branded=0;
strcpy(user->owned_by,"Nobody");
user->bet=0;
user->win=0;
user->textcolor=3;
botuser=user;
load_user_details(botuser);
connect_user(botuser);
i=user_object_store(botuser->name,0,botuser);
}

void destroy_bot(void)
{
if (botuser!=NULL) { disconnect_user(botuser); botuser=NULL; }
}

void bot_trigger(UR_OBJECT user, char *str)
{
char tempname[USER_NAME_LEN+3]; /* We need extra space to add a space */
char tempstr[(ARR_SIZE)*2+1];
char *colour_com_strip();

/* convert our checks to lowercase, to help make things not case sensitive */
strncpy(tempname,botuser->name,USER_NAME_LEN);
strncpy(tempstr,colour_com_strip(str),ARR_SIZE*2);
strtolower(tempname);
strtolower(tempstr);
strcat(tempname," ");
if (!strstr(tempstr,tempname)) { return; }

/* If We Got This Far, Then the bot's name is in the string */
if (strstr(tempstr,"hello")) {
	sprintf(text,"%s ~FTsays~CW:~RS Hello %s!\n",botuser->recap,user->recap);
	write_room(botuser->room,text);
	record(botuser->room,text);
	return;
	}
/* There is a space after hi for a reason.  Otherwise "him" "His" "Hirosima"
   will all trigger the bot to say hi if his name happens to be in the string" */
if (strstr(tempstr,"hi ")) {
	sprintf(text,"%s ~FTsays~CW:~RS Hi %s!\n",botuser->recap,user->recap);
	write_room(botuser->room,text);
	record(botuser->room,text);
	return;	
	}
}
