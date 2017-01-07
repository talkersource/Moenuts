/*****************************************************************************

 Moesoft's HTML Generation Functions For Moenuts v1.61e (C)2001 Michael Irving
 (C)1999 Moesoft Developments, All Rights Reserved.  The following contains
 code to automatically update a talker's webpage with the current users stats
 within 24 hours.  The talker's webpage MUST be on the same server, aswell,
 the talker process MUST have write permissions to the talker's HTML
 directory.

*****************************************************************************/

#include "prototypes.h"

#define WEBUPDATE	0  /* Update Talker's Webpage?  1 = Yes, 0 = No */
#define WEBWHOUPDATE    0  /* Do A Webwho Datafile?     1 = Yes, 0 = No */
#define WEBDIR		"../public_html"
#define WEBUSERDIR	"../public_html"
#define WEBUSERFILETYPE	"web" /* Type the userpage's final output will be (.php, .html etc) */
#define WEBBANNER	"banner.gif"
#define WEBSEPARATOR	"sepline.gif"
#define WEBBACKGROUND	"background.gif"
#define WEBBGCOLOR	"#000000"  /* BGCOLOR for talker generated pages    */
#define WEBFGCOLOR	"#CCCCCC"  /* FGCOLOR for talker generated pages    */
#define WEBLINKCOLOR	"#FF0000"  /* LINKCOLOR for Talker Generated Pages  */
#define WEBVLINKCOLOR	"#00FF00"  /* VLINKCOLOR for Talker Generated Pages */
#define WEBWHODIR	"../public_html"
#define WEBWHOFILE	"webwho.temp"
#define UPVERSION	"1"

/* Moesoft HTML Rutines (C)1999-2004 Michael Irving, All Rights Reserved */

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

if (!WEBWHOUPDATE) return 1;

/* Lets attempt to open the file first?? :) */

sprintf(filename,"%s/%s",WEBWHODIR,WEBWHOFILE);
if (!(fp=fopen(filename,"w"))) {
        sprintf(text,"Error Opening : %s : in WebWho()\n",filename);
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

void makeuserpages(UR_OBJECT user)
{

UR_OBJECT u;
FILE *fp, *up;

char filename[81],line[USER_NAME_LEN+1],ir[30],levelname[USER_ALIAS_LEN+2],levelname2[50],gendertype[16];
char filename2[81], filename3[81];
int udcount=0,days,hours,mins,timelen,days2,hours2,mins2,online=0,bytes=0;

write_room(NULL,"\n~CB[~CYMoenuts~CB]~CY: ~CTUpdating Talker User Web Files...\n");
sprintf(filename,"%s/%s",USERFILES,USERLIST);
if (!(fp=fopen(filename,"r"))) {
        sprintf(text,"Error Opening: %s :in makeuserwebpages()\n",filename);
	write_syslog(text,1);
	return;
	}
fgets(line,USER_NAME_LEN+1,fp);
line[strlen(line)-1]='\0';
while(!feof(fp)) {
	if (feof(fp)) { break; }
	if (!(u=get_user(line))) {
	        if ((u=create_user())==NULL) {
        	        write_syslog("ERROR: Unable to create temporary user object in makeuserpages().\n",0);
                	return;
                	}
 	        strcpy(u->name,line);
        	if (!load_user_details(u)) {
                	destruct_user(u);
                	destructed=0;
			if (!feof(fp)) { continue; }
			else { return; }
                	}
		}
	else { online=1; }
	if (!u) { 
                write_syslog("ERROR: Unable to create temporary user object in makeuserpages().\n",0);
                return;
		}
	else {
		/* Lets Make Us Some User Webpages!! */
		sprintf(filename,"%s/%s.%s",WEBUSERDIR,line,WEBUSERFILETYPE);
		if ((up=fopen(filename,"w"))) {
			udcount++;
			days=u->total_login/86400;
			hours=(u->total_login%86400)/3600;
			mins=(u->total_login%3600)/60;
			timelen=(int)(time(0) - u->last_login);
			days2=timelen/86400;
			hours2=(timelen%86400)/3600;
			mins2=(timelen%3600)/60;
			strncpy(levelname,u->level_alias,USER_ALIAS_LEN);
			if (u->prompt & FEMALE) strncpy(levelname2,level_name_fem[u->level],49);
			else strncpy(levelname2,level_name[u->level],49);
			if (u->muzzled & JAILED) strcpy(levelname,"Jailed");
			strcpy(gendertype,"Neuter");
			if (u->prompt & 4) strcpy(gendertype,"Female");
			if (u->prompt & 8) strcpy(gendertype,"Male");
			if (u->invite_room==NULL) strcpy(ir,"<nowhere>");
			if (u->bdsm_type>MAX_BDSM_TYPES) u->bdsm_type=0;
			/* Lets Write The File Now */
			fprintf(up,"%s\n",UPVERSION);
			fprintf(up,"%s %s\n",u->recap,u->desc);
			fprintf(up,"%s\n",u->predesc);
			fprintf(up,"%s\n",levelname);
			fprintf(up,"%s\n",levelname2);
			fprintf(up,"%s\n",bdsm_types[u->bdsm_type]);
			/* fprintf(up,"%s\n",ctime((time_t *)&(u->last_login))); */
			fprintf(up,"%d days, %d hours, %d minutes ago\n",days2,hours2,mins2);
 			fprintf(up,"%d hours, %d minutes\n%d days, %d hours, %d minutes\n",u->last_login_len/3600,(u->last_login_len%3600)/60,days,hours,mins);
			fprintf(up,"%s\n",gendertype);
			fprintf(up,"%s\n",u->email);	
			fprintf(up,"%s\n",u->homepage);
			fprintf(up,"%s\n",u->fakeage);
			fprintf(up,"%s\n",u->birthday);
			fprintf(up,"%s\n",u->married);
			fprintf(up,"%s\n",u->icq);
			fprintf(up,"%d\n",u->bank_balance);
			fprintf(up,"%-5d\n%-5d\n%d\n",u->twin,u->tlose,u->tdraw);
			fclose(up);
			/* Lets Copy Their Profile Over So It Can Be Imported */
			sprintf(filename2,"%s/%s.P",USERFILES,line);
			sprintf(filename3,"%s/%s.profile.%s",WEBUSERDIR,line,WEBUSERFILETYPE);			
			bytes=copyfile(filename2,filename3);
			if (bytes==-1) { 
				sprintf(text,"Copying: %s -> %s~RS - Not Coppied.\n",filename2,filename3);
				write_user(user,text);
				}
			else { 
				sprintf(text,"~RSCopying: %s~RS -> %s~RS [%d bytes]\n",filename2,filename3,bytes);
				write_user(user,text);
				// sprintf(text,"~RSChanging Permissions of %s ",filename3);
				// write_user(user,text);
				// chmod(filename3,S_IROTH);
				write_user(user,"[OK]\n");
				}
			if (!online) {
			  destruct_user(u);
		       	  destructed=0;
			  }
			}
		else { 
		        sprintf(text,"Error: Could not open %s for writing...\n",filename);
	                write_syslog(text,1);
			}
		}
	online=0;
	fgets(line,USER_NAME_LEN+1,fp);
	line[strlen(line)-1]='\0';
	if (feof(fp)) break;
	}
online=0;
sprintf(text,"~CB[~CYM~CYoenuts~CB]~CY: ~CTUpdate Completed... Updated %d user's files...\n\n",udcount);
write_user(user,text);
fclose(fp);

return;
}

int copyfile(char *srcfile, char *dstfile)
{
FILE *sfp;
FILE *dfp;

char ch, sfname[81], dfname[81];
int x=0,fs=0;

strcpy(sfname,srcfile);
strcpy(dfname,dstfile);  
                 
if (!(sfp=fopen(sfname,"r"))) return -1;
if (!(dfp=fopen(dfname,"w"))) return -1;

while(!feof(sfp)) {
        ch=getc(sfp);
	if (feof(sfp)) { break; }
	x=fputc(ch,dfp);
	fs++;
        }
fclose(sfp);
fclose(dfp);
/* Return the filesize (-1 = failure) */
return fs;
}
