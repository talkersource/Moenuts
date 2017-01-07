/*****************************************************************************

 Moesoft's HTML Generation Functions For Moenuts v1.61e (C)2001 Michael Irving
 (C)1999 Moesoft Developments, All Rights Reserved.  The following contains
 code to automatically update a talker's webpage with the current users stats
 within 24 hours.  The talker's webpage MUST be on the same server, aswell,
 the talker process MUST have write permissions to the talker's HTML
 directory.

*****************************************************************************/

#include "protos.h"

#define WEBUPDATE	1  /* Update Talker's Webpage?  1 = Yes, 0 = No */
#define WEBDIR		"/home/username/public_html"
#define WEBUSERDIR	"/home/username/public_html/userpages"
#define WEBUSERHEADER	"userhead.html"
#define WEBUSERFOOTER	"userfoot.html"
#define WEBBANNER	"banner.gif"
#define WEBSEPARATOR	"blood.gif"
#define WEBBACKGROUND	"background.gif"
#define WEBBGCOLOR	"#000000"  /* BGCOLOR for talker generated pages    */
#define WEBFGCOLOR	"#CCCCCC"  /* FGCOLOR for talker generated pages    */
#define WEBLINKCOLOR	"#FF0000"  /* LINKCOLOR for Talker Generated Pages  */
#define WEBVLINKCOLOR	"#00FF00"  /* VLINKCOLOR for Talker Generated Pages */
#define WEBWHODIR	"/home/username/public_html/"
#define WEBWHOFILE	"webwho.temp"

/* Moesoft HTML Rutines (C)1999-2001 Michael Irving, All Rights Reserved */

int webwho(void)
{

FILE *fp;
UR_OBJECT u;
RM_OBJECT rm;

int cnt,total,invis,mins,idle,logins,hidden;
char line[USER_NAME_LEN+USER_DESC_LEN*2];
char rname[ROOM_NAME_LEN+1],levelname[30],www[ARR_SIZE];
char gender[7],text2[ARR_SIZE+1];
char filename[81];

total=0;  invis=0;  logins=0;  hidden=0;

/* Lets attempt to open the file first?? :) */

sprintf(filename,"%s/%s",WEBWHODIR,WEBWHOFILE);
if (!(fp=fopen(filename,"w"))) {
        sprintf(text,"Error Opening: %s :in WebWho()\n",filename);
	write_syslog(text,1);
#ifdef DEBUG
	write_room(NULL,text);
	write_room(NULL,filename);
#endif
        return 0;
        }

/*** Write Who List Header ***/
        sprintf(text2,"People On %s %s",TALKERNAME,long_date(1));
        fprintf(fp,"~FB_.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._\n");
        fprintf(fp,"~FB_.-[ ~OL~FT%-67.67s ~RS~FB]-._\n",center(text2,66));
        fprintf(fp,"~FB_.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._\n");

/*** Write The Wholist Guts ***/
for(rm=room_first;rm!=NULL;rm=rm->next) {
     if (rm->hidden) continue;
     if (inroom(rm)) {  /* Check To See If Room Is Empty */
        fprintf(fp,"~FB-~OL=~FT+  ~FG%-67.67s  ~FT+~FB=~RS~FB-\n",center(rm->name,66));
        for(u=user_first;u!=NULL;u=u->next) {
	  if (u->login) { logins++; continue; }
	  if (u->room!=rm) continue;
          mins=(int)(time(0) - u->last_login)/60;
          idle=(int)(time(0) - u->last_input)/60;
          if (!u->level && u->muzzled & SCUM) strcpy(levelname,"Scum");
          else strcpy(levelname,u->level_alias);
          if (u->muzzled & JAILED) strcpy(levelname,"Jailed");
          strcpy(gender,"~OL~FGN");
          if (u->prompt & FEMALE) strcpy(gender,"~OL~FMF");
          if (u->prompt & MALE)   strcpy(gender,"~OL~FTM");
          ++total;
          if (u->login) logins++;
          if (u->hidden) { total--; hidden++; continue; }
          if (!u->vis) {
               --total; ++invis;
		continue;
               }
          strcpy(rname,u->room->name);
	  strncpy(www,u->homepage,ARR_SIZE-2);
          sprintf(line," %s %s",u->recap,u->desc);
          if (u->afk) sprintf(line," %s ~RS~FRis away from the keyboard.",u->recap);
          else if (idle>10) sprintf(line," %s ~RS~FMappears to be sleeping.",u->recap);
          else if (u->malloc_start!=NULL) sprintf(line," %s ~RS~FTis using the text editor",u->recap);
	  /* Process User Status */
          cnt=colour_com_count(line);
          fprintf(fp,"~FB-~OL=~FT+ ~FY%s ~RS~FR: ~OL~FM%-12.12s ~RS~FR:~OL~FG%5d~FW:~FR%-5d~RS~FR:~RS<a%%20href=\"%s\">%-*.*s </a>~RS~OL~FT+~FB=~RS~FB-\n",gender,levelname,mins,idle,www,39+cnt*3,39+cnt*3,line);
          continue;
          }
	}
    }
    /*** Write Footer ***/
    fprintf(fp,"~FB_.-._.-._.-._.-._.-._.+._.-._.-._;-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._\n");
    fprintf(fp,"  ~OL~FBThere %s currently %d %s in the %s.\n",(total==1?"is":"are"),total,(total==1?"person":"people"),SHORTNAME);
    fprintf(fp,"~FB_.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._\n\n");
    fprintf(fp,"\n");
    fclose(fp);
    return 1; /* We Assume Everything Went Peachy Keen */
}
