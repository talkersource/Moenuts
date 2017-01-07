/*###########################################################################
#                                                                           #
#      _.-._ Moenuts v1.72 (C)1997 - 2004 Michael Irving, AKA Moe _.-._     #
#                   (C)1997 - 2004 Moesoft Developments                     #
#               Based on NUTS v3.3.3 (C)1996 Neil Robertson                 #
#                                                                           #
#############################################################################
#                                                                           #
#  I'd like to thank the following people for giving me a hand in the       #
#  creation of this code.  Reddawg, Thanks for the base code and the        #
#  help you've given me.  Andy, Thanks for adding the Samesite and          #
#  a couple other commands.  Squirt, thanks for supplying me with code      #
#  snippets that got lost, like the game modules, and Nerfingdummy for      #
#  orginally helping me add Poker and Tic Tac Toe, and for answering        #
#  some of the questions I've had.  Special thanks to Arny for supplying    #
#  the seamless reboot code.  Thanks for everything guys!                   #
#  And thanks for everyone else who have given me ideas along the           #
#  way for commands etc.                                                    #
#                                                                           #
#  Thanks Curmitt for helping debug some pointer things ;P                  #
#                                                                           #
#############################################################################
#                          Contact Information                              #
#############################################################################
#                                                                           #
#  Moenuts Official Distribution Site:  http://www.moenuts.com              #
#             Technical Support Email:     support@moenuts.com              #
#                        Project Info:    projects@moenuts.com              #
#                         Bug Reports:        bugs@moenuts.com              #
#                                                                           #
#  Before spamming our mailbox with general help questions, please visit    #
#  our forum on our webpage, and/or post a message there first.  We're      #
#  busy and can't always get to our mail promptly but the message board     #
#  is constantly monitored.   - Thanks.                                     #
#                                                                           #
#############################################################################

    NUTS version 3.3.3 (Triple Three :) - Copyright (C) Neil Robertson 1996
                     Last update: 18th November 1996
                         Last Mod 19 July 1997

 This software is provided as is. It is not intended as any sort of bullet
 proof system for commercial operation (though you may use it to set up a
 pay-to-use system, just don't attempt to sell the code itself) and I accept
 no liability for any problems that may arise from you using it. Since this is
 freeware (NOT public domain , I have not relinquished the copyright) you may
 distribute it as you see fit and you may alter the code to suit your needs.

 Read the COPYRIGHT file for further information.

 Neil Robertson.

 Email    : neil@ogham.demon.co.uk
 Home page: http://www.wso.co.uk/neil.html (need JavaScript enabled browser)
 NUTS page: http://www.wso.co.uk/nuts.html
 Newsgroup: alt.talkers.nuts

 NB: This program listing looks best when the tab length is 5 chars which is
 "set ts=5" in vi.

*****************************************************************************/

#include <stdio.h>
#ifdef _AIX
#include <sys/select.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <time.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <unistd.h>
#include <setjmp.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <crypt.h>

/* Needed to include this .h under Redhat Linux */
#ifdef INCLUDE_CRYPT_H
  #include <crypt.h>
#endif

#if !defined(__GLIBC__) || (__GLIBC__ < 2)
  #define SIGNAL(x,y) signal(x,y)
#else
  extern __sighandler_t __sysv_signal __P ((int __sig, __sighandler_t __handler));
  #define SIGNAL(x,y) __sysv_signal(x,y)
#endif

/* If you redefine HORIZ, Make sure its no more than 79 chars long or you
   will end up with display problems as MOST terminals are 80 Columns wide */

#define HORIZ ".-=-.-=-.-=-.-=-.-=-.-=-.-=-.-=-.-=-.-=-.-=-.-=-.-=-.-=-.-=-.-=-.-=-.-=-.-=-."

/***************************************************************************/
/*        Customizable Library Definitions And Include Files               */
/***************************************************************************/
#include "hostinfo.h"                   /* Host System Information         */
#include "atmospheres.h"                /* Default Atmosphere Library      */
#include "levels.h"                     /* Level Definitions               */
#include "sms.h"			/* Include SMS Message Function H  */
#include "moenuts172.h"                 /* Main Header File For Moenuts    */
#include "prompts.h"                    /* Moenuts Prompt File             */
#include "games.c"                      /* Include Game Library            */
#include "prototypes.h"                 /* ANSI C Protoypes For Moenuts    */
#include "afk.h"                        /* AFK Message Library             */
#include "hermanquotes.h"               /* Quote Of The Day Library        */
#include "moesofthtml.c"                /* Moesoft's Auto-HTML Functions   */
#include "figlet.c"                     /* Add Figlets To Moenuts ;)       */
#include "figlet.protos.h"              /* Figlet prototypes               */
/***************************************************************************/
/*                Please Do Not Change The Following                       */
/***************************************************************************/
#define VERSION "3.3.3"                 /* NUTS Base Version               */
#define MNVERSION "1.72"                /* Moenuts Version                 */
#define GWVERSION "0.9b"                /* Internet Post Office Gateway    */
/***************************************************************************/
/* This function calls the setup routines & contains the main program loop */
/***************************************************************************/

int main(int argc,char *argv[])
{
fd_set readmask;
int i,len,pid;
char inpstr[ARR_SIZE];
char *remove_first();
UR_OBJECT user,next;

strcpy(progname,argv[0]);
if (argc<2) strcpy(confile,CONFIGFILE);
else strcpy(confile,argv[1]);

credit_timer=0;
#ifndef DEBUG
printf("\033[2J\033[0m\033[1m\n");
#endif
printf("%s\n Starting Moenuts %s Telnet Chat Server...\n",HORIZ,MNVERSION);
printf(" Based on NUTS %s (C)1996 by Neil Robertson.\n%s\n",VERSION,HORIZ);
init_globals();
sprintf(text,"|> Starting Server: Moenuts v%s ...\n",MNVERSION);
write_syslog(text,0);
printf("|> Waiting For Old Process To Die Gracefully If Sreboot...\n");
sleep(2);
set_date_time();
init_signals();
load_and_parse_config();
if (!possibly_reboot()) init_sockets();
load_hist();
printf("|> Checking Message Boards...\n");
check_messages(NULL,1);
printf("|> Counting Current Userbase...\n");
count_users();

/* Run in background automatically. */
#ifndef NOFORK
switch(fork()) {
	case -1: boot_exit(11);      /* fork failure    */
	case  0: break;              /* child continues */
	default: sleep(1); exit(0);  /* parent dies     */
	}
#endif
reset_alarm();
pid=getpid();
if (!writepidfile(PIDFILE,pid)) printf("Writepidfile(): Unable to write pid file!\n");
printf("%s\n Moenuts %s Successfully Booted with Process ID %d\n%s\n",HORIZ,MNVERSION,getpid(),HORIZ);
sprintf(text,"|> Moenuts %s successfully booted with PID %d on %s \n\n",MNVERSION,pid,long_date(1));
write_syslog(text,0);

/**** Main program loop. *****/
setjmp(jmpvar); /* jump to here if we crash and crash_action = IGNORE */

while(1) {
	/* set up mask then wait */
	setup_readmask(&readmask);
	if (select(FD_SETSIZE,&readmask,0,0,0)==-1) continue;
	/* check for connection to listen sockets */
        for(i=0;i<2;++i) { /* 3 = Netlink */
		if (FD_ISSET(listen_sock[i],&readmask))
			accept_connection(listen_sock[i],i);
		}
	/* Cycle through users. Use a while loop instead of a for because
	    user structure may be destructed during loop in which case we
	    may lose the user->next link. */
	user=user_first;
	while(user!=NULL) {
		next=user->next; /* store in case user object is destructed */
		/* see if any data on socket else continue */
		if (!FD_ISSET(user->socket,&readmask)) { user=next;  continue; }
		/* see if client (eg telnet) has closed socket */
		inpstr[0]='\0';
		if (!(len=read(user->socket,inpstr,sizeof(inpstr)))) {
			disconnect_user(user);  user=next;
			continue;
			}
		/* ignore control code replies */
		if ((unsigned char)inpstr[0]==255) { user=next;  continue; }
		/* Deal with input chars. If the following if test succeeds we
		   are dealing with a character mode client so call function. */
		if (inpstr[len-1]>=32 || user->buffpos) {
			if (get_charclient_line(user,inpstr,len)) goto GOT_LINE;
			user=next;  continue;
			}
		else terminate(inpstr);

		GOT_LINE:
		no_prompt=0;
		com_num=-1;
		force_listen=0;
		destructed=0;
		user->buff[0]='\0';
		user->buffpos=0;
		user->last_input=time(0);
		if (user->login) {
			login(user,inpstr);
			user=next;
			continue;
			}
	       /* If a dot on its own then execute last inpstr unless its a misc_op */
	       if (!user->misc_op) {
               if ((!strcmp(inpstr,".") || !strcmp(inpstr,",")) && user->inpstr_old[0]) {
				strcpy(inpstr,user->inpstr_old);
				sprintf(text,"%s\n",inpstr);
				write_user(user,text);
				}
			/* else save current one for next time */
			else {
				if (inpstr[0]) strncpy(user->inpstr_old,inpstr,REVIEW_LEN);
				}
			}
		/* Main input check */
		clear_words();
		word_count=wordfind(inpstr);
		if (user->afk) {
			if (user->afk==2) {
				if (!word_count) {
					if (user->command_mode) prompt(user);
					user=next;  continue;
					}
				if (strcmp((char *)crypt(word[0],"NU"),user->pass)) {
					write_user(user,"Incorrect password.\n");
					prompt(user);  user=next;  continue;
					}
				cls(user);
				write_user(user,"Session unlocked, you are no longer AFK.\n");
				}
			user->afk_mesg[0]='\0';
			if (user->chkrev) {
                                sprintf(text,"\07~CW-> ~FMYou Have New Tells!  ~RStype ~FM.revtell ~RSto view them.\n");
				write_user(user,text);
				user->chkrev=0;
				}
	                sprintf(text,"%s comes back from being AFK.\n",user->name);
			write_syslog(text,1);
			if (user->vis && !user->hidden) {
				sprintf(afktext,bafk_msg[rand()%MAX_BAFK],user->recap);
				sprintf(text,BAFKSTRING,afktext);
				write_room(user->room,text);
				}
			else { write_user(user,"~FGYou return from being AFK.\n"); }
			if (user->afk==2) {
				user->afk=0;
				prompt(user);
				user=next;
				continue;
				}
			user->afk=0;
			user->warned=0;
			}
		if (!word_count) {
               if (misc_ops(user,inpstr))  {  user=next;  continue;  }
               if (user->command_mode) prompt(user);
               user=next;  continue;
			}
		if (misc_ops(user,inpstr))  {  user=next;  continue;  }
		com_num=-1;
		whitespace(inpstr);
	  /* Shortcuts */
          if (user->command_mode || strchr(".,;/!<>-'#[]@*&",inpstr[0]))
          exec_com(user,inpstr);
		else say(user,inpstr);
          if (!destructed) prompt(user);
		user=next;
		}

	} /* end while */
return(0);
}

int writepidfile(char *filename, int pid)
{
FILE *fp;

if (!(fp=fopen(filename,"w"))) return 0;
fprintf(fp,"%d",pid);
fclose(fp);
return 1;
}

/************ MAIN LOOP FUNCTIONS ************/

/*** Set up readmask for select ***/
void setup_readmask(fd_set *mask)
{
UR_OBJECT user;
int i;

FD_ZERO(mask);
for(i=0;i<2;++i) FD_SET(listen_sock[i],mask);
/* Do users */
for (user=user_first;user!=NULL;user=user->next)
	if (user->type==USER_TYPE) FD_SET(user->socket,mask);
}

/*** Accept incoming connections on listen sockets ***/
void accept_connection(int lsock,int num)
{
UR_OBJECT user,create_user();
char *get_ip_address(),*get_ip_number(),site[80];
struct sockaddr_in acc_addr;
int accept_sock,size;
int pick=0;

size=sizeof(struct sockaddr_in);
accept_sock=accept(lsock,(struct sockaddr *)&acc_addr,&size);
strcpy(site,get_ip_address(acc_addr));
if (!site_masked(site)) {
     sprintf(text,"~FB-~CB=~FT] ~FMConnection From~FW: ~FY%s ~FT[~CB=~FB-\n",site);
     write_level(ARCH,3,text,NULL);
     }
if (site_banned(site)) {
        write_sock(accept_sock,"\n\rLogins were banned from your site or domain thanks to others!\n\n\r");
	close(accept_sock);
	if (!site_masked(site)) {
           sprintf(text,"~CRAttempted login from banned site:~CW %s ~CRwas disconected.\n",site);
           write_level(WIZ,3,text,NULL);
	   }
        sprintf(text,"Attempted login from banned site: %s.\n",site);
	write_syslog(text,1);
	return;
	}
pick=rand()%MAX_MOTD1;
sprintf(text,"%s/%s.%d",SCREENFILES,MOTD1,pick);
more(NULL,accept_sock,text); /* send pre-login message */
if (num_of_users+num_of_logins>=max_users && !num) {
     write_sock(accept_sock,"\n\rSorry, the talker is full at the moment, Please Try Again Soon!\n\n\r");
	close(accept_sock);
	return;
	}
if ((user=create_user())==NULL) {
     sprintf(text,"\n\r%s: unable to create session.\nThe server may be experiencing dificulties or is out of resources!\n\n\r",syserror);
	write_sock(accept_sock,text);
	close(accept_sock);
	return;
	}
user->socket=accept_sock;
user->login=3;
user->last_input=time(0);
if (!num) {
     user->port=port[0];
     sprintf(text,"Welcome To %s!\n",TALKERNAME);
     write_user(user,text);
     }
else {
     user->port=port[1];
     sprintf(text,"Connection Made To Staff Login.\nWelcome To %s!\n",TALKERNAME);
     write_user(user,text);
     }
strcpy(user->site,site);
strcpy(site,get_ip_number(acc_addr));
strcpy(user->site_port,site);
echo_on(user);
write_user(user,login_prompt);
num_of_logins++;
}

/* Ed. note, written by tref, subbmitted by ruGG,                       -
 - Claimed useful for BSD systems in which gethostbyaddr() calls caused -
 - extreme hanging/blocking of the talker.				-
 - Note, popen is a blocking call however			        */
 /* Resolves a host name 					        */
char *resolve(char *host)
{
FILE *fp;
static char str[256];
char *txt,*t;

sprintf(str,"/usr/bin/host %s",host);
fp=popen(str,"r");

*str=0; fgets(str,255,fp);
pclose(fp);
txt=strchr(str,':');
if (txt) {
	txt++;
	while (isspace(*txt)) txt++;
	t=txt;
	while (*t && *t!='\n') t++;
	*t=0;
	return(txt);
	}
return(host);
}

/* Get net address of accepted connection */
char *get_ip_address(struct sockaddr_in acc_addr)
{
static char site[150];
char ip_site[150];
#ifndef RESOLVEIP
  struct hostent *host;
#endif

site[0]='\0';
strcpy(ip_site,(char *)inet_ntoa(acc_addr.sin_addr));
#ifdef RESOLVEIP
  strcpy(site,((char *)resolve(ip_site)));
#else
  if ((host=gethostbyaddr((char *)&acc_addr.sin_addr,4,AF_INET))!=NULL) {
	strcpy(site,host->h_name);
	strtolower(site);
	}
#endif
if (!site[0]) strcpy(site,ip_site);
return site;
}

/*** Get net address of accepted connection ***/
char *get_ip_number(struct sockaddr_in acc_addr)
{
static char site[80];

strcpy(site,(char *)inet_ntoa(acc_addr.sin_addr)); /* get number addr */
return site;
}

/*** See if users site is banned ***/
int site_banned(char *site)
{
FILE *fp;
char line[82],filename[80];

sprintf(filename,"%s/%s",DATAFILES,SITEBAN);
if (!(fp=fopen(filename,"r"))) return 0;
fscanf(fp,"%s",line);
while(!feof(fp)) {
	if (line[0]!='#' && line[0]) {
		if (strstr(site,line)) {  fclose(fp);  return 1;  }
		}
	fscanf(fp,"%s",line);
	}
fclose(fp);
return 0;
}

/*** See if connect site is to be masked ***/
int site_masked(char *site)
{
FILE *fp;
char line[82],filename[80];

sprintf(filename,"%s/%s",DATAFILES,SITEMASK);
if (!(fp=fopen(filename,"r"))) return 0;
fscanf(fp,"%s",line);
while(!feof(fp)) {
	if (line[0]!='#' && line[0]) {
		if (strstr(site,line)) {  fclose(fp);  return 1;  }
		}
	fscanf(fp,"%s",line);
	}
fclose(fp);
return 0;
}

/*** See if connect site is banned from using 'who' ***/
int whosite_banned(char *site)
{
FILE *fp;
char line[82],filename[80];

sprintf(filename,"%s/%s",DATAFILES,WHOSITEBAN);
if (!(fp=fopen(filename,"r"))) return 0;
fscanf(fp,"%s",line);
while(!feof(fp)) {
	if (line[0]!='#' && line[0]) {
		if (strstr(site,line)) {  fclose(fp);  return 1;  }
		}
	fscanf(fp,"%s",line);
	}
fclose(fp);
return 0;
}

/*** See if users site is banned ***/
int newsite_banned(char *site)
{
FILE *fp;
char line[82],filename[80];

sprintf(filename,"%s/%s",DATAFILES,NEWSITEBAN);
if (!(fp=fopen(filename,"r"))) return 0;
fscanf(fp,"%s",line);
while(!feof(fp)) {
	if (line[0]!='#' && line[0]) {
		if (strstr(site,line)) {  fclose(fp);  return 1;  }
		}
	fscanf(fp,"%s",line);
	}
fclose(fp);
return 0;
}

/*** See if user is banned ***/
int user_banned(char *name)
{
FILE *fp;
char line[82],filename[80];

sprintf(filename,"%s/%s",DATAFILES,USERBAN);
if (!(fp=fopen(filename,"r"))) return 0;
fscanf(fp,"%s",line);
while(!feof(fp)) {
	if (line[0]!='#' && line[0]) {
		if (!strcmp(line,name)) {  fclose(fp);  return 1;  }
		}
	fscanf(fp,"%s",line);
	}
fclose(fp);
return 0;
}

/*** Attempt to get '\n' terminated line of input from a character
     mode client else store data read so far in user buffer. ***/
int get_charclient_line(UR_OBJECT user,char *inpstr,int len)
{
int l;

for(l=0;l<len;++l) {
	/* see if delete entered */
	if (inpstr[l]==8 || inpstr[l]==127) {
		if (user->buffpos) {
			user->buffpos--;
			if (user->charmode_echo) write_user(user,"\b \b");
			}
		continue;
		}
	user->buff[user->buffpos]=inpstr[l];
	/* See if end of line */
	if (inpstr[l]<32 || user->buffpos+2==ARR_SIZE) {
		terminate(user->buff);
		strcpy(inpstr,user->buff);
		if (user->charmode_echo) write_user(user,"\n");
		return 1;
		}
	user->buffpos++;
	}
if (user->charmode_echo && ((user->login!=2 && user->login!=1)
    || password_echo)) write(user->socket,inpstr,len);
return 0;
}

/*** Put string terminate char. at first char < 32 ***/
void terminate(char *str)
{
int i;
for (i=0;i<ARR_SIZE;++i)  {
	if (*(str+i)<32) {  *(str+i)=0;  return;  }
	}
str[i-1]=0;
}


/*** Get words from sentence. This function prevents the words in the
     sentence from writing off the end of a word array element. This is
     difficult to do with sscanf() hence I use this function instead. ***/
int wordfind(char *inpstr)
{
int wn,wpos;

wn=0; wpos=0;
do {
	while(*inpstr<33) if (!*inpstr++) return wn;
	while(*inpstr>32 && wpos<WORD_LEN-1) {
		word[wn][wpos]=*inpstr++;  wpos++;
		}
	word[wn][wpos]='\0';
	wn++;  wpos=0;
	} while (wn<MAX_WORDS);
return wn-1;
}

/*** clear word array etc. ***/
void clear_words(void)
{
int w;
for(w=0;w<MAX_WORDS;++w) word[w][0]='\0';
word_count=0;
}

/************ PARSE CONFIG FILE **************/
void load_and_parse_config(void)
{
FILE *fp;
char line[81]; /* Should be long enough */
char filename[80];
int i,section_in,got_init,got_rooms;
RM_OBJECT rm1,rm2;

section_in=0;
got_init=0;
got_rooms=0;

sprintf(filename,"%s/%s",DATAFILES,confile);
printf("|> Parsing config file \"%s\"...\n",filename);
if (!(fp=fopen(filename,"r"))) {
        perror("|> Moenuts: Can't find config file or don't have permission to open it.\n");  boot_exit(1);
	}
/* Main reading loop */
config_line=0;
fgets(line,81,fp);
while(!feof(fp)) {
	config_line++;
	for(i=0;i<9;++i) wrd[i][0]='\0';
	sscanf(line,"%s %s %s %s %s %s %s %s %s",wrd[0],wrd[1],wrd[2],wrd[3],wrd[4],wrd[5],wrd[6],wrd[7],wrd[8]);
	if (wrd[0][0]=='#' || wrd[0][0]=='\0') {
		fgets(line,81,fp);  continue;
		}
	/* See if new section */
	if (wrd[0][strlen(wrd[0])-1]==':') {
		if (!strcmp(wrd[0],"INIT:")) section_in=1;
		else if (!strcmp(wrd[0],"ROOMS:")) section_in=2;
                        else {
                             fprintf(stderr,"|> Moenuts: Unknown section header \"%s\" on line %d.\n",wrd[0],config_line);
                             fclose(fp);  boot_exit(1);
                             }
		}
	switch(section_in) {
		case 1: parse_init_section();  got_init=1;  break;
		case 2: parse_rooms_section(); got_rooms=1; break;
		default:
               fprintf(stderr,"|> Moenuts: Section header expected on line %d.\n",config_line);
			boot_exit(1);
		}
	fgets(line,81,fp);
	}
fclose(fp);

/* See if required sections were present (SITES is optional) and if
   required parameters were set. */
if (!got_init) {
     fprintf(stderr,"|> Moenuts: \"INIT\" section missing from config file.\n");
	boot_exit(1);
	}
if (!got_rooms) {
     fprintf(stderr,"|> Moenuts: \"ROOMS\" section missing from config file.\n");
	boot_exit(1);
	}
if (!port[0]) {
     fprintf(stderr,"|> Moenuts: Main port number not set in config file.\n");
	boot_exit(1);
	}
if (!port[1]) {
     fprintf(stderr,"|> Moenuts: Wiz port number not set in config file.\n");
	boot_exit(1);
	}
if (port[0]==port[1]) {
     fprintf(stderr,"|> Moenuts: Main and Wiz Port numbers cannot be the same.\n");
	boot_exit(1);
	}
if (room_first==NULL) {
     fprintf(stderr,"|> Moenuts: No rooms configured in config file.\n");
	boot_exit(1);
	}

/* Parsing done, now check data is valid. Check room stuff first. */

for(rm1=room_first;rm1!=NULL;rm1=rm1->next) {
	for(i=0;i<MAX_LINKS;++i) {
		if (!rm1->link_label[i][0]) break;
		for(rm2=room_first;rm2!=NULL;rm2=rm2->next) {
			if (rm1==rm2) continue;
			if (!strcmp(rm1->link_label[i],rm2->label)) {
				rm1->link[i]=rm2;  break;
				}
			}
		if (rm1->link[i]==NULL) {
                fprintf(stderr,"|> Moenuts: Room \"%s\" has undefined link label \"%s\".\n",rm1->name,rm1->link_label[i]);
			boot_exit(1);
			}
		}
        if (!(load_atmospheres(rm1))) atmos_assign(rm1);
	}
/* --Moe-- */
}

/*** Parse init section ***/
void parse_init_section(void)
{
static int in_section=0;
int op,val;
char *options[]={
"mainport","wizport","linkport","system_logging","minlogin_level","mesg_life",
"wizport_level","prompt_def","gatecrash_level","min_private","ignore_mp_level",
"mesg_check_time",
"max_users","heartbeat","login_idle_time","user_idle_time","password_echo",
"ignore_sigterm","max_clones","ban_swearing","crash_action",
"colour_def","time_out_afks","kill_idle_users","charecho_def",
"time_out_maxlevel","*"
};

if (!strcmp(wrd[0],"INIT:")) {
	if (++in_section>1) {
          fprintf(stderr,"|> Moenuts: Unexpected \"INIT\" section header on line %d.\n",config_line);
		boot_exit(1);
		}
	return;
	}
op=0;
while(strcmp(options[op],wrd[0])) {
	if (options[op][0]=='*') {
          fprintf(stderr,"|> Moenuts: Unknown \"INIT\" option on line %d.\n",config_line);
		boot_exit(1);
		}
	++op;
	}
if (!wrd[1][0]) {
     fprintf(stderr,"|> Moenuts: Required parameter missing on line %d.\n",config_line);
	boot_exit(1);
	}
if (wrd[2][0] && wrd[2][0]!='#') {
     fprintf(stderr,"|> Moenuts: Unexpected word following init parameter on line %d.\n",config_line);
	boot_exit(1);
	}
val=atoi(wrd[1]);
switch(op) {
	case 0: /* main port */
	case 1: /* wiz port */
        /* case 2:  ? Netlink Port ? */
	if ((port[op]=val)<1 || val>65535) {
                fprintf(stderr,"|> Moenuts: Illegal port number on line %d.\n",config_line);
		boot_exit(1);
		}
	return;

	case 3:
	if ((system_logging=onoff_check(wrd[1]))==-1) {
          fprintf(stderr,"|> Moenuts: System_logging must be ON or OFF on line %d.\n",config_line);
		boot_exit(1);
		}
	return;

	case 4:
	if ((minlogin_level=get_level(wrd[1]))==-1) {
		if (strcmp(wrd[1],"NONE")) {
               fprintf(stderr,"|> Moenuts: Unknown level name for \"minlogin_level\" on line %d.\n",config_line);
			boot_exit(1);
			}
		minlogin_level=-1;
		}
	return;

	case 5:  /* message lifetime */
	if ((mesg_life=val)<1) {
          fprintf(stderr,"|> Moenuts: Illegal message lifetime on line %d.\n",config_line);
		boot_exit(1);
		}
	return;

	case 6: /* wizport_level */
	if ((wizport_level=get_level(wrd[1]))==-1) {
          fprintf(stderr,"|> Moenuts: Unknown level name for \"wizport_level\" on line %d.\n",config_line);
		boot_exit(1);
		}
	return;

	case 7: /* prompt defaults */
	if ((prompt_def=onoff_check(wrd[1]))==-1) {
          fprintf(stderr,"|> Moenuts: Prompt_def must be ON or OFF on line %d.\n",config_line);
		boot_exit(1);
		}
	return;

	case 8: /* gatecrash level */
	if ((gatecrash_level=get_level(wrd[1]))==-1) {
          fprintf(stderr,"|> Moenuts: Unknown level name for \"gatecrash_level\" on line %d.\n",config_line);
		boot_exit(1);
		}
	return;

	case 9:
	if (val<1) {
          fprintf(stderr,"|> Moenuts: \"min_private_users\" on line %d must be atleast 1.\n",config_line);
		boot_exit(1);
		}
	min_private_users=val;
	return;

	case 10:
	if ((ignore_mp_level=get_level(wrd[1]))==-1) {
          fprintf(stderr,"|> Moenuts: Unknown level name for \"ignore_mp_level\" on line %d.\n",config_line);
		boot_exit(1);
		}
	return;

        case 11: /* mesg_check_time */
	if (wrd[1][2]!=':'
	    || strlen(wrd[1])>5
	    || !isdigit(wrd[1][0])
	    || !isdigit(wrd[1][1])
	    || !isdigit(wrd[1][3])
	    || !isdigit(wrd[1][4])) {
          fprintf(stderr,"|> Moenuts: Invalid message check time on line %d.\n",config_line);
		boot_exit(1);
		}
	sscanf(wrd[1],"%d:%d",&mesg_check_hour,&mesg_check_min);
	if (mesg_check_hour>23 || mesg_check_min>59) {
          fprintf(stderr,"|> Moenuts: Invalid message check time on line %d.\n",config_line);
		boot_exit(1);
		}
	return;

        case 12:
	if ((max_users=val)<1) {
          fprintf(stderr,"|> Moenuts: The value for max_users on line %d must be atleast 1.\n",config_line);
		boot_exit(1);
		}
	return;

        case 13:
	if ((heartbeat=val)<1) {
          fprintf(stderr,"|> Moenuts: The heartbeat on line %d must be atleast 1.\n",config_line);
		boot_exit(1);
		}
	return;

        case 14:
	if ((login_idle_time=val)<10) {
          fprintf(stderr,"|> Moenuts: The login_idle_time on line %d must be atleast 10.\n",config_line);
		boot_exit(1);
		}
	return;

        case 15:
	if ((user_idle_time=val)<10) {
          fprintf(stderr,"|> Moenuts: The user_idle_time value on line %d must be atleast 10.\n",config_line);
		boot_exit(1);
		}
	return;

        case 16:
	if ((password_echo=yn_check(wrd[1]))==-1) {
          fprintf(stderr,"|> Moenuts: The Password_echo option must be YES or NO on line %d.\n",config_line);
		boot_exit(1);
		}
	return;

        case 17:
	if ((ignore_sigterm=yn_check(wrd[1]))==-1) {
          fprintf(stderr,"|> Moenuts: The Ignore_sigterm option must be YES or NO on line %d.\n",config_line);
		boot_exit(1);
		}
	return;

        case 18:
	if ((max_clones=val)<0) {
          fprintf(stderr,"|> Moenuts: The max_clones on line %d cannot be less than 0.\n",config_line);
		boot_exit(1);
		}
	return;

        case 19:
	if ((ban_swearing=yn_check(wrd[1]))==-1) {
          fprintf(stderr,"|> Moenuts: The Ban_swearing option must be YES or NO on line %d.\n",config_line);
		boot_exit(1);
		}
	return;

        case 20:
	  if (!strcmp(wrd[1],"NONE")) crash_action=0;
	  else if (!strcmp(wrd[1],"IGNORE")) crash_action=1;
	  else if (!strcmp(wrd[1],"REBOOT")) crash_action=2;
	  else if (!strcmp(wrd[1],"SREBOOT")) crash_action=3;
	  else {
		fprintf(stderr,"|> Moenuts: Crash_action must be NONE, IGNORE or REBOOT on line %d.\n",config_line);
		boot_exit(1);
		}
	  return;

        case 21:
	if ((colour_def=onoff_check(wrd[1]))==-1) {
          fprintf(stderr,"|> Moenuts: The Colour_def option must be ON or OFF on line %d.\n",config_line);
		boot_exit(1);
		}
	return;

        case 22:
	if ((time_out_afks=yn_check(wrd[1]))==-1) {
          fprintf(stderr,"|> Moenuts: The Time_out_afks option must be YES or NO on line %d.\n",config_line);
		boot_exit(1);
		}
	return;

        case 23:
	if ((kill_idle_users=yn_check(wrd[1]))==-1) {
          	fprintf(stderr,"|> Moenuts: The Kill_idle_users option must be YES or NO on line %d.\n",config_line);
		boot_exit(1);
		}
	return;

        case 24:
	if ((charecho_def=onoff_check(wrd[1]))==-1) {
          fprintf(stderr,"|> Moenuts: The Charecho_def option must be ON or OFF on line %d.\n",config_line);
		boot_exit(1);
		}
	return;

        case 25:
	if ((time_out_maxlevel=get_level(wrd[1]))==-1) {
          fprintf(stderr,"|> Moenuts: Unknown level name for \"time_out_maxlevel\" on line %d.\n",config_line);
		boot_exit(1);
		}
	return;
	}
}

/*** Parse rooms section ***/
void parse_rooms_section(void)
{
static int in_section=0;
int i;
char *ptr1,*ptr2,c;
RM_OBJECT room;

if (!strcmp(wrd[0],"ROOMS:")) {
	if (++in_section>1) {
          fprintf(stderr,"|> Moenuts: Unexpected \"ROOMS\" section header on line %d.\n",config_line);
		boot_exit(1);
		}
	return;
	}
if (!wrd[2][0]) {
     fprintf(stderr,"|> Moenuts: Required parameter(s) missing on line %d in rooms section.\n",config_line);
	boot_exit(1);
	}
if (strlen(wrd[0])>ROOM_LABEL_LEN) {
        fprintf(stderr,"|> Moenuts: Room label too long on line %d.\n",config_line);
        fprintf(stderr,"|> Moenuts: Label = \"%s\"  Length = %d, Max Length = %d.\n",wrd[0],strlen(wrd[0]),ROOM_LABEL_LEN);
	boot_exit(1);
	}
if (strlen(wrd[1])>ROOM_NAME_LEN) {
        fprintf(stderr,"|> Moenuts: Room Name is too long on line %d.\n",config_line);
        fprintf(stderr,"|> Moenuts: Name = \"%s\"  Length = %d, Max Length = %d.\n",wrd[0],strlen(wrd[0]),ROOM_NAME_LEN);
        boot_exit(1);
        }
/* Check for duplicate label or name */
for(room=room_first;room!=NULL;room=room->next) {
	if (!strcasecmp(room->label,wrd[0])) {
             fprintf(stderr,"|> Moenuts: OOPS!  There is a duplicate room label on line %d (%s).\n",config_line,wrd[0]);
	     boot_exit(1);
	     }
	if (!strcasecmp(room->name,wrd[1])) {
             fprintf(stderr,"|> Moenuts: OOPS!  There is a duplicate room name on line %d. (%s)\n",config_line,wrd[1]);
	     boot_exit(1);
	     }
	}
room=create_room();
strcpy(room->label,wrd[0]);
strcpy(room->name,wrd[1]);
strcpy(room->recap,room->name);
strcpy(room->owner2,wrd[5]);
if (!load_room_preferences(room)) {
	sprintf(text,"|> Moenuts: Room Preferences Not Loaded for room %s, Defaults will be created.",room->name);
	write_syslog(text,1);
	}

/* Parse internal links bit ie hl,gd,of etc. MUST NOT be any spaces between
   the commas */
i=0;
ptr1=wrd[2];
ptr2=wrd[2];
while(1) {
	while(*ptr2!=',' && *ptr2!='\0') ++ptr2;
	if (*ptr2==',' && *(ptr2+1)=='\0') {
          	fprintf(stderr,"|> Moenuts: There's a missing link label on line %d.\n",config_line);
		boot_exit(1);
		}
	c=*ptr2;  *ptr2='\0';
	if (!strcmp(ptr1,room->label)) {
          	fprintf(stderr,"|> Moenuts: Room links to itself, but it shouldn't, on line %d.\n",config_line);
		boot_exit(1);
		}
	strcpy(room->link_label[i],ptr1);
	if (c=='\0') break;
	if (++i>=MAX_LINKS) {
	        fprintf(stderr,"|> Moenuts: Too many links for the room on line %d. (Max Links: %d)\n",config_line,MAX_LINKS);
		boot_exit(1);
		}
	*ptr2=c;
	ptr1=++ptr2;
	}

/* Parse access privs */
if (wrd[3][0]=='#') {  room->access=PUBLIC;  return;  }
if (!wrd[3][0] || !strcmp(wrd[3],"BOTH")) room->access=PUBLIC;
else if (!strcmp(wrd[3],"PUB")) room->access=FIXED_PUBLIC;
	else if (!strcmp(wrd[3],"PRIV")) room->access=FIXED_PRIVATE;
		else if (!strcmp(wrd[3],"PER")) room->access=FIXED_PERSONAL;
			else if (!strcmp(wrd[3],"HIDE")) { room->access=FIXED_PERSONAL; room->hidden=1; }
	 			   else {
					fprintf(stderr,"|> Moenuts: Unknown room access type on line %d.\n",config_line);
					boot_exit(1);
					}
/* Parse owner and map stuff */
if (!wrd[4][0]) {
          fprintf(stderr,"|> Moenuts: There is no map name for the room on line %d.\n",config_line);
		boot_exit(1);
		}
if (strlen(wrd[4])>ROOM_LABEL_LEN) {
     fprintf(stderr,"|> Moenuts: The room map label on line %d is too long.\n",config_line);
	boot_exit(1);
	}
strcpy(room->map,wrd[4]);
if (!wrd[5][0] || wrd[5][0]=='#') return;
if (strlen(wrd[5])>USER_NAME_LEN) {
     fprintf(stderr,"|> Moenuts: Owner name too long on line %d.\n",config_line);
	boot_exit(1);
	}
strcpy(room->owner,wrd[5]);
strcpy(room->recap,room->name);
strcpy(room->owner2,wrd[5]);
if (!load_room_preferences(room)) {
	sprintf(text,"|> Moenuts: Room Preferences Not Loaded for room %s, Defaults will be created.",room->name);
	write_syslog(text,1);
	}

}

int yn_check(char *wd)
{
if (!strcmp(wd,"YES")) return 1;
if (!strcmp(wd,"NO")) return 0;
return -1;
}

int onoff_check(char *wd)
{
if (!strcmp(wd,"ON")) return 1;
if (!strcmp(wd,"OFF")) return 0;
return -1;
}

/************ INITIALISATION FUNCTIONS *************/

/*** Initialise globals ***/
void init_globals(void)
{
verification[0]='\0';
port[0]=0;
port[1]=0;
max_users=50;
max_clones=1;
ban_swearing=0;
heartbeat=2;
keepalive_interval=60; /* DO NOT TOUCH!!! */
login_idle_time=180;
user_idle_time=300;
time_out_afks=0;
wizport_level=WIZ;
minlogin_level=-1;
mesg_life=1;
no_prompt=0;
num_of_users=0;
num_of_logins=0;
system_logging=1;
password_echo=0;
ignore_sigterm=0;
crash_action=2;
prompt_def=1;
colour_def=1;
charecho_def=0;
time_out_maxlevel=USER;
mesg_check_hour=0;
mesg_check_min=0;
kill_idle_users=1;
allow_caps_in_name=0; /* Not really used */
rs_countdown=0;
rs_announce=0;
rs_which=-1;
rs_user=NULL;
gatecrash_level=WIZ; /* minimum user level which can enter private rooms */
min_private_users=2; /* minimum num. of users in room before can set to priv */
ignore_mp_level=GOD; /* User level which can ignore the above var. */
user_first=NULL;
user_last=NULL;
room_first=NULL;
room_last=NULL;
po_game_first=NULL; /*** Poker ***/
po_game_last=NULL;  /*** Poker ***/
max_po_hist=0;      /*** Poker ***/
clear_words();
time(&boot_time);
user_count=0;
if (ALLOW_AUTO_PROMOTE) sys_allow_autopromote=1;
else sys_allow_autopromote=0;
if (ALLOW_SUICIDE) sys_allow_suicide=1;
else sys_allow_suicide=0;
}

/*** Initialise the signal traps etc ***/
void init_signals(void)
{
void sig_handler();

SIGNAL(SIGTERM,sig_handler);
SIGNAL(SIGSEGV,sig_handler);
SIGNAL(SIGBUS,sig_handler);
SIGNAL(SIGILL,SIG_IGN);
SIGNAL(SIGTRAP,SIG_IGN);
SIGNAL(SIGIOT,SIG_IGN);
SIGNAL(SIGTSTP,SIG_IGN);
SIGNAL(SIGCONT,SIG_IGN);
SIGNAL(SIGHUP,sig_handler);
SIGNAL(SIGINT,SIG_IGN);
SIGNAL(SIGQUIT,SIG_IGN);
SIGNAL(SIGABRT,SIG_IGN);
SIGNAL(SIGFPE,SIG_IGN);
SIGNAL(SIGPIPE,SIG_IGN);
SIGNAL(SIGTTIN,SIG_IGN);
SIGNAL(SIGTTOU,SIG_IGN);
}


/*** Talker signal handler function. Can either shutdown , ignore or reboot
	if a unix error occurs though if we ignore it we're living on borrowed
	time as usually it will crash completely after a while anyway. ***/

void sig_handler(int sig)
{
force_listen=1;
switch(sig) {
	case SIGTERM:
	if (ignore_sigterm) {
                write_syslog("|> Moenuts: SIGTERM signal received - Ignoring It.\n",1);
		return;
		}
	write_room(NULL,"\n~CB[~CYM~CYoenuts~CB]~CY: ~FT Talker recieved termination signal from server, shutting down.\n\n");
	talker_shutdown(NULL,"a termination signal (SIGTERM)",0);

	case SIGSEGV:
	switch(crash_action) {
		case 0:
                write_room(NULL,"\n\07~CB[~CYM~CYoenuts~CB]~CY:~CR ERROR~CW: ~FTSegmentation fault, initiating shutdown!\n");
		talker_shutdown(NULL,"a segmentation fault (SIGSEGV)",0);

		case 1:
                write_room(NULL,"\n\07~CB[~CYM~CYoenuts~CB]~CY:~CY A segmentation fault has just occured, Ignoreing!\n");
		write_syslog("!! WARNING: A segmentation fault occured!\n",1);
		longjmp(jmpvar,0);

		case 2:
                write_room(NULL,"\n\07~CB[~CYM~CYoenuts~CB]~CY: ~CRSegmentation fault error.  Rebooting talker!\n");
		talker_shutdown(NULL,"a segmentation fault (SIGSEGV)",1);

		case 3:
                write_room(NULL,"\n\07~CB[~CYM~CYoenuts~CB]~CY: ~CRSegmentation fault error.  Attempting To Recover From It!\n");
		do_reboot(NULL);
		}

	case SIGBUS:
	switch(crash_action) {
		case 0:
          	write_room(NULL,"\n\07~CB[~CYM~CYoenuts~CB]~CY: ~CRPANIC!  Bus error, shutting down talker.\n");
		talker_shutdown(NULL,"a bus error (SIGBUS)",0);

		case 1:
          	write_room(NULL,"\n\07~CB[~CYM~CYoenuts~CB]~CY: ~CYWARNING~CW: ~CRA bus error has just occured, ignoreing it!\n");
		write_syslog("WARNING: A bus error occured!\n",1);
		longjmp(jmpvar,0);

		case 2:
          	write_room(NULL,"\n\07~CB[~CYM~CYoenuts~CB]~CY: ~CRPANIC!  A Bus error just Occured!  Rebooting Talker!\n\n");
		talker_shutdown(NULL,"a bus error (SIGBUS)",1);

		case 3:
          	write_room(NULL,"\n\07~CB[~CYM~CYoenuts~CB]~CY: ~CRPANIC!  A Bus error just occured, Attempting to recover!\n\n");
		do_reboot(NULL);
		}
	case SIGHUP:
		{
		write_room(NULL,"\n~BP~CB[~CYM~CYoenuts~CB]~CY: ~CTATTENTION~CW: The talker recieved a reboot request from the server\n");
		write_room(NULL,"~CB[~CYM~CYoenuts~CB]~CY: ~CG           Please wait while the talker attempts to reboot.\n");
		do_reboot(NULL);
		}
	}
}

/*** Initialise sockets on ports ***/
void init_sockets(void)
{
struct sockaddr_in bind_addr;
int i,on,size;

printf("|> Moenuts: Initialising Network Sockets, Main: %d and Wiz: %d\n",port[0],port[1]);
on=1;
size=sizeof(struct sockaddr_in);
bind_addr.sin_family=AF_INET;
bind_addr.sin_addr.s_addr=INADDR_ANY;
for(i=0;i<2;++i) {
	/* create sockets */
	if ((listen_sock[i]=socket(AF_INET,SOCK_STREAM,0))==-1) boot_exit(i+2);

	/* allow reboots on port even with TIME_WAITS */
	setsockopt(listen_sock[i],SOL_SOCKET,SO_REUSEADDR,(char *)&on,sizeof(on));

	/* bind sockets and set up listen queues */
	bind_addr.sin_port=htons(port[i]);
	if (bind(listen_sock[i],(struct sockaddr *)&bind_addr,size)==-1)
		boot_exit(i+5);
	if (listen(listen_sock[i],10)==-1) boot_exit(i+8);

	/* Set to non-blocking , do we need this? Not really. */
	fcntl(listen_sock[i],F_SETFL,O_NDELAY);
	}
}

/************* WRITE FUNCTIONS ************/

/*** Write a NULL terminated string to a socket ***/
void write_sock(int sock,char *str)
{
write(sock,str,strlen(str));
}

/*** Send message to user ***/
void write_user(UR_OBJECT user,char *str)
{
UR_OBJECT u;
int buffpos,sock,i;
char *start,buff[OUT_BUFF_SIZE],*colour_com_strip();
char tempstr[ARR_SIZE*4+2],name[USER_RECAP_LEN+5];
char text2[ARR_SIZE*4+2];
char *namestring();

if (user==NULL) return;

/** modified **/
memset(tempstr,0,sizeof(tempstr)-1);
memset(name,0,sizeof(name)-1);
strncpy(tempstr,str,(ARR_SIZE*3)-2);
strncpy(text2,tempstr,sizeof(text2)-1);

for(u=user_first;u!=NULL;u=u->next) {
	memset(name,0,sizeof(name)-1);
	strncpy(name,u->recap,USER_RECAP_LEN);
	strcat(name,textcolor[user->textcolor]);
	strncpy(tempstr,strrep(text2,u->name,name,ARR_SIZE*3),ARR_SIZE*3);
	strncpy(text2,tempstr,sizeof(text2)-1);
	memset(tempstr,0,sizeof(tempstr)-1);
	}
strncpy(tempstr,text2,sizeof(tempstr)-1);
/* MSN Style Emoticons In Text? */

strrep(tempstr,"(F)","~CR@~CG}--'--,---~RS",ARR_SIZE*3);
strrep(tempstr,"(8)","~CBo/`~RS",ARR_SIZE*3);
strrep(tempstr,"(B)","~CW[_]3~RS",ARR_SIZE*3);
strrep(tempstr,"(D)","~CBY~CG`~RS",ARR_SIZE*3);

/* End Of Emoticons */

str=tempstr;
start=str;
buffpos=0;
sock=user->socket;
/* Process string and write to buffer. We use pointers here instead of arrays
   since these are supposedly much faster (though in reality I guess it depends
   on the compiler) which is necessary since this routine is used all the
   time. */
while(*str) {
	if (*str=='\n') {
		if (buffpos>OUT_BUFF_SIZE-6) {
			write(sock,buff,buffpos);  buffpos=0;
			}
		/* Reset terminal before every newline */
		if (user->colour) {
			memcpy(buff+buffpos,"\033[0m",4);  buffpos+=4;
			}
		*(buff+buffpos)='\n';  *(buff+buffpos+1)='\r';
		buffpos+=2;  ++str;
		}
	else {
          /* See if its a ^ before a ~ , if so then we print colour command
		   as text */
          if (*str=='^' && *(str+1)=='~') {  ++str;  continue;  }
          if (str!=start && *str=='~' && *(str-1)=='^') {
			*(buff+buffpos)=*str;  goto CONT;
			}
          /* Process colour commands eg ~FM. We have to strip out the commands
		   from the string even if user doesnt have colour switched on hence
		   the user->colour check isnt done just yet */
		if (*str=='~') {
			if (buffpos>OUT_BUFF_SIZE-6) {
				write(sock,buff,buffpos);  buffpos=0;
				}
			++str;
			for(i=0;i<NUM_COLS;++i) {
				if (!strncmp(str,colcom[i],2)) {
					/* addict mods for random-ness */
					switch(i) {
					 case 33:
					  i = 6 + rand() % 7;
					 break;
					 case 34:
					  i = 13 + rand() % 7;
					 break;
					 case 35:
					  i = 25 + rand() % 7;
					 break;
					}

					if (user->colour) {
						memcpy(buff+buffpos,colcode[i],strlen(colcode[i]));
						buffpos+=strlen(colcode[i])-1;
						}
					else buffpos--;
					++str;
					goto CONT;
					}
				}
			*(buff+buffpos)=*(--str);
			}
		else *(buff+buffpos)=*str;
		CONT:
		++buffpos;   ++str;
		}
	if (buffpos==OUT_BUFF_SIZE) {
		write(sock,buff,OUT_BUFF_SIZE);  buffpos=0;
		}
	}
if (buffpos) write(sock,buff,buffpos);
/* Reset terminal at end of string */
if (user->colour) write_sock(sock,"\033[0m");
}

/*** Write to users of level 'level' and above or below depending on above
     variable; if 1 then above else below ***/
void write_level(int level,int above,char *str,UR_OBJECT user)
{
UR_OBJECT u;
int logmsgs,bcastmsgs,wizmsgs,atmos;

logmsgs=bcastmsgs=wizmsgs=atmos=0;
switch(above) {
	case 2:
		logmsgs=1;
		bcastmsgs=0;
		wizmsgs=0;
		above=0;
		atmos=0;
		break;
	case 3:
		logmsgs=1;
		bcastmsgs=0;
		wizmsgs=0;
		above=1;
		atmos=0;
		break;
	case 4:
                bcastmsgs=1;
		wizmsgs=0;
		logmsgs=0;
		above=0;
		atmos=0;
		break;
	case 5:
		wizmsgs=1;
		logmsgs=0;
		bcastmsgs=0;
		above=1;
		atmos=0;
		break;
	case 6:
		wizmsgs=0;
		logmsgs=0;
		bcastmsgs=0;
		above=0;
		atmos=1;
		break;
	case 7:
		wizmsgs=0;
		logmsgs=0;
		bcastmsgs=0;
		above=1;
		atmos=1;
		break;
	}
for(u=user_first;u!=NULL;u=u->next) {
	if (u!=user && !u->login && u->type!=CLONE_TYPE) {
		if ((logmsgs && u->ignall) || (bcastmsgs && u->ignall) || (wizmsgs && u->ignall)) continue;
		if (logmsgs && u->ignore & LOGON_MSGS)  continue;
		if (bcastmsgs && u->ignore & BCAST_MSGS)  continue;
		if (wizmsgs && u->ignore & WIZARD_MSGS)  continue;
		if (atmos && u->ignore & ATMOS_MSGS)  continue;
		if ((above && u->level>=level) || (!above && u->level<=level))
			write_user(u,str);
		}
	}
}

/*** Subsid function to below but this one is used the most ***/
void write_room(RM_OBJECT rm,char *str)
{
write_room_except(rm,str,NULL);
}

/*** Write to everyone in room rm except for "user". If rm is NULL write
     to all rooms. ***/
void write_room_except(RM_OBJECT rm,char *str,UR_OBJECT user)
{
UR_OBJECT u;
char text2[ARR_SIZE];

for(u=user_first;u!=NULL;u=u->next) {
	if (u->login
	    || (u->room!=rm && rm!=NULL)
	    || (u->ignall && !force_listen)
	    || ((u->ignore & SHOUT_MSGS) && (com_num==SHOUT || com_num==SEMOTE))
	    || u==user) continue;
	if (u->type==CLONE_TYPE) {
		if (u->clone_hear==CLONE_HEAR_NOTHING || u->owner->ignall) continue;
		/* Ignore anything not in clones room, eg shouts, system messages
		   and semotes since the clones owner will hear them anyway. */
		if (rm!=u->room) continue;
		if (u->clone_hear==CLONE_HEAR_SWEARS) {
			if (!contains_swearing(str)) continue;
			}
		sprintf(text2,"~CT[ %s ~RS~CT]:~RS %s",u->room->recap,str);
		write_user(u->owner,text2);
		}
	else write_user(u,str);
	}
}


/*** Write a string to system log ***/
void write_syslog(char *str,int write_time)
{
FILE *fp;

if (!system_logging || !(fp=fopen(SYSLOG,"a"))) return;
if (!write_time) fputs(str,fp);
else fprintf(fp,"%02d/%02d %02d:%02d:%02d: %s",tmday,tmonth+1,thour,tmin,tsec,str);
fclose(fp);
}

/*** Write a string to login log ***/
void write_loginlog(char *str,int write_time)
{
FILE *fp;

if (!system_logging || !(fp=fopen(LOGINLOG,"a"))) return;
if (!write_time) fputs(str,fp);
else fprintf(fp,"%02d/%02d %02d:%02d:%02d: %s",tmday,tmonth+1,thour,tmin,tsec,str);
fclose(fp);
}

/*** Write a string to user's arrest file ***/
void write_arrestrecord(UR_OBJECT user,char *str,int write_time)
{
FILE *fp;
char filename[80];

sprintf(filename,"%s/%s.A",USERFILES,user->name);
if (!(fp=fopen(filename,"a"))) return;
if (!write_time) fputs(str,fp);
else fprintf(fp,"%02d/%02d %02d:%02d:%02d: %s",tmday,tmonth+1,thour,tmin,tsec,str);
fclose(fp);
}

/******** LOGIN/LOGOUT FUNCTIONS ********/

/*** Login function. Lots of nice inline code :) ***/
void login(UR_OBJECT user,char *inpstr)
{
UR_OBJECT u;
int i;
char *center();
char name[ARR_SIZE+2],passwd[ARR_SIZE+2],site[81];

name[0]='\0';  passwd[0]='\0';

switch(user->login) {
	case 3:
	sscanf(inpstr,"%s",name);
	strcpy(site,user->site);
	if(name[0]<33) {
	        write_user(user,login_prompt);
		return;
		}
	if (!strcmp(name,"quit")) {
                sprintf(text,"~FB-~CB=~FT] ~FMQuit At Login From Site~FT: ~FY%s ~FT[~CB=~FB-\n",site);
                write_level(WIZ,3,text,NULL);
                write_user(user,login_quit);
		disconnect_user(user);
		return;
		}
     if (!strcasecmp(name,"lastlog")) {
                sprintf(text,"~FB-~CB=~FT] ~FMLast Login Log Requested From~FT:~FY %s ~FT[~CB=~FB-\n",site);
	        write_level(WIZ,3,text,NULL);
		lastlogin(user,0);
		return;
		}
     if (!strcasecmp(name,"who")) {
		if (whosite_banned(site)) {
		if (!site_masked(site)) {
                        sprintf(text,"~FB-~CB=~FT] ~FMWho List Requested From Who Banned Site~FT:~FY %s ~FT[~CB=~FB-\n",site);
	                write_level(WIZ,3,text,NULL);
			}
                        write_user(user,"~CRPeople from your site must login to see who's on!\n");
			return;
			}
		else {
			if (!site_masked(site)) {
                                sprintf(text,"~FB-~CB=~FT] ~FMWho List Requested From Site~FT:~FY %s ~FT[~CB=~FB-\n",site);
                		write_level(OWNER,3,text,NULL);
				}
                	who(user,1);
       	         	write_user(user,login_prompt);
			return;
			}
	       }
     if (!strcasecmp(name,"version")) {
          sprintf(text,"~FB-~CB=~FT] ~FMVersion Info Requested From~FT:~FY %s ~FT[~CB=~FB-\n",site);
          write_level(WIZ,3,text,NULL);
	  cls(user);
          show_version(user);
          return;
	  }
	if (strlen(name)<3) {
          write_user(user,login_shortname);
		attempts(user);
		return;
		}
	if (strlen(name)>USER_NAME_LEN) {
          write_user(user,login_longname);
		attempts(user);
		return;
		}
	/* see if only letters in login */
	for (i=0;i<strlen(name);++i) {
		if (!isalpha(name[i])) {
	                write_user(user,login_lettersonly);
			attempts(user);
			return;
			}
		}
	strtolower(name);
	name[0]=toupper(name[0]);
	strcpy(user->name,name);
	strcpy(user->recap,name);
	if (user_banned(name)) {
          	sprintf(text,user_banned_prompt,TALKERNAME);
          	write_user(user,text);
		disconnect_user(user);
		sprintf(text,"Attempted login by banned user %s.\n",name);
		write_syslog(text,1);
		return;
		}
	/* If user has hung on another login clear that session */
	for(u=user_first;u!=NULL;u=u->next) {
		if (u->login && u!=user && !strcmp(u->name,user->name)) {
		disconnect_user(u);  break;
		}
        }
	if (!load_user_details(user)) {
	       if (user->port==port[1]) {
               		write_user(user,"\nSorry, new logins cannot be created on the staff port.\n");
               		sprintf(text,"Please try logging on to port %d!\n",port[0]);
               		write_user(user,text);
			disconnect_user(user);
			return;
			}
		if (minlogin_level>-1) {
               		write_user(user,"\nSorry, new logins cannot be created at this time.\nPlease Try Again Later!\n");
			disconnect_user(user);
			return;
			}
		strcpy(site,user->site);
		if (!site_masked(site)) {
          		sprintf(text,"~CB[~CYNew User~CB]~CW:~CG %s ~CB(~FT%s~CB)\n",name,site);
          		write_level(WIZ,3,text,NULL);
			}
		if (newsite_banned(site)) {
		write_user(user,"\nLogins from your site/domain are banned.\n\n");
		if (!site_masked(site)) {
                sprintf(text,"Attempted login by: %s from banned site for NEW users %s.\n",name,site);
		write_syslog(text,1);
                sprintf(text,"~CB[~CYNew User~CB]~CW:~CG %s ~CB(~CM%s~CB) ~CR[From Banned Site]\n",name,site);
                write_level(WIZ,3,text,NULL);
		}
                disconnect_user(user);
		return;
		}
           sprintf(text,login_welcome,TALKERNAME);
           write_user(user,text);
	   }
	else {
	   if (!user->hidden) {
               sprintf(text,"~CB[ ~CBPre-Login ~CB]~CW: ~RS%s ~RS~CB(~FT%s~CB)\n",user->recap,site);
               write_level(WIZ,3,text,user);
               }
           if (user->port==port[1] && user->level<wizport_level) {
               sprintf(text,"\nSorry, only users of level %s and above can log in on this port.\n\n",level_name[wizport_level]);
			write_user(user,text);
			disconnect_user(user);
			return;
			}
		if (user->level<minlogin_level) {
			write_user(user,"\nSorry, the talker is locked out to users of your level, please try again later.\n\n");
			disconnect_user(user);
			return;
			}
		}
     write_user(user,password_prompt);
	echo_off(user);
	user->login=2;
	return;

	case 2:

	sscanf(inpstr,"%s",passwd);
	if (strlen(passwd)<3) {
          write_user(user,password_short);
		attempts(user);
		return;
		}
	if (strlen(passwd)>PASS_LEN) {
          write_user(user,password_long);
		attempts(user);
		return;
		}
	/* if new user... */
	if (!user->pass[0]) {
		strcpy(user->pass,(char *)crypt(passwd,"NU"));
          write_user(user,password_again);
		user->login=1;
		}
	else {
		if (!strcmp(user->pass,(char *)crypt(passwd,"NU"))) {
          if (!(in_userlist(user->name))) add_userlist(user->name);
		cls(user);
		sprintf(file,"%s",MOTD2);
		show_screen(user);
		if (!user->high_ascii) {
			sprintf(text,"%s\n",enterprompt);
                        write_user(user,center(text,79));
			}
		write_user(user,center("~FTMoenuts v1.72, (C)1997-2003 Michael Irving, All Rights Reserved.\n",79));
		/*  Display Version Prompt */
		user->login=4;
		return;
		}
          write_user(user,password_wrong);
	 	attempts(user);
		}
	return;

	case 1:

	sscanf(inpstr,"%s",passwd);
	if (strcmp(user->pass,(char*)crypt(passwd,"NU"))) {
          write_user(user,password_nomatch);
		attempts(user);
		return;
		}
	echo_on(user);
        strcpy(user->recap,user->name);
        strncpy(user->desc,DEFAULTDESC,USER_DESC_LEN);
        strncpy(user->in_phrase,default_inphr,PHRASE_LEN);
        strncpy(user->out_phrase,default_outphr,PHRASE_LEN);
        strncpy(user->level_alias,default_level_alias,USER_ALIAS_LEN);
        strcpy(user->roomname,"None");
        strcpy(user->roomtopic,"No Topic Set.");
        strcpy(user->roomgiver,"Nobody");
        strcpy(user->login_room,room_first->name);
        strcpy(user->email,"None");
        strcpy(user->homepage,"None");
        strcpy(user->married,"Nobody");
        strcpy(user->birthday,"None");
        strcpy(user->predesc,"None");
        strcpy(user->owned_by,"Nobody");
	strcpy(user->branded_by,"Nobody");
	strcpy(user->brand_desc,"None");
	user->bank_balance=BANKBALANCE;
	strcpy(user->icq,"None");
	user->roombanned=0;
	user->last_site[0]='\0';
	user->level=NEWLEVEL;
	user->muzzled=0;
	user->command_mode=0;
	user->prompt=prompt_def;
	user->colour=colour_def;
	user->charmode_echo=charecho_def;
	user->win=0;
	user->bet=0;
	/* Fight Variable Initialization */
	user->fight_win=0;
	user->fight_lose=0;
	user->fight_draw=0;
        /* Paintball Variable Initialization */
	user->paintballs=0;
	user->splatters=0;
	user->painted=0;
	user->hps=0;
        /* USERVAR */
	save_user_details(user,1);
        sprintf(text,"~CT[~FMNew user~FT]~FW: ~FG%s ~FYcreated.\n",user->name);
        write_level(WIZ,3,text,NULL);
        sprintf(text,"New user \"%s\" created.\n",user->name);
	write_syslog(text,1);
	write_loginlog(text,1);
	add_userlist(user->name);
	cls(user);
	sprintf(file,"%s",MOTD2);
	show_screen(user);
	sprintf(text,"%s",enterprompt);
        write_user(user,text);
	user->login=4;
	return;

	case 4: /* Connect User */

	echo_on(user);
        write_user(user,"\n");
	/* Causes some terminals to allow the display of high ascii */
	if (user->high_ascii==2) write_user(user,"\033(U");
	if (user->high_ascii) cls(user);
	connect_user(user);
	}
}

/*** Count up attempts made by user to login ***/
void attempts(UR_OBJECT user)
{
user->attempts++;
if (user->attempts==3) {
     write_user(user,login_attempts);
	disconnect_user(user);
	return;
	}
user->login=3;
user->pass[0]='\0';
write_user(user,login_prompt);
echo_on(user);
}

/*** Load the users details ***/
int load_user_details(UR_OBJECT user)
{
FILE *fp;
char line[ARR_SIZE],filename[80];
int temp1=0,temp2=0,temp3=0;

sprintf(filename,"%s/%s.D",USERFILES,user->name);
if (!(fp=fopen(filename,"r"))) return 0;

fscanf(fp,"%s",user->pass); /* password */
fscanf(fp,"%d %d %d %d %d %d %d %d %d %d",&temp1,&temp2,&user->last_login_len,&temp3,&user->level,&user->prompt,&user->muzzled,&user->charmode_echo,&user->command_mode,&user->colour);
fscanf(fp,"%d %d %d %d %d %d\n",&user->wrap,&user->mashed,&user->invis,&user->age,&user->whostyle,&user->last_room);
user->last_login=(time_t)temp1;
user->total_login=(time_t)temp2;
user->read_mail=(time_t)temp3;
user->gaged=user->mashed;
fscanf(fp,"%s\n",line);
strncpy(user->last_site,line,SITE_NAME_LEN);
/* Need to do the rest like this 'cos they may be more than 1 word each */
fgets(line,USER_DESC_LEN+2,fp);
line[strlen(line)-1]=0;
strncpy(user->desc,line,USER_DESC_LEN);
fgets(line,PHRASE_LEN+2,fp);
line[strlen(line)-1]=0;
strncpy(user->in_phrase,line,PHRASE_LEN);
fgets(line,PHRASE_LEN+2,fp);
line[strlen(line)-1]=0;
strncpy(user->out_phrase,line,PHRASE_LEN);
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strncpy(user->email,line,EMAIL_ADDR_LEN);
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strncpy(user->homepage,line,URL_LEN);
/* Added */
if (!feof(fp)) {
     fscanf(fp,"%d %d %d %d %d %d %d %d %d %d %d %d\n",&user->start_script,&user->rules,&user->fmail,&user->default_wrap,&user->shackled,&user->shackle_level,&user->hidden,&user->high_ascii,&user->twin,&user->tlose,&user->tdraw,&user->roombanned);
     fgets(line,USER_RECAP_LEN+1,fp);
     line[strlen(line)-1]=0;
     strncpy(user->recap,line,USER_RECAP_LEN);
     fgets(line,USER_ALIAS_LEN+1,fp);
     line[strlen(line)-1]=0;
     strncpy(user->level_alias,line,USER_ALIAS_LEN);
     fgets(line,ROOM_NAME_LEN+1,fp);
     line[strlen(line)-1]=0;
     strncpy(user->roomname,line,ROOM_NAME_LEN);
     fgets(line,ARR_SIZE,fp);
     line[strlen(line)-1]=0;
     strncpy(user->roomtopic,line,TOPIC_LEN);
     fgets(line,USER_NAME_LEN+1,fp);
     line[strlen(line)-1]=0;
     strncpy(user->roomgiver,line,USER_NAME_LEN);
     fgets(line,ROOM_NAME_LEN+1,fp);
     line[strlen(line)-1]=0;
     strncpy(user->login_room,line,ROOM_NAME_LEN);
     fgets(line,BIRTHDAY_LEN+1,fp);
     line[strlen(line)-1]=0;
     strncpy(user->birthday,line,BIRTHDAY_LEN);
     fgets(line,FAKE_AGE_LEN+1,fp);
     line[strlen(line)-1]=0;
     strncpy(user->fakeage,line,FAKE_AGE_LEN);
     fgets(line,USER_RECAP_LEN+1,fp);
     line[strlen(line)-1]=0;
     strncpy(user->married,line,USER_RECAP_LEN);
     fgets(line,ICQ_NUMBER_LEN+1,fp);
     line[strlen(line)-1]=0;
     strncpy(user->icq,line,ICQ_NUMBER_LEN);
     }
fclose(fp);
if (!load_extended_user_details(user)) {
	sprintf(text,"Error Loading Extended Details for %s.\n",user->name);
	write_syslog(text,1);
	}
return 1;
}

/*** Save a users extended stats ***/
int save_extended_user_details(UR_OBJECT user)
{
FILE *fp;
char filename[80],tempstr[ARR_SIZE];
int temp;

sprintf(filename,"%s/%s.E",USERFILES,user->name);
if (!(fp=fopen(filename,"w"))) {
	sprintf(text,"%s: failed to save your extended details.\n",syserror);
	write_user(user,text);
	sprintf(text,"save_extended_user_details(): Failed to save %s's extended details.\n",user->name);
	write_syslog(text,1);
	return 0;
	}

/* I'm Guessing 96 Integers Will Be Enough For Future Expansion For A While? */

temp=0;  strcpy(tempstr,"None");  /* Temporary Variable Initialization */

fprintf(fp,"%d %d %d %d %d %d\n",user->ignore,user->branded,user->callared,user->bank_update,user->hideroom,user->bdsm_type);
fprintf(fp,"%d %d %d %d %d %d\n",user->mailbox_limit,user->bank_balance,user->bdsm_life_type,user->hide_email,temp,temp);
fprintf(fp,"%d %d %d %d %d %d\n",user->paintballs,user->hps,user->painted,user->splatters,user->textcolor,temp);
fprintf(fp,"%d %d %d %d %d %d\n",temp,temp,temp,temp,temp,temp);
fprintf(fp,"%d %d %d %d %d %d\n",temp,temp,temp,temp,temp,temp);
fprintf(fp,"%d %d %d %d %d %d\n",temp,temp,temp,temp,temp,temp);
fprintf(fp,"%d %d %d %d %d %d\n",temp,temp,temp,temp,temp,temp);
fprintf(fp,"%d %d %d %d %d %d\n",temp,temp,temp,temp,temp,temp);
fprintf(fp,"%d %d %d %d %d %d\n",temp,temp,temp,temp,temp,temp);
fprintf(fp,"%d %d %d %d %d %d\n",temp,temp,temp,temp,temp,temp);
fprintf(fp,"%d %d %d %d %d %d\n",temp,temp,temp,temp,temp,temp);
fprintf(fp,"%d %d %d %d %d %d\n",temp,temp,temp,temp,temp,temp);
fprintf(fp,"%d %d %d %d %d %d\n",temp,temp,temp,temp,temp,temp);
fprintf(fp,"%d %d %d %d %d %d\n",temp,temp,temp,temp,temp,temp);
fprintf(fp,"%d %d %d %d %d %d\n",temp,temp,temp,temp,temp,temp);
fprintf(fp,"%d %d %d %d %d %d\n",temp,temp,temp,temp,temp,temp);

/* And 25 Character Spots Are Good Enough For Now For Expansion? */

fprintf(fp,"%s\n",user->branded_by);
fprintf(fp,"%s\n",user->brand_desc);
fprintf(fp,"%s\n",user->callared_by);
fprintf(fp,"%s\n",user->callared_desc);
fprintf(fp,"%s\n",tempstr);
fprintf(fp,"%s\n",user->predesc);
fprintf(fp,"%s\n",user->owned_by);
fprintf(fp,"%s\n",tempstr);
fprintf(fp,"%s\n",tempstr);
fprintf(fp,"%s\n",tempstr);
fprintf(fp,"%s\n",tempstr);
fprintf(fp,"%s\n",tempstr);
fprintf(fp,"%s\n",tempstr);
fprintf(fp,"%s\n",tempstr);
fprintf(fp,"%s\n",tempstr);
fprintf(fp,"%s\n",tempstr);
fprintf(fp,"%s\n",tempstr);
fprintf(fp,"%s\n",tempstr);
fprintf(fp,"%s\n",tempstr);
fprintf(fp,"%s\n",tempstr);
fprintf(fp,"%s\n",tempstr);
fprintf(fp,"%s\n",tempstr);
fprintf(fp,"%s\n",tempstr);
fprintf(fp,"%s\n",tempstr);
fprintf(fp,"%s\n",tempstr);
fclose(fp);
return 1;
}

/*** Load the extended users details ***/
int load_extended_user_details(UR_OBJECT user)
{
FILE *fp;
char line[ARR_SIZE],filename[80],tempstr[6];
int temp1=0,temp2=0,temp3=0,temp4=0,temp5=0,temp6=0;

sprintf(filename,"%s/%s.E",USERFILES,user->name);
if (!file_exists(filename)) {
        write_user(user,"~CYMoenuts: Your extended user information has not yet been created...\n");
        write_user(user,"~CY         One moment while I create them for you...\n");
        if (!save_extended_user_details(user)) write_user(user,"~CR         Update Failed -- Notify An Admin!\n");
        else write_user(user,"~CY         Update Completed, Loading Extended Information!\n");
	}
if (!(fp=fopen(filename,"r"))) return 0;

/* I'm sure 96 Integer Spots Will Be More Than Enough For Future Expansion */

fscanf(fp,"%d %d %d %d %d %d\n",&user->ignore,&user->branded,&user->callared,&user->bank_update,&user->hideroom,&user->bdsm_type);
fscanf(fp,"%d %d %d %d %d %d\n",&user->mailbox_limit,&user->bank_balance,&user->bdsm_life_type,&user->hide_email,&temp5,&temp6);
fscanf(fp,"%d %d %d %d %d %d\n",&user->paintballs,&user->hps,&user->painted,&user->splatters,&user->textcolor,&temp6);
fscanf(fp,"%d %d %d %d %d %d\n",&temp1,&temp2,&temp3,&temp4,&temp5,&temp6);
fscanf(fp,"%d %d %d %d %d %d\n",&temp1,&temp2,&temp3,&temp4,&temp5,&temp6);
fscanf(fp,"%d %d %d %d %d %d\n",&temp1,&temp2,&temp3,&temp4,&temp5,&temp6);
fscanf(fp,"%d %d %d %d %d %d\n",&temp1,&temp2,&temp3,&temp4,&temp5,&temp6);
fscanf(fp,"%d %d %d %d %d %d\n",&temp1,&temp2,&temp3,&temp4,&temp5,&temp6);
fscanf(fp,"%d %d %d %d %d %d\n",&temp1,&temp2,&temp3,&temp4,&temp5,&temp6);
fscanf(fp,"%d %d %d %d %d %d\n",&temp1,&temp2,&temp3,&temp4,&temp5,&temp6);
fscanf(fp,"%d %d %d %d %d %d\n",&temp1,&temp2,&temp3,&temp4,&temp5,&temp6);
fscanf(fp,"%d %d %d %d %d %d\n",&temp1,&temp2,&temp3,&temp4,&temp5,&temp6);
fscanf(fp,"%d %d %d %d %d %d\n",&temp1,&temp2,&temp3,&temp4,&temp5,&temp6);
fscanf(fp,"%d %d %d %d %d %d\n",&temp1,&temp2,&temp3,&temp4,&temp5,&temp6);
fscanf(fp,"%d %d %d %d %d %d\n",&temp1,&temp2,&temp3,&temp4,&temp5,&temp6);
fscanf(fp,"%d %d %d %d %d %d\n",&temp1,&temp2,&temp3,&temp4,&temp5,&temp6);

if (feof(fp)) {
	write_user(user,"Error Loading Extended User Stats, Pre-mature End Of File Detected!\n");
	fclose(fp);
	return 0;
	}

/* And 25 Character Spots Are Good Enough For Now For Expansion? */

fgets(line,USER_NAME_LEN+1,fp);
line[strlen(line)-1]=0;
strncpy(user->branded_by,line,USER_NAME_LEN);
fgets(line,BRAND_DESC_LEN+1,fp);
line[strlen(line)-1]=0;
strncpy(user->brand_desc,line,BRAND_DESC_LEN);
fgets(line,USER_NAME_LEN+1,fp);
line[strlen(line)-1]=0;
strncpy(user->callared_by,line,USER_NAME_LEN);
fgets(line,BRAND_DESC_LEN+1,fp);
line[strlen(line)-1]=0;
strncpy(user->callared_desc,line,BRAND_DESC_LEN);
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strncpy(tempstr,line,FAKE_AGE_LEN);
fgets(line,USER_PREDESC_LEN+1,fp);
line[strlen(line)-1]=0;
strncpy(user->predesc,line,USER_PREDESC_LEN);
fgets(line,USER_NAME_LEN+1,fp);
line[strlen(line)-1]=0;
strncpy(user->owned_by,line,USER_NAME_LEN);
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strncpy(tempstr,line,5);
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strncpy(tempstr,line,5);
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strncpy(tempstr,line,5);
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strncpy(tempstr,line,5);
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strncpy(tempstr,line,5);
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strncpy(tempstr,line,5);
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strncpy(tempstr,line,5);
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strncpy(tempstr,line,5);
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strncpy(tempstr,line,5);
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strncpy(tempstr,line,5);
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strncpy(tempstr,line,5);
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strncpy(tempstr,line,5);
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strncpy(tempstr,line,5);
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strncpy(tempstr,line,5);
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strncpy(tempstr,line,5);
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strncpy(tempstr,line,5);
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strncpy(tempstr,line,5);
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strncpy(tempstr,line,5);
fclose(fp);
return 1;
}

/*** Save a users stats ***/
int save_user_details(UR_OBJECT user,int save_current)
{
FILE *fp;
char filename[80],temp[ARR_SIZE];

sprintf(filename,"%s/%s.D",USERFILES,user->name);
if (!(fp=fopen(filename,"w"))) {
	sprintf(text,"%s: failed to save your details.\n",syserror);
	write_user(user,text);
	sprintf(text,"SAVE_USER_STATS: Failed to save %s's details.\n",user->name);
	write_syslog(text,1);
	return 0;
	}
fprintf(fp,"%s\n",user->pass);
if (save_current) fprintf(fp,"%d %d %d ",(int)time(0),(int)user->total_login,(int)(time(0)-user->last_login));
else fprintf(fp,"%d %d %d ",(int)user->last_login,(int)user->total_login,user->last_login_len);
fprintf(fp,"%d %d %d %d %d %d %d\n",(int)user->read_mail,user->level,user->prompt,user->muzzled,user->charmode_echo,user->command_mode,user->colour);
fprintf(fp,"%d %d %d %d %d %d\n",user->wrap,user->mashed,user->invis,user->age,user->whostyle,user->last_room);
if (save_current) fprintf(fp,"%s\n",user->site);
else fprintf(fp,"%s\n",user->last_site);
fprintf(fp,"%s\n",user->desc);
fprintf(fp,"%s\n",user->in_phrase);
fprintf(fp,"%s\n",user->out_phrase);
fprintf(fp,"%s\n",user->email);
fprintf(fp,"%s\n",user->homepage);
fprintf(fp,"%d %d %d %d %d %d %d %d %d %d %d %d\n",user->start_script,user->rules,user->fmail,user->default_wrap,user->shackled,user->shackle_level,user->hidden,user->high_ascii,user->twin,user->tlose,user->tdraw,user->roombanned);
temp[0]='\0';
strncpy(temp,user->recap,USER_RECAP_LEN);
fprintf(fp,"%s\n",temp);
temp[0]='\0';
strncpy(temp,user->level_alias,USER_ALIAS_LEN);
fprintf(fp,"%s\n",temp);
temp[0]='\0';
strncpy(temp,user->roomname,ROOM_NAME_LEN);
fprintf(fp,"%s\n",temp);
temp[0]='\0';
strncpy(temp,user->roomtopic,TOPIC_LEN);
fprintf(fp,"%s\n",temp);
temp[0]='\0';
strncpy(temp,user->roomgiver,USER_NAME_LEN);
fprintf(fp,"%s\n",temp);
temp[0]='\0';
strncpy(temp,user->login_room,ROOM_NAME_LEN);
fprintf(fp,"%s\n",temp);
fprintf(fp,"%s\n",user->birthday);
fprintf(fp,"%s\n",user->fakeage);  /* Fake Age String */
fprintf(fp,"%s\n",user->married);
fprintf(fp,"%s\n",user->icq);
fclose(fp);
if (!save_extended_user_details(user)) {
	sprintf(text,"Error Saving Extended Details for %s.\n",user->name);
	write_syslog(text,1);
	}
return 1;
}


/*** Get room struct pointer from abbreviated name ***/
RM_OBJECT get_room(char *name)
{
RM_OBJECT rm;
char trname[ROOM_RECAP_LEN+2];
char *colour_com_strip();
char rmname[ROOM_RECAP_LEN+2];
int cnt=0;

/* Search fo EXACT match first */
for(rm=room_first;rm!=NULL;rm=rm->next) {
	if (!strcasecmp(rm->name,name)) return rm;
	}

strncpy(rmname,name,ROOM_RECAP_LEN);
rm=NULL;
for(rm=room_first;rm!=NULL;rm=rm->next) {
	strcpy(trname,colour_com_strip(rm->recap));
	strtolower(trname);
	if (!strncmp(trname,rmname,strlen(rmname))) {
		cnt++;
		return rm;
		}
	}
return NULL;
}

/*** Connect the user to the talker proper ***/
void connect_user(UR_OBJECT user)
{
UR_OBJECT u,u2;
RM_OBJECT rm;
RM_OBJECT jail;
RM_OBJECT room;
RM_OBJECT newroom;

char temp[30],levelname[20],text2[ARR_SIZE];

/* See if user already connected */
strcpy(user->ignuser,"NONE");
for(u=user_first;u!=NULL;u=u->next) {
	if (user!=u && user->type!=CLONE_TYPE && !strcmp(user->name,u->name)) {
		rm=u->room;
		write_user(user,"\n\n~CWYou are already logged in.\n~CWSwapping Sessions...\n");
		sprintf(text,"%s swapped sessions. (%s:%s)\n",user->name,user->site,user->site_port);
		write_loginlog(text,1);
		close(u->socket);
		u->socket=user->socket;
		strcpy(u->ignuser,user->ignuser);
		strcpy(u->site,user->site);
		strcpy(u->site_port,user->site_port);
          	if (user->pop!=NULL) u->pop=user->pop;  /* poker */
                sprintf(text,"%s ~RS %s %s\n",session_swap,user->recap,user->desc);
		if (user->hidden) { write_level(OWNER,3,text,NULL); }
		else if (!user->vis && !user->hidden) { write_level(WIZ,3,text,NULL); }
		else { write_room(NULL,text); }
		destruct_user(user);
		num_of_logins--;
                look(u);
                prompt(u);
		/* Reset the sockets on any clones */
		for(u2=user_first;u2!=NULL;u2=u2->next) {
			if (u2->type==CLONE_TYPE && u2->owner==user) {
				u2->socket=u->socket;  u->owner=u;
				}
			}
		return;
		}
	}

/* Announce users logon. You're probably wondering why Ive done it this strange
   way , well its to avoid a minor bug that arose when as in 3.3.1 it created
   the string in 2 parts and sent the parts seperately over a netlink. If you
   want more details email me. */

if (user->invis==USERINVIS) user->vis=0;
else user->vis=1;

/* Set User's Room To The Right Room If They Are Jailed Upon Login */

user->room=room_first;
jail=get_room(jail_room);
room=get_room(user->login_room);
newroom=get_room(newbie_room);
if (!newroom) newroom=room_first;
if (user->muzzled & JAILED) user->room=jail;
if (user->last_room && room!=NULL) {
	user->room=room;
	if (room->access==PERSONAL && strcmp(user->name,room->owner)) user->room=room_first;
	if (room->access==FIXED_PERSONAL && strcmp(user->name,room->owner)) user->room=room_first;
	if (room->access==PRIVATE) user->room=room_first;
	}
if (user->level==NEW && strcasecmp(newbie_room,"none")) user->room=newroom; /* Force Newbies To newbie Room */
strcpy(levelname,level_name[user->level]);
if (user->prompt & FEMALE) strcpy(levelname,level_name_fem[user->level]);
sprintf(text2,announce_prompt,SHORTNAME);
if (!user->hidden) {
     if (user->vis) {
          sprintf(text,"%s ~RS%s %s ~RS(~FT%s~RS)\n",text2,user->recap,user->desc,user->room->recap);
          write_level(NEW,3,text,user);
          }
     else {
          sprintf(text,"%s ~RS%s %s ~RS(~FT%s~RS)\n",text2,user->recap,user->desc,user->room->recap);
          write_level(user->level,3,text,NULL);
          }
     if (!user->vis && user->level>USER) {
          if (strstr(user->site,user->site_port)) sprintf(text,"~FMConnecting From Site~FW: ~FT%s\n",user->site);
          else sprintf(text,"~FMConnecting From Site~FW: ~FT%s:%s\n",user->site,user->site_port);
          write_level(user->level,5,text,NULL);
          }
     else {
          if (strstr(user->site,user->site_port)) sprintf(text,"~FMConnecting From Site~FW: ~FT%s\n",user->site);
          else sprintf(text,"~FMConnecting From Site~FW: ~FT%s:%s\n",user->site,user->site_port);
          write_level(WIZ,3,text,NULL);
          }
     }
else {
    sprintf(text,"\n~CB[ ~FMHidden ~CB]~CW: ~RS%s %s ~RS(~FT%s~RS)\n",user->recap,user->desc,user->room->recap);
    write_level(OWNER,3,text,NULL);
    }
if (user->muzzled & JAILED) {
     sprintf(text,login_arrest1,user->recap);
     write_room_except(room_first,text,user);
     sprintf(text,login_arrest2);
     write_user(user,text);
     }

/* send post-login message and other logon stuff to user */

/* Set Telnet Window Title - addIct */
sprintf(text,"\033]0;%s\a", TALKERNAME);
write_user(user,text);

sprintf(text2,announce_prompt,SHORTNAME);
/* Turn On User's Wrap If (user->wrap==1) */
if (user->wrap) { sprintf(text,"%c[?7h",27);  write_user(user,text); }
if (user->last_site[0]) {
	sprintf(temp,"%s",ctime(&user->last_login));
	temp[strlen(temp)-1]=0;
        sprintf(text,"%s ~RS%s %s ~RS(~FT%s~RS)\n~RS~FMYou were last on %s from %s.\n\n",text2,user->recap,user->desc,user->room->recap,temp,user->last_site);
	}
else sprintf(text,"%s ~RS%s %s ~RS(~FT%s~RS)\n",text2,user->recap,user->desc,user->room->recap);
write_user(user,text);
user->last_login=time(0); /* set to now */
look(user);

/* Auto Promote Message */

if ((user->level==NEW && !(user->muzzled & JAILED)) && sys_allow_autopromote) {
     sprintf(file,"%s/%s",DATAFILES,WELCOMEUSER);
     if (!show_screen(user)) {
     write_user(user,"\n");
     sprintf(text,"~CTWelcome To %s, %s!~CG  Please take a moment and set the following items.\n",user->name,TALKERNAME);
     write_user(user,text);
     write_user(user,"~CGThen you will be Auto-Promoted to the next level.\n\n");
     write_user(user,"~CTStep ~CM1~CB... ~CGType~CW:  ~RS.accreq ~FT[Your ~CWREAL VALID~FT E-Mail Address] (REQIRED!)\n");
     write_user(user,"~CTStep ~CM2~CB... ~CGType~CW:  ~RS.desc ~FT<a new description>  i.e.  .desc is new at this.\n");
     write_user(user,"~CTStep ~CM3~CB... ~CGType~CW:  ~RS.set gender ~FTm/f   ~FR(It is Required)\n");
     write_user(user,"~CTStep ~CM4~CB... ~CGType~CW:  ~RS.entpro  ~FTAnd enter a small profile of yourself.\n");
     write_user(user,"~CTStep ~CM5~CB... ~CGType~CW:  ~RS.set age ~FT<your REAL age>  (Nobody will see this age)\n\n");
     write_user(user,"~CGAnd you will be promoted to the next level.\n");
     write_user(user,"~CG** Note:  You can turn color on or off by typeing .color **\n\n");
     write_user(user,"~CM~ULPlease make sure the ~CWemail address~CM you use is ~CWvalid~CM!\n");
     write_user(user,"~CMThe talker will be sending you an email with some info you should know!\n");
     }
}

if (has_unread_mail(user)) {
   sprintf(file,"%s",NEWMAILFILE);
   if (!(show_screen(user))) { write_user(user,newmail_prompt); }
   }

/* write to loginlog and set up some vars */
sprintf(text,"%s logged in on port %d from %s : %s.\n",user->name,user->port,user->site,user->site_port);
write_loginlog(text,1);
num_of_users++;
num_of_logins--;
user->login=0;
reset_hangman(user);

/* Create user's Room */
if (ALLOW_USER_ROOMS && (strcasecmp(user->roomname,"None"))) makeroom(user);
if (!user->bank_update) { update_time_bank(user); }
if (user->start_script) { startup_script(user); }
write_user(user,"\n");
trivia_banner(user);
write_user(user,"\n~BP");

/* Quote Of The Day */
if (SYS_FORTUNE) {
sprintf(text,"~CT%s\n~CB-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n",center("Your Fortune For Today Is:",78));
write_user(user,text);

switch(double_fork()) {
  case -1 : return;
  case  0 : sprintf(text2,"%s/output.%s",TEMPFILES,user->name);
	    unlink(text2);
            if (SYS_FORTUNE==2) sprintf(text,"%s > %s",FORTUNE_COMMAND,text2);
            else sprintf(text,"fortune > %s",text2);
            system(text);
	    switch(more(user,user->socket,text2)) {
                case 0: write_user(user,"~CRCould not create fortune...\n\n");
		case 1: user->misc_op=2;
		}
            _exit(1);
	    break;
	    }
	}
else {
  write_user(user,"\n~CTTodays thought~FW:\n");
  sprintf(text,"~FB> ~RS%s\n",quotes[rand()%MAX_QUOTES]);
  write_user(user,text);
  }
if (!webwho()) write_room(NULL,"WebWho(): Fail!\n");
write_user(user,"\n");
prompt(user);
}

/*** Disconnect user from talker ***/
void disconnect_user(UR_OBJECT user)
{
RM_OBJECT rm;
char text2[ARR_SIZE];
char *temp;

rm=user->room;
if (user->login) {
	close(user->socket);
	destruct_user(user);
	num_of_logins--;
	return;
	}
     if (user->pop!=NULL) { leave_poker(user); } /* poker */
     if (user->cpot>0) depositbank(user,user->cpot,1);
     if (!user->vis) user->invis=USERINVIS; else user->invis=USERVIS;
     /* Deal With Fight Stats */
     if ((fight.first_user==user) || (fight.second_user==user)) {
	if (fight.second_user==user) {
		sprintf(text,"~CRYou leave the fight and %s ~CRwins by default...\n",fight.first_user->recap);
		write_user(fight.second_user,text);
		sprintf(text,"~CR%s ~CRhas left the building, and therefore you win by default...\n",fight.second_user->recap);
		write_user(fight.first_user,text);
		}
	if (fight.first_user==user) {
		sprintf(text,"~CRYou leave the fight and %s ~CRwins by default...\n",fight.second_user->recap);
		write_user(fight.first_user,text);
		sprintf(text,"~CR%s ~CRhas left the building, and therefore you win by default...\n",fight.first_user->recap);
		write_user(fight.second_user,text);
		}
        fight.first_user = NULL;
        fight.second_user = NULL;
        fight.issued = 0;
        fight.time = 0;
	}
     if (user->temp_level>0) {
        user->level=user->temp_level;
        user->temp_level=0;
        write_user(user,"~CMYour original level has been restored from your temp promotion!\n");
        }
     strcpy(user->login_room,user->room->name);
     strcpy(text2,user->alias);
     temp=colour_com_strip(text2);
     if (!strcasecmp(temp,user->name)) strncpy(user->recap,user->alias,USER_RECAP_LEN);
     save_user_details(user,1);
     sprintf(text,fairwell_prompt,TALKERNAME,user->recap);
     write_user(user,text);
     destruct_user_room(user);
     close(user->socket);
     sprintf(text2,exit_prompt,SHORTNAME);
     if (user->vis && !user->hidden) {
	sprintf(text,"%s logged out while visible.\n",user->name);
	write_loginlog(text,1);
        sprintf(text,"%s ~FT%s %s \n",text2,user->recap,user->desc);
        write_level(OWNER+1,2,text,NULL);
	lastlogin(user,1); /* Record User In Last Login Log */
	}
     if (!user->vis && !user->hidden) {
		sprintf(text,"%s logged out while invisible.\n",user->name);
		write_loginlog(text,1);
                sprintf(text,"%s %s %s~RS ~FT[~CTI~FT] \n",text2,user->recap,user->desc);
		write_level(user->level,OWNER+1,text,NULL);
		lastlogin(user,1); /* Record User In Last Login Log */
		}
     else if (user->hidden) {
          sprintf(text,"~CW~BB[~FRHidden Departure~FW]~RS~CW: ~FT%s %s \n",user->recap,user->desc);
          write_level(OWNER+1,3,text,NULL);
          }
if (user->malloc_start!=NULL) free(user->malloc_start);
num_of_users--;

/* Destroy any clones, and the user's room if possible */
destroy_user_clones(user);
destruct_user(user);
reset_access(rm);
destructed=0;
if (!webwho()) return;
}

/*** Logoff user from the talker quietly ***/
void logoff_user(UR_OBJECT user)
{
user->hidden=1;
user->vis=0;
disconnect_user(user);
if (!webwho()) return;
}

/*** Tell telnet not to echo characters - for password entry ***/
void echo_off(UR_OBJECT user)
{
char seq[4];

if (password_echo) return;
sprintf(seq,"%c%c%c",255,251,1);
write_user(user,seq);
}


/*** Tell telnet to echo characters ***/
void echo_on(UR_OBJECT user)
{
char seq[4];

if (password_echo) return;
sprintf(seq,"%c%c%c",255,252,1);
write_user(user,seq);
}

/************ MISCELLANIOUS FUNCTIONS *************/

/*** Stuff that is neither speech nor a command is dealt with here ***/
int misc_ops(UR_OBJECT user,char *inpstr)
{
char *remove_first();

switch(user->misc_op) {
	case 1:
	if (toupper(inpstr[0])=='Y') {
		if (rs_countdown && !rs_which) {
			if (rs_countdown>60)
                             sprintf(text,"\n~BP~CB[~CYM~CYoenuts~CB]~CW: ~CMShutting Down In %d minutes, %d seconds.\n\n",rs_countdown/60,rs_countdown%60);
                        else sprintf(text,"\n~BP~CB[~CYM~CYoenuts~CB]~CW: ~CMShutting Down In %d seconds.\n\n",rs_countdown);
			write_room(NULL,text);
			sprintf(text,"%s initiated a %d seconds SHUTDOWN countdown.\n",user->name,rs_countdown);
			write_syslog(text,1);
			rs_user=user;
			rs_announce=time(0);
			user->misc_op=0;
			prompt(user);
			return 1;
			}
		talker_shutdown(user,NULL,0);
		}
	/* This will reset any reboot countdown that was started, oh well */
	rs_countdown=0;
	rs_announce=0;
	rs_which=-1;
	rs_user=NULL;
	user->misc_op=0;
	prompt(user);
	return 1;

	case 2:
	if (toupper(inpstr[0])=='E'
	    || more(user,user->socket,user->page_file)!=1) {
		user->misc_op=0;  user->filepos=0;  user->page_file[0]='\0';
		prompt(user);
		}
	return 1;

	case 3: /* writing on board */
	case 4: /* Writing mail */
	case 5: /* doing profile */
 	case 8: /* doing room desc */
 	case 15: /* doing email */
	case 16: /* Adding To The Newsfile */
	 	 editor(user,inpstr);
		 return 1;
	case 6:
	if (toupper(inpstr[0])=='Y') delete_user(user,1);
	else {  user->misc_op=0;  prompt(user);  }
	return 1;

	case 7:
	if (toupper(inpstr[0])=='Y') {
		if (rs_countdown && rs_which==1) {
			if (rs_countdown>60)
                        sprintf(text,"\n\07~CB[~CYM~CYoenuts~CB]~CW: ~FM~LIREBOOT INITIATED, rebooting in %d minutes, %d seconds!\n\n",rs_countdown/60,rs_countdown%60);
                        else sprintf(text,"\n\07~CB[~CYM~CYoenuts~CB]~CW: ~FM~LIREBOOT INITIATED, rebooting in %d seconds!\n\n",rs_countdown);
			write_room(NULL,text);
			sprintf(text,"%s initiated a %d seconds REBOOT countdown.\n",user->name,rs_countdown);
			write_syslog(text,1);
			rs_user=user;
			rs_announce=time(0);
			user->misc_op=0;
			prompt(user);
			return 1;
			}
		talker_shutdown(user,NULL,1);
		}
	if (rs_which==1 && rs_countdown && rs_user==NULL) {
		rs_countdown=0;
		rs_announce=0;
		rs_which=-1;
		}
	user->misc_op=0;
	prompt(user);
	return 1;

        case 9:
        if (!inpstr[0]) {
	        write_user(user,"Abandoning your samesite look-up.\n");
		user->misc_op=0;  user->samesite_all_store=0;  user->samesite_check_store[0]='\0';
		prompt(user);
		}
        else {
          user->misc_op=0;
          word[0][0]=toupper(word[0][0]);
          strcpy(user->samesite_check_store,word[0]);
          samesite(user,1);
          }
          return 1;

        case 10:
	if (!inpstr[0]) {
	        write_user(user,"Abandoning your samesite look-up.\n");
		user->misc_op=0;  user->samesite_all_store=0;  user->samesite_check_store[0]='\0';
		prompt(user);
		}
	else {
	  user->misc_op=0;
	  strcpy(user->samesite_check_store,word[0]);
	  samesite(user,2);
	  }
	return 1;

	case 11: /* Craps */
     		user->cwager=atoi(inpstr);
     		if (user->cwager<CRAPS_MINWAGER) {
         		write_user(user,"You have to wager more than that!\n\n");
			user->misc_op=0;
			depositbank(user,user->cpot,1);
	  		craps(user,0);
         		}
     		else if (user->cwager>user->cpot) {
         		write_user(user,"You don't have that much money on you!\n\n");
			user->misc_op=0;
			depositbank(user,user->cpot,1);
         		craps(user,0);
         		}
    		else {
			user->misc_op=0;
			craps(user,1);
			}
		return 1;

	case 12: /* Craps #2 */
		if (inpstr[0]=='y' || inpstr[0]=='Y') {
			user->misc_op=0;
			depositbank(user,user->cpot,1);
			user->cpot=0;
			user->cwager=0;
			craps(user,0);
			}
		else {
			user->misc_op=0;
                        sprintf(text,"\n~CTYou leave the craps table with your winnings of ~FG$%d\n",user->cpot);
			write_user(user,text);
                        write_user(user,"~CMYour winnings will be deposited in your FI Natonal Bank Account.\n");
		        depositbank(user,user->cpot,0);
			craps_endgame(user);
			prompt(user);
			}
		return 1;
	case 13:  /* Tic Tac Toe Menu */
        write_user(user,"\n~CB(Tic Tac Toe Commands~FW: ~FY<square number>, play, redraw, reset or end)\n~CGEnter ~FMTic Tac Toe ~FGcommand or your move~FY: \n");
	if (!inpstr[0]) {
		user->misc_op=0;
		ticmenu(user,inpstr,10);
		}
	if (strstr(inpstr,"help")) {
		user->misc_op=0;
		ticmenu(user,inpstr,16);
		}
	if (strstr(inpstr,"play")) {
		user->misc_op=0;
		ticmenu(user,inpstr,15);
		}
	if (strstr(inpstr,"end")) {
		user->misc_op=0;
		ticmenu(user,inpstr,14);
		}
	if (strstr(inpstr,"red")) {
		user->misc_op=0;
		ticmenu(user,inpstr,11);
		}
	if (strstr(inpstr,"res")) {
		user->misc_op=0;
		ticmenu(user,inpstr,12);
		}
	if (strstr(inpstr,"say")) {
		user->misc_op=0;
		inpstr=remove_first(inpstr);
		ticmenu(user,inpstr,13);
		}
	if (atoi(inpstr)<0 || atoi(inpstr)>0) {
                write_user(user,"~CRYou must chose a square between ~FT1 ~FRand ~FT9~FR!\n");
		user->misc_op=0;
		ticmenu(user,inpstr,10);
		}
	return 1;
	case 14:
	if (inpstr[0]=='y' || inpstr[0]=='Y') {
		user->misc_op=0;
		inpstr[0]='\0';
		confirm(user,2);
		}
	if (inpstr[0]=='n' || inpstr[0]=='N') {
		user->misc_op=0;
		inpstr[0]='\0';
		confirm(user,1);
		}
	else confirm(user,0);
	return 1;
	case 17:
	  switch (toupper(inpstr[0])) {
		case 'Y':
		  if (rs_countdown && rs_which==2) {
                        if (rs_countdown>60) sprintf(text,"\n\07~CB[~CYM~CYoenuts~CB]~CW: ~FM~LISEAMLESS REBOOT INITIATED, rebooting in %d minutes, %d seconds!\n\n",rs_countdown/60,rs_countdown%60);
                        else sprintf(text,"\n\07~CB[~CYM~CYoenuts~CB]~CW: ~FM~LISEAMLESS REBOOT INITIATED, rebooting in %d seconds!\n\n",rs_countdown);
			write_room(NULL,text);
			sprintf(text,"[SREBOOT] - User: [%s] Time: [%d seconds]\n",user->name,rs_countdown);
			write_syslog(text,1);
			rs_user=user;	rs_announce=time(0);  user->misc_op=0;
			prompt(user);
			return 1;
			}
		  else {
			user->misc_op=0;  do_reboot(user);  return 1;
			}
		case 'N':
		  if (rs_which==2 && rs_countdown && rs_user==NULL) {
			rs_countdown=0;  rs_announce=0;  rs_which=-1;  }
		  user->misc_op=0;  prompt(user);
		  return 1;
		default :
		  write_user(user,"~FG You have to specify YES or NO:~RS ");
		}
	  return 1;
	}
return 0;
}

/*** The editor used for writing profiles, mail and messages on the boards ***/
void editor(UR_OBJECT user,char *inpstr)
{
int cnt,line;
char *edprompt="\n~CM[~FRS~CM]~FMave~FW, ~CM[~FRV~CM]~FMiew~FW, ~CM[~FRR~CM]~FMedo~FW, ~CM[~FRA~CM]~FMbort~FW: ~RS";
char *ptr;
char genderx[4];

strcpy(genderx,"its");
if (user->prompt & 4) strcpy(genderx,"her");
if (user->prompt & 8) strcpy(genderx,"his");

if (user->edit_op) {
	switch(toupper(*inpstr)) {
		case 'S':
	        sprintf(text,"%s ~RSfinishes writing %s message.\n",user->recap,genderx);
		write_room_except(user->room,text,user);
		switch(user->misc_op) {
			case 3: write_board(user,NULL,1);  break;
			case 4: smail(user,NULL,1);  break;
			case 5: enter_profile(user,1);  break;
			case 8: room_desc(user,1);  break;
	                case 15: make_email(user,NULL,1); break;
			case 16: addnews(user,inpstr,1); break;
			}
		editor_done(user);
		return;

		case 'R':
		user->edit_op=0;
		user->edit_line=1;
		user->charcnt=0;
		user->malloc_end=user->malloc_start;
		*user->malloc_start='\0';
          sprintf(text,"\n\n\n~CG|> ~FTRedo message...\n\n");
	  write_user(user,text);
          sprintf(text,"   ~CTYou can write a maximum of %d lines, Enter a '.' on a line by itself to end!\n\n",MAX_LINES);
          write_user(user,text);
          if (user->high_ascii) sprintf(text,"   ~CM⁄ƒƒƒƒƒƒƒƒ~FG1~FMƒƒƒƒƒƒƒƒƒ~FG2~FMƒƒƒƒƒƒƒƒƒ~FG3~FMƒƒƒƒƒƒƒƒƒ~FG4~FMƒƒƒƒƒƒƒƒƒ~FG5~FMƒƒƒƒƒƒƒƒƒ~FG6~FMƒƒƒƒƒƒƒƒƒ~FG7~FMƒƒƒƒø\n");
          else sprintf(text,"   ~CG[~FM--------~FG1~FM---------~FG2~FM---------~FG3~FM---------~FG4~FM---------~FG5~FM---------~FG6~FM---------~FG7~FM----~FG]\n");
          write_user(user,text);
          if (user->high_ascii) sprintf(text,"~CR%2.2d~RS~FBØ~RS",user->edit_line);
          else sprintf(text,"~CR%2.2d~RS~FB>~RS",user->edit_line);
          write_user(user,text);
          return;

          case 'A':
          if (user->high_ascii) write_user(user,"\n~FGØ~CGØ ~FWMessage aborted...\n");
          else write_user(user,"\n~FG>~CG> ~FWMessage aborted...\n");
          sprintf(text,"~CM%s~CM decided to give up %s writing career.\n",user->recap,genderx);
		write_room_except(user->room,text,user);
		editor_done(user);
		return;
         case 'V':
          write_user(user,"");
          if(user->malloc_start==NULL) {
           write_user(user,"\n~FG>~CG> ~FWSeems you haven't written anything.\n");
          }
          else {
           write_user(user,"\n~FG>~CG> ~FWYou squint down at your scribbles. Barely legible is the following:\n");
           write_user(user, user->malloc_start);
           write_user(user,"~FG>~CG> ~FW This feature added on 4/20 :)-~\n");
           write_user(user,edprompt);
          }
          return;

          default:
		write_user(user,edprompt);
		return;
		}
	}
/* Allocate memory if user has just started editing */
if (user->malloc_start==NULL) {
	if ((user->malloc_start=(char *)malloc(MAX_LINES*81))==NULL) {
		sprintf(text,"%s: failed to allocate buffer memory.\n",syserror);
		write_user(user,text);
		write_syslog("ERROR: Failed to allocate memory in editor().\n",0);
		user->misc_op=0;
		prompt(user);
		return;
		}
	user->ignall_store=user->ignall;
	user->ignall=1; /* Dont want chat mucking up the edit screen */
	user->edit_line=1;
	user->charcnt=0;
	user->malloc_end=user->malloc_start;
	*user->malloc_start='\0';
     sprintf(text,"   ~CTYou can write a maximum of %d lines, Enter a '.' on a line by itself to end!\n\n",MAX_LINES);
	write_user(user,text);
     if (user->high_ascii) sprintf(text,"   ~CM⁄ƒƒƒƒƒƒƒƒ~FG1~FMƒƒƒƒƒƒƒƒƒ~FG2~FMƒƒƒƒƒƒƒƒƒ~FG3~FMƒƒƒƒƒƒƒƒƒ~FG4~FMƒƒƒƒƒƒƒƒƒ~FG5~FMƒƒƒƒƒƒƒƒƒ~FG6~FMƒƒƒƒƒƒƒƒƒ~FG7~FMƒƒƒƒø\n");
     else sprintf(text,"   ~CG[~FM--------~FG1~FM---------~FG2~FM---------~FG3~FM---------~FG4~FM---------~FG5~FM---------~FG6~FM---------~FG7~FM----~FG]\n");
     write_user(user,text);
     if (user->high_ascii) sprintf(text,"~CR%2.2d~RS~FBØ~RS",user->edit_line);
     else sprintf(text,"~CR%2.2d~RS~FB>~RS",user->edit_line);
	write_user(user,text);
     sprintf(text,"%s gets out %s pen and begins to write...\n",user->recap,genderx);
	write_room_except(user->room,text,user);
	return;
	}

/* Check for empty line */
if (!word_count) {
	if (!user->charcnt) {
          if (user->high_ascii) sprintf(text,"~CR%2.2d~RS~FBØ~RS",user->edit_line);
          else sprintf(text,"~CR%2.2d~RS~FB>~RS",user->edit_line);
		write_user(user,text);
		return;
		}
	*user->malloc_end++='\n';
	if (user->edit_line==MAX_LINES) goto END;
     if (user->high_ascii) sprintf(text,"~CR%2.2d~RS~FBØ~RS",++user->edit_line);
     else sprintf(text,"~CR%2.2d~RS~FB>~RS",++user->edit_line);
     write_user(user,text);
	user->charcnt=0;
     return;
	}
/* If nothing carried over and a dot is entered then end */
if (!user->charcnt && !strcmp(inpstr,".")) goto END;

line=user->edit_line;
cnt=user->charcnt;

/* loop through input and store in allocated memory */
while(*inpstr) {
	*user->malloc_end++=*inpstr++;
	if (++cnt==77) {  user->edit_line++;  cnt=0;  }
	if (user->edit_line>MAX_LINES
	    || user->malloc_end - user->malloc_start>=MAX_LINES*81) goto END;
	}
if (line!=user->edit_line) {
	ptr=(char *)(user->malloc_end-cnt);
	*user->malloc_end='\0';
     if (user->high_ascii) sprintf(text,"~CR%2.2d~RS~FBØ~RS%s",user->edit_line,ptr);
     else sprintf(text,"~CR%2.2d~RS~FB>~RS%s",user->edit_line,ptr);
	write_user(user,text);
	user->charcnt=cnt;
	return;
	}
else {
	*user->malloc_end++='\n';
	user->charcnt=0;
	}
if (user->edit_line!=MAX_LINES) {
     if (user->high_ascii) sprintf(text,"~CR%2.2d~RS~FBØ~RS",++user->edit_line);
     else sprintf(text,"~CR%2.2d~RS~FB>~RS",++user->edit_line);
	write_user(user,text);
	return;
	}

/* User has finished his message , prompt for what to do now */
END:
*user->malloc_end='\0';
if (*user->malloc_start) {
	write_user(user,edprompt);
	user->edit_op=1;  return;
	}
if (user->high_ascii) write_user(user,"\n~FGØ~CGØ ~CRNo text has been written...\n");
else write_user(user,"\n~FG>~CG> ~FRNo text has been written...\n");
sprintf(text,"~CM%s~CM gives up %s writing career...\n",user->recap,genderx);
write_room_except(user->room,text,user);
editor_done(user);
}

/*** Reset some values at the end of editing ***/
void editor_done(UR_OBJECT user)
{
user->misc_op=0;
user->edit_op=0;
user->edit_line=0;
free(user->malloc_start);
user->malloc_start=NULL;
user->malloc_end=NULL;
user->ignall=user->ignall_store;
prompt(user);
if (user->chkrev) {
     if (user->high_ascii) sprintf(text,"~FGÆ~CGÆ ~FM YOU HAVE NEW TELLS ~CGØ~FGØ\n~CMtype ~CG.revtell ~CMto view them.\n");
     else sprintf(text,"~FG<~CG< ~CMYOU HAVE NEW TELLS ~CG>~FG>\n~CMtype ~CG.revtell ~CMto view them.\n");
	write_user(user,text);
	user->chkrev=0;
	}
}

/*** Record speech and emotes in the room. ***/
void record(RM_OBJECT rm,char *str)
{
strncpy(rm->revbuff[rm->revline],str,REVIEW_LEN);
rm->revbuff[rm->revline][REVIEW_LEN]='\n';
rm->revbuff[rm->revline][REVIEW_LEN+1]='\0';
rm->revline=(rm->revline+1)%REVIEW_LINES;
}

/*** Records tells and pemotes sent to the user. ***/
void record_tell(UR_OBJECT user,char *str)
{
strncpy(user->revbuff[user->revline],str,REVIEW_LEN);
user->revbuff[user->revline][REVIEW_LEN]='\n';
user->revbuff[user->revline][REVIEW_LEN+1]='\0';
user->revline=(user->revline+1)%REVTELL_LINES;
}


/*** Records tells and pemotes sent to the user. ***/
void record_wiz(UR_OBJECT user,char *str)
{
     strncpy(wizrevbuff[wrevline],str,REVIEW_LEN);
     wizrevbuff[wrevline][REVIEW_LEN]='\n';
     wizrevbuff[wrevline][REVIEW_LEN+1]='\0';
     wrevline=(wrevline+1)%REVIEW_LINES;
}

/*** Records tells and pemotes sent to the user. ***/
void record_shout(char *str)
{
     strncpy(shoutrevbuff[srevline],str,REVIEW_LEN);
     shoutrevbuff[srevline][REVIEW_LEN]='\n';
     shoutrevbuff[srevline][REVIEW_LEN+1]='\0';
     srevline=(srevline+1)%REVIEW_LINES;
}

/*** Set room access back to public if not enough users in room ***/
void reset_access(RM_OBJECT rm)
{
UR_OBJECT u;
int cnt;

if (rm->access!=PRIVATE || rm->access & PERSONAL) return;
cnt=0;
for(u=user_first;u!=NULL;u=u->next) if (u->room==rm) ++cnt;
if (cnt<min_private_users) {
        write_room(rm,"% Room returned to ~FGpublic~RS %\n");
	rm->access=PUBLIC;

	/* Reset any invites into the room & clear review buffer */
	for(u=user_first;u!=NULL;u=u->next) {
		if (u->invite_room==rm) u->invite_room=NULL;
		}
        write_room(rm,"% Review buffer has been cleared %\n");
	clear_revbuff(rm);
	}
}

/*** Exit because of error during bootup ***/
void boot_exit(int code)
{
switch(code) {
	case 1:
	write_syslog("BOOT FAILURE: Error while parsing configuration file.\n",0);
	exit(1);

	case 2:
	perror("NUTS: Can't open main port listen socket");
	write_syslog("BOOT FAILURE: Can't open main port listen socket.\n",0);
	exit(2);

	case 3:
	perror("NUTS: Can't open wiz port listen socket");
	write_syslog("BOOT FAILURE: Can't open wiz port listen socket.\n",0);
	exit(3);

	case 4:
	perror("NUTS: Can't open link port listen socket");
	write_syslog("BOOT FAILURE: Can't open link port listen socket.\n",0);
	exit(4);

	case 5:
	perror("NUTS: Can't bind to main port");
	write_syslog("BOOT FAILURE: Can't bind to main port.\n",0);
	exit(5);

	case 6:
	perror("NUTS: Can't bind to wiz port");
	write_syslog("BOOT FAILURE: Can't bind to wiz port.\n",0);
	exit(6);

	case 7:
	perror("NUTS: Can't bind to link port");
	write_syslog("BOOT FAILURE: Can't bind to link port.\n",0);
	exit(7);

	case 8:
	perror("NUTS: Listen error on main port");
	write_syslog("BOOT FAILURE: Listen error on main port.\n",0);
	exit(8);

	case 9:
	perror("NUTS: Listen error on wiz port");
	write_syslog("BOOT FAILURE: Listen error on wiz port.\n",0);
	exit(9);

	case 10:
	perror("NUTS: Listen error on link port");
	write_syslog("BOOT FAILURE: Listen error on link port.\n",0);
	exit(10);

	case 11:
	perror("NUTS: Failed to fork");
	write_syslog("BOOT FAILURE: Failed to fork.\n",0);
	exit(11);
	}
}

/*** User prompt ***/
void prompt(UR_OBJECT user)
{
int hr,min;
hr=0; min=0;

if (no_prompt) return;
hr=(int)(time(0)-user->last_login)/3600;
min=((int)(time(0)-user->last_login)%3600)/60;
if (user->command_mode && !user->misc_op) {
     if (user->hidden) {
          sprintf(text,"~CW[~FT%s, ~FM%2.2d:%2.2d ~FR#~FW]: ",user->room->recap,hr,min);
          write_user(user,text);
          }
     else if (!user->vis) {
          sprintf(text,"~CW[~FT%s, ~FM%2.2d:%2.2d~FW]: ",user->room->recap,hr,min);
          write_user(user,text);
          }
     else {
          sprintf(text,"~CW[~FT%s, ~FM%2.2d:%2.2d~FW]: ",user->room->recap,hr,min);
          write_user(user,text);
          }
	return;
	}
if (!(user->prompt & 1) || user->misc_op) return;
/* New Prompt System */
sprintf(text,"~CW[~FT%s~RS~CW: ~FG%2.2d:%2.2d, ~FM%s~FW]\n",user->recap,hr,min,user->room->recap);
if (!user->vis) sprintf(text,"~CW[~RS~FR!~CT%s~RS~CW: ~FG%2.2d:%2.2d, ~FM%s~FW]\n",user->recap,hr,min,user->room->recap);
if (user->hidden) sprintf(text,"~CW[~RS~FR#~CT%s~RS~CW: ~FG%2.2d:%2.2d, ~FM%s~FW]\n",user->recap,hr,min,user->room->recap);
if (!user->vis && user->hidden) sprintf(text,"~CW[~FR#!~CT%s~RS~CW: ~FG%2.2d:%2.2d, ~FM%s~FW]",user->recap,hr,min,user->room->recap);
write_user(user,text);
}

/*** Page a file out to user. Colour commands in files will only work if
     user!=NULL since if NULL we dont know if his terminal can support colour
     or not. Return values:
	        0 = cannot find file, 1 = found file, 2 = found and finished ***/
int more(UR_OBJECT user,int sock,char *filename)
{
int i,buffpos,num_chars,lines,retval,len,rows;
char buff[OUT_BUFF_SIZE],*str,*colour_com_strip(),*center();
FILE *fp;

if (com_num==MAP)  rows=35;
else  rows=21;
if (!(fp=fopen(filename,"r"))) {
	if (user!=NULL) user->filepos=0;
	return 0;
	}
/* jump to reading posn in file */
if (user!=NULL) fseek(fp,user->filepos,0);

text[0]='\0';
buffpos=0;
num_chars=0;
retval=1;
len=0;

lines=0;  fgets(text,sizeof(text)-1,fp);

/* Go through file */
while(!feof(fp) && (lines<rows || user==NULL)) {
	str=text;
	/* Process line from file */
	while(*str) {
		if (*str=='\n') {
			if (buffpos>OUT_BUFF_SIZE-6) {
				write(sock,buff,buffpos);  buffpos=0;
				}
			/* Reset terminal before every newline */
			if (user==NULL || user->colour) {
				memcpy(buff+buffpos,"\033[0m",4);  buffpos+=4;
				}
			*(buff+buffpos)='\n';  *(buff+buffpos+1)='\r';
			buffpos+=2;  ++str;
			}
		else {
			/* Process colour commands in the file. See write_user()
			   function for full comments on this code. */
               if (*str=='^' && *(str+1)=='~') {  ++str;  continue;  }
               if (str!=text && *str=='~' && *(str-1)=='^') {
				*(buff+buffpos)=*str;  goto CONT;
				}
			if (*str=='~') {
				if (buffpos>OUT_BUFF_SIZE-6) {
					write(sock,buff,buffpos);  buffpos=0;
					}
				++str;
				for(i=0;i<NUM_COLS;++i) {
					if (!strncmp(str,colcom[i],2)) {
						if (user==NULL || user->colour) {
							memcpy(buffpos+buff,colcode[i],strlen(colcode[i]));
							buffpos+=strlen(colcode[i])-1;
							}
						else buffpos--;
						++str;
						goto CONT;
						}
					}
				*(buff+buffpos)=*(--str);
				}
			else *(buff+buffpos)=*str;
			CONT:
			++buffpos;   ++str;
			}
		if (buffpos==OUT_BUFF_SIZE) {
			write(sock,buff,OUT_BUFF_SIZE);  buffpos=0;
			}
		}
	len=strlen(text);
	num_chars+=len;
	lines+=len/80+(len<80);
	fgets(text,sizeof(text)-1,fp);
	}
if (buffpos && sock!=-1) write(sock,buff,buffpos);

/* if user is logging on dont page file */
if (user==NULL) {  fclose(fp);  return 2;  };
if (feof(fp)) {
	user->filepos=0;  no_prompt=0;  retval=2;
	}
else  {
	/* store file position and file name */
	user->filepos+=num_chars;
	strcpy(user->page_file,filename);
        write_user(user,center(more_prompt,78));
	no_prompt=1;
	}
fclose(fp);
return retval;
}

/*** Set global vars. hours,minutes,seconds,date,day,month,year ***/
void set_date_time(void)
{
struct tm *tm_struct; /* structure is defined in time.h */
time_t tm_num;

/* Set up the structure */
time(&tm_num);
tm_struct=localtime(&tm_num);

/* Get the values */
tday=tm_struct->tm_yday;
tyear=1900+tm_struct->tm_year; /* Will this work past the year 2000? Hmm... */
tmonth=tm_struct->tm_mon;
tmday=tm_struct->tm_mday;
twday=tm_struct->tm_wday;
thour=tm_struct->tm_hour;
tmin=tm_struct->tm_min;
tsec=tm_struct->tm_sec;
}

/*** Return pos. of second word in inpstr ***/
char *remove_first(char *inpstr)
{
char *pos=inpstr;
while(*pos<33 && *pos) ++pos;
while(*pos>32) ++pos;
while(*pos<33 && *pos) ++pos;
return pos;
}

/*** Get user struct pointer from name ***/
UR_OBJECT get_user(char *name)
{
UR_OBJECT u;
char tuname[USER_RECAP_LEN+2],  *colour_com_strip();

name[0]=toupper(name[0]);
/* Search for exact name */
for(u=user_first;u!=NULL;u=u->next) {
	if (u->login || u->type==CLONE_TYPE) continue;
	if (!strcasecmp(u->name,name))  return u;
	}

/* Search for close match name */
for(u=user_first;u!=NULL;u=u->next) {
	if (u->login || u->type==CLONE_TYPE) continue;
	if (strstr(u->name,name))  return u;
	}

/* Search for close match name */
for(u=user_first;u!=NULL;u=u->next) {
	if (u->login || u->type==CLONE_TYPE) continue;
	strncpy(tuname,colour_com_strip(u->recap),USER_RECAP_LEN);
	if (!strncasecmp(name,tuname,strlen(tuname))) { return u; }
	}
return NULL;
}

/*** Get user struct pointer from name ***/
UR_OBJECT get_exact_user(char *name)
{
UR_OBJECT u;

name[0]=toupper(name[0]);
/* Search for exact name */
for(u=user_first;u!=NULL;u=u->next) {
	if (u->login || u->type==CLONE_TYPE) continue;
	if (!strcmp(u->name,name))  return u;
	}
return NULL;
}

/*** Return level value based on level name ***/
int get_level(char *name)
{
int i;

i=0;
while(level_name[i][0]!='*') {
	if (!strcasecmp(level_name[i],name)) return i;
	++i;
	}
return -1;
}

/*** See if a user has access to a room. If room is fixed to private then
     it is considered a wizroom so grant permission to any user of WIZ and
	above for those. ***/
int has_room_access(UR_OBJECT user,RM_OBJECT rm)
{
if ((rm->access & PRIVATE)
    && user->level<gatecrash_level
    && user->invite_room!=rm
    && !((rm->access & FIXED) && user->level>=WIZ)) return 0;
return 1;
}

/*** See if user has unread mail, mail file has last read time on its
     first line ***/
int has_unread_mail(UR_OBJECT user)
{
FILE *fp;
int tm;
char filename[80];

sprintf(filename,"%s/%s.M",USERFILES,user->name);
if (!(fp=fopen(filename,"r"))) return 0;
fscanf(fp,"%d",&tm);
fclose(fp);
if (tm>(int)user->read_mail) return 1;
return 0;
}

/*** This is function that sends mail to other users ***/

void send_mail(UR_OBJECT user,char *to,char *ptr)
{
UR_OBJECT u;
FILE *infp,*outfp;
char d,filename[80],line[DNL+1];
int offline=0;

/* Local mail */
if (!(outfp=fopen("tempfile","w"))) {
	write_user(user,"Error in mail delivery.\n");
	write_syslog("ERROR: Couldn't open tempfile in send_mail().\n",0);
	return;
	}
/* Write current time on first line of tempfile */
fprintf(outfp,"%d\r",(int)time(0));
/* Copy current mail file into tempfile if it exists */
sprintf(filename,"%s/%s.M",USERFILES,to);
if ((infp=fopen(filename,"r"))) {
	/* Discard first line of mail file. */
	fgets(line,DNL,infp);
	/* Copy rest of file */
	d=getc(infp);
	while(!feof(infp)) {  putc(d,outfp);  d=getc(infp);  }
	fclose(infp);
	}
/* Put new mail in tempfile */
if (user!=NULL) fprintf(outfp,"~CGFrom~FW: ~FT%s \n~FT%s \n",user->recap,long_date(0));
else fprintf(outfp,"~CGFrom~FW: ~FRMoenuts Mailer \n~FM%s\n",long_date(0));
fputs(ptr,outfp);
fputs("\n",outfp);
fclose(outfp);

/* Post Office Crapola :) */
if (!(u=get_user(to))) {
	if ((u=create_user())==NULL) {
		sprintf(text,"%s: unable to create temporary user object.\n",syserror);
		write_user(user,text);
		write_syslog("ERROR: Unable to create temporary user object in send_mail().\n",0);
		u=NULL;
		}
	offline=1;
	strcpy(u->name,to);
	if (!load_user_details(u)) {
		write_user(user,nosuchuser);
		destruct_user(u);
		destructed=0;
		u=NULL;
		}
	}
if (u!=NULL && u->fmail) post_office(user,u->name,u->email,NULL,"tempfile",1);

/* End Of Post Office Crapola :) */

rename("tempfile",filename);
count_messages(user);
sprintf(text,"%s sent smail to %s\n",user->name,to);
write_syslog(text,1);
sprintf(text,sendmail_prompt,to);
write_user(user,text);
sprintf(text,"\07\n~CGNew Mail Has Arived For You From~FW: ~FM%s \n~CBSent To You~FW: ~FG%s \n",user->recap,long_date(0));
write_user(get_user(to),text);
if (offline) { destruct_user(u); destructed=0; }
}

/*** See if string contains any swearing ***/
int contains_swearing(char *str)
{
char *s;
int i;

if ((s=(char *)malloc(strlen(str)+1))==NULL) {
	write_syslog("ERROR: Failed to allocate memory in contains_swearing().\n",0);
	return 0;
	}
strcpy(s,str);
strtolower(s);
i=0;
while(swear_words[i][0]!='*') {
	if (strstr(s,swear_words[i])) {  free(s);  return 1;  }
	++i;
	}
free(s);
return 0;
}

/*** Count the number of colour commands in a string ***/
int colour_com_count(char *str)
{
char *s;
int i,cnt;

s=str;  cnt=0;
while(*s) {
     if (*s=='~') {
          ++s;
          for(i=0;i<NUM_COLS;++i) {
               if (!strncmp(s,colcom[i],2)) {
                    cnt++;  s++;  continue;
                    }
               }
          continue;
          }
     ++s;
     }
return cnt;
}


/*** Strip out colour commands from string for when we are sending strings
     over a netlink to a talker that doesn't support them ***/

char *colour_com_strip(char *str)
{
char *s,*t;
static char text2[ARR_SIZE];
int i;

s=str;  t=text2;
while(*s) {
	if (*s=='~') {
		++s;
		for(i=0;i<NUM_COLS;++i) {
			if (!strncmp(s,colcom[i],2)) {  s++;  goto CONT;  }
			}
		--s;  *t++=*s;
		}
	else *t++=*s;
	CONT:
        s++;
	}
*t='\0';
return text2;
}

/*** Date string for board messages, mail, .who and .allclones ***/
char *long_date(int which)
{
static char dstr[80],curtime[12];
int ampm=0,hour=0;
/* Time and Date Conversions */
if (thour>12) { hour=(thour-12); ampm=1; }
else { hour=thour; ampm=0; }
sprintf(curtime,"%02d:%02d%s",hour,tmin,(!ampm?"am":"pm"));

if (which==1) sprintf(dstr,"on %s %s, %d %d at %s",day[twday],month[tmonth],tmday,tyear,curtime);
else if (which==2) sprintf(dstr,"%s %s, %d %d at %s",day[twday],month[tmonth],tmday,tyear,curtime);
else sprintf(dstr,"[ %s %s %d %d at %s ]",day[twday],month[tmonth],tmday,tyear,curtime);
return dstr;
}

/*** Clear the review buffer in the room ***/
void clear_revbuff(RM_OBJECT rm)
{
int c;

for(c=0;c<REVIEW_LINES;++c) rm->revbuff[c][0]='\0';
rm->revline=0;
}

/*** Clear the shout review buffer in the room ***/
void clear_shoutrevbuff(void)
{
int c;

for(c=0;c<REVIEW_LINES;++c) shoutrevbuff[c][0]='\0';
srevline=0;
}

/*** Clear the wiz review buffer in the room ***/
void clear_wizrevbuff(void)
{
int c;

for(c=0;c<REVIEW_LINES;++c) wizrevbuff[c][0]='\0';
wrevline=0;
}

/*** Clear the screen ***/
void cls(UR_OBJECT user)
{
int i;

if (user->high_ascii) write_user(user,"\033[2J");
else for(i=0;i<5;++i) write_user(user,"\n\n\n\n\n\n\n\n\n\n");
}

/*** Display Time And Date To User ***/

void showtime(UR_OBJECT user)
{
char bstr[40],filename[81],argtext[81];
int secs,mins,hours,days;

/* Get some values */
strcpy(bstr,ctime(&boot_time));
secs=(int)(time(0)-boot_time);
days=secs/86400;
hours=(secs%86400)/3600;
mins=(secs%3600)/60;
secs=secs%60;
sprintf(text,"\nSystem booted              : ~OL%s~RS~FT~BK",bstr);
write_user(user,text);
sprintf(text,"Uptime                     : ~OL%d days, %d hours, %d minutes, %d seconds~RS~FT~BK\n",days,hours,mins,secs);
write_user(user,text);
sprintf(text,"The current system time is : ~OL%s, %d %s, %02d:%02d:%02d %d~RS~FT~BK\n",day[twday],tmday,month[tmonth],thour,tmin,tsec,tyear);
write_user(user,text);
sprintf(text,"%s was compiled at %s on %s\n\n",TALKERNAME,COMPILE_TIME,COMPILE_DATE);
write_user(user,text);
/* Create and Show Calender For Current Month */
switch(double_fork()) {
  case -1 : return;
  case  0 : sprintf(filename,"%s/output.%s",TEMPFILES,user->name);
	    unlink(filename);
	    sprintf(argtext,"%2.2d %4.4d",tmonth+1,tyear);
	    sprintf(text,"cal %s > %s",argtext,filename);
            system(text);
	    switch(more(user,user->socket,filename)) {
                case 0: write_user(user,"~CRCould not create calender...\n\n");
		case 1: user->misc_op=2;
		}
            _exit(1);
	    break;
	    }
}

/*** Clear All The Screens In The Room screen ***/
void cls_all(UR_OBJECT user)
{
int i;

for(i=0;i<5;++i) { write_room(user->room,"\n\n\n\n\n\n\n\n\n\n"); }
	if (user->level<ARCH) {
		sprintf(text,"\n~CS~CR%s ~RS~CRhas cleared everyone's screen...\n",user->recap);
		write_room(user->room,text);
		}
}

/*** Convert string to upper case ***/
void strtoupper(char *str)
{
while(*str) {  *str=toupper(*str);  str++; }
}

/*** Convert string to lower case ***/
void strtolower(char *str)
{
while(*str) {  *str=tolower(*str);  str++; }
}

/*** Returns 1 if string is a positive number ***/
int isnumber(char *str)
{
while(*str) if (!isdigit(*str++)) return 0;
return 1;
}

/************ OBJECT FUNCTIONS ************/

/*** Construct user/clone object ***/
UR_OBJECT create_user(void)
{
UR_OBJECT user;
int i;

if ((user=(UR_OBJECT)malloc(sizeof(struct user_struct)))==NULL) {
	write_syslog("ERROR: Memory allocation failure in create_user().\n",0);
	return NULL;
	}

/* Append object into linked list. */
if (user_first==NULL) {
	user_first=user;  user->prev=NULL;
	}
else {
	user_last->next=user;  user->prev=user_last;
	}
user->next=NULL;
user_last=user;

/* initialise user structure */
user->type=USER_TYPE;
user->name[0]='\0';
user->desc[0]='\0';
user->in_phrase[0]='\0';
user->out_phrase[0]='\0';
user->ignuser[0]='\0';
user->afk_mesg[0]='\0';
user->pass[0]='\0';
user->site[0]='\0';
user->site_port[0]='\0';
user->last_site[0]='\0';
user->page_file[0]='\0';
user->mail_to[0]='\0';
user->inpstr_old[0]='\0';
user->buff[0]='\0';
user->buffpos=0;
user->filepos=0;
user->read_mail=time(0);
user->room=NULL;
user->invite_room=NULL;
user->port=0;
user->login=0;
user->socket=-1;
user->attempts=0;
user->command_mode=0;
user->level=0;
user->vis=1;
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
user->prompt=prompt_def;
user->colour=colour_def;
user->charmode_echo=charecho_def;
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
user->age=0;
user->whostyle=5;
user->last_room=0;
user->email[0]='\0';
user->homepage[0]='\0';
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
user->married[0]='\0';
user->birthday[0]='\0';
user->bank_balance=0;
user->bank_update=0;
user->bank_temp=0;
user->level_alias[0]='\0';
user->recap[0]='\0';
user->icq[0]='\0';
user->login_room[0]='\0';
user->roomname[0]='\0';
user->roomtopic[0]='\0';
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
user->owned_by[0]='\0';
user->bet=0;
user->win=0;
user->textcolor=0;
return user;
}

/*** Destruct an object. ***/
void destruct_user(UR_OBJECT user)
{
/* Remove from linked list */
if (user==user_first) {
	user_first=user->next;
	if (user==user_last) user_last=NULL;
	else user_first->prev=NULL;
	}
else {
	user->prev->next=user->next;
	if (user==user_last) {
		user_last=user->prev;  user_last->next=NULL;
		}
	else user->next->prev=user->prev;
	}
free(user);
destructed=1;
}

/*** Destruct a room object. ***/
void destruct_room(RM_OBJECT rm)
{
/* Remove from linked list */
if (rm==room_first) {
  room_first=rm->next;
  if (rm==room_last) room_last=NULL;
  else room_first->prev=NULL;
  }
else {
  rm->prev->next=rm->next;
  if (rm==room_last) {
    room_last=rm->prev;  room_last->next=NULL;
    }
  else rm->next->prev=rm->prev;
  }
free(rm);
}

/*** Construct room object ***/
RM_OBJECT create_room(void)
{
RM_OBJECT room;
int i;

if ((room=(RM_OBJECT)malloc(sizeof(struct room_struct)))==NULL) {
	fprintf(stderr,"NUTS: Memory allocation failure in create_room().\n");
	boot_exit(1);
	}
/* Append object into linked list. */
if (room_first==NULL) {
  room_first=room;  room->prev=NULL;
  }
else {
  room_last->next=room;  room->prev=room_last;
  }
room->next=NULL;
room_last=room;
room->name[0]='\0';
room->label[0]='\0';
room->desc[0]='\0';
room->topic[0]='\0';
room->owner[0]='\0';
room->map[0]='\0';
room->access=-1;
room->revline=0;
room->mesg_cnt=0;
room->hidden=0;
for(i=0;i<MAX_LINKS;++i) {
	room->link_label[i][0]='\0';  room->link[i]=NULL;
	}
for(i=0;i<REVIEW_LINES;++i) room->revbuff[i][0]='\0';
return room;
}

/*** Destroy all clones belonging to given user ***/
void destroy_user_clones(UR_OBJECT user)
{
UR_OBJECT u;

for(u=user_first;u!=NULL;u=u->next) {
	if (u->type==CLONE_TYPE && u->owner==user) {
		destruct_user(u);
		}
	}
}

/*** Deal with user input ***/
void exec_com(UR_OBJECT user,char *inpstr)
{
int i,len;
/* ,fel_ran; */
char filename[80],*comword=NULL;

com_num=-1;
if (word[0][0]=='.' || word[0][0]==',') comword=(word[0]+1);
else comword=word[0];
if (!comword[0]) {
     write_user(user,unknown_command);
     return;
     }
if (user->gaged) {
        write_user(user,"~CW-> ~FRYou are not allowed to do anything at the moment!\n");
	return;
	}
/* get com_num */
     if (inpstr[0]=='>')  { strcpy(word[0],"tell");   inpstr=inpstr+1; }
else if (inpstr[0]=='<')  { strcpy(word[0],"pemote"); inpstr=inpstr+1; }
else if (inpstr[0]=='[')  { strcpy(word[0],"wiztell"); inpstr=inpstr+1; }
else if (inpstr[0]==']')  { strcpy(word[0],"wemote"); inpstr=inpstr+1; }
else if (inpstr[0]=='/')  { strcpy(word[0],"pemote"); inpstr=inpstr+1; }
else if (inpstr[0]=='-')  { strcpy(word[0],"echo");   inpstr=inpstr+1; }
else if (inpstr[0]=='#')  { strcpy(word[0],"shout");  inpstr=inpstr+1; }
else if (inpstr[0]==';')  { strcpy(word[0],"emote");  inpstr=inpstr+1; }
else if (inpstr[0]==':')  { strcpy(word[0],"emote");  inpstr=inpstr+1; }
else if (inpstr[0]=='!')  { strcpy(word[0],"semote"); inpstr=inpstr+1; }
else if (inpstr[0]=='&')  { strcpy(word[0],"semote"); inpstr=inpstr+1; }
else if (inpstr[0]=='\'') { strcpy(word[0],"to");     inpstr=inpstr+1; }
else if (inpstr[0]=='@')  { strcpy(word[0],"who");    inpstr=inpstr+1; }
/*          Some people like * = Cbuff, but I find it a pain so;)           */
/* else if (inpstr[0]=='*')  { strcpy(word[0],"cbuff");  inpstr=inpstr+1; } */
else inpstr=remove_first(inpstr);
whitespace(inpstr);
i=0;
len=strlen(comword);
while(command[i][0]!='*') {
	if (!strncasecmp(command[i],comword,len)) {  com_num=i;  break;  }
	++i;
	}
if (com_num==-1) {
	if (!do_socials(user,inpstr)
         || user->level<USER
            || user->muzzled & JAILED
            || user->muzzled & SCUM) {
          sprintf(text,bad_command,word[0]);
 		write_user(user,text);
		return;
		}
	return;
	}
if ((com_level[com_num] > user->level)) {
     sprintf(text,denied_command,command[com_num]);
	write_user(user,text);
	return;
	}

/* Main switch */
switch(com_num) {
     case QUIT     : if (user->muzzled & JAILED) {
                          write_user(user,"~FTWhat's that command for?\n");
                          return;
                          }
                     quit_user(user);
                     break;
     case LOOK     : cls(user); look(user);  break;
     case MODE     : toggle_mode(user);  break;
     case SAY      : if (word_count<2) {
                          write_user(user,"What is it you want to say?\n");
                          return;
                          }
                     say(user,inpstr);
                     break;
     case SHOUT    : shout(user,inpstr);  break;
     case TICTAC   : tictac(user,inpstr); break;
     case FINGER   : finger_host(user); break;
     case POKER    : sprintf(filename,"%s/%s.%s",SCREENFILES,POKERFILE,fileext[user->high_ascii]);
		     switch(more(user,user->socket,filename)) {
                          case 0: write_user(user,"Sorry, but I was unable to find the rules to poker.\n"); break;
		          case 1: user->misc_op=2;
		          }
		     break;
     case STARTPO  : if (user->room->access==FIXED_PUBLIC) write_user(user,"\n~CM~BRYou cannot play poker in public rooms!  Please go to a private room.~RS\n");
          	     else start_poker(user);
	  	     break;
     case JOINPO   : if (user->room->access==FIXED_PUBLIC) write_user(user,"\n~CM~BRYou cannot play poker in public rooms!  Please go to a private room.~RS\n");
          	     else join_poker(user);
	  	     break;
     case LEAVEPO  : leave_poker(user); break;
     case GAMESPO  : list_po_games(user); break;
     case SCOREPO  : show_po_players(user); break;
     case DEALPO   : deal_poker(user); break;
     case FOLDPO   : fold_poker(user, inpstr); break;
     case BETPO    : bet_poker(user); break;
     case CHECKPO  : check_poker(user); break;
     case RAISEPO  : raise_poker(user,inpstr); break;
     case SEEPO    : see_poker(user); break;
     case DISCPO   : disc_poker(user); break;
     case HANDPO   : hand_poker(user); break;
     case CHIPSPO  : chips_po(user); break;
     case RANKPO   : rank_po(user); break;
     /* End Of Poker */
     case TELL     : sendtell(user,inpstr);   break;
     case EMOTE    : emote(user,inpstr);  break;
     case SEMOTE   : semote(user,inpstr); break;
     case PEMOTE   : pemote(user,inpstr); break;
     case ECHO     : echo(user,inpstr);   break;
     case GO       : go(user,inpstr);  break;
     case IGNALL   : toggle_ignall(user);  break;
     case PROMPT   : toggle_prompt(user);  break;
     case DESC     : set_desc(user,inpstr);  break;
     case INPHRASE :
     case OUTPHRASE: set_iophrase(user,inpstr);  break;
     case PUBCOM   :
     case PRIVCOM  : set_room_access(user);  break;
     case KNOCK    : letmein(user);  break;
     case INVITE   : invite(user);   break;
     case TO       : to(user,inpstr);  break;
     case TOPIC    : set_topic(user,inpstr);  break;
     case MOVE     : move(user);  break;
     case BCAST    : bcast(user,inpstr);  break;
     case WHO      : who(user,user->whostyle);  break;
     case PEOPLE   : showpeople(user,0);  break;
     case HELP     : help(user);  break;
     case SHUTDOWN : shutdown_com(user);  break;
     case SREBOOT  : sreboot_com(user);			break;
     case NEWS     : news(user); break;
     case RULES    : sprintf(filename,"%s/%s.%s",SCREENFILES,RULESFILE,fileext[user->high_ascii]);
                     switch(more(user,user->socket,filename)) {
                     case 0: write_user(user,"Sorry, but I was unable to find the rules file.\n");  break;
                     case 1: user->misc_op=2; } break;
     case READ     : read_board(user);  break;
     case WRITE    : write_board(user,inpstr,0);  break;
     case WIPE     : wipe_board(user);  break;
     case SEARCH   : search_boards(user);  break;
     case REVIEW   : review(user);  break;
     case EMAIL    : make_email(user,inpstr,0);  break;
     case STATUS   : status(user);  break;
     case VER      : show_version(user); break;
     case RMAIL    : rmail(user);  break;
     case SMAIL    : smail(user,inpstr,0);  break;
     case DMAIL    : dmail(user);  break;
     case FROM     : mail_from(user);  break;
     case ENTPRO   : enter_profile(user,0);  break;
     case EXAMINE  : examine(user);  break;
     case RMST     : rooms(user,1);  break;
     case LASTLOGIN: lastlogin(user,0);  break;
     case MAKEUSER : make_newuser(user); break;
     case FINDUSER : finduser(user); break;
     case CLASTLOGIN : lastlogin(user,2); break;
     case NSLOOKUP : nslookup(user); break;
     case PASSWD   : change_pass(user);  break;
     case KILL     : kill_user(user);  break;
     case PROMOTE  : promote(user);  break;
     case DEMOTE   : demote(user);  break;
     case LISTBANS : listbans(user);  break;
     case BAN      : ban(user);  break;
     case UNBAN    : unban(user);  break;
     case VIS      : visibility(user,1);  break;
     case INVIS    : visibility(user,0);  break;
     case SITE     : site(user);  break;
     case WAKE     : wake(user);  break;
     case WIZTELL  : wizshout(user,inpstr);  break;
     case MUZZLE   : muzzle(user);  break;
     case UNMUZZLE : unmuzzle(user);  break;
     case MAP      : map(user);  break;
     case SRULES   : if (user->muzzled & JAILED) {
                     write_user(user,"~FT%% ~FMYou cannot use the ~FY\"~FGsrules~FY\" ~FMcommand...\n");
                     return;
                     }
                     sprintf(filename,"%s/%s.%s",SCREENFILES,SRULESFILE,fileext[user->high_ascii]);
                     switch(more(user,user->socket,filename)) {
                     case 0: write_user(user,"% The Staff Rules Are Not Currently Available, Please Try Again Later! %\n");  break;
                     case 1: user->misc_op=2;
                     }
                     break;
     case LOGGING  : logging(user); break;
     case MINLOGIN : minlogin(user);  break;
     case SYSTEM   : system_details(user);  break;
     case CHARECHO : toggle_charecho(user);  break;
     case CLEARLINE: clearline(user);  break;
     case FIX      : change_room_fix(user,1);  break;
     case UNFIX    : change_room_fix(user,0);  break;
     case VIEWLOG  : viewloginlog(user);  break;
     case VIEWSYS  : viewsyslog(user);  break;
     case ACCREQ   : account_request(user,inpstr);  break;
     case CBUFF    : revclr(user);  break;
     case CREATE   : create_clone(user);  break;
     case DESTROY  : destroy_clone(user);  break;
     case MYCLONES : myclones(user);  break;
     case ALLCLONES: allclones(user);  break;
     case SWITCH   : clone_switch(user);  break;
     case CSAY     : clone_say(user,inpstr);  break;
     case CHEAR    : clone_hear(user);  break;
     case CEMOTE   : clone_emote(user,inpstr);  break;
     case CALENDER : calender(user); break;
     case SWBAN    : swban(user);  break;
     case AFK      : afk(user,inpstr);  break;
     case CLS      : cls(user);  break;
     case COLOR    : toggle_colour(user);  break;
     case LISTEN   : listening(user);  break;
     case IGNORE   : ignoreing(user);  break;
     case SUICIDE  : suicide(user); break;
     case NUKE     : delete_user(user,0);  break;
     case REBOOT   : reboot_com(user);  break;
     case RECOUNT  : check_messages(user,2);  break;
     case REVTELL  : revtell(user);  break;
     case THINK    : think(user,inpstr);  break;
     case WEMOTE   : wemote(user,inpstr);  break;
     case IMNEW    : if (user->muzzled & JAILED) {
                     write_user(user,"~FT%% ~FMYou cannot use the ~FY\"~FGnewbie~FY\" ~FMcommand...\n");
                     return;
                     }
                     sprintf(filename,"%s/%s.%s",SCREENFILES,NEWBIEFILE,fileext[user->high_ascii]);
                     switch(more(user,user->socket,filename)) {
                     case 0: write_user(user,"% The New User File Is Not Currently Availble, Sorry. %\n");  break;
                     case 1: user->misc_op=2;
                     }
                     break;
     case STAFF    : if (user->muzzled & JAILED) {
                     write_user(user,"~FT%% ~FMYou cannot use the ~FY\"~FGstaff~FY\" command..\n");
                     return;
                     }
                     sprintf(filename,"%s/%s.%s",SCREENFILES,STAFFFILE,fileext[user->high_ascii]);
                     switch(more(user,user->socket,filename)) {
                     case 0: write_user(user,"% The Staff List Is Not Currently Available, Sorry! %\n");  break;
                     case 1: user->misc_op=2;
                     }
                     break;
     case MANAGEMENT : staff_on(user);  break;
     case ENTROOM    : room_desc(user,0);  break;
     case BOOT       : remove_user(user);  break;
     case ARREST     : arrest(user);  break;
     case UNARREST   : unarrest(user);  break;
     case GENDER     : gender(user);  break;
     case RECORD     : view_record(user);  break;
     case CHANGE     : changesys(user);  break;
     case REVWIZ     : revwiztell(user);  break;
     case SETRANK    : setrank(user,inpstr);  break;
     case BEEP       : beep(user,inpstr);  break;
     case LOBBY      : lobby(user);  break;
     case JOIN       : join(user);  break;
     case GAG        : gag(user);  break;
     case FREEZE     : freeze(user);  break;
     case UNFREEZE   : unfreeze(user);  break;
     case SING       : sing(user,inpstr);  break;
     case NAKED      : naked(user,inpstr); break;
     case SSING      : ssing(user,inpstr); break;
     case DOH        : doh(user); break;
     case QECHO      : qecho(user,inpstr); break;
     case HUG        : hug(user); break;
     case FORCE      : force(user,inpstr); break;
     case KISS       : kiss(user); break;
     case FRENCH     : frenchkiss(user); break;
     case NETSEX     : netsex(user); break;
     case NETSEXTWO  : netsextwo(user); break;
     case YELL       : yell(user,inpstr); break;
     case LOGOFF     : logoff_user(user); break;
     case CLSALL     : cls_all(user); break;
     case WELCOME    : welcome_user(user,inpstr); break;
     case TIME       : showtime(user); break;
     case RANKS      : show_ranks(user); break;
     case GREET      : greet(user,inpstr);  break;
     case SAMESITE   : samesite(user,0);  break;
     case WRAP       : linewrap(user); break;
     case UNINVITE   : uninvite(user); break;
     case STREAK     : streak(user); break;
     case LICK       : lick(user); break;
     case BOP        : bop(user); break;
     case SET        : setinfo(user,inpstr); break;
     case PADDLE     : paddle(user); break;
     case TALKERS    : sprintf(filename,"%s/%s",SCREENFILES,TALKERFILE);
                       switch(more(user,user->socket,filename)) {
                       case 0: write_user(user,"There are no talkers to list.\n"); break;
                       case 1: user->misc_op=2;
                       }
                       break;
     case MUDS       : sprintf(filename,"%s/%s",SCREENFILES,MUDFILE);
                       switch(more(user,user->socket,filename)) {
                       case 0: write_user(user,"There are no muds to list.\n"); break;
                       case 1: user->misc_op=2;
                       }
                       break;
     case EWTOO      : sprintf(filename,"%s/%s",SCREENFILES,EWTOOFILE);
                       switch(more(user,user->socket,filename)) {
                       case 0: write_user(user,"There are no ewtoo sites to list.\n"); break;
                       case 1: user->misc_op=2;
                       }
                       break;
     case NUTS       : sprintf(filename,"%s/%s",SCREENFILES,NUTSFILE);
                       switch(more(user,user->socket,filename)) {
                       case 0: write_user(user,"There are no NUTS talkers to list.\n"); break;
                       case 1: user->misc_op=2;
                       }
                       break;
     case PICLIST    : piclist(user); break;
     case PICTELL    : pictell(user,inpstr); break;
     case ROOMPIC    : roompic(user,inpstr); break;
     case VIEWPIC    : viewpic(user,inpstr); break;
     case WREVCLR    : wizrevclr(user); break;
     case SREVCLR    : shoutrevclr(user); break;
     case REVSHOUT   : revshout(user); break;
     case MYROOM     : goto_myroom(user); break;
     case EDIT       : if (user->room->access==FIXED_PERSONAL) room_desc(user,0);
                       else write_user(user,"~CRThis is not a personal room... You cannot edit the room description here!\n");
                       break;
     case RMKILL     : rmkill(user); break;
     case RMBAN      : rmban(user); break;
     case RMUNBAN    : rmunban(user); break;
     case GIVEROOM   : giveroom(user); break;
     case SHACKLE    : shackle(user); break;
     case UNSHACKLE  : unshackle(user); break;
     case MAKEINVIS  : makeinvis(user); break;
     case MAKEVIS    : makevis(user); break;
     case HIDE       : toggle_hidden(user); break;
     case SHOW       : show_command(user,inpstr); break;
     case ALLEXEC    : execall(user,inpstr); break;
     case CRAPS      : sprintf(text,"\n~CG%s~RS~CG wanders off to the craps table...\n",user->recap);
                       write_room(user->room,text);
                       user->ignall_store=user->ignall;
                       user->ignall=1;
                       craps(user,0);
                       break;
     case GIVECASH   : givecash(user); break;
     case LENDCASH   : lendcash(user); break;
     case ATMOS      : edit_atmos(user,inpstr); break;
     case BACKUP     : force_backup(user); break;
     case HANGMAN    : play_hangman(user); break;
     case GUESS      : guess_hangman(user); break;
     case GIVEPOCHIPS: give_chips_po(user); break;
     case TPROMOTE   : tpromote(user); break;
     case SOCIALS    : list_socials(user); break;
     case ICQPAGE    : icqpage(user,inpstr); break;
     case TOEMOTE    : toemote(user,inpstr); break;
     case ADDNEWS    : addnews(user,inpstr,0); break;
     case FIGLET     : figlet(user,inpstr); break;
     case CTOPIC     : ctopic(user); break;
     case ALIAS      : alias (user,inpstr); break;
     case WIRE       : newswire(user); break;
     case BLAH	     : blah(user); break;
     case REBUILD    : rebuild_userlist(user); break;
     case BRAND	     : brand_user(user,inpstr); break;
     case UNBRAND    : unbrand_user(user); break;
     case CAPTURE    : capture(user); break;
     case SLOTS      : slot(user); break;
     case TRIVSTART  : trivia_start_stop(user); break;
     case TRIVWIN    : trivia_win(user); break;
     case TRIVQUEST  : trivia_question(user,inpstr); break;
     case TRIVANSWER : trivia_answer(user,inpstr); break;
     case LISTKEYS   : list_keys(user); break;
     case ROOMCAP    : room_recap(user,inpstr); break;
     case DICTIONARY : dictionary(user,inpstr); break;
     case STEAL      : steal_user_money(user); break;
     case SPIN       : spin(user); break;
     case ROLLDICE   : roll_dice(user); break;
     case FLIPCOIN   : flip_coin(user); break;
     case FIGHT      : fight_another(user, inpstr); break;
     case FIGHTRESET : write_room(user->room,"~CRThe Fight Has Been Canceled!!\n");
		       reset_chal(user, inpstr); break;
     case RROULETTE  : russian_roulette(user); break;
     case PAINTBALL  : paintball(user, inpstr); break;
     case PBRELOAD   : paintball_reload(user); break;
     case EIGHTBALL  : eight_ball(user,inpstr); break;
     case MUTTER     : mutter(user,inpstr); break;
     case REVMUTTER  : revmutter(user); break;
     case CMUTTER    : mutrevclr(user); break;
     case TEXTCOLOR  : text_color(user); break;
     case MAKEUPAGES : makeuserpages(user); break;
     default         : write_user(user,"~CRERROR: ~FMCommand not executed in ~FTexec_com~FB()\n");
     }
}

/**** POKER POKER POKER ***/
/*** Get po_game struct pointer from room ***/
struct po_game *get_po_game_here(RM_OBJECT room)
{
struct po_game *game;

/* Search for game in the room */
for(game=po_game_first;game!=NULL;game=game->next) {
        if (game->room == room)  return game;
        }
return NULL;
}

/*************************************************************************/
/******************************* Speech FUNTIONS *************************/
/*************************************************************************/

/*** Say user speech ***/
void say(UR_OBJECT user,char *inpstr)
{
RM_OBJECT rm;
char type[12];
char type2[22];
char *name;
char text2[(ARR_SIZE*2)+1];
int question=0;

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (user->muzzled & 1) {
	write_user(user,"You are muzzled, you cannot speak.\n");
	return;
	}
if (word_count<2 && user->command_mode) {
     write_user(user,"Usage: say <message>\n");
     return;
	}
strcpy(type,"say");
if (inpstr[strlen(inpstr)-4]=='?') { question=1; }
switch(inpstr[strlen(inpstr)-1]) {
     case '?': strcpy(type,"ask");      break;
     case '!': strcpy(type,"exclaim");  break;
     case ')': if (inpstr[strlen(inpstr)-2]==';') strcpy(type,"wink");
               else if (inpstr[strlen(inpstr)-2]==':') strcpy(type,"smile");
               break;
     case 'P': if ((inpstr[strlen(inpstr)-2]==':') || (inpstr[strlen(inpstr)-2]==';')) strcpy(type,"pfft");
               break;
     case 'D': if ((inpstr[strlen(inpstr)-2]==':') || (inpstr[strlen(inpstr)-2]==';')) strcpy(type,"laugh");
               break;
     case 'X': if ((inpstr[strlen(inpstr)-2]==':') || (inpstr[strlen(inpstr)-2]==';')) strcpy(type,"smooche");
               break;
     case 'O': if ((inpstr[strlen(inpstr)-2]==':') || (inpstr[strlen(inpstr)-2]==';')) strcpy(type,"oooh");
               break;
     case '(': if ((inpstr[strlen(inpstr)-2]==':') || (inpstr[strlen(inpstr)-2]==';')) strcpy(type,"frown");
               break;
      default : strcpy(type,"say");
     }
if (question) {
	strcpy(type2,"asks and ");
	strcat(type2,type);
	}
else {
	strcpy(type2,type);
	}

if (user->type==CLONE_TYPE) {
     sprintf(text,say_style,user->recap,type,textcolor[user->textcolor],inpstr);
	write_room(user->room,text);
	record(user->room,text);
	return;
	}
rm=user->room;
if (rm->access==FIXED_PUBLIC && ban_swearing && contains_swearing(inpstr)) {
	write_user(user,noswearing);
	return;
	}
if (user->muzzled & FROZEN) {
	if (user->vis) name=user->recap; else name=invisname;
        if (user->hidden) name=hiddenname;
        sprintf(text,say_style,name,type2,textcolor[user->textcolor],inpstr);
        if (strcasecmp(user->predesc,"none")) sprintf(text2,"%s %s",user->predesc,text);
        else sprintf(text2,"%s",text);
        write_user(user,text2);
	return;
	}
if (user->vis) name=user->recap; else name=invisname;
if (user->hidden) name=hiddenname;
sprintf(text,say_style,name,type2,textcolor[user->textcolor],inpstr);
if (strcasecmp(user->predesc,"none")) sprintf(text2,"%s %s",user->predesc,text);
else sprintf(text2,"%s",text);
write_room(user->room,text2);
record(user->room,text);
}

/*** Shout something ***/
void shout(UR_OBJECT user,char *inpstr)
{
char *name;

if (user->vis) name=user->recap; else name=invisname;
if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (user->muzzled & 1) {
	write_user(user,"You are muzzled, you cannot shout.\n");
	return;
	}
if (!inpstr[0]) {
	write_user(user,"Usage:  .shout <message>\n");
	return;
	}
if (ban_swearing && contains_swearing(inpstr)) {
	write_user(user,noswearing);
     return;
	}
if (user->muzzled & FROZEN) {
        sprintf(text,"~CT!! ~FG%s ~RS~CGshouts~CW:~FT %s\n",name,inpstr);
     write_user(user,text);
	return;
	}
if (user->hidden) name=hiddenname;
sprintf(text,"~CT!! ~FG%s ~RS~CGshouts~CW:~FT %s\n",name,inpstr);
write_room(NULL,text);
record_shout(text);
}

/*** Welcome A User ***/
void welcome_user(UR_OBJECT user,char *inpstr)
{

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (user->muzzled & 1) {
     write_user(user,"You are muzzled, you cannot welcome anyone.\n");
     return;
     }
if (word_count<2) {
     write_user(user,"Usage:  .welcome <message>\n");
	return;
	}
if (ban_swearing && contains_swearing(inpstr)) {
	write_user(user,noswearing);
     return;
	}
if (user->muzzled & FROZEN) {
	write_user(user,"\n");
     sprintf(text,welcome_style,inpstr);
	write_user(user,center(text,74));
	write_user(user,"\n");
	return;
	}
write_user(user,"\n");
sprintf(text,welcome_style,inpstr);
write_room(NULL,center(text,74));
record_shout(center(text,74));
write_user(user,"\n");
}

/*** Show a room how to use a command (Type--> <whatever>) ***/
void show_command(UR_OBJECT user,char *inpstr)
{

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (user->muzzled & 1) {
	write_user(user,"You are muzzled -- I Don't Think so!!\n");
	return;
	}
if (!inpstr[0]) {
	write_user(user,"Usage:  .show <command> <syntax>\n");
	return;
	}
if (ban_swearing && contains_swearing(inpstr)) {
	write_user(user,noswearing);  return;
	}
if (user->muzzled & FROZEN) {
	write_user(user,"\n");
        sprintf(text,show_style,inpstr);
	write_user(user,text);
	return;
	}
write_user(user,"\n");
sprintf(text,show_style,inpstr);
write_room(user->room,text);
}

/*** Tell another user something ***/
void sendtell(UR_OBJECT user,char *inpstr)
{
UR_OBJECT u;
RM_OBJECT rm;
RM_OBJECT urm;
char type[5];

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (user->muzzled & 1) {
	write_user(user,"You are muzzled, you cannot tell anyone anything.\n");
	return;
	}
if (!inpstr[0]) {
     write_user(user,"Usage:  .tell <user> <message>\n");  return;
     }
word_count=wordfind(inpstr);
if (!(u=get_user(word[0]))) {
	write_user(user,notloggedon);
	return;
	}
if (u==user) {
	write_user(user,"Talking to yourself is the first sign of madness.\n");
	return;
	}
if (u->hidden && user->level<OWNER) { write_user(user,notloggedon); return; }
if (u->afk) {
     if (u->afk_mesg[0]) sprintf(text,"~CR%s ~FGis ~FMAFK: %s\n",u->recap,u->afk_mesg);
	else sprintf(text,"%s is AFK at the moment.\n",u->recap);
	write_user(user,text);
	if (user->muzzled & FROZEN) {
                sprintf(text,"~CTMESSAGE RECORDED! ~RS%s will get a mesage to do a .revtell.\n",u->recap);
		write_user(user,text);
		return;
		}
        sprintf(text,"~CTMESSAGE RECORDED! ~RS%s will get a mesage to do a .revtell.\n",u->recap);
	write_user(user,text);
	inpstr=remove_first(inpstr);
	if (inpstr[strlen(inpstr)-1]=='?') strcpy(type,"ask");
	else strcpy(type,"tell");
	sprintf(text,"~OL-> You %s %s:~RS %s\n",type,u->recap,inpstr);
        if (!user->vis) sprintf(text,"~FR(invis)~CW-> You %s %s:~RS %s\n",type,u->recap,inpstr);
	if (user->hidden) sprintf(text,"~FR(hidden)~OL-> You %s %s:~RS %s\n",type,u->recap,inpstr);
	write_user(user,text);
        sprintf(text,"~FR[AFK]~CW-> You %s %s:~RS %s\n",type,u->recap,inpstr);
	record_tell(user,text);
 	sprintf(text,"~OL-> %s %ss you:~RS %s\n",user->recap,type,inpstr);
	if (!user->vis) sprintf(text,"~FR(invis)~FW~OL-> %s %ss you:~RS %s\n",user->recap,type,inpstr);
	if (user->hidden) sprintf(text,"~OL-> %s %ss you:~RS %s\n",invisname,type,inpstr);
	record_tell(u,text);
        write_user(u,text);
	u->chkrev=1;
	return;
	}
if (u->malloc_start!=NULL) {
	sprintf(text,"%s is using the editor at the moment.\n",u->recap);
	write_user(user,text);
	if (user->muzzled & FROZEN) {
                sprintf(text,"~CTMESSAGE RECORDED! ~RS%s will get a mesage to do a .revtell.\n",u->recap);
		write_user(user,text);
		return;
		}
        sprintf(text,"~CTMESSAGE RECORDED! ~RS%s will get a mesage to do a .revtell.\n",u->recap);
	write_user(user,text);
	inpstr=remove_first(inpstr);
	if (inpstr[strlen(inpstr)-1]=='?') strcpy(type,"ask");
	else strcpy(type,"tell");
	sprintf(text,"~OL-> You %s %s:~RS %s\n",type,u->recap,inpstr);
        if (!user->vis) sprintf(text,"~FR(invis)~CW-> You %s %s:~RS %s\n",type,u->recap,inpstr);
	if (user->hidden) sprintf(text,"~FR(hidden)~OL-> You %s %s:~RS %s\n",type,u->recap,inpstr);
	write_user(user,text);
 	sprintf(text,"~OL-> %s %ss you:~RS %s\n",user->recap,type,inpstr);
	if (!user->vis) sprintf(text,"~FR(invis)~FW~OL-> %s %ss you:~RS %s\n",user->recap,type,inpstr);
	if (user->hidden) sprintf(text,"~OL-> %s %ss you:~RS %s\n",invisname,type,inpstr);
	record_tell(u,text);
        sprintf(text,"~FR[EDITOR]~CW-> You %s %s:~RS %s\n",type,u->recap,inpstr);
	record_tell(user,text);
	u->chkrev=1;
	return;
	}
if (u->ignall && user->level<OWNER) {
     sprintf(text,"~CT%s is ignoring everyone at the moment.\n",u->recap);
	write_user(user,text);
	return;
	}
if ((u->ignore & ALL_TELLS) && user->level<OWNER) {
  if (u->vis) {
        sprintf(text,"~CT%s is ignoring private tells at the moment.\n",u->recap);
	write_user(user,text);
	}
        else write_user(user,notloggedon);
	return;
	}
if (!strcmp(user->name,u->ignuser) && user->level<OWNER) {
  if (u->vis) {
        sprintf(text,"~CT%s has chosen to ignore you.\n",u->recap);
	write_user(user,text);
	}
   else write_user(user,notloggedon);
   return;
   }
rm=user->room;
urm=u->room;
if ((rm!=urm) && (u->ignore & OUT_ROOM_TELLS) && user->level<OWNER) {
   if (u->vis) {
        sprintf(text,"~CT%s is ignoring private tells outside of the room.\n",u->recap);
	write_user(user,text);
	}
    else write_user(user,notloggedon);
    return;
    }
if (user->muzzled & FROZEN) {
	inpstr=remove_first(inpstr);
	if (inpstr[strlen(inpstr)-1]=='?') strcpy(type,"ask");
	else strcpy(type,"tell");
	sprintf(text,"~OL-> You %s %s:~RS %s\n",type,u->recap,inpstr);
        if (!user->vis) sprintf(text,"~FR(invis)~CW-> You %s %s:~RS %s\n",type,u->recap,inpstr);
	if (user->hidden) sprintf(text,"~FR(hidden)~OL-> You %s %s:~RS %s\n",type,u->recap,inpstr);
	write_user(user,text);
	record_tell(user,text);
	return;
	}
inpstr=remove_first(inpstr);
if (inpstr[strlen(inpstr)-1]=='?') strcpy(type,"ask");
else strcpy(type,"tell");
sprintf(text,"~OL-> You %s %s:~RS %s\n",type,u->recap,inpstr);
if (!user->vis) sprintf(text,"~FR(invis)~CW-> You %s %s:~RS %s\n",type,u->recap,inpstr);
if (user->hidden) sprintf(text,"~FR(hidden)~OL-> You %s %s:~RS %s\n",type,u->recap,inpstr);
write_user(user,text);
record_tell(user,text);
sprintf(text,"~OL-> %s %ss you:~RS %s\n",user->recap,type,inpstr);
if (!user->vis) sprintf(text,"~FR(invis)~FW~OL-> %s %ss you:~RS %s\n",user->recap,type,inpstr);
if (user->hidden) sprintf(text,"~OL-> %s %ss you:~RS %s\n",invisname,type,inpstr);
write_user(u,text);
record_tell(u,text);
}

/*** Emote something ***/
void emote(UR_OBJECT user,char *inpstr)
{
	RM_OBJECT rm;
	char *name;
	char text2[(ARR_SIZE*2)+1];

	if (user->muzzled & JAILED)
	{
		sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
		write_user(user,text);
		return;
	}
	if (user->muzzled & 1)
	{
		write_user(user,"You are muzzled, you cannot emote.\n");
		return;
	}
	if (word_count<2 && inpstr[1]<33)
	{
		write_user(user,"Usage: .emote <emotion>\n");
		return;
	}
	if (user->type==CLONE_TYPE)
	{
		sprintf(text,"%s %s%s\n",user->recap,textcolor[user->textcolor],inpstr);
		write_room(user->room,text);
		record(user->room,text);
		return;
	}
	rm=user->room;
	if (rm->access==FIXED_PUBLIC && ban_swearing && contains_swearing(inpstr))
	{
		write_user(user,noswearing);
		return;
	}
	if (user->muzzled & FROZEN)
	{
		if (user->vis) name=user->recap; else name=invisname;
		if (user->hidden) name = hiddenname;
		if (inpstr[0]==';')
		{
	                if (strchr("\'",inpstr[0]) || strchr("\'",inpstr[1]))
			{
				sprintf(text,"%s%s%s\n",name,textcolor[user->textcolor],inpstr+1);
			}
			else
			{
				sprintf(text,"%s %s%s\n",name,textcolor[user->textcolor],inpstr+1);
			}
			if (strcasecmp(user->predesc,"none")) sprintf(text2,"%s %s",user->predesc,text);
			else sprintf(text2,"%s",text);
			write_user(user,text2);
			return;
		}
		else
		{
	                if (strchr("\'",inpstr[0]) || strchr("\'",inpstr[1]))
			{
				sprintf(text,"%s%s%s\n",name,textcolor[user->textcolor],inpstr);
			}
			else
			{
				sprintf(text,"%s %s%s\n",name,textcolor[user->textcolor],inpstr);
			}
			if (strcasecmp(user->predesc,"none")) sprintf(text2,"%s %s",user->predesc,text);
			else sprintf(text2,"%s",text);
			write_user(user,text2);
			return;
		}
	}
	if (user->vis) name=user->recap; else name=invisname;
	if (user->hidden) name=hiddenname;
	if (inpstr[0]==';')
	{
		if (strchr("\'",inpstr[0]) || strchr("\'",inpstr[1]))
		{
			sprintf(text,"%s%s%s\n",name,textcolor[user->textcolor],inpstr+1);
		}
		else
		{
			sprintf(text,"%s %s%s\n",name,textcolor[user->textcolor],inpstr+1);
		}
		if (strcasecmp(user->predesc,"none")) sprintf(text2,"%s %s",user->predesc,text);
		else sprintf(text2,"%s",text);
		write_room(user->room,text2);
		record(user->room,text2);
		return;
	}
	else
	{
		if (strchr("\'",inpstr[0]) || strchr("\'",inpstr[1]))
		{
			sprintf(text,"%s%s%s\n",name,textcolor[user->textcolor],inpstr);
		}
		else
		{
			sprintf(text,"%s %s%s\n",name,textcolor[user->textcolor],inpstr);
		}
		if (strcasecmp(user->predesc,"none")) sprintf(text2,"%s %s",user->predesc,text);
		else sprintf(text2,"%s",text);
		write_room(user->room,text2);
		record(user->room,text2);
		return;
	}
}

/*** Do a shout emote ***/
void semote(UR_OBJECT user,char *inpstr)
{
char *name;

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (user->muzzled & 1) {
	write_user(user,"You are muzzled, you cannot emote.\n");  return;
	}
if (word_count<2 && inpstr[1]<33) {
     write_user(user,"Usage: .semote <emotion>\n");  return;
	}
if (user->muzzled & FROZEN) {
	if (user->vis) name=user->recap; else name=invisname;
        if (user->hidden) name=hiddenname;
        if (inpstr[0]=='!') sprintf(text,"~CT!! ~FM%s %s%s\n",name,textcolor[user->textcolor],inpstr+1);
        else sprintf(text,"~CT!! ~FM%s %s%s\n",name,textcolor[user->textcolor],inpstr);
	write_user(user,text);
	return;
	}
if (user->vis) name=user->recap; else name=invisname;
if (user->hidden) name=hiddenname;
if (inpstr[0]=='!') sprintf(text,"~CT!! ~FM%s %s%s  ~RS\n",name,textcolor[user->textcolor],inpstr+1);
else sprintf(text,"~CT!! ~FM%s %s%s\n",name,textcolor[user->textcolor],inpstr);
write_room(NULL,text);
record_shout(text);
}

/*** Do a private emote ***/
void pemote(UR_OBJECT user,char *inpstr)
{
char *name;
UR_OBJECT u;
RM_OBJECT rm;
RM_OBJECT urm;

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (user->muzzled & 1) {
	write_user(user,"You are muzzled, you cannot emote.\n");  return;
	}
if (!inpstr[0]) {
     write_user(user,"Usage:  .pemote <user> <emotion>\n");  return;
	}
word_count=wordfind(inpstr);
inpstr=remove_first(inpstr);
if (!(u=get_user(word[0]))) {
	write_user(user,notloggedon);  return;
	}
if (u==user) {
	write_user(user,"~FB%% ~FMDon'tcha think private emoting to yourself is a little crazy?\n");
	return;
	}
if (u->hidden && (!u->vis) && user->level<OWNER) {
	write_user(user,notloggedon);
	return;
	}
if (!inpstr[0]) {
	sprintf(text,"~FB%% ~FMPrivate Emote What to %s~RS~FM?\n",u->recap);
	write_user(user,text);
	return;
	}
if (u->afk) {
     if (u->afk_mesg[0]) sprintf(text,"~CT%s is AFK: %s\n",u->recap,u->afk_mesg);
	else sprintf(text,"%s is AFK at the moment.\n",u->recap);
	write_user(user,text);
		if (user->muzzled & FROZEN) {
                sprintf(text,"~CTMESSAGE RECORDED! ~RS%s will get a message to do a .revtell.\n",u->recap);
		write_user(user,text);
		return;
		}
        sprintf(text,"~CTMESSAGE RECORDED! ~RS%s will get a message to do a .revtell.\n",u->recap);
	write_user(user,text);
	if (user->vis) name=user->recap; else name=invisname;
        sprintf(text,"~FR[AFK]~CBTo %s: ~CW->~RS %s %s\n",u->recap,user->recap,inpstr);
	record_tell(user,text);
        sprintf(text,"~CW-> %s %s\n",user->recap,inpstr);
        if (!user->vis) sprintf(text,"~FR(invisible)~CW-> %s %s\n",user->recap,inpstr);
        if (user->hidden) sprintf(text,"~CW-> %s %s\n",invisname,inpstr);
	record_tell(u,text);
	u->chkrev=1;
	return;
	}
if (u->malloc_start!=NULL) {
	sprintf(text,"%s is using the editor at the moment.\n",u->recap);
	write_user(user,text);
		if (user->muzzled & FROZEN) {
                sprintf(text,"~CTMESSAGE RECORDED! ~RS%s will get a message to do a .revtell.\n",u->recap);
		write_user(user,text);
		return;
		}
        sprintf(text,"~CTMESSAGE RECORDED! ~RS%s will get a message to do a .revtell.\n",u->recap);
	write_user(user,text);
	if (user->vis) name=user->recap; else name=invisname;
        sprintf(text,"~FR[EDITOR]~CBTo %s: ~CW-> %s %s\n",u->recap,user->recap,inpstr);
	record_tell(user,text);
        sprintf(text,"~CW-> %s %s\n",user->recap,inpstr);
        if (!user->vis) sprintf(text,"~FR(invisible)~CW-> %s %s \n",user->recap,inpstr);
        if (user->hidden) sprintf(text,"~CW-> %s %s\n",invisname,inpstr);
	record_tell(u,text);
	u->chkrev=1;
	return;
	}
if (u->ignall && user->level<OWNER) {
	sprintf(text,"%s is ignoring everyone at the moment.\n",u->recap);
	write_user(user,text);
	return;
	}
if ((u->ignore & ALL_TELLS) && user->level<OWNER) {
	sprintf(text,"%s is ignoring private emotes at the moment.\n",u->recap);
	write_user(user,text);
	return;
	}
if (!strcmp(user->name,u->ignuser) && user->level<OWNER) {
	sprintf(text,"%s is ignoring you at the moment...\n",u->recap);
	write_user(user,text);
	return;
	}
rm=user->room;
urm=u->room;
if ((rm!=urm) && (u->ignore & OUT_ROOM_TELLS) && user->level<OWNER) {
	sprintf(text,"%s is ignoring private emotes outside the room.\n",u->recap);
	write_user(user,text);
	return;
	}
if (user->muzzled & FROZEN) {
	if (user->vis) name=user->recap; else name=invisname;
	inpstr=remove_first(inpstr);
        sprintf(text,"~CBTo %s: ~FW-> %s %s\n",u->name,name,inpstr);
	write_user(user,text);
	return;
	}
if (user->vis) name=user->recap; else name=invisname;
sprintf(text,"~CBTo %s: ~CW-> %s %s\n",u->recap,user->recap,inpstr);
if (!user->vis) sprintf(text,"~CBTo %s: ~RS~FR(invis)~CW-> %s %s\n",u->recap,name,inpstr);
if (user->hidden) sprintf(text,"~CBTo %s: ~CW-> %s %s\n",u->recap,invisname,inpstr);
write_user(user,text);
record_tell(user,text);
sprintf(text,"~CW-> %s %s\n",user->recap,inpstr);
if (!user->vis) sprintf(text,"~FR(invis)~CW-> %s %s\n",user->recap,inpstr);
if (user->hidden) sprintf(text,"~CW-> %s %s\n",hiddenname,inpstr);
write_user(u,text);
record_tell(u,text);
}

/*** Echo something to screen ***/
void echo(UR_OBJECT user,char *inpstr)
{
RM_OBJECT rm;
UR_OBJECT u;
char *temp, *colour_com_strip();

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (user->muzzled & 1) {
	write_user(user,"You are muzzled, you cannot echo.\n");  return;
	}
if (!inpstr[0]) {
     write_user(user,"Usage:  .echo <message>\n");  return;
	}
word_count=wordfind(inpstr);
strtolower(word[0]);
word[0][0]=toupper(word[0][0]);
temp=colour_com_strip(word[0]);
u=get_user(temp);

if (u) {
        write_user(user,"You can not echo user's names!\n");
	return;
	}
rm=user->room;
if (rm->access==FIXED_PUBLIC && ban_swearing && contains_swearing(inpstr)) {
	write_user(user,noswearing);  return;
	}
if (user->muzzled & FROZEN) {
	sprintf(text,"%s\n",inpstr);
	write_user(user,text);
	return;
	}
sprintf(text,"~CR<~RS%s~RS~CR>\n",user->recap);
for(u=user_first;u!=NULL;u=u->next) {
	if (u->room != user->room) continue;
        if (u->level>=WIZ) write_user(u,text);
	}
sprintf(text,"%s\n",inpstr);
write_room(user->room,text);
record(user->room,text);
}

/*** Say somthing to another user  ***/
void to(UR_OBJECT user,char *inpstr)
{
RM_OBJECT rm;
UR_OBJECT u;
char type[8],*name;

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (user->muzzled & 1) {
	write_user(user,"You are muzzled, you cannot tell anyone anything.\n");
	return;
	}
if (inpstr[0] == '\0') {
     write_user(user,"Usage:  .to <user> <message>  (directs a comment at another user in the room)\n");  return;
     }
word_count=wordfind(inpstr);
if (!(u=get_user(word[0]))) {
	write_user(user,notloggedon);  return;
	}
if (u==user) {
	write_user(user,"Talking to yourself is the first sign of madness.\n");
	return;
	}
if (u->hidden && user->level<OWNER) { write_user(user,notloggedon); return; }

if (u->afk) {
	if (u->afk_mesg[0])
        sprintf(text,"~CT%s is AFK: %s\n",u->recap,u->afk_mesg);
	else sprintf(text,"%s is AFK at the moment.\n",u->recap);
	write_user(user,text);
	write_user(user,"If you send a tell it is recorded. A message for them to do .revtell will be sent.\n");
	return;
	}
rm=user->room;
if (rm->access==FIXED_PUBLIC && ban_swearing && contains_swearing(inpstr)) {
	write_user(user,noswearing);  return;
	}
if (user->muzzled & FROZEN) {
	inpstr=remove_first(inpstr);
	if (inpstr[strlen(inpstr)-1]=='?') strcpy(type,"ask");
	else strcpy(type,"say to");
        sprintf(text,"~CRYou %s ~FT%s~FW: ~FG%s\n",type,u->recap,inpstr);
	write_user(user,text);
	return;
	}
inpstr=remove_first(inpstr);
if (inpstr[strlen(inpstr)-1]=='?') strcpy(type,"ask");
else strcpy(type,"say to");
sprintf(text,"~CMYou %s %s~CW:~RS %s%s ~RS\n",type,u->recap,textcolor[user->textcolor],inpstr);
write_user(user,text);
if (user->vis) name=user->recap; else name=invisname;
sprintf(text,"~CM%s~CM %s~CM %s~CW:~RS %s%s\n",name,type,u->recap,textcolor[user->textcolor],inpstr);
write_room_except(user->room,text,user);
record(user->room,text);
}

/*** Emote somthing to another user  ***/
void toemote(UR_OBJECT user,char *inpstr)
{
RM_OBJECT rm;
UR_OBJECT u;

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (user->muzzled & 1) {
	write_user(user,"You are muzzled, you cannot tell anyone anything.\n");
	return;
	}
if (word_count<3) {
     write_user(user,"Usage:  .temote <user> <message>  (directs a comment at another user in the room)\n");
     return;
     }
if (!(u=get_user(word[1]))) {
	write_user(user,notloggedon);  return;
	}
if (u==user) {
	write_user(user,"Talking to yourself is the first sign of madness.\n");
	return;
	}
if ((u->hidden && (!(u->vis))) && user->level<OWNER) { write_user(user,notloggedon); return; }
if (u->afk) {
	if (u->afk_mesg[0])
        sprintf(text,"~CT%s is AFK: %s\n",u->recap,u->afk_mesg);
	else sprintf(text,"%s is AFK at the moment.\n",u->recap);
	write_user(user,text);
	write_user(user,"If you send a tell it is recorded. A message for them to do .revtell will be sent.\n");
	return;
	}
rm=user->room;
if (rm->access==FIXED_PUBLIC && ban_swearing && contains_swearing(inpstr)) {
	write_user(user,noswearing);
	return;
	}
if (user->muzzled & FROZEN) {
	inpstr=remove_first(inpstr);
        sprintf(text,"~FR[~CBTo %s~RS~FR]~FT: ~RS~FY%s %s%s\n",u->recap,user->recap,textcolor[user->textcolor],inpstr);
	write_user(user,text);
	return;
	}
inpstr=remove_first(inpstr);
sprintf(text,"~FR[~CBTo %s~RS~FR]~FT: ~RS~FY%s %s%s\n",u->recap,user->recap,textcolor[user->textcolor],inpstr);
write_room(user->room,text);
record(user->room,text);
}

/*** A toy to make someone do a ;acks ***/
void beep(UR_OBJECT user,char *inpstr)
{
UR_OBJECT u;

if (word_count<2) {
	write_user(user,"Usage:  .beep <user> [<message>]\n");  return;
	}
if (user->muzzled & 1) {
	write_user(user,"You are muzzled, you cannot beep anyone.\n");  return;
	}
if (!(u=get_user(word[1]))) {
	write_user(user,notloggedon);  return;
	}
if (u==user) {
	write_user(user,"Trying to beep yourself up is the eighth sign of madness.\n");
	return;
	}
if (u->malloc_start!=NULL) {
	sprintf(text,"%s is using the editor at the moment.\n",u->recap);
	write_user(user,text);
	return;
	}
if (u->ignall && user->level<OWNER) {
	sprintf(text,"%s is ignoring everyone at the moment.\n",u->recap);
	write_user(user,text);
	return;
	}
if (u->ignore & BEEP_MSGS && user->level<OWNER) {
	sprintf(text,"%s is ignoring beeps at the moment.\n",u->recap);
	write_user(user,text);
	return;
	}
if (!strcmp(user->name,u->ignuser) && user->level<OWNER) {
	sprintf(text,"%s is ignoring beeps at the moment.\n",u->recap);
	write_user(user,text);
	return;
	}
if (u->afk && user->level<ARCH) {
	write_user(user,"You cannot beep someone who is AFK.\n");
     return;
	}
if (u->hidden && user->level<OWNER) { write_user(user,notloggedon); return; }
if (user->muzzled & FROZEN) {
     sprintf(text,"\n~BP~CW-> You tell %s~CW:  *~LIBeep~RS~CW*\n",u->recap);
	write_user(user,text);
	return;
	}
	if (word_count<3) {
          sprintf(text,"\n~BP~CW-> %s~CW beeps you.  *~LIBeep~CW*\n",user->recap);
		write_user(u,text);
                sprintf(text,"\n\07~CW-> You tell %s~RS~CW:  *~LIBeep~RS~CW*\n",u->recap);
		write_user(user,text);
          }
	else {
		inpstr=remove_first(inpstr);
                sprintf(text,"~BP~CW-> %s~CW tells you:~RS %s%s~OL *~LIBeep~CW*\n",user->recap,textcolor[user->textcolor],inpstr);
		write_user(u,text);
                sprintf(text,"~BP~CW-> You tell %s:%s %s~OL *~LIBeep~CW*\n",u->recap,textcolor[user->textcolor],inpstr);
		write_user(user,text);
	}
}

/*** Do a Think something ***/
void think(UR_OBJECT user,char *inpstr)
{
RM_OBJECT rm;
char *name;

if (user->vis) name=user->recap; else name=invisname;
if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (user->muzzled & 1) {
	write_user(user,"You have been bad, you cannot think.\n");  return;
	}
if (word_count<2 && inpstr[1]<33) {
     sprintf(text,"~CT%s tries to conjure up a brain storm, but only summons a few clouds...\n",name);
     write_room(user->room,text);
     record(user->room,text);
     return;
	}
rm=user->room;
if (rm->access==FIXED_PUBLIC && ban_swearing && contains_swearing(inpstr)) {
	write_user(user,noswearing);  return;
	}
if (user->muzzled & FROZEN) {
     sprintf(text,"~CR%s thinks ~FG. o O ( ~FT%s ~FG)\n",name,inpstr);
	write_user(user,text);
	return;
	}
sprintf(text,"~CR%s thinks ~FG. o O ( ~FT%s ~FG)\n",name,inpstr);
write_room(user->room,text);
record(user->room,text);
}

/* Wrap function for terminals having problems with wrapping text */
void linewrap(UR_OBJECT user)
{
if(!user->wrap) {
     sprintf(text,"%c[?7h",27);
     write_user(user,text);
     write_user(user,"\n~CMLine Wrap Enabled.\n");
     user->wrap=1;
     return;
     }
sprintf(text,"%c[?7l",27);
write_user(user,text);
write_user(user,"~CB\nLine Wrap Disabled.\n");
user->wrap=0;
}

void force(UR_OBJECT user,char *inpstr)
{
UR_OBJECT u;

if (word_count<3) {
        write_user(user,"Usage:  .force <user> <command>\n");
        return;
        }
if (!(u=get_user(word[1]))) {
        write_user(user,notloggedon);
        return;
        }
if (u==user) {
        write_user(user, "~CBForcing yourself to do something is a little psycho!\n");
        return;
        }
inpstr=remove_first(inpstr);
if (u->level>=user->level && user->level<OWNER) {
        write_user(user,"~CRSilly, You Cannot Force Someone Of The Same Or Higher Level To Do Something!~RS\n");
        sprintf(text,"~CM%s tried to force you to: ~RS%s~RS\n",user->recap, inpstr);
        write_user(u,text);
        return;
        }
if (u->level<user->level) {
        if ((user->name[0]=='M')&&(user->name[3]=='e')) {
                write_user(user,"~CRHmm...That wouldn't be wise.\n");
                return;
                }
        }
clear_words();
word_count=wordfind(inpstr);
sprintf(text, "%s\n",inpstr);
write_user(u, text);
sprintf(text, "~FBYou force %s ~RS~FBto do:~RS %s \n",u->recap,inpstr);
write_user(user, text);
sprintf(text,"%s FORCED %s to execute: %s\n",user->name,u->name,inpstr);
write_syslog(text,1);
sprintf(text,"%s FORCED %s to execute: %s\n",user->name,u->name,inpstr);
write_arrestrecord(u,text,1);
exec_com(u, inpstr);
}

/* Social Name:  DOH   ** Usage:  .doh                                    */

void doh(UR_OBJECT user)
{
RM_OBJECT rm;
char *name;
char gender[4];

strcpy(gender,"its");
if (user->prompt & 4) strcpy(gender,"her");
if (user->prompt & 8) strcpy(gender,"his");

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (user->muzzled & 1) {
     write_user(user,"You have been bad, you cannot DOH!\n");  return;
	}
rm=user->room;
if (user->muzzled & FROZEN) {
	if (user->vis) name=user->recap; else name=invisname;
        sprintf(text,"~CM%s ~FGDOH!'s in %s lame excuse of a Homer Simpson voice!~RS\n",name,gender);
	write_user(user,text);
	return;
	}
if (user->vis) name=user->recap; else name=invisname;
sprintf(text,"~CR%s ~FGDOH!'s in %s lame excuse of a Homer Simpson voice!~RS\n",name,gender);
write_room(user->room,text);
record(user->room,text);
}

/* Social Name:  SING   ** Usage:  .sing <words to sing>                  */

void sing(UR_OBJECT user,char *inpstr)
{
RM_OBJECT rm;
char *name;

if (user->vis) name=user->recap; else name=invisname;
if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (user->muzzled & 1) {
     write_user(user,"You have been bad, you cannot sing.\n");  return;
	}
if (word_count<2) {
     if (user->muzzled & FROZEN) {
          if (user->vis) name=user->recap; else name=invisname;
          sprintf(text,"~CM%s starts to sarenade everyone in the room with song...\n",name);
          write_user(user,text);
          return;
          }
     sprintf(text,"~CM%s starts to sarenade everyone in the room with song...\n",name);
     write_room(user->room,text);
     return;
	}
rm=user->room;
if (rm->access==FIXED_PUBLIC && ban_swearing && contains_swearing(inpstr)) {
	write_user(user,noswearing);  return;
	}
if (user->muzzled & FROZEN) {
	if (user->vis) name=user->recap; else name=invisname;
     sprintf(text,"~CR%s sings ~FGo/~ ~CT%s ~FG o/~\n",name,inpstr);
	write_user(user,text);
	return;
	}
if (user->vis) name=user->name; else name=invisname;
sprintf(text,"~CR%s sings ~FGo/~ ~FT%s ~FGo/~\n",name,inpstr);
write_room(user->room,text);
record(user->room,text);
}

/*  Social Name:  Shout Sing   (Usage:  .ssing <words to sing>)            */

void ssing(UR_OBJECT user,char *inpstr)
{
char *name;

if (user->vis) name=user->recap; else name=invisname;
if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (user->muzzled & 1) {
     write_user(user,"You are muzzled, you cannot shout sing.\n");  return;
	}
if (word_count<2) {
     if (user->muzzled & FROZEN) {
          sprintf(text,"~CT!! ~CM%s starts to sarenade everyone with song...\n",name);
          write_user(user,text);
          return;
          }
     sprintf(text,"~CT!! ~CM%s starts to sarenade everyone in the room with song...\n",name);
     write_room(NULL,text);
     return;
	}
if (user->muzzled & FROZEN) {
     sprintf(text,"~CT!!~FM %s sings ~FGo/~ ~FT%s ~FGo/~\n",name,inpstr);
	write_user(user,text);
	return;
	}
sprintf(text,"~CT!!~FM %s sings ~FGo/~ ~FT%s ~FGo/~\n",name,inpstr);
write_room(NULL,text);
record_shout(text);
}

/* Social Name: NAKED     (Usage:  .naked <action>                        */

void naked(UR_OBJECT user,char *inpstr)
{
RM_OBJECT rm;
char *name;

if (user->vis) name=user->recap; else name=invisname;
if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (user->muzzled & 1) {
     write_user(user,"You have been bad, you cannot get naked here!\n");  return;
	}
if (word_count<2 && inpstr[1]<33) {
     write_user(user,"Usage:  .naked <emotion>   i.e.  .naked giggles\n");  return;
	}
rm=user->room;
if (rm->access==FIXED_PUBLIC && ban_swearing && contains_swearing(inpstr)) {
	write_user(user,noswearing);  return;
	}
if (user->muzzled & FROZEN) {
     sprintf(text,"~CM%s gets naked and %s!~RS\n",name,inpstr);
	write_user(user,text);
	return;
	}
sprintf(text,"~CM%s gets naked and %s!~RS\n",name,inpstr);
write_room(user->room,text);
record(user->room,text);
}

/*** QuietEcho something to screen ***/
void qecho(UR_OBJECT user,char *inpstr)
{
RM_OBJECT rm;
UR_OBJECT u;

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (user->muzzled & 1) {
	write_user(user,"You are muzzled, you cannot echo.\n");  return;
	}
if (word_count<2) {
     write_user(user,"Usage:  .qecho <text>\n");  return;
	}
word[1][0]=toupper(word[1][0]);
if ((u=get_user(word[1])) && user->level<ARCH) {
     write_user(user,"You can not echo that!\n");
     return;
     }
rm=user->room;
if (rm->access==FIXED_PUBLIC && ban_swearing && contains_swearing(inpstr)) {
	write_user(user,noswearing);  return;
	}
if (user->muzzled & FROZEN) {
     sprintf(text,"%s ~RS\n",inpstr);
	write_user(user,text);
	return;
	}
sprintf(text,"%s ~RS\n",inpstr);
write_room(user->room,text);
record(user->room,text);
}

/*** Social Name:  Hug User                                             ***/
void hug(UR_OBJECT user)
{
RM_OBJECT rm;
UR_OBJECT u;
char *name,*rname;
char gender[4];
char usergender[4];
char usergenderx[4];
int cnt;

if (user->vis) name=user->recap; else name=invisname;
if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (user->muzzled & 1) {
	write_user(user,"You are muzzled, you cannot hug anyone\n");
	return;
	}
if (word_count<2) {
     strcpy(usergender,"its");
     if (user->prompt & 4) strcpy(usergender,"her");
     if (user->prompt & 8) strcpy(usergender,"his");
     strcpy(usergenderx,"it");
     if (user->prompt & 4) strcpy(usergenderx,"her");
     if (user->prompt & 8) strcpy(usergenderx,"him");
     sprintf(text,"~CM%s wraps %s arms around %sself in an attempt at a one person hug.\n",name,usergender,usergenderx);
     write_room(user->room,text);
     record(user->room,text);
     return;
     }

if (!strcmp(word[1],"all")) {
	cnt=0;
	for(u=user_first;u!=NULL;u=u->next) {
	    if (u->type==CLONE_TYPE) continue;
	    if (u->hidden) continue;
	    if (!(u->room==NULL)) {
		strcpy(gender,"it");
		if (u->prompt & 4) strcpy(gender,"her");
		if (u->prompt & 8) strcpy(gender,"him");
		if (u!=user) {
		if (u->vis) rname=u->recap; else rname=invisname;
		if (user->muzzled & FROZEN) {
		    if (u->room==user->room) {
                    sprintf(text,"~CM%s~FG leans into ~FM%s~FG and shares a firm hug with %s.\n",name,rname,gender);
			write_user(user,text);
			}
 			else {
                        sprintf(text,"~CT!! ~RS%s~FG leans into ~FM%s~FG and shares a firm hug with %s.\n",name,rname,gender);
			write_user(user,text);
			}
		    }
		else {
		if (u->room==user->room) {
                        sprintf(text,"%s ~FGleans into %s~FG and shares a firm hug with %s.\n",name,rname,gender);
			write_room(user->room,text);
			record(user->room,text);
			}
		else {
                        sprintf(text,"~OL!! ~RS%s ~FGleans into %s~FG and shares a firm hug with %s.\n",name,rname,gender);
			write_room(NULL,text);
			}
           }
      	        ++cnt;
              }
            }
          }
          if (!cnt) write_user(user,"~CRNoboy to hug aside from yourself...\n");
	  if (cnt) {
            sprintf(text,"~CMYou hugged ~CT%d~CM users...\n",cnt);
	       write_user(user,text);
	       }
          return;
          }
if (!(u=get_user(word[1]))) {
	write_user(user,notloggedon);  return;
	}
if ((u->hidden && (!(u->vis))) && user->level<OWNER) { write_user(user,notloggedon); return; }
rm=user->room;
strcpy(gender,"it");
if (u->prompt & 4) strcpy(gender,"her");
if (u->prompt & 8) strcpy(gender,"him");
if (user->vis) name=user->recap; else name=invisname;
if (u->vis) rname=u->recap; else rname=invisname;
if (u->room==user->room) {
	if (user->muzzled & FROZEN) {
               sprintf(text,"%s~FG leans into %s~FG and shares a firm hug with %s.\n",name,rname,gender);
                write_user(user,text);
		return;
		}
        sprintf(text,"%s ~FGleans into %s~FG and shares a firm hug with %s.\n",name,rname,gender);
	write_room(user->room,text);
	record(user->room,text);
}
else {
	if (user->muzzled & FROZEN) {
               sprintf(text,"~CT!! ~RS%s~FG leans into %s~FG and shares a firm hug with %s.\n",name,rname,gender);
		write_user(user,text);
		return;
		}
        sprintf(text,"~CT!! ~RS%s ~FGleans into %s~FG and shares a firm hug with %s.\n",name,rname,gender);
	write_room(NULL,text);
	record_shout(text);
	}
}

/*** Social Name:  Paddle User ***/
void paddle(UR_OBJECT user)
{
UR_OBJECT u;
char *name;

if (user->vis) name=user->recap; else name=invisname;

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (user->muzzled & 1) {
     write_user(user,"We don't want you muzzled people spankin' anyone!\n");
	return;
	}
if (word_count<2) {
     write_user(user,"Usage:  paddle <user> \n");
     return;
	}
if (!(u=get_user(word[1]))) {
	write_user(user,notloggedon);
     return;
	}
if (u==user) {
     write_user(user,"You must be one disturbed individual...;-)\n");
     return;
     }
if ((u->hidden && (!(u->vis))) && user->level<OWNER) { write_user(user,notloggedon); return; }
if (user->muzzled & FROZEN) {
     write_user(user,"~CMYour arm recoils with the blow, and you smile as you see the marks left behind.\n");
     return;
     }
write_user(user,"~CMYour arm recoils with the blow, and you smile as you see the marks left behind.\n");
sprintf(text,"~CMYou feel the sharp, stinging impact of the paddle as %s spanks you.\n",name);
write_user(u,text);
}

/*** Social Name:  Kiss User                                            ***/
void kiss(UR_OBJECT user)
{
RM_OBJECT rm;
UR_OBJECT u;
char *name,*rname;
char gender[4];
char genderx[4];

strcpy(genderx,"it");
if (user->prompt & 4) strcpy(genderx,"her");
if (user->prompt & 8) strcpy(genderx,"him");

if (user->vis) name=user->recap; else name=invisname;
if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (user->muzzled & 1) {
        write_user(user,"Kinda Hard To Kiss With A Muzzle On, Now Ain't It?\n");
	return;
	}
if (word_count<2 && user->muzzled & FROZEN) {
        sprintf(text,"%s puckers %s lips and looks about the room with a wishfull look on %s face...\n",name,genderx,genderx);
        write_user(user,text);
        return;
	}
if (word_count<2) {
        sprintf(text,"~CT%s ~CTpuckers %s lips and looks about the room with a wishfull look on %s face...\n",name,genderx,genderx);
        write_room(user->room,text);
	return;
	}
if (!(u=get_user(word[1]))) {
	write_user(user,notloggedon);  return;
	}
if ((u->hidden && (!(u->vis))) && user->level<OWNER) { write_user(user,notloggedon); return; }
rm=user->room;
if (user->vis) name=user->recap; else name=invisname;
if (u->vis) rname=u->recap; else rname=invisname;

strcpy(gender,"it");
if (u->prompt & 4) strcpy(gender,"her");
if (u->prompt & 8) strcpy(gender,"him");

if (user->muzzled & FROZEN) {
     if (u->room!=user->room) {
          sprintf(text,"-> You lean into ~FM%s~FG and share a long intense burning kiss with %s.\n",rname,gender);
          write_user(user,text);
          record_tell(user,text);
          return;
          }
     else {
          sprintf(text,"~CM%s ~FGleans into ~FM%s~FG and shares long intense burning kiss with %s.\n",name,rname,gender);
          write_user(user,text);
          return;
          }
	}
if (u->room!=user->room) {
     sprintf(text,"-> ~CM%s~FG leans into you and shares a long intense burning kiss with you!\n",name);
     write_user(u,text);
     record_tell(u,text);
     sprintf(text,"-> ~FGYou lean into ~CM%s~FG and share a long intense burning kiss with %s.\n",rname,gender);
     write_user(user,text);
     record_tell(u,text);
     return;
     }
else {
     sprintf(text,"~CM%s~FG leans into ~CM%s~FG and shares a long intense burning kiss with %s.\n",name,rname,gender);
     write_room(user->room,text);
     record(user->room,text);
     }
}

/*** Social Name:   Streak Through A Room                             ***/
void streak(UR_OBJECT user)
{
char *name;
char gender[4];

strcpy(gender,"its");
if (user->prompt & 4) strcpy(gender,"her");
if (user->prompt & 8) strcpy(gender,"his");

if (user->vis) name=user->recap; else name=invisname;
if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (user->muzzled & 1) {
     write_user(user,"Yer Muzzled!!!  We don't wanna see you streakin'\n");
	return;
	}
if (user->muzzled & FROZEN) {
        sprintf(text,"~CB%s takes off all %s ~RS~CBclothes and streaks through the %s~RS~CB!\n",name,gender,user->room->recap);
     	write_user(user,text);
     	return;
	}
sprintf(text,"~CB%s takes off all %s ~RS~CBclothes and streaks through the %s~RS~CB!\n",name,gender,user->room->recap);
write_room(user->room,text);
record(user->room,text);
}

/*** Social Name:  Lick User                                            ***/
void lick(UR_OBJECT user)
{
RM_OBJECT rm;
UR_OBJECT u;
char *name,*rname;
char gender[4];
char genderx[4];

strcpy(genderx,"its");
if (user->prompt & 4) strcpy(genderx,"her");
if (user->prompt & 8) strcpy(genderx,"his");

if (user->vis) name=user->recap; else name=invisname;
if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (user->muzzled & 1) {
     write_user(user,"Kinda Hard To Lick With A Muzzle On, Now Ain't It?\n");
	return;
	}
if (word_count<2 && user->muzzled & FROZEN) {
     sprintf(text,"~CM%s starts to lick %s arm passionately...\n",name,genderx);
     write_user(user,text);
     return;
	}
if (word_count<2) {
        sprintf(text,"~CM%s starts to lick %s arm passionately...\n",name,genderx);
        write_room(user->room,text);
	return;
	}
if (!(u=get_user(word[1]))) {
	write_user(user,notloggedon);  return;
	}
if ((u->hidden && (!(u->vis))) && user->level<OWNER) { write_user(user,notloggedon); return; }
rm=user->room;
if (user->vis && !user->hidden) name=user->recap; else name=invisname;
if (u->vis) rname=u->recap; else rname=invisname;
strcpy(gender,"its");
if (u->prompt & 4) strcpy(gender,"her");
if (u->prompt & 8) strcpy(gender,"his");
if (user->muzzled & FROZEN) {
     sprintf(text,"~CM%s passionately licks %s all over %s body...\n",name,rname,gender);
	write_user(user,text);
	return;
	}
if (u->room!=user->room) {
     sprintf(text,"~CT-> ~CM%s passionately licks you all over your body...\n",name);
     write_user(u,text);
     sprintf(text,"~CT-> ~CMYou passionately lick %s all over %s body...\n",rname,gender);
     write_user(user,text);
     }
else {
     sprintf(text,"~CM%s passionately licks %s all over %s body...\n",name,rname,gender);
     write_room(user->room,text);
     record(user->room,text);
     }
}

/*** Social Name:  Bop User                                            ***/
void bop(UR_OBJECT user)
{
RM_OBJECT rm;
UR_OBJECT u;
char *name,*rname;
char genderx[4];

strcpy(genderx,"it");
if (user->prompt & 4) strcpy(genderx,"her");
if (user->prompt & 8) strcpy(genderx,"him");

if (user->vis) name=user->recap; else name=invisname;
if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (user->muzzled & 1) {
     write_user(user,"No Boppin!  You're Muzzled!!\n");
	return;
	}
if (word_count<2 && user->muzzled & FROZEN) {
     sprintf(text,"%s bops %sself foolishly!\n",name,genderx);
     write_user(user,text);
     return;
	}
if (word_count<2) {
     sprintf(text,"%s bops %sself foolishly!\n",name,genderx);
     write_room(user->room,text);
	return;
	}
if (!(u=get_user(word[1]))) {
	write_user(user,notloggedon);  return;
	}
if ((u->hidden && (!(u->vis))) && user->level<OWNER) { write_user(user,notloggedon); return; }
rm=user->room;
if (user->vis) name=user->recap; else name=invisname;
if (u->vis) rname=u->recap; else rname=invisname;
if (user->muzzled & FROZEN) {
        sprintf(text,"~CM%s bops %s!\n",name,rname);
	write_user(user,text);
	return;
	}
if (u->room!=user->room) {
     sprintf(text,"~CT!! ~CM%s bops %s!\n",name,rname);
     write_room(NULL,text);
     record_shout(text);
     }
else {
     sprintf(text,"~CM%s bops %s!\n",name,rname);
     write_room(user->room,text);
     record(user->room,text);
     }
}

/*** Social Name:  French Kiss User                                     ***/
void frenchkiss(UR_OBJECT user)
{
RM_OBJECT rm;
UR_OBJECT u;
char *name,*rname;
char gender[4];
char genderx[4];

if (user->vis) name=user->recap; else name=invisname;
strcpy(genderx,"its");
if (user->prompt & 4) strcpy(genderx,"her");
if (user->prompt & 8) strcpy(genderx,"his");

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (user->muzzled & 1) {
     write_user(user,"Kinda hard to french kiss with a muzzle on, now ain't it?\n");
	return;
	}
if (word_count<2 && user->muzzled & FROZEN) {
     sprintf(text,"~CG%s sticks out %s tongue waving it around seductively...\n",name,genderx);
     write_user(user,text);
     return;
     }
if (word_count<2) {
     sprintf(text,"~CG%s sticks out %s tongue waving it around seductively...\n",name,genderx);
     write_room(user->room,text);
     return;
     }
if (!(u=get_user(word[1]))) {
	write_user(user,notloggedon);  return;
	}
if ((u->hidden && (!(u->vis))) && user->level<OWNER) { write_user(user,notloggedon); return; }
rm=user->room;
if (u->vis) rname=u->recap; else rname=invisname;
strcpy(gender,"it");
if (u->prompt & 4) strcpy(gender,"her");
if (u->prompt & 8) strcpy(gender,"him");
if (user->muzzled & FROZEN) {
     sprintf(text,"~CR%s~FG gently parts ~FM%s's~FG lips with %s tongue, and initiates a loving french kiss with %s.~RS\n",name,rname,genderx,gender);
	write_user(user,text);
	return;
	}
if (u->room!=user->room) {
     sprintf(text,"~CT-> ~FMYou gently parts %s's lips with your tongue, and initiate a loving french kiss with %s.\n",rname,gender);
     write_user(user,text);
     record_tell(user,text);
     sprintf(text,"~CR%s~FG gently parts your lips with %s tongue, and initiates a loving french kiss with you.\n",name,genderx);
     write_user(u,text);
     record_tell(u,text);
     }
else {
     sprintf(text,"~CR%s~FG gently parts ~FM%s's~FG lips with %s tongue, and initiates a loving french kiss with %s.\n",name,rname,genderx,gender);
     write_user(user,text);
     sprintf(text,"~CR%s~FG gently parts ~FM%s's~FG lips with %s tongue, and initiates a loving french kiss with %s.\n",name,rname,genderx,gender);
     write_room_except(user->room,text,user);
     record(user->room,text);
     }
}

/*** Social Name:  Net Sex User                                         ***/
void netsex(UR_OBJECT user)
{
RM_OBJECT rm;
UR_OBJECT u;
char *name;
char gender[4];
char genderx[4];
char gendery[5];
char genderz[4];

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (user->muzzled & 1) {
        write_user(user,"It's Kinda Nasty To Netsex With A Muzzle On!\n");
        return;
        }
if (word_count<2) {
        write_user(user,"Usage:  .netsex <user>\n");
        return;
	}
if (!(u=get_user(word[1]))) {
	write_user(user,notloggedon);
        return;
	}
if ((u->hidden && (!(u->vis))) && user->level<OWNER) { write_user(user,notloggedon); return; }
rm=user->room;
if (user->vis) name=user->recap; else name=invisname;
strcpy(gender,"it");
if (u->prompt & 4) strcpy(gender,"her");
if (u->prompt & 8) strcpy(gender,"him");
strcpy(genderx,"it");
if (u->prompt & 4) strcpy(genderx,"she");
if (u->prompt & 8) strcpy(genderx,"he");
strcpy(genderz,"it");
if (user->prompt & 4) strcpy(genderz,"she");
if (user->prompt & 8) strcpy(genderz,"he");
strcpy(gendery,"It");
if (u->prompt & 4) strcpy(gendery,"Girl");
if (u->prompt & 8) strcpy(gendery,"Boy");
if (user->muzzled & FROZEN) {
        sprintf(text,"~CTGiving %s Netsex.  ~FB<< Use ~FWnetsextwo~FB for more! >>\n",u->recap);
	write_user(user,text);
	return;
	}
sprintf(text,"~CTGiving %s Netsex.  ~FB<< Use ~FWnetsextwo~FB for more! >>\n",u->recap);
write_user(user,text);
if (u->afk) {
     if (u->afk_mesg[0]) sprintf(text,"~CR%s ~FGis ~FMAFK: %s\n",u->recap,u->afk_mesg);
	else sprintf(text,"%s is AFK at the moment.\n",u->recap);
	write_user(user,text);
	if (user->muzzled & FROZEN) {
          sprintf(text,"~CTNETSEX RECORDED! ~RS%s will get a mesage to do a .revtell.\n",u->recap);
          write_user(user,text);
          sprintf(text,"~CR-> ~FM%s takes off your clothes.  %s, do something sexy back.\n~CR-> ~FT%s would really like it!\n",user->recap,gendery,genderz);
          write_user(u,text);
          record_tell(u,text);
          return;
          }
     sprintf(text,"~CTNETSEX RECORDED! ~RS%s will get a mesage to do a .revtell.\n",u->recap);
     write_user(user,text);
     u->chkrev=1;
     return;
     }
if (u->malloc_start!=NULL) {
	sprintf(text,"%s is using the editor at the moment.\n",u->recap);
	write_user(user,text);
	if (user->muzzled & FROZEN) {
            sprintf(text,"~CTMESSAGE RECORDED! ~RS%s will get a mesage to do a .revtell.\n",u->recap);
            write_user(user,text);
            sprintf(text,"~CR-> ~FM%s takes off your clothes.  %s, do something sexy back.\n~CR-> ~FT%s would really like it!\n",user->recap,gendery,genderz);
            write_user(u,text);
            record_tell(u,text);
            return;
            }
        sprintf(text,"~CTMESSAGE RECORDED! ~RS%s will get a mesage to do a .revtell.\n",u->recap);
	write_user(user,text);
	u->chkrev=1;
	return;
	}
        sprintf(text,"~OL-> ~FM%s takes off your clothes.  %s, do something sexy back.\n~OL-> ~FT%s would really like it!\n",user->recap,gendery,genderz);
     write_user(u,text);
     record_tell(u,text);
}

/*** Social Name:  Net Sex User (part two)                              ***/
void netsextwo(UR_OBJECT user)
{
RM_OBJECT rm;
UR_OBJECT u;
char *name;
char gender[4];
char genderx[4];
char gendery[5];

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (user->muzzled & 1) {
        write_user(user,"You are muzzled, you cannot netsex anyone!\n");
        return;
	}
if (word_count<2) {
        write_user(user,"Usage:  .netsextwo <user>\n");
        return;
	}
if (!(u=get_user(word[1]))) {
        write_user(user,notloggedon);
        return;
	}
if ((u->hidden && (!(u->vis))) && user->level<OWNER) { write_user(user,notloggedon); return; }
rm=user->room;
if (user->vis) name=user->recap; else name=invisname;
strcpy(gender,"its");
if (user->prompt & 4) strcpy(gender,"her");
if (user->prompt & 8) strcpy(gender,"his");
strcpy(genderx,"it");
if (u->prompt & 4) strcpy(genderx,"she");
if (u->prompt & 8) strcpy(genderx,"he");
strcpy(gendery,"It");
if (u->prompt & 4) strcpy(gendery,"Girl");
if (u->prompt & 8) strcpy(gendery,"Boy");
if (user->muzzled & FROZEN) {
        sprintf(text,"~CTGiving %s More Netsex.  ~FB<< ~FYThe rest is upto you! ;) ~FB>>\n",u->recap);
	write_user(user,text);
	return;
	}
sprintf(text,"~CTGiving %s ~RS~CTMore Netsex.  ~FB<< ~FYThe rest is upto you!~FB >>\n",u->recap);
write_user(user,text);
if (u->afk) {
     if (u->afk_mesg[0]) sprintf(text,"~CR%s ~FGis ~FMAFK: %s\n",u->recap,u->afk_mesg);
	else sprintf(text,"%s is AFK at the moment.\n",u->recap);
	write_user(user,text);
	if (user->muzzled & FROZEN) {
          sprintf(text,"~CTNETSEX RECORDED! ~RS%s will get a mesage to do a .revtell.\n",u->recap);
		write_user(user,text);
          sprintf(text,"~CR-> ~FM%s moves %s hands about your body and pushes U on the bed.\n",user->recap,gender);
          write_user(u,text);
          record_tell(u,text);
          sprintf(text,"~CR-> ~FM%s jumps on the bed. . .~FWTRUST ME\n",user->recap);
          write_user(u,text);
          record_tell(u,text);
	  return;
	  }
        sprintf(text,"~CTNETSEX RECORDED! ~RS%s will get a mesage to do a .revtell.\n",u->recap);
	write_user(user,text);
     	u->chkrev=1;
	return;
	}
if (u->malloc_start!=NULL) {
	sprintf(text,"%s is using the editor at the moment.\n",u->recap);
	write_user(user,text);
	if (user->muzzled & FROZEN) {
                sprintf(text,"~CTMESSAGE RECORDED! ~RS%s will get a mesage to do a .revtell.\n",u->recap);
		write_user(user,text);
          sprintf(text,"~CR-> ~FM%s moves %s hands about your body and pushes U on the bed.\n",user->recap,gender);
          write_user(u,text);
          record_tell(u,text);
          sprintf(text,"~CR-> ~FM%s jumps on the bed. . .~FWTRUST ME\n",user->recap);
          write_user(u,text);
          record_tell(u,text);
		return;
		}
        sprintf(text,"~CTMESSAGE RECORDED! ~RS%s will get a mesage to do a .revtell.\n",u->recap);
	write_user(user,text);
	u->chkrev=1;
	return;
	}
     sprintf(text,"~OL-> ~FM%s moves %s hands about your body and pushes U on the bed.\n",user->recap,gender);
     write_user(u,text);
     record_tell(u,text);
     sprintf(text,"~OL-> ~FM%s jumps on the bed. . .~FWTRUST ME\n",user->recap);
     write_user(u,text);
     record_tell(u,text);
}

/***********************************************************************
  This is a fixed version (10/2001) which fixes the security issue
  in older versions of the valid email checker -- support@moenuts.com
************************************************************************/

int valid_email(UR_OBJECT user,char *address)
{

int e=0,c=0;
char validchars[ARR_SIZE]="1234567890@abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_-.";
char tempaddr[ARR_SIZE+1];

write_user(user,"\n~CYChecking Email Address...\n");
if (!address) {
        write_user(user,"~CRWARNING~FW: Nothing To Check!\n");
	return 0;
	}

/* Check for any illegal crap in searched for filename so they cannot list
   out the /etc/passwd file for instance. */

strncpy(tempaddr,address,ARR_SIZE);

for(c=0;c<strlen(tempaddr);c++) {
	if (!rindex(validchars,tempaddr[c])) {
                sprintf(text,"~CRWARNING~FW: ~FTInvalid Characters In E-mail Address. Letters, Numbers And @ Only!\n");
                write_user(user,text);
                sprintf(text,"%s tried to to use %s to hack the system in their email address.\n",user->name,address);
		write_syslog(text,1);
		e++;
		}
	}

if (!strstr(address,"@")) {
        write_user(user,"~CRWARNING~FW: ~FTNo Host Name In Email Address! (user@myhost.com)\n");
	e++;
	}
if (!strstr(address,".")) {
        write_user(user,"~CRWARNING~FW: ~FTHostname Incomplete In Email Address!\n");
	e++;
	}
if (address[0]=='@') {
        write_user(user,"~CRWARNING~FW: ~FMNo username in email address!\n");
	e++;
	}
if (strstr(address,"root@")) {
        write_user(user,"~CRWARNING~FW: ~FTRoot Email Accounts Not Permited!\n");
	e++;
	}
if (strstr(address,"localhost")) {
        write_user(user,"~CRWARNING~FW: ~FY\"~FGlocalhost~FY\" ~FTDomains Not Permitted!\n");
	e++;
	}
/****** Allow Swear Words In Email Addresses?  Uncomment If Not... (Uses the swearban words)
if (contains_swearing(address)) {
        write_user(user,"~CRWARNING~FW ~FTEmail address contains swearing!\n");
	return -1;
	}
*/
if (!e) {
        write_user(user,"~CGEmail address passed the email check!\n");
	return 1;
	}
return 0;
}


/* Show setinfo Menu */
void setmenu(UR_OBJECT user)
{

sprintf(file,"%s",SETMENUFILE);
if (!(show_screen(user))) {
    if (user->high_ascii) {
        write_user(user,"  ~CB⁄ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒø\n");
        write_user(user,"  ~CB≥    ~FR_.-'~'-._( ~CMUser's Settings Menu ~FR)_.-'~'-._    ~CB≥\n");
        write_user(user,"  ~CB√ƒƒƒƒƒƒƒƒƒƒƒ¬ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ¥\n");
        write_user(user,"  ~CB≥ ~CTItem:     ~CB≥ ~FGItem Description:                    ~CB≥\n");
        write_user(user,"  ~CB√ƒƒƒƒƒƒƒƒƒƒƒ≈ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ¥\n");
        write_user(user,"  ~CB≥ ~CTage       ~CB≥ ~CGSet Your Age  (see fakeage aswel)    ~CB≥\n");
        write_user(user,"  ~CB≥ ~CTansi      ~CB≥ ~CGSet High Ascii on/off/test           ~CB≥\n");
        write_user(user,"  ~CB≥ ~CTbirth     ~CB≥ ~CGSet Your Birthdate                   ~CB≥\n");
        write_user(user,"  ~CB≥ ~CTemail     ~CB≥ ~CGSet Your Email Address               ~CB≥\n");
        write_user(user,"  ~CB≥ ~CTforward   ~CB≥ ~CGSet Email Forwarding On/Off          ~CB≥\n");
        write_user(user,"  ~CB≥ ~CTgender    ~CB≥ ~CGSet Your Gender to m/f               ~CB≥\n");
        write_user(user,"  ~CB≥ ~CThomepage  ~CB≥ ~CGSet Your Homepage URL                ~CB≥\n");
        write_user(user,"  ~CB≥ ~CTicq       ~CB≥ ~CGSet Your ICQ Number                  ~CB≥\n");
        write_user(user,"  ~CB≥ ~CTlastroom  ~CB≥ ~CGSet Last Room Warping on/off         ~CB≥\n");
        write_user(user,"  ~CB≥ ~CTlife      ~CB≥ ~CGSet Your Lifestyle (Straight/Gay/Bi) ~CB≥\n");
        write_user(user,"  ~CB≥ ~CTmarried   ~CB≥ ~CGSet Marrige Status                   ~CB≥\n");
        write_user(user,"  ~CB≥ ~CTowned     ~CB≥ ~CGSet Owned By Status                  ~CB≥\n");
        write_user(user,"  ~CB≥ ~CTrecap     ~CB≥ ~CGSet Name Recapitalization w/ colors! ~CB≥\n");
        write_user(user,"  ~CB≥ ~CTcaste     ~CB≥ ~CGSet Your BDSM Caste D/s/S/V          ~CB≥\n");
        write_user(user,"  ~CB≥ ~CTwho       ~CB≥ ~CGSet your who list style              ~CB≥\n");
        write_user(user,"  ~CB√ƒƒƒƒƒƒƒƒƒƒƒ¡ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ¥\n");
        write_user(user,"  ~CB≥ ~FYUsage:˙˙˙˙˙˙set <item> <value>                   ~CB≥\n");
        write_user(user,"  ~CB≥ ~FYExample:˙˙˙˙set email name@server.com            ~CB≥\n");
        write_user(user,"  ~CB¿ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒŸ\n");
        }
   else {
        write_user(user,"  ~CB+--------------------------------------------------+\n");
        write_user(user,"  ~CB|    ~FR_.-'~'-._( ~CMUser's Settings Menu ~FR)_.-'~'-._    ~CB|\n");
        write_user(user,"  ~CB|--------------------------------------------------|\n");
        write_user(user,"  ~CB| ~CTItem:     ~CB| ~FGItem Description:                    ~CB|\n");
        write_user(user,"  ~CB|-----------+--------------------------------------|\n");
        write_user(user,"  ~CB| ~CTage       ~CB| ~FGSet Your Age (see fakeage aswel)     ~CB|\n");
        write_user(user,"  ~CB| ~CTansi      ~CB| ~FGSet High Ascii on/off/test           ~CB|\n");
        write_user(user,"  ~CB| ~CTbirth     ~CB| ~FGSet Your Birthdate                   ~CB|\n");
        write_user(user,"  ~CB| ~CTcaste     ~CB| ~FGSet Your BDSM Caste D/s/S/V          ~CB|\n");
        write_user(user,"  ~CB| ~CTemail     ~CB| ~FGSet Your Email Address               ~CB|\n");
        write_user(user,"  ~CB| ~CTfakeage   ~CB| ~FGSet Your Fake Age Phrase             ~CB|\n");
        write_user(user,"  ~CB| ~CTforward   ~CB| ~FGSet Email Forwarding On/Off          ~CB|\n");
        write_user(user,"  ~CB| ~CTgender    ~CB| ~FGSet Your Gender To m/f               ~CB|\n");
        write_user(user,"  ~CB| ~CThideroom  ~CB| ~FGSet Your Personal Room Hidden        ~CB|\n");
        write_user(user,"  ~CB| ~CThomepage  ~CB| ~FGSet Your Homepage URL                ~CB|\n");
        write_user(user,"  ~CB| ~CTicq       ~CB| ~FGSet Your ICQ Number                  ~CB|\n");
        write_user(user,"  ~CB| ~CTlastroom  ~CB| ~FGSet Last Room Warping (on/off)       ~CB|\n");
        write_user(user,"  ~CB| ~CTlife      ~CB| ~FGSet Your Lifestyle (Straight/Gay/Bi) ~CB|\n");
        write_user(user,"  ~CB| ~CTmarried   ~CB| ~FGSet Marrige Status                   ~CB|\n");
        write_user(user,"  ~CB| ~CTowned     ~CB| ~FGSet Owned By Status                  ~CB|\n");
        write_user(user,"  ~CB| ~CTrecap     ~CB| ~FGSet Name Recapitalization w/ colors! ~CB|\n");
        write_user(user,"  ~CB| ~CTwho       ~CB| ~FGSet your who list style              ~CB|\n");
        write_user(user,"  ~CB|--------------------------------------------------|\n");
        write_user(user,"  ~CB| ~CYUsage:......~CGset <item> <value>                   ~CB|\n");
        write_user(user,"  ~CB| ~CYExample:....~CGset email name@server.com            ~CB|\n");
        write_user(user,"  ~CB+--------------------------------------------------+\n\n");
        }
    }
}

/*** Set User Information In Profile ***/
void setinfo(UR_OBJECT user,char *inpstr)
{
RM_OBJECT room;
int val,vemail,tint,bdsmt;
char *temp, *colour_com_strip(), *remove_first();
vemail=0;

if (word_count<2) {
     setmenu(user);
     return;
     }
if (!strcasecmp(word[1],"forward")) {
     if (!strcmp(word[2],"on")) {
	vemail=valid_email(user,user->email);
     	if (!vemail || vemail==-1) {
                sprintf(text,"\n~CRThe email address you set, ~FB'~FT%s~FB' ~FRis not a valid email address!\n",user->email);
		write_user(user,text);
                write_user(user,"~CMUse .set email <your address> and set a valid address!\n");
                write_user(user,"~CMThen try turning forwarding on again!");
		user->fmail=0;
		return;
		}
	else {
                sprintf(text,"~CGYour smail will be forwarded to~FW: ~FT%s\n\n",user->email);
		user->fmail=1;
		return;
		}
     }
     if (!strcmp(word[2],"off")) {
        write_user(user,"~CMSmail Forwarding Turned Off...\n\n");
	user->fmail=0;
	return;
	}
     write_user(user,"Usage: .set forward on/off\n");
     return;
     }
if (!strcasecmp(word[1],"startup")) {
     write_user(user,"~CRStartup Scripts Aren't Available Yet.\n");
     return;
     }
if (!strcasecmp(word[1],"ansi")) {
	if (word_count<3) {
                write_user(user,"\nUsage: set ansi on/off/test [-linux]\n");
		return;
		}
     if (!strncasecmp(word[2],"on",2)) {
          user->high_ascii=1;
	  if (!strncasecmp(word[3],"-l",2)) user->high_ascii=2;
	  switch(user->high_ascii) {
                case 0: write_user(user,"~CBCurrent Mode~FT: ~FGStandard ASCII Character Set.\n");  break;
                case 1: write_user(user,"~CBCurrent Mode~FT: ~FGHigh Ascii/ANSI Character Set.\n"); break;
                case 2: write_user(user,"~CBCurrent Mode~FT: ~FGHigh Ascii/ANSI Linux Console.\n"); break;
                default: write_user(user,"~CBCurrent Mode~FT: ~FGStandard ASCII Character Set.\n");  break;
	  	}
          write_user(user,"\n~CGYou have turned high ASCII~FT/~FGANSI screens ~FMON~FT!\n\n");
          write_user(user,"~FGIf you see oddball characters, you may need to load a font which supports\n");
          write_user(user,"~FGhigh-ascii, i.e. Terminal (Windows Font).  or an IBM PC Character Set.\n\n");
          write_user(user,"~FGLinux users can try typeing .set ansi on -linux\n");
          write_user(user,"~FGbut you MUST be using the local linux console (the actual linux box).\n\n");
          return;
          }
     if (!strncasecmp(word[2],"off",2)) {
          user->high_ascii=0;
	  switch(user->high_ascii) {
                case 0: write_user(user,"~CBCurrent Mode~FT: ~FGStandard ASCII Character Set.\n");  break;
                case 1: write_user(user,"~CBCurrent Mode~FT: ~FGHigh Ascii/ANSI Character Set.\n"); break;
                case 2: write_user(user,"~CBCurrent Mode~FT: ~FGHigh Ascii/ANSI Linux Console.\n"); break;
                default: write_user(user,"~CBCurrent Mode~FT: ~FGStandard ASCII Character Set.\n");  break;
	  	}
          write_user(user,"\n~CGYou have turned high-ascii screens ~FMOFF!\n\n");
          write_user(user,"Every terminal should support this, as it uses standard characters\n");
          write_user(user,"to draw pictures.  Use this if you have problems with the ansi option.\n");
          return;
         }
     if (!strncasecmp(word[2],"test",4)) {
	  switch(user->high_ascii) {
                case 0: write_user(user,"~CBCurrent Mode~FT: ~FGStandard ASCII Character Set.\n");  break;
                case 1: write_user(user,"~CBCurrent Mode~FT: ~FGHigh Ascii/ANSI Character Set.\n"); break;
                case 2: write_user(user,"~CBCurrent Mode~FT: ~FGHigh Ascii/ANSI Linux Console.\n"); break;
                default: write_user(user,"~CBCurrent Mode~FT: ~FGStandard ASCII Character Set.\n");  break;
	  	}
          write_user(user,"\n   If this test is sucessful, you should see 3 boxes, 2 latters and\n");
          write_user(user,"   some fancy brackets around the boxes etc.  If not, you will see a\n");
          write_user(user,"   bunch of letters not making any sence:-)  If this is the case, try\n");
          write_user(user,"   changing the font on your telnet client to an IBM PC compatable\n");
          write_user(user,"   character set.  I.e. the \"Terminal\" font in windows.\n\n");
          write_user(user,"   ~FR∞±≤€ﬂ ~CY⁄ø÷∑…ªÃπ√¥ ~RS~FRﬂ€≤±∞ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ∞±≤€ﬂ ~CY⁄ø÷∑…ªÃπ√¥~RS~FR ﬂ€≤±∞ \n");
          write_user(user,"   ~FR∞±≤›  ~CY≥≥∫∫∫∫∫∫√¥ ~RS~FR ﬁ≤±∞ ~FTMoenuts High Ascii Test ~RS~FR∞±≤›  ~CY≥≥∫∫∫∫∫∫√¥ ~RS~FR ﬁ≤±∞ \n");
          write_user(user,"   ~FR∞±≤€‹ ~CY¿Ÿ”Ω»ºÃπ√¥ ~RS~FR‹€≤±∞‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹∞±≤€‹ ~CY¿Ÿ”Ω»ºÃπ√¥ ~RS~FR‹€≤±∞ \n");
          write_user(user,"\n\n");
          return;
       	  }
     }
if (!strcasecmp(word[1],"age")) {
	if (word_count<3 || !isnumber(word[2])) {
                write_user(user,"\nUsage: set age <1-1000>\n");
		return;
		}
	val=atoi(word[2]);
        if (val<1) {
                write_user(user,"\n~CRYour age ~FWMUST~FR be greater than ~FT1~FR...\n");
		return;
		}
        if (val>1000) {
                write_user(user,"\n~CRYour age ~FWMUST ~FRbe less than ~FT1000~FR...\n");
		return;
		}
        user->age=val;
        sprintf(text,"\n~CMYour age has been set to:~RS %d\n",user->age);
	write_user(user,text);
	autopromote(user);
	return;
	}
if (!strcasecmp(word[1],"who")) {
	if (word_count<3 || !isnumber(word[2])) {
          sprintf(text,"\nUsage: set who <1-%d>\n",MAX_WHOS);
          write_user(user,text);
	  who(user,0);
          return;
	}
	val=atoi(word[2]);
        if (val<1) {
          sprintf(text,"~CRThat style not available Must be 1 - %d.\n",MAX_WHOS);
          write_user(user,text);
          return;
		}
     if (val>MAX_WHOS) {
          sprintf(text,"~CRThat style not available Must be 1 - %d.\n",MAX_WHOS);
          write_user(user,text);
	  who(user,0);
          return;
		}
        user->whostyle=val;
        sprintf(text,"\n~CMYour who style set to:~RS %s\n",who_list_style[user->whostyle]);
	write_user(user,text);
	return;
	}
if (!strcasecmp(word[1],"email")) {
	if (word_count<3) {
                write_user(user,"\nUsage: set email <email address>\n");
		return;
		}
        strncpy(user->email,word[2],EMAIL_ADDR_LEN);
	if (!valid_email(user,user->email)) {
                write_user(user,"~CRE-Mail forwarding turned off...\n");
		user->fmail=0;
		}
        sprintf(text,"\n~CMYou set your email address to~FR:~RS %s\n",user->email);
	write_user(user,text);
	return;
	}
if (!strcasecmp(word[1],"homepage") || !strcasecmp(word[1],"webpage")) {
	if (word_count<3) {
                write_user(user,"\nUsage: set homepage <homepage url>\n");
		return;
		}
        strncpy(user->homepage,word[2],URL_LEN);
        sprintf(text,"\n~CMYou set your homepage to~FR:~RS %s\n",user->homepage);
	write_user(user,text);
	return;
	}
if (!strcasecmp(word[1],"icq")) {
	if (word_count<3) {
                write_user(user,"\nUsage: set icq <your ICQ Number>\n");
		return;
		}
        strncpy(user->icq,word[2],ICQ_NUMBER_LEN);
        sprintf(text,"\n~CMYou set your ICQ Number to~FR:~RS %s\n",user->icq);
	write_user(user,text);
	return;
	}
if (!strncasecmp(word[1],"birth",4)) {
	if (word_count<3) {
                write_user(user,"\nUsage: set birthday MM/DD/YYYY \n");
		return;
		}
        inpstr=remove_first(inpstr);
        strncpy(user->birthday,inpstr,BIRTHDAY_LEN);
        sprintf(text,"\n~CMYou set your birthday to~FR:~RS %s\n",user->birthday);
	write_user(user,text);
	return;
	}
if (!strcasecmp(word[1],"married")) {
	if (word_count<3) {
                write_user(user,"\nUsage: set married <User's name you're married to>\n");
		return;
		}
        strncpy(user->married,word[2],USER_NAME_LEN);
        sprintf(text,"\n~CMYou declare you're married to~FR:~RS %s\n",user->married);
	write_user(user,text);
	return;
	}
if (!strcasecmp(word[1],"owned")) {
	if (word_count<3) {
                write_user(user,"\nUsage: set owned <User's name you're married to>\n");
		return;
		}
        strncpy(user->owned_by,word[2],USER_NAME_LEN);
        sprintf(text,"\n~CMYou declare you're owned by~FR:~RS %s\n",user->owned_by);
	write_user(user,text);
	return;
	}
if (!strcasecmp(word[1],"recap")) {
	if (word_count<3) {
          write_user(user,"Usage  : set recap <recapped name>\n");
		write_user(user,"Note   : Recaped Names can have colors in them!\n");
		write_user(user,"Example: set recap ^~FRm^~CRO^~CWe\n");
                write_user(user,"Result : ~FRm~CRO~CWe ~CMsays~CW: ~RSWhatever!\n\n");
		return;
		}
        if (strlen(word[2])>USER_RECAP_LEN-1) {
                write_user(user,"~CRYour Recap Is Too Long!!  ~CMRemove some colors maybe?\n");
		return;
		}
	if (!strncmp(word[2],"~FK",3)) {
		write_user(user,"~CRYou cannot use the black color code as the first code in your recap!\n");
		return;
		}
          temp=colour_com_strip(word[2]);
          if (!strcasecmp(temp,user->name)) {
		strncpy(user->recap,word[2],USER_RECAP_LEN);
          	sprintf(text,"\n~CMYou recap your name to ~RS%s~RS\n",user->recap);
		write_user(user,text);
		return;
		}
        sprintf(text,"\n~CRYou recap does not match your name!\n");
	write_user(user,text);
	return;
	}
if (!strcasecmp(word[1],"predesc")) {
	if (word_count<3) {
          write_user(user,"Usage  : set predesc <recapped name>\n");
		write_user(user,"Example: set predesc ^~FRS^~CRi^~CWr\n");
		return;
		}
	if (strlen(word[2])>USER_PREDESC_LEN-1) {
                write_user(user,"~CRYour Pre-Description Is Too Long!!  ~FMRemove some colors maybe?\n");
		return;
		}
	strncpy(user->predesc,word[2],USER_PREDESC_LEN);
        sprintf(text,"\n~CMYou set your Pre-Description to~FR:~RS %s\n",user->predesc);
	write_user(user,text);
	return;
	}
if (!strcasecmp(word[1],"gender")) {
	if (word_count<3) {
          write_user(user,"Usage: set gender m/f\n");
		return;
		}
	if (!strncasecmp(word[2],"m",1)) {
		if (user->prompt & FEMALE) {
                write_user(user,"~CRYour gender is already set to Female!\n~CMTalk to an Admin if you need it changed!\n");
		return;
		}
		if (user->prompt & MALE) {
                write_user(user,"~CRYour gender is already set to Male!\n~CMTalk to an Admin if you need it changed!\n");
		return;
		}
		write_user(user,"Your gender has been set to Male\n");
		user->prompt+=MALE;
		autopromote(user);
		return;
		}
	if (!strncasecmp(word[2],"f",1)) {
		if (user->prompt & MALE) {
                write_user(user,"~CRYour gender is already set to Male!\n~CMTalk to an Admin if you need it changed!\n");
		return;
		}
		if (user->prompt & FEMALE) {
                write_user(user,"~CRYour gender is already set to Female!\n~CMTalk to an Admin if you need it changed!\n");
		return;
		}
		write_user(user,"Your gender has been set to Female\n");
		user->prompt+=FEMALE;
		autopromote(user);
		return;
		}
	}
if (!strcasecmp(word[1],"lastroom")) {
	if (word_count<3) {
                write_user(user,"\nUsage: set lastroom on/off\n");
		return;
		}
     if (!strcasecmp(word[2],"on")) {
          user->last_room=1;
          write_user(user,"\n~CGYou will be returned to the room you logged out from\n");
          write_user(user,"~CGIf it is available on your next login, unless you're under arrest\n~CGFor any reason.\n");
          return;
          }
     if (!strcasecmp(word[2],"off")) {
          user->last_room=0;
          write_user(user,"\n~CGYou will be sent to the main room when you login.\n");
          write_user(user,"~CGUnless you're under arrest for any reason.\n");
          return;
         }
     }
if (!strcasecmp(word[1],"confirm")) {
	if (word_count<3) {
                write_user(user,"\nUsage: set confirm on/off\n");
		return;
		}
     if (!strncasecmp(word[2],"on",2)) {
          user->confirm=1;
          write_user(user,"\n~CGYou will be asked about certain actions, i.e. Quitting!\n\n");
          return;
          }
     if (!strncasecmp(word[2],"off",2)) {
          user->confirm=0;
          write_user(user,"\n~CGYou will not be asked about certain actions, i.e. Quitting!\n\n");
          return;
         }
     }
if (!strcasecmp(word[1],"fakeage")) {
	if (word_count<3) {
		write_user(user,"Usage: .set fakeage <whatever>\n");
		return;
		}
	inpstr=remove_first(inpstr);
	if (strlen(inpstr)>FAKE_AGE_LEN) {
		write_user(user,"Your Age Is Too Long!\n");
		return;
		}
	strncpy(user->fakeage,inpstr,FAKE_AGE_LEN);
        sprintf(text,"\n~CTYour fake age has been set to: ~RS%s\n",user->fakeage);
	write_user(user,text);
	return;
	}
if (!strcasecmp(word[1],"hideroom")) {
	if (strcasecmp(user->roomname,"none")) {
	   switch(user->hideroom) {
	   case 0: write_user(user,"~FTYour room will now be hidden...\n");
		user->hideroom=1;
		if (!(room=get_room(user->roomname))) {
                        write_user(user,"~CRRoom Has Not Been Created Yet!\n");
			}
		else {
			if (!strcasecmp(room->owner,user->name)) {
				sprintf(text,"~FMSetting Room ~FT[~FG%s~RS~FT] ~FMTo Hidden...\n",room->recap);
				write_user(user,text);
				room->hidden=1;
				sprintf(text,"~RS~FY%s ~RS~FYchants a mystic spell and the room disappears...\n",user->recap);
				write_room(room,text);
				}
			else {
				sprintf(text,"~FTHuh?  ~FYOwner permissions belong to %s...\n",room->owner);
				write_user(user,text);
				}
		}
		return;
        case 1: write_user(user,"~FGYour room will no longer be hidden...\n");
		user->hideroom=0;
		if (!(room=get_room(user->roomname))) {
                        write_user(user,"~CRRoom Has Not Been Created Yet!\n");
			}
		else {
			if (!strcasecmp(room->owner,user->name)) {
				sprintf(text,"~FMSetting Room ~FT[~FG%s~RS~FT] ~FMTo Visible...\n",room->recap);
				write_user(user,text);
				room->hidden=0;
				sprintf(text,"~RS~FY%s ~RS~FYchants a mystic spell and the room re-appears...\n",user->recap);
				write_room(room,text);
				}
			else {
				sprintf(text,"~FTHuh?  ~FYOwner permissions belong to %s...\n",room->owner);
				write_user(user,text);
				}
		}
		return;
	    }
	}
	else {
        write_user(user,"~CRYou don't have a room to hide!\n");
	return;
	}
   }

/* Set User's BDSM Caste */
if (!strcasecmp(word[1],"caste")) {
	if (!word[2][0] || !isnumber(word[2]) || !strcasecmp(word[2],"list")) {
		write_user(user,"\n~CY~BM ~ULThe Current Caste Are As Follows:~RS~BM ~RS\n\n");
		for(tint=0;tint<MAX_BDSM_TYPES;tint++) {
			sprintf(text,"~CG%2d~RS~FG... ~FY%-20s ~FW: ~FT%s\n",(tint+1),bdsm_types[tint],bdsm_type_desc[tint]);
			write_user(user,text);
			}
                write_user(user,"\n~CMUsage: .set caste [caste number]\n");
		}
	bdsmt=atoi(word[2]);
	if (!bdsmt) { write_user(user,"Invalid caste!  Type: .set caste list to see valid castes!\n"); return; }
	if (bdsmt>MAX_BDSM_TYPES) { write_user(user,"Try again!  Type: .set caste list to see valid castes!\n"); return; }
	bdsmt--;
	user->bdsm_type=bdsmt;
	sprintf(text,"~FTYour caste is now set to~CM: ~CY%s\n",bdsm_types[user->bdsm_type]);
	write_user(user,text);
	return;
	}

/* Set User's BDSM Lifestyle */
if (!strcasecmp(word[1],"life")) {
	if (!word[2][0] || !isnumber(word[2]) || !strcasecmp(word[2],"list")) {
		write_user(user,"\n~CY~BM ~ULThe Current Lifestyle Settings Are:~RS~BM ~RS\n\n");
		for(tint=0;tint<MAX_BDSM_LIFE_TYPES;tint++) {
			sprintf(text,"~CG%2d~RS~FG... ~FY%-20s ~FW: ~FT%s\n",(tint+1),bdsm_life_types[tint],bdsm_life_type_desc[tint]);
			write_user(user,text);
			}
                write_user(user,"\n~CMUsage: .set life [lifestyle number]\n");
		}
	bdsmt=atoi(word[2]);
	if (!bdsmt) { write_user(user,"Invalid lifestyle!  Type: .set life list to see valid lifestyles!\n\n"); return; }
	if (bdsmt>MAX_BDSM_LIFE_TYPES) { write_user(user,"Try again!  Type: .set role list to see valid lifestyles!\n\n"); return; }
	bdsmt--;
	user->bdsm_life_type=bdsmt;
	sprintf(text,"~FTYour lifestyle is now~CM: ~CY%s\n\n",bdsm_life_types[user->bdsm_life_type]);
	write_user(user,text);
	return;
	}
setmenu(user);
}

/*** Yell something ***/
void yell(UR_OBJECT user,char *inpstr)
{
char *name;

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (user->muzzled & 1) {
	write_user(user,"You are muzzled, you cannot yell.\n");
	return;
	}
if (word_count<2) {
	write_user(user,"Usage:  .yell <message>\n");
	return;
	}
if (ban_swearing && contains_swearing(inpstr)) {
	write_user(user,noswearing);
	return;
	}
if (user->vis) name=user->recap; else name=invisname;
if (user->hidden) name=hiddenname;
if (user->muzzled & FROZEN) {
     sprintf(text,"~CT!! ~FY%s ~CYyells ~FT--> ~FG%s\n",name,inpstr);
	write_user(user,text);
	return;
	}
sprintf(text,"~CT!! ~FY%s ~CYyells ~FT--> ~FG%s\n",name,inpstr);
write_room(NULL,text);
}

/** Show Available Ranks **/

void show_ranks(UR_OBJECT user)
{
sprintf(file,"%s",RANKSFILE);
if (!(show_screen(user))) {
     write_user(user,"~FG>~CG> ~CRRanks Not Available At This Time!\n");
     }
}

/*** Display details of room ***/
void look(UR_OBJECT user)
{
RM_OBJECT rm;
UR_OBJECT u;
FILE *fp;
char filename[80],line[513];
char null[1],*ptr;
char *afk="~FB[~FRA-F-K~FB]";
char *profsub();
int i,exits,users,cnt;

rm=user->room;
sprintf(filename,"%s/%s.R",DATAFILES,rm->name);
if (user->high_ascii) {
	sprintf(filename,"%s/%s.R.%s",DATAFILES,rm->name,fileext[1]);
	if (!file_exists(filename)) sprintf(filename,"%s/%s.R",DATAFILES,rm->name);
	}
if (!(fp=fopen(filename,"r"))) {
     if (rm->access==FIXED_PERSONAL && !strcasecmp(rm->owner,user->name))
        write_user(user,"~CRYou can use ~FB'~FT.edit~FB' ~CRto create a room description...\n");
	}
else {
	fgets(line,512,fp);
	while(!feof(fp)) {
		write_user(user,profsub(line,user));
		line[0]=0;
		fgets(line,512,fp);
		}
	fclose(fp);
	}
if (rm->access & PRIVATE) sprintf(text,"\n~CBYou are in the ~FR%s~CB, in the ~FT%s ~CBarea which is ",rm->recap,rm->map);
else if (rm->access & PERSONAL) sprintf(text,"\n~CBYou are in ~CT%s~CB's room, \"~FT%s~CB\" in the ~FT%s ~CBarea\n~CBThis room is ",rm->owner,rm->recap,rm->map);
else sprintf(text,"\n~CBYou are in the ~FG%s~CB, in the ~FT%s~CB area which is ",rm->recap,rm->map);
switch(rm->access) {
     case PUBLIC        : strcat(text,"~CGPublic");  break;
     case PRIVATE       : strcat(text,"~CRPrivate");  break;
     case PERSONAL      : strcat(text,"~CTPersonal");  break;
     case FIXED_PUBLIC  : strcat(text,"~CRfixed ~CGPublic");  break;
     case FIXED_PRIVATE : strcat(text,"~CRfixed ~CRPrivate");  break;
     case FIXED_PERSONAL: strcat(text,"~CRfixed ~CTPersonal");  break;
     }
strcat(text,"~CB.\n");
write_user(user,text);

if (!rm->mesg_cnt) sprintf(text,no_message_prompt);
else if (rm->mesg_cnt==1) sprintf(text,single_message_prompt);
else sprintf(text,message_prompt,rm->mesg_cnt);
write_user(user,text);
if (rm->topic[0]) {
     sprintf(text,topic_prompt,rm->topic);
	write_user(user,text);
	}
exits=0;  cnt=0; null[0]='\0';
for(i=0;i<MAX_LINKS;++i) {
     if (rm->link[i]==NULL) break;
	if (rm->link[i]->access & PERSONAL && user->level<=WIZ) continue;
	++exits;
	}
if (!exits) write_user(user,no_exits);
else {
     write_user(user,"~CTExits~CW:  ");
     	for(i=0;i<MAX_LINKS;++i) {
     	   if (rm->link[i]==NULL) break;
     	   if (rm->link[i]->access & PERSONAL && user->level<=WIZ) continue;
     	   if (rm->link[i]->access & PRIVATE) sprintf(text,"~CR%-20s  ",rm->link[i]->name);
     	   else if (rm->link[i]->access & PERSONAL) sprintf(text,"~CM%-20s  ",rm->link[i]->name);
     	   else sprintf(text,"~CG%-20s  ",rm->link[i]->name);
     	   ++cnt;
	   if (cnt==3) { strcat(text,"\n        "); cnt=0; }
     	   write_user(user,text);
 	  }
       }
users=0;
for(u=user_first;u!=NULL;u=u->next) {
     if (u->room!=rm || u==user || (!u->vis && u->level>user->level)) continue;
     if (u->hidden && user->level<OWNER) continue;
     if (!users++) write_user(user,people_here_prompt);
     if (u->afk) ptr=afk; else ptr=null;
     if (!u->vis) sprintf(text,"    ~CR! ~RS%s %s~RS  ~BR%s\n",u->recap,u->desc,ptr);
	else sprintf(text,"      ~RS%s %s~RS  ~FR%s\n",u->recap,u->desc,ptr);
	write_user(user,text);
	}
if (!users) write_user(user,no_people_here_prompt);
write_user(user,"\n");
}

/*** Move to another room ***/
void go(UR_OBJECT user, char *inpstr)
{
RM_OBJECT rm;
int i;

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (user->shackled) { write_user(user,"~CRYou are shackled to this room, you cannot leave!\n");  return; }
if (word_count<2) {
	write_user(user,"Where would you like to go today?\n");
	write_user(user,"Try .main to get to the main room.\n");
	return;
	}
if ((rm=get_room(inpstr))==NULL) {
	write_user(user,nosuchroom);
        sprintf(text,room_not_found,inpstr);
	write_user(user,text);
 	return;
	}
if (!strcasecmp(rm->name,"Members_Only") && user->level<MEMBER) {
        sprintf(text,"~CRYou must be a member to enter that room!\n");
	write_user(user,text);
	return;
	}
if (rm==user->room) {
        sprintf(text,already_in_room,rm->recap);
 	write_user(user,text);
	return;
	}
/* See if link from current room */
for(i=0;i<MAX_LINKS;++i) {
	if (user->room->link[i]==rm) {
		if (rm->access & PERSONAL && (!(user->invite_room==rm))) {
			if (strcmp(user->name,rm->owner) && !has_key(rm,user)) {
                            if (user->level<WIZ) sprintf(text,"~CRThe ~CW%s~CR is not adjoined to here.\n",rm->recap);
                            else sprintf(text,"~CRThe ~CW%s~CR room is a ~FTpersonal~CR room and you are not the owner.\n",rm->recap);
				write_user(user,text);
				return;
				}
			move_user(user,rm,0);
			return;
			}
		move_user(user,rm,0);
		return;
		}
	}
if (rm->access & PERSONAL && (user->invite_room==rm || has_key(rm,user))) {
     if (user->level<WIZ) move_user(user,rm,2);
	else move_user(user,rm,1);
	return;
	}
if (user->level<WIZ) {
        sprintf(text,"~CRThe ~FT%s~CR is not adjoined to here.\n",rm->recap);
	write_user(user,text);
	return;
	}
if (rm->access & PERSONAL && user->level<OWNER && !has_key(rm,user)) {
	if (strcmp(user->name,rm->owner)) {
		sprintf(text,"~CRThe ~CT%s~CR room is a ~CMpersonal~CR room and you are not the owner\n~CRNor do you posess a key to enter...\n",rm->recap);
		write_user(user,text);
		return;
		}
	}
move_user(user,rm,1);
}

/*** Called by go() and move() ***/
void move_user(UR_OBJECT user,RM_OBJECT rm,int teleport)
{
RM_OBJECT old_room;

old_room=user->room;
if (teleport!=2 && !has_room_access(user,rm)) {
        write_user(user,"~CRThat room is currently private, you cannot enter.\n");
	return;
	}
/* Reset invite room if in it */
if (user->invite_room==rm) user->invite_room=NULL;
if (user->roombanned && (!strcasecmp(rm->owner,user->name)) && !teleport==-1) {
        sprintf(text,"~CRYou're banned from the %s~CR room, you cannot be moved there!",rm->recap);
	write_user(user,text);
	return;
	}
else {
	teleport=2;
	goto OWNERMOVE;
	}
if (!user->vis || user->hidden) {
	write_room(rm,invisenter);
	write_room_except(user->room,invisleave,user);
	goto SKIP;
	}
if (teleport==1) {
	if (user->vis && !user->hidden) {
                sprintf(text,"\n~CT%s disapears into thin air...\n\n",user->recap);
		write_room_except(old_room,text,user);
                sprintf(text,"~CM%s appears from out of nowhere...\n",user->recap);
		write_room(rm,text);
		}
	else if (user->hidden) {
		write_room_except(old_room,"\nYou lose the feeling that someone was watching you...\n",user);
		write_room(rm,"\nDo you ever get the feeling that someone is watching you?\n");
		}
	goto SKIP;
	}
if (teleport==3) {
	if (user->vis && !user->hidden) {
          sprintf(text,"\n~CM%s %s.~RS\n",user->recap,user->in_phrase);
	  write_room(rm,text);
	  }
	goto SKIP;
	}

OWNERMOVE:

if (teleport==2) {
	sprintf(text,"\n~CM%s %s ~CMto the %s~CM.~RS\n",user->recap,user->out_phrase,rm->recap);
        write_room_except(old_room,text,user);
        write_user(user,move_prompt_user);
        sprintf(text,"\n~CM%s %s.~RS\n",user->recap,user->in_phrase);
	write_room(rm,text);
	goto SKIP;
	}


SKIP:
user->room=rm;
cls(user);
look(user);
reset_access(old_room);
}

/*** Wizard moves a user to another room ***/
void move(UR_OBJECT user)
{
UR_OBJECT u;
RM_OBJECT rm;
char *name;

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
        write_user(user,text);
	return;
	}
if (word_count<2) {
	write_user(user,"Usage: .move <user> [<room>]\n");
	return;
	}
if (!(u=get_user(word[1]))) {
	write_user(user,notloggedon);
	return;
	}
if (user==u) {
        write_user(user,"~CRHey... Why not just 'go' there yourself?\n");
	return;
	}
if (u->shackled && u->shackle_level>user->level) {
        write_user(user,"~CRThat user is shackled and you are not powerful enough to move them.\n");
	return;
	}
if (word_count<3) rm=user->room;
else {
	if ((rm=get_room(word[2]))==NULL) {
		write_user(user,nosuchroom);
		return;
		}
	}
if ((u->level>=user->level) && user->level<OWNER) {
        write_user(user,"~CRTrying to move someone of equal or higher level is a little crazy!\n");
	return;
	}
if (rm==u->room) {
        sprintf(text,"%s~FG is already in the ~RS%s.\n",u->recap,rm->recap);
	write_user(user,text);
	return;
	};
if (!has_room_access(user,rm) && user->level<OWNER) {
        sprintf(text,"~CRThe %s~CR is currently private, ~RS%s~CR cannot be moved there.\n",rm->recap,u->recap);
	write_user(user,text);
	return;
	}
if ((rm->access & PERSONAL) && user->level<OWNER) {
	if (!(!strcmp(user->name,rm->owner)) || user->level<OWNER) {
                sprintf(text,"~CRThe ~RS%s~CR is currently ~FTpersonal, ~RS%s~CR cannot be moved there.\n",rm->recap,u->recap);
		write_user(user,text);
		return;
		}
	}
if (((rm->access & PERSONAL) && (!strcasecmp(u->name,rm->owner)) && user->roombanned) && user->level==OWNER) {
     if (user->vis) name=user->recap; else name=invisname;
     if (user->hidden) name=hiddenname;
     sprintf(text,illegal_move_prompt,name);
	write_room(user->room,text);
        sprintf(text,"%s~CR illegally moved ~RS%s~CR from room ~RS%s~CR to room ~RS%s\n",user->name,u->name,u->room->recap,rm->recap);
	write_syslog(text,1);
	move_user(u,rm,-1);
	return;
	}
if ((rm->access & PERSONAL) && (!strcasecmp(u->name,rm->owner)) && user->roombanned) {
	write_user(user,"That user is banned from that room and cannot be moved there!\n");
	return;
	}
if (user->vis) name=user->recap; else name=invisname;
sprintf(text,user_room_move_prompt,name);
write_room(user->room,text);
sprintf(text,"%s moved %s from room %s to room %s\n",user->name,u->name,u->room->recap,rm->recap);
write_syslog(text,1);
move_user(u,rm,2);
prompt(u);
}

/*** Set rooms to public or private ***/
void set_room_access(UR_OBJECT user)
{
UR_OBJECT u;
RM_OBJECT rm;
char *name;
int cnt;

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
rm=user->room;
if (rm->access & PERSONAL || rm->access==FIXED_PERSONAL) {
	write_user(user,"Access to personal rooms cannot be changed.\n");
	return;
	}
if (word_count<2) rm=user->room;
else {
	if (user->level<gatecrash_level) {
        	write_user(user,"You are not a high enough level to use the room option.\n");
		return;
		}
	if ((rm=get_room(word[1]))==NULL) {
		write_user(user,nosuchroom);
		return;
		}
	}
if (user->vis) name=user->recap; else name=invisname;
if (user->hidden) name="~CR(~FM?~FR)~RS";
if (rm->access>PRIVATE) {
	if (rm==user->room)
		write_user(user,"This room's access is fixed.\n");
	else write_user(user,"That room's access is fixed.\n");
	return;
	}
if (com_num==PUBCOM && rm->access==PUBLIC) {
	if (rm==user->room)
		write_user(user,"This room is already public.\n");
	else write_user(user,"That room is already public.\n");
	return;
	}
if (com_num==PRIVCOM) {
	if (rm->access==PRIVATE) {
		if (rm==user->room)
			write_user(user,"This room is already private.\n");
		else write_user(user,"That room is already private.\n");
		return;
		}
	cnt=0;
	for(u=user_first;u!=NULL;u=u->next) if (u->room==rm) ++cnt;
	if (cnt<min_private_users && user->level<ignore_mp_level) {
		sprintf(text,"You need at least %d users in a room before it can be made private.\n",min_private_users);
		write_user(user,text);
		return;
		}
	write_user(user,"Room set to ~FRPRIVATE.\n");
	if (rm==user->room) {
		sprintf(text,"%s has set the room to ~FRPRIVATE.\n",name);
		write_room(rm,text);
		}
	else write_room(rm,"This room has been set to ~FRPRIVATE.\n");
	rm->access=PRIVATE;
	return;
	}
write_user(user,"Room set to ~FGPUBLIC.\n");
if (rm==user->room) {
	sprintf(text,"%s has set the room to ~FGPUBLIC.\n",name);
	write_room(rm,text);
	}
else write_room(rm,"This room has been set to ~FGPUBLIC.\n");
rm->access=PUBLIC;

/* Reset any invites into the room & clear review buffer */
for(u=user_first;u!=NULL;u=u->next) {
	if (u->invite_room==rm) u->invite_room=NULL;
	}
clear_revbuff(rm);
}

/* Uninvite user(s) from the room */
void uninvite(UR_OBJECT user)
{
UR_OBJECT u;
RM_OBJECT rm;
int cnt;

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (word_count<2) {
     write_user(user,"Usage:  uninvite <user>/all\n");
     return;
     }
rm=user->room;
if ((rm->access & PERSONAL) && user->level<OWNER) {
	if (strcmp(user->name,rm->owner)) {
                sprintf(text,"~CRThe ~RS%s~CR room is a ~FTpersonal~CR room and you are not the owner.\n",rm->recap);
		write_user(user,text);
		return;
		}
	}
if (rm->access==PUBLIC) {
        write_user(user,"~CRRoom is public... You cannot uninvite people from here...\n");
	return;
	}
if (!strcasecmp(word[1],"all")) {
	cnt=0;
        write_user(user,"~CRUn-inviting all users invited into this room...\n");
	for(u=user_first;u!=NULL;u=u->next) {
        	if (u->invite_room==rm) {
                	u->invite_room=NULL;
                        sprintf(text,"~CWUninvite~FT: ~CB%s~CB's invite has been removed...\n",u->recap);
                	write_user(user,text);
                        sprintf(text,"~CT>>>> ~CRYour invite to the ~FT%s~CR room has been taken away...\n",rm->recap);
                	write_user(u,text);
			++cnt;
               		}
		}
        if (!cnt) write_user(user,"~CWUninvite: ~FRThere was nobody to uninvite!\n");
	else {
                sprintf(text,"~CWUninvite:~FG %d users were uninvited from the room.\n",cnt);
		write_user(user,text);
		}
	return;
	}
if (!(u=get_user(word[1]))) {
	write_user(user,notloggedon);
	return;
	}
if (u->invite_room==rm) {
	u->invite_room=NULL;
        sprintf(text,"~CWUninvite: ~RS%s~CR's invite has been removed...\n",u->recap);
	write_user(user,text);
        sprintf(text,"~CT>>>> ~CRYour invite to the ~RS%s~~CR room has been taken away...\n",rm->recap);
	write_user(u,text);
	}
}

/*** Ask to be let into a private or personal room ***/
void letmein(UR_OBJECT user)
{
RM_OBJECT rm;
int i;

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (user->shackled) {
        write_user(user,"~CRYou're currently shackled, so no use knocking!\n");
	return;
	}
if (word_count<2) {
        write_user(user,"~CMKnock on which room's door?\n");
	return;
	}
if ((rm=get_room(word[1]))==NULL) {
	write_user(user,nosuchroom);
	return;
	}
if (rm==user->room) {
	sprintf(text,"~FGYou're already in the ~FT%s~FG room silly!\n",rm->recap);
	write_user(user,text);
	return;
	}
if (user->muzzled & FROZEN) {
	sprintf(text,user_knock_prompt,rm->recap);
	write_user(user,text);
	return;
	}
if (rm->access & PERSONAL) {
	sprintf(text,user_knock_prompt,rm->recap);
	write_user(user,text);
	sprintf(text,room_knock_prompt,user->recap);
	write_room(rm,text);
	return;
	}
for(i=0;i<MAX_LINKS;++i) if (user->room->link[i]==rm) goto GOT_IT;
sprintf(text,"~CRThe ~FM%s~FR is not adjoined to here.\n",rm->recap);
write_user(user,text);
return;

GOT_IT:
if (!(rm->access & PRIVATE)) {
        sprintf(text,"~CGThe ~RS%s~CG is currently public, try .go, or .join a user there!\n",rm->recap);
	write_user(user,text);
	return;
	}
sprintf(text,user_knock_prompt,rm->recap);
write_user(user,text);
sprintf(text,user_room_knock_prompt,user->recap,rm->recap);
write_room_except(user->room,text,user);
sprintf(text,room_knock_prompt,user->recap);
write_room(rm,text);
}

/*** Invite a user into a private room ***/
void invite(UR_OBJECT user)
{
UR_OBJECT u;
RM_OBJECT rm;
char *name;

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (word_count<2) {
        write_user(user,"~CRWho do you wish to invite?\n");
	return;
	}
rm=user->room;
if (!(rm->access & PERSONAL)) {
	if (!(rm->access & PRIVATE)) {
                write_user(user,"~CRThis room is public anyways!~RS\n~CTJust ask them to join you!\n");
		return;
		}
	}
if (!(u=get_user(word[1]))) {
	write_user(user,notloggedon);  return;
	}
if (u==user) {
        write_user(user,"~CRDoes mental insanity run in your family?\n");
	return;
	}
if (u->shackled) {
        write_user(user,"~CRThat user is currently shackled, and cannot leave their current room.\n");
	return;
	}
if (u->room==rm) {
        sprintf(text,"%s~CG is already here!\n",u->recap);
	write_user(user,text);
	return;
	}
if (u->invite_room==rm) {
        sprintf(text,"%s~CR has already been invited into here.\n",u->recap);
	write_user(user,text);
	return;
	}
if (rm->access & PERSONAL) {
	if (strcmp(user->name,rm->owner)) {
                sprintf(text,"~CRThe ~RS%s~CR room is a ~FTpersonal~CR room and you are not the owner.\n",rm->recap);
		write_user(user,text);
		return;
		}
	}
sprintf(text,"~CGYou invite ~RS%s~CG in.\n",u->recap);
write_user(user,text);
if (user->vis) name=user->recap; else name=invisname;
if (!user->hidden) sprintf(text,"%s~CG has invited you into the~RS %s~CG.\n",name,rm->recap);
else sprintf(text,"~CMAn unseen entity has invited you into ~RS%s~CG.\n",rm->recap);
write_user(u,text);
if (user->vis) name=user->recap; else name=invisname;
sprintf(text,"%s~CB has invited ~RS%s ~CBto join you.\n",name,u->recap);
write_room_except(user->room,text,user);
u->invite_room=rm;
}

/*** Set the room topic ***/
void set_topic(UR_OBJECT user,char *inpstr)
{
RM_OBJECT rm;
char *name;

if (user->muzzled & JAILED) {
	write_user(user,"Are you mental?  I don't know that command!\n");
	return;
	}
rm=user->room;
if (word_count<2) {
	if (!strlen(rm->topic)) {
		write_user(user,"No topic has been set yet.\n");  return;
		}
	sprintf(text,"The current topic is: %s\n",rm->topic);
	write_user(user,text);
	return;
	}
if (strlen(inpstr)>TOPIC_LEN) {
	write_user(user,"Topic too long.\n");  return;
	}
if (user->muzzled & FROZEN) {
	sprintf(text,"Topic set to: %s\n",inpstr);
	write_user(user,text);
	return;
	}
sprintf(text,"Topic set to: %s\n",inpstr);
write_user(user,text);
if (user->vis) name=user->recap; else name=invisname;
sprintf(text,"~CG%s ~CGhas set the topic to~CW:~RS %s \n",name,inpstr);
write_room_except(rm,text,user);
strcpy(rm->topic,inpstr);
if (!strcasecmp(user->name,rm->owner)) strncpy(user->roomtopic,inpstr,TOPIC_LEN);
}

/*** Show talker rooms ***/
void rooms(UR_OBJECT user,int show_topics)
{
RM_OBJECT rm;
UR_OBJECT u;
char access[9];
int cnt,rooms,hidden;
rooms=0, hidden=0;

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
sprintf(text,"~CM~BG>> Rooms Available <<");
write_user(user,center(text,78));
write_user(user,"\n\n~FTRoom name                       : Access  Users  Mesgs  Map   Recap Name\n\n");
for(rm=room_first;rm!=NULL;rm=rm->next) {
	if (rm->access & PRIVATE) strcpy(access," ~FRPRIV");
	else if (rm->access & PERSONAL) strcpy(access,"  ~FTPER");
	else if (rm->hidden) strcpy(access," ~FMHIDE");
	else strcpy(access,"  ~FGPUB");
	if (rm->access & FIXED) access[0]='*';
	cnt=0;
	if (rm->hidden && user->level<OWNER) continue;
        if (rm->access==FIXED_PERSONAL && user->level<WIZ) continue;
	rooms++;
	for(u=user_first;u!=NULL;u=u->next)
		if (u->type!=CLONE_TYPE && u->room==rm) ++cnt;
/*      sprintf(text,"%-20s : %9s~RS    %3.3d    %3.3d  %s  %-27.27s\n",rm->name,access,cnt,rm->mesg_cnt,rm->map,colour_com_strip(rm->topic)); */
        sprintf(text,"%-*.*s : %9s~RS    %3.3d    %3.3d  %-6.6s  %s\n",ROOM_NAME_LEN,ROOM_NAME_LEN,rm->name,access,cnt,rm->mesg_cnt,rm->map,rm->recap);
        write_user(user,text);
        }
sprintf(text,"\n~CGThere are ~FT%d~CG rooms currently available.\n",rooms);
write_user(user,text);
}

/*** Moves a user to the lobby (aka the main room) ***/
void lobby(UR_OBJECT user)
{
RM_OBJECT rm;

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
rm=room_first;
if (rm==user->room) {
        sprintf(text,"~CGYou are already in the ~RS%s~CG!\n",rm->recap);
	write_user(user,text);
	return;
	}
move_user(user,rm,0);
}

/*** Join Another User ***/
void join(UR_OBJECT user)
{
RM_OBJECT rm;
UR_OBJECT u;

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (!(u=get_user(word[1]))) {
	write_user(user,notloggedon);
	return;
	}
if (u->hidden && user->level<OWNER) { write_user(user,notloggedon); return; }
if (user->shackled) { write_user(user,"~CRYou are shackled to this room, you cannot leave!\n");  return; }
rm=u->room;
if (rm==user->room) {
        sprintf(text,"~CGYou are already in the ~FT%s!\n",rm->recap);
	write_user(user,text);
	return;
	}
if ((rm->access & PERSONAL) && has_key(rm,user)) {
	write_user(user,"~CGYou place the key in the lock and turn... CLICK!  The door is unlocked.\n~CGYou enter the room.\n");
	}
else if ((rm->access & PERSONAL) && (!(user->invite_room==rm))) {
	sprintf(text,"~CR%s ~CRis in a personal room, and you are not the owner, nor do you have a key!\n",u->name);
	write_user(user,text);
        sprintf(text,user_join_request);
	write_user(user,text);
        sprintf(text,join_request,user->recap);
	write_room(u->room,text);
	return;
	}
if (rm->access & PRIVATE && (!(user->invite_room==rm))) {
        sprintf(text,"~CRThe ~FT%s ~FRis currently private.\n",rm->recap);
	write_user(user,text);
        sprintf(text,user_join_request);
	write_user(user,text);
        sprintf(text,join_request,user->recap);
	write_room(u->room,text);
	return;
  	}
if (!strcasecmp(rm->name,jail_room) && user->level<WIZ) {
        sprintf(text,"~CRThat room is the JAIL room ~CM%s ~CRcould arrest you!\n",u->recap);
	write_user(user,text);
        sprintf(text,join_request,user->recap);
	write_room(u->room,text);
	return;
	}
move_user(user,rm,3);
}

/***************************** IGNORING COMMANDS **************************/

/*** Switch ignoring all on and off ***/
void toggle_ignall(UR_OBJECT user)
{

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (!user->ignall) {
        write_user(user,"~CGYou are now ignoring everyone.\n");
        sprintf(text,"~CR%s is now ignoring everyone.\n",user->recap);
	write_room_except(user->room,text,user);
	user->ignall=1;
	return;
	}
write_user(user,"~CGYou will now hear everyone again.\n");
sprintf(text,"~CG%s is listening again.\n",user->recap);
write_room_except(user->room,text,user);
user->ignall=0;
user->ignore=0;
}

void listening(UR_OBJECT user)
{
UR_OBJECT u;

if (word_count<2) {
	if (user->level>=WIZ) write_user(user,"Usage: listen <all/tells/other/shout/log/beep/bcast/most/wiz/atmos> or <user name>\n");
	else write_user(user,"Usage: listen <all/tells/other/shout/log/beep/bcast/most/atmos> or <user name>\n");
	return;
	}
if (strstr(word[1],"all")) {
	if (user->ignore || user->ignall) {
                write_user(user,"~CGYou are now listening to everything.\n");
		}
	if (user->ignall) {
		sprintf(text,"%s is listening again.\n",user->recap);
		write_room_except(user->room,text,user);
		}
	user->ignore=0;
	user->ignall=0;
	strcpy(user->ignuser,"NONE");
	return;
	}
if (strstr(word[1],"tells") && (user->ignore & ALL_TELLS)) {
        write_user(user,"~CGYou are now listening to all tells and private emotes.\n");
	user->ignore-=ALL_TELLS;
	return;
	}
if (strstr(word[1],"tells")) {
        write_user(user,"~CRYou were already listening to all tells and private emotes.\n");
	return;
	}
if (strstr(word[1],"other") && (user->ignore & OUT_ROOM_TELLS)) {
        write_user(user,"~CRYou are listening to tells and private emotes from other rooms.\n");
	user->ignore-=OUT_ROOM_TELLS;
	return;
	}
if (strstr(word[1],"other")) {
        write_user(user,"~CGYou were already listening to tells and private emotes from other rooms.\n");
	return;
	}
if (strstr(word[1],"shout") && (user->ignore & SHOUT_MSGS)) {
        write_user(user,"~CGYou are listening to shouts.\n");
	user->ignore-=SHOUT_MSGS;
	return;
	}
if (strstr(word[1],"shout")) {
        write_user(user,"~CRYou were already listening to shouts.\n");
	return;
	}
if (strstr(word[1],"log") && (user->ignore & LOGON_MSGS)) {
        write_user(user,"~CGYou are now listening to login mesages.\n");
	user->ignore-=LOGON_MSGS;
	return;
	}
if (strstr(word[1],"log")) {
        write_user(user,"~CRYou were already listening to login mesages.\n");
	return;
	}
if (strstr(word[1],"beep") && (user->ignore & BEEP_MSGS)) {
        write_user(user,"~CGYou are listening to beep mesages.\n");
	user->ignore-=BEEP_MSGS;
	return;
	}
if (strstr(word[1],"beep")) {
        write_user(user,"~CRYou were already listening to beep mesages.\n");
	return;
	}
if (strstr(word[1],"bcast") && (user->ignore & BCAST_MSGS)) {
        write_user(user,"~CGYou are listening to broadcast mesages.\n");
	user->ignore-=BCAST_MSGS;
	return;
	}
if (strstr(word[1],"bcast")) {
        write_user(user,"~CRYou were already listening to brodcast mesages.\n");
	return;
	}
if (strstr(word[1],"picture") && (user->ignore & ROOM_PICTURE)) {
        write_user(user,"~CGYou are listening to pictures.\n");
	user->ignore-=ROOM_PICTURE;
	return;
	}
if (strstr(word[1],"picture")) {
        write_user(user,"~CRYou were already listening to pictures.\n");
	return;
	}
if (strstr(word[1],"most") && (user->ignore==MOST_MSGS)) {
        write_user(user,"~CGYou are ignoring almost everything now.\n");
	user->ignore=ZERO;
	return;
	}
if (strstr(word[1],"most")) {
        write_user(user,"~CRYou were already listening to all most everything except tells and private emotes from your room.\n");
	return;
	}
if (strstr(word[1],"at") && (user->ignore & ATMOS_MSGS)) {
        write_user(user,"~CGYou are now listening to atmospheres.\n");
	user->ignore-=ATMOS_MSGS;
	return;
	}
if (strstr(word[1],"at")) {
        write_user(user,"~CRYou were already listening to atmospheres mesages.\n");
	return;
	}
if (strstr(word[1],"wiz") && (user->ignore & WIZARD_MSGS)) {
        write_user(user,"~CGYou are listening to wiztells and wemotes.\n");
	user->ignore-=WIZARD_MSGS;
	return;
	}
if (strstr(word[1],"wiz")) {
        write_user(user,"~CGYou were already listening to wiztells and wemotes.\n");
	return;
	}
if (!(u=get_user(word[1]))) {
	write_user(user,notloggedon);
        if (user->level<WIZ) write_user(user,"Usage: listen <all/tells/other/shout/log/beep/bcast/most/atmos> or <user name>\n");
        if (user->level>=WIZ) write_user(user,"Usage: listen <all/tells/other/shout/log/beep/bcast/most/wiz/atmos> or <user name>\n");
	return;
	}
if (u==user) {
	write_user(user,"Trying to listen to yourself is a sign of madness.\n");
	return;
	}
sprintf(text,"~CGYou are now listening tells & pemotes from: ~RS%s\n",u->recap);
write_user(user,text);
strcpy(user->ignuser,"NONE");
}

void ignoreing(UR_OBJECT user)
{
UR_OBJECT u;
char igntell[4],ignother[4],ignshouts[4],ignlog[4],ignbeep[4],ignbcast[4],ignroom[4],ignwiz[4];
char ignatmos[4];

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (user->ignore & ALL_TELLS)  strcpy(igntell,"YES");
else strcpy(igntell,"NO ");
if (user->ignore & OUT_ROOM_TELLS)  strcpy(ignother,"YES");
else strcpy(ignother,"NO ");
if (user->ignore & SHOUT_MSGS)  strcpy(ignshouts,"YES");
else strcpy(ignshouts,"NO ");
if (user->ignore & LOGON_MSGS)  strcpy(ignlog,"YES");
else strcpy(ignlog,"NO ");
if (user->ignore & BEEP_MSGS)  strcpy(ignbeep,"YES");
else strcpy(ignbeep,"NO ");
if (user->ignore & BCAST_MSGS)  strcpy(ignbcast,"YES");
else strcpy(ignbcast,"NO ");
if (user->ignore & ROOM_PICTURE)  strcpy(ignroom,"YES");
else strcpy(ignroom,"NO ");
if (user->ignore & WIZARD_MSGS)  strcpy(ignwiz,"YES");
else strcpy(ignwiz,"NO ");
if (user->ignore & ATMOS_MSGS)  strcpy(ignatmos,"YES");
else strcpy(ignatmos,"NO ");
if (word_count<2 && user->level<WIZ) {
	write_user(user,"Usage: ignore <tells/other/shout/log/beep/bcast/most> or <user name>\n\n");
	sprintf(text,"Igngoring All tells             : %s \n",igntell);
	write_user(user,text);
	sprintf(text,"Ignoring Tells from other rooms : %s \n",ignother);
	write_user(user,text);
	sprintf(text,"Ignoring Shouts                 : %s \n",ignshouts);
	write_user(user,text);
	sprintf(text,"Ignoring Logon mesages          : %s \n",ignlog);
	write_user(user,text);
	sprintf(text,"Ignoring Beep mesages           : %s \n",ignbeep);
	write_user(user,text);
	sprintf(text,"Ignoring Broadcast mesages      : %s \n",ignbcast);
	write_user(user,text);
	sprintf(text,"Ignoring Atmospheres            : %s \n",ignatmos);
	write_user(user,text);
	sprintf(text,"Ignoring user                   : %s \n",user->ignuser);
	write_user(user,text);
	return;
	}
if (word_count<2 && user->level>=WIZ) {
	write_user(user,"Usage: ignore <tells/other/shout/log/beep/bcast/most/wiz> or <user name>\n\n");
	sprintf(text,"Ignoring All tells              : %s \n",igntell);
	write_user(user,text);
	sprintf(text,"Ignoring Tells from other rooms : %s \n",ignother);
	write_user(user,text);
	sprintf(text,"Ignoring Shouts                 : %s \n",ignshouts);
	write_user(user,text);
	sprintf(text,"Ignoring Logon mesages          : %s \n",ignlog);
	write_user(user,text);
	sprintf(text,"Ignoring Beep mesages           : %s \n",ignbeep);
	write_user(user,text);
	sprintf(text,"Ignoring Broadcast mesages      : %s \n",ignbcast);
	write_user(user,text);
	sprintf(text,"Ignoring Atmospheres            : %s \n",ignatmos);
	write_user(user,text);
	sprintf(text,"Ignoring Wizard mesages         : %s \n",ignwiz);
	write_user(user,text);
	sprintf(text,"Ignoring user                   : %s \n",user->ignuser);
	write_user(user,text);
	return;
	}
if (strstr(word[1],"tells") && (user->ignore & ALL_TELLS)) {
        write_user(user,"~CRYou are already ignoring all tells and private emotes.\n");
	return;
	}
if (strstr(word[1],"tells")) {
        write_user(user,"~CGYou are now ignoring all tells and private emotes.\n");
	user->ignore+=ALL_TELLS;
	return;
	}
if (strstr(word[1],"other") && (user->ignore & OUT_ROOM_TELLS)) {
        write_user(user,"~CRYou are already ignoring tells and private emotes from other rooms.\n");
	return;
	}
if (strstr(word[1],"other")) {
        write_user(user,"~CGYou are ignoring tells and private emotes from other rooms.\n");
	user->ignore+=OUT_ROOM_TELLS;
	return;
	}
if (strstr(word[1],"shout") && (user->ignore & SHOUT_MSGS)) {
        write_user(user,"~CRYou are already ignoring shouts.\n");
	return;
	}
if (strstr(word[1],"shout")) {
        write_user(user,"~CGYou are ignoing shouts.\n");
	user->ignore+=SHOUT_MSGS;
	return;
	}
if (strstr(word[1],"log") && (user->ignore & LOGON_MSGS)) {
        write_user(user,"~CRYou are already ignoring login mesages.\n");
	return;
	}
if (strstr(word[1],"log")) {
        write_user(user,"~CGYou are ignoring login mesages.\n");
	user->ignore+=LOGON_MSGS;
	return;
	}
if (strstr(word[1],"beep") && (user->ignore & BEEP_MSGS)) {
        write_user(user,"~CRYou are already ignoring beep mesages.\n");
	return;
	}
if (strstr(word[1],"beep")) {
        write_user(user,"~CGYou are ignoring beep mesages.\n");
	user->ignore+=BEEP_MSGS;
	return;
	}
if (strstr(word[1],"bcast") && (user->ignore & BCAST_MSGS)) {
        write_user(user,"~CRYou are already ignoring broadcast mesages.\n");
	return;
	}
if (strstr(word[1],"bcast")) {
        write_user(user,"~CGYou are ignoring brodcast mesages.\n");
	user->ignore+=BCAST_MSGS;
	return;
	}
if (strstr(word[1],"picture") && (user->ignore & ROOM_PICTURE)) {
        write_user(user,"~CRYou are already ignoring pictures.\n");
	return;
	}
if (strstr(word[1],"picture")) {
        write_user(user,"~CGYou are ignoring pictures.\n");
	user->ignore+=ROOM_PICTURE;
	return;
	}
if (strstr(word[1],"most") && (user->ignore==MOST_MSGS)) {
        write_user(user,"~CRYou are ignoring everything except tells and private emotes from your room.\n");
	return;
	}
if (strstr(word[1],"most")) {
        write_user(user,"~CGYou are ignoring everything except tells and private emotes from your room.\n");
	user->ignore=MOST_MSGS;
	return;
	}
if (strstr(word[1],"wiz") && (user->ignore & WIZARD_MSGS)) {
        write_user(user,"~CRYou are already wiztells and wemotes.\n");
	return;
	}
if (strstr(word[1],"wiz")) {
        write_user(user,"~CGYou are ignoring wiztells and wemotes.\n");
	user->ignore+=WIZARD_MSGS;
	return;
	}
if (strstr(word[1],"at") && (user->ignore & ATMOS_MSGS)) {
        write_user(user,"~CRYou are already ignoring atmospheres.\n");
	return;
	}
if (strstr(word[1],"at")) {
        write_user(user,"~CGYou are ignoring atmospheres.\n");
	user->ignore+=ATMOS_MSGS;
	return;
	}
if (!(u=get_user(word[1]))) {
	write_user(user,notloggedon);
        if (user->level<WIZ) write_user(user,"Usage: ignore <tells/other/shout/log/beep/bcast/most/atmos> or <user name>\n");
        if (user->level>=WIZ) write_user(user,"Usage: ignore <tells/other/shout/log/beep/bcast/most/wiz/atmos> or <user name>\n");
	return;
	}
if (u==user) {
	write_user(user,"Trying to ignore yourself up is a sign of madness.\n");
	return;
	}
sprintf(text,"~CGYou are now ignoring tells & pemotes from: ~RS%s\n",u->recap);
write_user(user,text);
strcpy(user->ignuser,u->name);
}

/************************** MISCELLANIOUS COMMANDS ************************/

/*** Switch between command (Ew-too) and speech (nuts) mode ***/
void toggle_mode(UR_OBJECT user)
{
if (user->command_mode) {
     write_user(user,"~CYYou are now in ~FGSpeech~FY mode.\n~CYUse ~FM.mode~FY to switch to ~FREw-Too~FM mode.\n");
	user->command_mode=0;
     return;
	}
write_user(user,"~CTYou are now in ~FREw-Too ~FTmode.\n~CTUse ~FMmode~FT to switch back!\n");
user->command_mode=1;
}


/*** Switch prompt on and off ***/
void toggle_prompt(UR_OBJECT user)
{

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (user->muzzled & SCUM) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (user->prompt & 1) {
     write_user(user,"~CYTime Prompt Is Now Off.  Type .prompt to toggle prompt on/off.\n");
	user->prompt--;  return;
	}
write_user(user,"~CYTime Prompt Is Now On.  Type .prompt to toggle prompt on/off\n");
user->prompt++;
}


/*** Toggle user COLOR on and off ***/
void toggle_colour(UR_OBJECT user)
{

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (user->muzzled & SCUM) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (user->colour) {
     write_user(user,"~RSAnsi Color has been turned off!.\n");
	user->colour=0;
	}
else {
	user->colour=1;
     write_user(user,"~CGAnsi Color has been turned on!\n");
	}
}

void print_useage(UR_OBJECT user,int what_type)
{
char name[8];

if (what_type) strcpy(name,"wipe");
else strcpy(name,"dmail");
sprintf(text,"Usage: %s all/top <number of messages>/bottom <number of messages>\nExample: .%s all\nExample: .%s top 6 (deletes top 6 messages).\nExample: .%s bottom 2  (deletes two from the bottom of the list).\n",name,name,name,name);
write_user(user,text);
sprintf(text,"Usage: %s <Number of message to delete> (to delete only one message).\nExample: .%s 4  (will delete the 4th message).\n",name,name);
write_user(user,text);
sprintf(text,"Usage: %s <Number of message to delete> - <Number of message to delete>\nExample: .%s 3 - 5  (will delete a Range specified by the two numbers)\n",name,name);
write_user(user,text);
sprintf(text,"Usage: %s <Number of message to delete> <Number of message to delete>\nUp to 5 numbers can be used and do not have to be in order.\nExample: .%s 1 4 5 9 7 or .%s 2 6 3 ect.\n",name,name,name);
write_user(user,text);
return;
}

/*** Shows a file about the staff and what staff members are loged on ***/
void staff_on(UR_OBJECT user)
{
char *colour_com_strip();
int cnt;
UR_OBJECT u;

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
cnt=0;
if (user->high_ascii) write_user(user,"~FTƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
else write_user(user,"~FT------------------------------------------------------------------------------\n");
for(u=user_first;u!=NULL;u=u->next) {
	if (u->type==CLONE_TYPE) continue;
        if (!u->vis || u->hidden) continue;
        if (u->level<WIZ) continue;
	if (++cnt==1) {
                sprintf(text,"\n~FM-~CM=~CR[ ~RSStaff Present at %s %s ~CR]~CM=~FM-\n\n",TALKERNAME,long_date(1));
 	 	write_user(user,text);
	}
        sprintf(text," ~CMName: ~FT%-20.20s ~FMRank: ~FT%-20.20s ~FMLevel: %s\n",colour_com_strip(u->recap),u->level_alias,level_name[u->level]);
	write_user(user,text);
	}
if (!cnt) {
        write_user(user,"There are no staff members currently logged on.\n");
        }
else    {
	if (user->high_ascii) write_user(user,"~FRƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
	else write_user(user,"~FR------------------------------------------------------------------------------\n");
        sprintf(text,"~CGThere are ~CM%d~CG staff members online!\n",cnt);
	write_user(user,text);
	}
if (user->high_ascii) write_user(user,"~FTƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
else write_user(user,"~FT------------------------------------------------------------------------------\n");
}

/*** Do AFK ***/
void afk(UR_OBJECT user,char *inpstr)
{

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (word_count>1) {
	if (!strcmp(word[1],"lock")) {
		inpstr=remove_first(inpstr);
		if (strlen(inpstr)>AFK_MESG_LEN) {
			write_user(user,"AFK message too long.\n");
               		return;
			}
			if (user->muzzled & FROZEN) {
                                write_user(user,"~CWYou are now AFK with the session locked, enter your password to unlock it.\n");
				if (inpstr[0]) {
					strcpy(user->afk_mesg,inpstr);
					}
				user->afk=2;
				return;
				}
                write_user(user,"~CWYou are now AFK with the session locked, enter your password to unlock it.\n");
		if (inpstr[0]) {
			strcpy(user->afk_mesg,inpstr);
			}
		user->afk=2;
		}
	else {
		if (strlen(inpstr)>AFK_MESG_LEN) {
			write_user(user,"AFK message too long.\n");  return;
			}
		if (user->muzzled & FROZEN) {
                if (user->afk_mesg[0])
                   sprintf(text,"~CW-~RS %s %s~RS ~FW- ~CB(~CR-A-F-K-~CB)\n",user->recap,user->afk_mesg);
                   else {
			sprintf(afktext,afk_msg[rand()%MAX_AFK],user->recap);
			sprintf(text,AFKSTRING,afktext);
			}
             	write_user(user,text);
             	if (inpstr[0]) {
                 strcpy(user->afk_mesg,inpstr);
                 }
			user->afk=1;
			return;
			}
		if (inpstr[0]) {
			strcpy(user->afk_mesg,inpstr);
			}
		user->afk=1;
		}
	}
else {
	user->afk=1;
	}
if (user->vis && !user->hidden) {
	if (user->afk_mesg[0])
                sprintf(text,"~CW-~RS %s %s~RS ~FW- (AFK)\n",user->recap,user->afk_mesg);
                else {
			sprintf(afktext,afk_msg[rand()%MAX_AFK],user->recap);
			sprintf(text,AFKSTRING,afktext);
			}
	write_room(user->room,text);
	sprintf(text,"%s went AFK\n",user->recap);
	write_syslog(text,1);
	}
	else { write_user(user,"~FGYou are now AFK.\n"); }
}


/*** A newbie is requesting an account. Get his email address off him so we
     can validate who he is before we promote him and let him loose as a
     proper user. ***/
void account_request(UR_OBJECT user,char *inpstr)
{

char filename[256];

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (user->muzzled & SCUM) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (user->level>NEW) {
	write_user(user,"This command is for new users only, you already have a full account.\n");
	return;
	}
/* This is so some pillock doesnt keep doing it just to fill up the syslog */
if (user->accreq) {
	write_user(user,"You have already requested an account.\n");
	return;
	}
if (word_count<2) {
	write_user(user,"Usage: accreq <a VALID email address>>\n");
	return;
	}
/* Could check validity of email address I guess but its a waste of time.
   If they give a duff address they don't get an account, simple. ***/
if (valid_email(user,inpstr)) {
	sprintf(text,"ACCOUNT REQUEST from %s: %s.\n",user->name,inpstr);
	write_syslog(text,1);
        sprintf(text,"~CMAccount Request from ~FT%s: %s\n",user->name,inpstr);
	write_level(WIZ,1,text,NULL);
        write_user(user,"~CTIf the email address you gave is valid, you should receive\n");
        write_user(user,"~CTan email from us shortly welcoming you to our talker!\n");
	user->accreq=1;
	sprintf(filename,"%s/%s",DATAFILES,WELCOMEMAIL);
	send_email(filename,inpstr);
	autopromote(user);
	}
}

/*** Clear the review buffer ***/
void revclr(UR_OBJECT user)
{
RM_OBJECT rm;
char *name;

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (word_count<2 || !ALLOW_EXTERNAL_CBUFF || user->level<WIZ) rm=user->room;
else {
	if ((rm=get_room(word[1]))==NULL) {
		write_user(user,nosuchroom);  return;
		}
	}
if (rm->access==FIXED_PUBLIC && user->level<WIZ && !ALLOW_PUBLIC_CBUFF) {
        write_user(user,"~CRYou are not powerfull enough to clear the review buffer here.\n");
	return;
	}
if (user->level>=ARCH) {
	clear_revbuff(rm);
	sprintf(text,out_of_room_cbuff,rm->recap);
	write_user(user,text);
	return;
	}
clear_revbuff(user->room);
if (user->hidden) return;
if (user->vis) name=user->recap; else name=invisname;
sprintf(text,cbuff_prompt,name);
write_room(user->room,text);
}

/*** Clear the review buffer ***/
void wizrevclr(UR_OBJECT user)
{

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
clear_wizrevbuff();
write_user(user,wiz_cbuff_prompt);
}

/*** Clear the review buffer ***/
void shoutrevclr(UR_OBJECT user)
{
if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
clear_shoutrevbuff();
write_user(user,shout_cbuff_prompt);
}

/*** See review of conversation ***/
void review(UR_OBJECT user)
{
RM_OBJECT rm=user->room;
int i,line,cnt,grepmode=0,matches=0;
char *center();
char temp[(ARR_SIZE*2)+1];

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (user->muzzled & FROZEN) {
        write_user(user,no_review_prompt);
	return;
	}
rm=user->room;
if (!strcasecmp(word[1],"grep") && word_count>1) { grepmode=1; goto GREPSKIP; }

   if (ALLOW_EXTERNAL_REVIEW && user->level>=WIZ && word_count>1) {
	if ((rm=get_room(word[1]))==NULL) {
		write_user(user,nosuchroom);
		return;
		}
	if (!has_room_access(user,rm)) {
          write_user(user,private_review_prompt);
		return;
		}
     if (rm->access & PERSONAL && rm!=user->room && user->level<OWNER) {
                write_user(user,personal_review_prompt);
		return;
		}
	}

GREPSKIP:

cnt=0;
temp[0]='\0';
for(i=0;i<REVIEW_LINES;++i) {
	line=(rm->revline+i)%REVIEW_LINES;
	if (rm->revbuff[line][0]) {
		cnt++;
		if (cnt==1) {
		write_user(user,"\n");
                sprintf(text,review_header,rm->recap);
                write_user(user,center(text,78));
                if (user->high_ascii) write_user(user,"\n~CMƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
                else write_user(user,"~CM------------------------------------------------------------------------------\n");
	        }
	if (grepmode) {
		strncpy(temp,rm->revbuff[line],ARR_SIZE*2);
		strtolower(temp);
		strtolower(word[2]);
		if (strstr(colour_com_strip(temp),word[2])) { write_user(user,rm->revbuff[line]); matches++; }
		}
	else write_user(user,rm->revbuff[line]);
	  }
	}
if (!cnt) {
     sprintf(text,no_review_prompt);
	write_user(user,center(text,78));
	}
else {
     if (user->high_ascii) write_user(user,"~CMƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
     else write_user(user,"~CM------------------------------------------------------------------------------\n");
     if (grepmode) {
             sprintf(text,"~CYTotal Lines That Contains ~FT\"%s\"~FB:~FG %d ~FYLines\n",word[2],matches);
	     write_user(user,center(text,79));
	     }
     }
}

/*** Show recorded tells and pemotes ***/
void revtell(UR_OBJECT user)
{
int i,cnt,line;
char *center();

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
cnt=0;
for(i=0;i<REVTELL_LINES;++i) {
	line=(user->revline+i)%REVTELL_LINES;
	if (user->revbuff[line][0]) {
		cnt++;
                if (cnt==1) {
		    write_user(user,"\n");
                    sprintf(text,tell_review_header);
		    write_user(user,center(text,78));
                    if (user->high_ascii) write_user(user,"\n~CMƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
                    else write_user(user,"\n~CM------------------------------------------------------------------------------\n");
                    }
	  	write_user(user,user->revbuff[line]);
	  	}
	}
if (!cnt) write_user(user,no_tell_review_prompt);
else {
     if (user->high_ascii) write_user(user,"~CMƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
     else write_user(user,"~CM------------------------------------------------------------------------------\n");
     }
}

/*** Show recorded wiztells and wemotes ***/
void revwiztell(UR_OBJECT user)
{
int i,cnt,line;
char *center();

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
cnt=0;
for(i=0;i<REVIEW_LINES;++i) {
	line=(wrevline+i)%REVIEW_LINES;
	if (wizrevbuff[line][0]) {
          cnt++;
          if (cnt==1) {
		write_user(user,"\n");
          sprintf(text,wiz_review_header);
		write_user(user,center(text,78));
                if (user->high_ascii) write_user(user,"\n~CMƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
                else write_user(user,"\n~CM------------------------------------------------------------------------------\n");
                }
           write_user(user,wizrevbuff[line]);
	   }
	}
if (!cnt) write_user(user,no_wiz_review_prompt);
else {
     if (user->high_ascii) write_user(user,"~CMƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
     else write_user(user,"~CM------------------------------------------------------------------------------\n");
     }
}

/*** Show recorded shouts and shout emotes ***/
void revshout(UR_OBJECT user)
{
int i,cnt,line;
char *center();

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
cnt=0;
for(i=0;i<REVIEW_LINES;++i) {
	line=(srevline+i)%REVIEW_LINES;
	if (shoutrevbuff[line][0]) {
		cnt++;
          if (cnt==1) {
		write_user(user,"\n");
          sprintf(text,shout_review_header);
		write_user(user,center(text,78));
                if (user->high_ascii) write_user(user,"\n~CMƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
                else write_user(user,"\n~CM------------------------------------------------------------------------------\n");
               }
          write_user(user,shoutrevbuff[line]);
          }
	}
if (!cnt) {
     write_user(user,no_shout_review_prompt);
	return;
	}
else {
     if (user->high_ascii) write_user(user,"~CMƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
     else write_user(user,"~CM------------------------------------------------------------------------------\n");
     }
}

void home(UR_OBJECT user)
{
write_user(user,"Due to the fact that the netlink has been removed in Moenuts v1.55+...\n");
write_user(user,"You are already on your home system! ;-)\n");
}

/*** Set the character mode echo on or off. This is only for users logging in
     via a character mode client, those using a line mode client (eg unix
     telnet) will see no effect. ***/
void toggle_charecho(UR_OBJECT user)
{
if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (user->muzzled & SCUM) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (!user->charmode_echo) {
     write_user(user,"Echoing for character mode clients has been turned ~FGon.\n");
	user->charmode_echo=1;
	}
else {
     write_user(user,"Echoing for character mode clients has been turned ~FRoff.\n");
	user->charmode_echo=0;
	}
}

/*** Set user description ***/
void set_desc(UR_OBJECT user,char *inpstr)
{

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (user->muzzled & SCUM) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (word_count<2) {
        sprintf(text,"~CTYou are known as~FW: ~RS%s %s\n",user->recap,user->desc);
	write_user(user,text);
	return;
	}
if (strstr(word[1],"(CLONE)")) {
	write_user(user,"You cannot have that description.\n");
	return;
	}
if (strlen(inpstr)>USER_DESC_LEN-1) write_user(user,"~FRYour description is too long, it has been truncated to fit.\n~RS");
strncpy(user->desc,inpstr,USER_DESC_LEN-1);
sprintf(text,"~CTYou will be known as~FW: ~RS%s %s\n",user->recap,user->desc);
write_user(user,text);
autopromote(user);
}

/*** Enter user profile ***/
void enter_profile(UR_OBJECT user,int done_editing)
{
FILE *fp;
char *c,filename[82],*center();

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (!done_editing) {
     sprintf(text,profile_start,user->recap);
	write_room_except(user->room,text,user);
	write_user(user,"\n");
        sprintf(text,profile_edit_header);
        write_user(user,center(text,78));
     write_user(user,"\n\n");
	user->misc_op=5;
	editor(user,NULL);
	return;
	}
sprintf(filename,"%s/%s.P",USERFILES,user->name);
if (!(fp=fopen(filename,"w"))) {
	sprintf("ERROR: Couldn't open file %s to write in enter_profile().\n",filename);
	write_user(user,text);
	sprintf("ERROR: Couldn't open file %s to write in enter_profile().\n",filename);
	write_syslog(text,0);
	return;
	}
c=user->malloc_start;
while(c!=user->malloc_end) putc(*c++,fp);
fclose(fp);
write_user(user,"~CGYour Profile Has Been Stored.\n");
sprintf(text,profile_end,user->recap);
write_room(user->room,text);
autopromote(user);
}

void gender(UR_OBJECT user)
{
UR_OBJECT u;

if (word_count<2) {
        write_user(user,"Usage:  .gender <user> <m/f/n>\n");
        return;
        }
if (!(u=get_user(word[1]))) {
        write_user(user,notloggedon);
        return;
        }
if ((user->level<=u->level) && u!=user && user->level<OWNER) {
        write_user(user,"You cannot change the gender of someone the same or higher level than you!\n");
        sprintf(text,"%s tried to change %s's gender.\n",user->name,u->name);
        write_syslog(text,1);
        return;
        }
if (!strncasecmp(word[2],"m",1)) {
     if (u->prompt & MALE) { write_user(user,"User's Gender Is Already Set To Male!\n"); return; }
     if (u->prompt & FEMALE) u->prompt-=FEMALE;
     u->prompt+=MALE;
     sprintf(text,"~CW-> ~FM%s~RS~CM's gender has been set to Male.\n",u->recap);
     write_user(user,text);
     sprintf(text,"~CW-> ~FM%s~RS~CM has set your gender to Male.\n",user->recap);
     write_user(u,text);
     return;
     }
if (!strncasecmp(word[2],"f",1)) {
     if (u->prompt & FEMALE) { write_user(user,"User's Gender Is Already Set To Female!\n"); return; }
     if (u->prompt & MALE) u->prompt-=MALE;
     u->prompt+=FEMALE;
     sprintf(text,"~CW-> ~FM%s~RS~CM's gender has been set to Female.\n",u->recap);
     write_user(user,text);
     sprintf(text,"~CW-> ~FM%s~RS~CM has set your gender to Female.\n",user->recap);
     write_user(u,text);
     return;
     }
if (!strncasecmp(word[2],"n",1)) {
     if (u->prompt & MALE) u->prompt-=MALE;
     else if (u->prompt & FEMALE) u->prompt-=FEMALE;
     u->prompt+=NEUTER;
     sprintf(text,"~CW-> ~FM%s~RS~CM's gender has been set to Neuter.\n",u->recap);
     write_user(user,text);
     sprintf(text,"~CW-> ~FM%s~RS~CM has set your gender to Neuter.\n",user->recap);
     write_user(u,text);
     return;
     }
     write_user(user,"Usage:  .gender <user> <m/f/n>\n");
}

/*** Shows user the map for the area he or she is in ***/
void map(UR_OBJECT user)
{

RM_OBJECT rm;
char filename[80];

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
rm=user->room;
if (!strcasecmp(rm->map,"users")) { drawusermap(user); return; }
sprintf(filename,"%s/map-%s.%s",SCREENFILES,rm->map,fileext[user->high_ascii]);
switch(more(user,user->socket,filename)) {
	case 0:	write_user(user,"Sorry, There is no map for this area.\n");
		return;
	case 1: user->misc_op=2;
	}
}

void drawusermap(UR_OBJECT user)
{
RM_OBJECT rm;
int total,per,pub,priv,cnt;
total=0;  per=0; pub=0;  priv=0;  cnt=0;

cls(user);
if (user->high_ascii) write_user(user,"\n~FTÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕ~FM[ ~CGCurrent User Rooms ~RS~FM]~FTÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕ\n\n");
else write_user(user,"\n~FT---------------------------~FM[ ~CGCurrent User Rooms ~RS~FM]~FT-----------------------------\n\n");
if (user->high_ascii) sprintf(text,"~FRƒƒƒ~FM[ ~CG%-20s ~RS~FM]~FRƒƒƒƒ∑\n",room_first->name);
else sprintf(text,"~FR---~FM[ ~CG%-20s ~RS~FM]~FR----+\n",room_first->name);
write_user(user,text);
for(rm=room_first;rm!=NULL;rm=rm->next) {
	if (!strcasecmp(rm->map,"users")) total++;
	}
for(rm=room_first;rm!=NULL;rm=rm->next) {
	if (strcasecmp(rm->map,"users")) continue; /* Not on the 'users' map */
	cnt++;
	if (rm->access==PUBLIC || rm->access==FIXED_PUBLIC) pub++;
	if (rm->access==PRIVATE || rm->access==FIXED_PRIVATE) priv++;
	if (rm->access==PERSONAL || rm->access==FIXED_PERSONAL) per++;
     if (user->high_ascii) write_user(user,"                               ~FR∫\n");
     else write_user(user,"                               ~FR|\n");
     if (cnt==total) {
          if (user->high_ascii) sprintf(text,"                               ~FR”~FBƒƒƒ~FTØ ~CR%s\n",rm->recap);
          else sprintf(text,"                               ~FR+~FB---~FT=~FG]~FM> ~CR%s\n",rm->recap);
          }
     else {
          if (user->high_ascii) sprintf(text,"                               ~FR«~FBƒƒƒ~FTØ ~CR%s\n",rm->recap);
          else sprintf(text,"                               ~FR|~FB---~FT=~FG]~FM> ~CR%s\n",rm->recap);
          }
	write_user(user,text);
	}
sprintf(text,"\n~CGPublic Rooms~FT: ~FM%-3d  ~FGPrivate Rooms~FW: ~FM%-3d  ~FGPersonal Rooms~FW: ~FM%-3d  ~FGTotal~FW: ~FM%d\n",pub,priv,per,cnt);
write_user(user,text);
}

/*** Change room description ***/
void room_desc(UR_OBJECT user,int done_editing)
{
RM_OBJECT rm;
FILE *fp;
char *c,filename[80];

rm=user->room;
if (!done_editing) {
     sprintf(text,entroom_start,user->recap);
     write_room_except(user->room,text,user);
	write_user(user,"\n");
     sprintf(text,entroom_edit_header);
     write_user(user,center(text,78));
	write_user(user,"\n\n");
	user->misc_op=8;
	editor(user,NULL);
	return;
	}
/* Write The Editor Contents To A File */
sprintf(filename,"%s/%s.R",DATAFILES,rm->name);
if (!(fp=fopen(filename,"w"))) {
	sprintf(text,"%s: couldn't save the room description.\n",syserror);
	write_user(user,text);
	sprintf("ERROR: Couldn't open file %s to write in room_desc().\n",filename);
	write_syslog(text,0);
	return;
	}
c=user->malloc_start;
while(c!=user->malloc_end) putc(*c++,fp);
fclose(fp);
sprintf(text,entroom_end,user->recap);
write_room_except(user->room,text,user);
write_user(user,"Your Room Description Has Been Saved.\n");
look(user);
}

/*** Set in and out phrases ***/
void set_iophrase(UR_OBJECT user,char *inpstr)
{

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (strlen(inpstr)>PHRASE_LEN) {
	write_user(user,"Phrase too long.\n");
        return;
	}
if (com_num==INPHRASE) {
	if (word_count<2) {
		sprintf(text,"Your current in phrase is: %s\n",user->in_phrase);
		write_user(user,text);
		return;
		}
        strncpy(user->in_phrase,inpstr,PHRASE_LEN);
        write_user(user,"~CMWhen you enter a room, others will see:\n");
        sprintf(text,"%s %s\n",user->name,user->in_phrase);
        write_user(user,text);
	return;
	}
if (word_count<2) {
	sprintf(text,"Your current out phrase is: %s\n",user->out_phrase);
	write_user(user,text);
	return;
	}
strncpy(user->out_phrase,inpstr,PHRASE_LEN);
write_user(user,"When you leave a room, others will see:\n");
sprintf(text,"%s %s\n",user->name,user->out_phrase);
write_user(user,text);
}

/*** Show who is on ***/
void who(UR_OBJECT user,int style)
{
int req=0;

if (strstr(word[1],"help")) {
        write_user(user,"Usage: who [<#> or <name>]\n");
        write_user(user,"Note : If no arguments specified, your default who style is used.\n\n");
        write_user(user,"~CGThe Current Who List Stles Are:\n");
        write_user(user,"~FT~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
        write_user(user,"~CT1~FY... ~FMShort Who (Show Only Usernames)\n");
        write_user(user,"~CT2~FY... ~FMHouse Of Pleasure And Ecstacy Who List Style\n");
        write_user(user,"~CT3~FY... ~FMStairway To Heaven Who List Style\n");
        write_user(user,"~CT4~FY... ~FMM~FMoenuts Byroom Who List Style\n");
        write_user(user,"~CT5~FY... ~FMShackles Who List Style\n\n");
        write_user(user,"~CT6~FY... ~FMMedival Times Who List Style\n");
        write_user(user,"~CT7~FY... ~FMCamelot Who List Style\n");
        write_user(user,"~CT8~FY... ~FMDark Garden Who List Style\n");
        return;
        }
req=atoi(word[1]);
if (req>0 && req<MAX_WHOS+1) style=req;
if (!req && !strncasecmp(word[1],"short",5)) style=1;
else if (!req && !strncasecmp(word[1],"hope",4))  style=2;
else if (!req && !strncasecmp(word[1],"stair",5)) style=3;
else if (!req && !strncasecmp(word[1],"byr",3)) style=4;
else if (!req && !strncasecmp(word[1],"room",4)) style=4;
else if (!req && !strncasecmp(word[1],"shack",5)) style=5;
else if (!req && !strncasecmp(word[1],"med",3))  style=6;
else if (!req && !strncasecmp(word[1],"tim",3))  style=6;
else if (!req && !strncasecmp(word[1],"mt",2))  style=6;
else if (!req && !strncasecmp(word[1],"cam",2))  style=7;
else if (!req && !strncasecmp(word[1],"dar",2))  style=8;
else if (!req && !strncasecmp(word[1],"gar",2))  style=8;

/* We put this here so that arrested/scum ALWAYS get the SHORT WHO list! */
if (user->muzzled & JAILED || user->muzzled & SCUM) style=1;

switch(style) {
     case 1: shortwho(user); break;
     case 2: hopewho(user);  break;
     case 3: stairwho(user); break;
     case 4: byroom(user);   break;
     case 5: shackles_who(user); break;
     case 6: medival_times_who(user);  break;
     case 7: camelot_who(user);  break;
     case 8: dark_garden_who(user);  break;
     default:
        write_user(user,"~CGThe Current Who List Stles Are:\n");
	write_user(user,"~FT~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
        write_user(user,"~CT1~FY... ~FMShort Who (Show Only Usernames)\n");
        write_user(user,"~CT2~FY... ~FMHouse Of Pleasure And Ecstacy Who List Style\n");
        write_user(user,"~CT3~FY... ~FMStairway To Heaven Who List Style\n");
        write_user(user,"~CT4~FY... ~FMM~FMoenuts Byroom Who List Style\n");
        write_user(user,"~CT5~FY... ~FMShackles Who List style\n");
        write_user(user,"~CT6~FY... ~FMMedival Times Who List Style\n");
        write_user(user,"~CT7~FY... ~FMCamelot Who List Style\n");
        write_user(user,"~CT8~FY... ~FMDark Garden Who List Style\n");
     }
}

/*** Show some user stats ***/
void status(UR_OBJECT user)
{
UR_OBJECT u;
char ir[ROOM_NAME_LEN+1],levelname[32],isbad[25],gendertype[16],waittell[6],ignoringstuff[6];
int days,hours,mins,hs,offline=0;
char *center();

if (user->muzzled & JAILED) {
	sprintf(text,"~CR%% ~CMYou cannot use the %s command...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (user->muzzled & SCUM) {
	sprintf(text,"~CR%% ~CMYou cannot use the %s command...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (word_count<2) {
     u=user;
     sprintf(text,user_ustat_header,TALKERNAME);
     write_user(user,center(text,78));
     if (user->high_ascii) write_user(user,"\n~Cƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
     else write_user(user,"\n~CM------------------------------------------------------------------------------\n");
     }
else {
	if (!(u=get_user(word[1]))) {
	   if ((u=create_user())==NULL) {
		sprintf(text,"%s: unable to create temporary user object.\n",syserror);
		write_user(user,text);
          write_syslog("ERROR: Unable to create temporary user object in status().\n",0);
		return;
		}
	offline=1;
     word[1][0]=toupper(word[1][0]);
	strcpy(u->name,word[1]);
	if (!load_user_details(u)) {
		write_user(user,nosuchuser);
		destruct_user(u);
		destructed=0;
		return;
		}
	}
     sprintf(text,ustat_header,u->recap,TALKERNAME);
     	write_user(user,center(text,78));
     	if (user->high_ascii) write_user(user,"\n~CMƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
     	else write_user(user,"\n~CM------------------------------------------------------------------------------\n");
     	}
strcpy(ignoringstuff,"No ");
if (u->ignore)  strcpy(ignoringstuff,"Yes");
strcpy(waittell,"No ");
if (u->chkrev)  strcpy(waittell,"Yes");
strcpy(isbad,"Good");
if (u->muzzled & SCUM) {
        strcpy(levelname,"Pond Scum");
        strcpy(isbad,"Bad ");
        }
else {
	strcpy(levelname,level_name[u->level]);
 	if (u->prompt & FEMALE) strcpy(levelname,level_name_fem[u->level]);
	}
if (u->muzzled & JAILED) {
     strncpy(levelname,ustat_jail_levelname,30);
     strcpy(isbad,"Arrested");
     }
if (u->gaged) strcpy(isbad,"Gaged");
if (u->muzzled & 1 && u->gaged) strcpy(isbad,"Muzzled & Gaged");
strcpy(gendertype,"Neuter");
if (u->prompt & 4) strcpy(gendertype,"Female");
if (u->prompt & 8) strcpy(gendertype,"Male  ");
if (u->invite_room==NULL) strcpy(ir,"Nowhere");
else strcpy(ir,u->invite_room->name);
if (!offline) hs=1;
else hs=0;
/* Display the stats */
sprintf(text,"~CBLevel         ~CM: ~CG%-22.22s  ~CBRank          ~CM: ~CG%s\n",levelname,u->level_alias);
write_user(user,text);
if (!offline) {
	sprintf(text,"~CBWaiting tells ~CM: ~CG%-22.22s",waittell);
	write_user(user,text);
     if (u->ignall) sprintf(text,"  ~CBIgnoring      ~CM: ~CGEverything\n");
     else sprintf(text,"  ~CBIgnoring      ~CM: ~CG%s\n",ignoringstuff);
	write_user(user,text);
	}
sprintf(text,"~CBGender        ~CM: ~CG%-22.22s  ~CBUser Is Online~CM: ~CG%s\n",gendertype,noyes2[hs]);
write_user(user,text);
sprintf(text,"~CBVisible       ~CM: ~CG%-22.22s  ~CBStatus        ~CM: ~CG%s\n",noyes2[u->vis],isbad);
write_user(user,text);
sprintf(text,"~CBUnread mail   ~CM: ~CG%-22.22s  ~CBCharacter Echo~CM: ~CG%s\n",noyes2[has_unread_mail(u)],offon[u->charmode_echo]);
write_user(user,text);
sprintf(text,"~CBAnsi Colour   ~CM: ~CG%-22.22s  ~CBAnsi Emulation~CM: ~CG%s\n",offon[u->colour],offon[u->high_ascii]);
write_user(user,text);
if (!offline) sprintf(text,"~CBLogin Room    ~CM: ~CG%-22.22s  ~CBInvited To    ~CM: ~CG%s\n",u->login_room,ir);
else sprintf(text,"~CBLogin Room    ~CM: ~CG%-22.22s  ~CBLog Into Last ~CM: ~CG%s\n",u->login_room,offon[u->last_room]);
write_user(user,text);
sprintf(text,"~CBDescription   ~CM: %s~RS %s\n",u->recap,u->desc);
write_user(user,text);
sprintf(text,"~CBIn phrase     ~CM: %s~RS %s\n",u->recap,u->in_phrase);
write_user(user,text);
sprintf(text,"~CBOut phrase    ~CM: %s~RS %s to the [room name]\n",u->recap,u->out_phrase);
write_user(user,text);
if (strcasecmp(u->roomname,"none")) {
     	if (user->high_ascii) write_user(user,"~CMƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
     	else write_user(user,"~CM------------------------------------------------------------------------------\n");
	sprintf(text,"~CBRoom Name     ~CM: ~CG%s",u->roomname);
	write_user(user,text);
	if (u->roombanned) write_user(user,"     ~CR-=] ~CMBANNED~CR [=-\n");
	else write_user(user,"\n");
	sprintf(text,"~CBRoom Topic    ~CM: ~RS%s\n",u->roomtopic);
	write_user(user,text);
	if (user->level>WIZ) {
		sprintf(text,"~CBRoom Given By ~CM: ~CG%s\n",u->roomgiver);
		write_user(user,text);
		}
     	if (user->high_ascii) write_user(user,"~CMƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
     	else write_user(user,"~CM------------------------------------------------------------------------------\n");
	}
days=u->total_login/86400;
hours=(u->total_login%86400)/3600;
mins=(u->total_login%3600)/60;
sprintf(text,"~CBTotal login   ~CM: ~CY%d ~CTdays, ~CY%d ~CThours, ~CY%d ~CTminutes\n",days,hours,mins);
write_user(user,text);
if (user->high_ascii) write_user(user,"~CMƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
else write_user(user,"~CM------------------------------------------------------------------------------\n");
if (offline) { destruct_user(u); destructed=0; }
}

/*** Show talker system parameters etc ***/
void system_details(UR_OBJECT user)
{
RM_OBJECT rm;
UR_OBJECT u;
char bstr[40],minlogin[5],*center();
char *ca[]={ "None  ","Ignore","Reboot","Sreboot" };
int days,hours,mins,secs;
int rms,num_clones,mem,size;

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (user->high_ascii) write_user(user,"~CMƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
else write_user(user,"~CM------------------------------------------------------------------------------\n");
sprintf(text,"~CYMoenuts version %s - Telnet Chat Server System Status",MNVERSION);
write_user(user,center(text,78));
if (user->high_ascii) write_user(user,"\n~CMƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
else write_user(user,"\n~CM------------------------------------------------------------------------------\n");
/* Get some values */
strcpy(bstr,ctime(&boot_time));
secs=(int)(time(0)-boot_time);
days=secs/86400;
hours=(secs%86400)/3600;
mins=(secs%3600)/60;
secs=secs%60;
num_clones=0;
mem=0;
size=sizeof(struct user_struct);
for(u=user_first;u!=NULL;u=u->next) {
	if (u->type==CLONE_TYPE) num_clones++;
	mem+=size;
	}
rms=0;
size=sizeof(struct room_struct);
for(rm=room_first;rm!=NULL;rm=rm->next) {
	++rms;  mem+=size;
	}
if (minlogin_level==-1) strcpy(minlogin,"NONE");
else strcpy(minlogin,level_name[minlogin_level]);
/* Show header parameters */
sprintf(text,"~FTProcess ID   : ~FG%d\n~FTTalker booted: ~FG%s~FTUptime       : ~FG%d days, %d hours, %d minutes, %d seconds\n",getpid(),bstr,days,hours,mins,secs);
write_user(user,text);
sprintf(text,"~FTPorts (M/W)  : ~FG%d,  %d\n",port[0],port[1]);
write_user(user,text);
if (user->high_ascii) write_user(user,"~CMƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
else write_user(user,"~CM------------------------------------------------------------------------------\n");
/* Show others */
sprintf(text,"Max users              : %-4.4d         Current num. of users  : %-4.4d\n",max_users,num_of_users);
write_user(user,text);
sprintf(text,"Max clones             : %-4.4d         Current num. of clones : %-4.4d\n",max_clones,num_clones);
write_user(user,text);
sprintf(text,"Current minlogin level : %-11.11s  Login idle time out    : %-4.4d secs.\n",minlogin,login_idle_time);
write_user(user,text);
sprintf(text,"User idle time out     : %-4.4d secs.   Heartbeat              : %-4.4d\n",user_idle_time,heartbeat);
write_user(user,text);
sprintf(text,"Wizport min login level: %-11.11s  Gatecrash level        : %-13.13s\n",level_name[wizport_level],level_name[gatecrash_level]);
write_user(user,text);
sprintf(text,"Time out maxlevel      : %-11.11s  Private room min count : %-4.4d\n",level_name[time_out_maxlevel],min_private_users);
write_user(user,text);
sprintf(text,"Message lifetime       : %2.2d days      Message check time     : %2.2d:%2.2d\n",mesg_life,mesg_check_hour,mesg_check_min);
write_user(user,text);
sprintf(text,"Number of rooms        : %-4.4d         Ignoring sigterm       : %-13.13s\n",rms,noyes2[ignore_sigterm]);
write_user(user,text);
sprintf(text,"Echoing passwords      : %-11.11s  Swearing banned        : %-13.13s\n",noyes2[password_echo],noyes2[ban_swearing]);
write_user(user,text);
sprintf(text,"Time out afks          : %-11.11s  Kick Idle Users Offline: %-13.13s\n",noyes2[time_out_afks],noyes2[kill_idle_users]);
write_user(user,text);
sprintf(text,"New user prompt default: %-11.11s  New user color default : %-13.13s\n",offon[prompt_def],offon[colour_def]);
write_user(user,text);
sprintf(text,"New user charecho def. : %-11.11s  System logging         : %-13.13s\n",offon[charecho_def],offon[system_logging]);
write_user(user,text);
sprintf(text,"Crash action           : %-11.11s  Object memory allocated: %d Bytes\n",ca[crash_action],mem);
write_user(user,text);
if (user->high_ascii) write_user(user,"~CMƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
else write_user(user,"~CM------------------------------------------------------------------------------\n");
}

/*** Examine a user ***/
void examine(UR_OBJECT user)
{
UR_OBJECT u,u2;
u=NULL;
u2=NULL;

FILE *fp;
char filename[80],line[129],ir[30],levelname[USER_ALIAS_LEN+2],levelname2[50],gendertype[16],genderx[4];
char gaylesbian[21];
char *profsub(), *center();
int new_mail,days,hours,mins,timelen,days2,hours2,mins2,idle;

if (user->muzzled & JAILED) {
        write_user(user,"~CR%% ~CMYou are in jail, you cannot use the examine command...\n");
	return;
	}
if (word_count<2) strcpy(word[1],user->name);
if (!(u=get_user(word[1])) || u->hidden) {
	if ((u=create_user())==NULL) {
		sprintf(text,"%s: unable to create temporary user object.\n",syserror);
		write_user(user,text);
		write_syslog("ERROR: Unable to create temporary user object in examine().\n",0);
		return;
		}
	strcpy(u->name,word[1]);
	if (!load_user_details(u)) {
		write_user(user,nosuchuser);
		destruct_user(u);
		destructed=0;
		return;
		}
	u2=NULL;
	}
else u2=u;
if (u->hidden) { save_user_details(user,1); }
sprintf(text,examine_style,u->recap,u->desc);
write_user(user,center(text,80));
if (user->high_ascii) write_user(user,"~CMƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
else write_user(user,"~CM------------------------------------------------------------------------------\n");
sprintf(filename,"%s/%s.P",USERFILES,u->name);
if (!(fp=fopen(filename,"r"))) write_user(user,no_profile_prompt);
else {
	fgets(line,128,fp);
	while(!feof(fp)) {
		line[strlen(line)-1]=0;
		strcat(line,"\n");
		write_user(user,profsub(line,user));
		line[0]=0;
		tempstr[0]=0;
		fgets(line,128,fp);
		}
	fclose(fp);
	}
sprintf(filename,"%s/%s.M",USERFILES,u->name);
if (!(fp=fopen(filename,"r"))) new_mail=0;
else {
	fscanf(fp,"%d",&new_mail);
	fclose(fp);
	}
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
if (u->bdsm_life_type==1) {
	strcpy(gaylesbian,"Gay");
	if (u->prompt & 4) strcpy(gaylesbian,"Lesbian");
	}
else strncpy(gaylesbian,bdsm_life_types[u->bdsm_life_type],20);
/* XXX */
if (user->high_ascii) write_user(user,"~CMƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
else write_user(user,"~CM------------------------------------------------------------------------------\n");
if (u2==NULL) {
	sprintf(text,"~CGLevel Alias  ~CW: ~CT%-30.30s  ~CGLevel Name    ~CW: ~CT%s\n",levelname,levelname2);
	write_user(user,text);
	sprintf(text,"~CGCaste        ~CW: ~CT%-30.30s  ~CGLifestyle     ~CW: ~CT%s\n",bdsm_types[u->bdsm_type],gaylesbian);
	write_user(user,text);
        sprintf(text,"~CGAge          ~CW: ~CT%-30.30s  ~CGGender        ~CW: ~CT%s\n",u->fakeage,gendertype);
	write_user(user,text);
        sprintf(text,"~CGBirth Date   ~CW: ~CT%-30.30s  ~CGMarried To    ~CW: ~CT%s\n",u->birthday,u->married);
        write_user(user,text);
        sprintf(text,"~CGICQ Number   ~CW: ~CT%-30.30s  ~CGOwned By      ~CW: ~CT%s\n",u->icq,u->owned_by);
        write_user(user,text);
        sprintf(text,"~CGBranded By   ~CW: ~CT%-30.30s  ~CGBank Balance  ~CW: ~CT$%d\n",u->branded_by,u->bank_balance);
        write_user(user,text);
        sprintf(text,"~CGBrand Desc   ~CW: ~CT%s\n",u->brand_desc);
        write_user(user,text);
        sprintf(text,"~CGEmail Address~CW: ~CT%s\n",u->branded_by);
        write_user(user,text);
        sprintf(text,"~CGWebpage URL  ~CW: ~CT%s\n",u->homepage);
        write_user(user,text);
        if (user->level==OWNER) {
             sprintf(text,"~CRReal Age     ~CW: ~FT%d\n",u->age);
	     write_user(user,text);
  	     }
	if (user->high_ascii) write_user(user,"~CMƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
	else write_user(user,"~CM------------------------------------------------------------------------------\n");
        sprintf(text,"~CGTic Tac Toe  ~CW: ~CYWins~CW:~CT %-5d ~CRLoses~CW:~CT %-5d ~CMDraws~CW:~CT %d\n",u->twin,u->tlose,u->tdraw);
        write_user(user,text);
	sprintf(text,"~CGLast Login   ~CW: ~CT%s",ctime((time_t *)&(u->last_login)));
	write_user(user,text);
     	sprintf(text,"~CGWhich was    ~CW: ~CT%d days, %d hours, %d minutes ago\n",days2,hours2,mins2);
	write_user(user,text);
        sprintf(text,"~CGWas on for   ~CW: ~CT%d days, ~CT%d hours, %d minutes\n~CGTotal login ~CW:~CM %d days, %d hours, %d minutes\n",(u->last_login_len/86400),(u->last_login_len/3600),(u->last_login_len%3600)/60,days,hours,mins);
	write_user(user,text);
     	if (user->level>=WIZ) {
        	sprintf(text,"~CGLast site    ~CW: ~CT%s\n",u->last_site);
	  	write_user(user,text);
	  	}
     	strcpy(genderx,"its");
     	if (u->prompt & 4) strcpy(genderx,"her");
     	if (u->prompt & 8) strcpy(genderx,"his");
     	if (new_mail>u->read_mail) {
           sprintf(text,"~CGMail Status  ~CW: ~CM%s~RS~CM hasn't read %s new mail yet!\n",u->recap,genderx);
	   write_user(user,text);
	   }
     if (user->high_ascii) write_user(user,"~CMƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
     else write_user(user,"~CM------------------------------------------------------------------------------\n");
     write_user(user," ~CGFor more information, use: ~CY.stat <username>\n");
     if (user->high_ascii) write_user(user,"~CMƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
     else write_user(user,"~CM------------------------------------------------------------------------------\n");
     destruct_user(u);
     destructed=0;
     return;
     }
     idle=(int)(time(0) - u->last_input)/60;
     sprintf(text,"~CGLevel Alias  ~CW: ~CT%-30.30s  ~CGLevel Name    ~CW: ~CT%s\n",levelname,levelname2);
     write_user(user,text);
     sprintf(text,"~CGCaste        ~CW: ~CT%-30.30s  ~CGLifestyle     ~CW: ~CT%s\n",bdsm_types[u->bdsm_type],gaylesbian);
     write_user(user,text);
     sprintf(text,"~CGAge          ~CW: ~CT%-30.30s  ~CGGender        ~CW: ~CT%s\n",u->fakeage,gendertype);
     write_user(user,text);
     sprintf(text,"~CGBirth Date   ~CW: ~CT%-30.30s  ~CGMarried To    ~CW: ~CT%s\n",u->birthday,u->married);
     write_user(user,text);
     sprintf(text,"~CGICQ Number   ~CW: ~CT%-30.30s  ~CGOwned By      ~CW: ~CT%s\n",u->icq,u->owned_by);
     write_user(user,text);
     sprintf(text,"~CGBranded By   ~CW: ~CT%-30.30s  ~CGBank Balance  ~CW: ~CT$%d\n",u->branded_by,u->bank_balance);
     write_user(user,text);
     sprintf(text,"~CGBrand Desc   ~CW: ~CT%s\n",u->brand_desc);
     write_user(user,text);
     sprintf(text,"~CGEmail Address~CW: ~CT%s\n",u->email);
     write_user(user,text);
     sprintf(text,"~CGWebpage URL  ~CW: ~CT%s\n",u->homepage);
     write_user(user,text);
     if (user->level==OWNER) {
 	     sprintf(text,"~CRReal Age     ~CW: ~CT%d\n",u->age);
	     write_user(user,text);
	}
     if (user->high_ascii) write_user(user,"~CMƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
     else write_user(user,"~CM------------------------------------------------------------------------------\n");
     sprintf(text,"~CGTic Tac Toe  ~CW: ~CYWins~CW:~CT %-5d ~CRLoses~CW:~CT %-5d ~CMDraws~CW:~CT %d\n",u->twin,u->tlose,u->tdraw);
     write_user(user,text);
     if (u->afk) {
	     sprintf(text,"~CGIdle for     ~CW:~CM %d minutes ~BR~CG-= ~CY~BRAFK ~CG~BR=-~RS \n",idle);
	     write_user(user,text);
	     if (u->afk_mesg[0]) {
          	sprintf(text,"~CGAFK message  ~CW: ~CT%s\n",u->afk_mesg);
		write_user(user,text);
		}
	     }
     else {
             sprintf(text,"~CGIdle for     ~CW: ~CM%d minutes\n",idle);
	     write_user(user,text);
	     }
    sprintf(text,"~CGTotal login  ~CW: ~CM%d days, %d hours, %d minutes\n",days,hours,mins);
    write_user(user,text);
    sprintf(text,"~CGOn since     ~CW: ~CM%s~CGOn for       ~CW:~CT %d days, ~CT%d hours, %d minutes\n",ctime((time_t *)&u->last_login),days2,hours2,mins2);
    write_user(user,text);
    if (new_mail>u->read_mail) {
        sprintf(text,"~CGMail Status  ~CW: ~CM%s~RS ~CMhas unread mail.\n",u->recap);
        write_user(user,text);
        }
    if (user->level>=WIZ) {
        sprintf(text,"~CMSite         ~CW: ~CT%s:%s\n",u->site,u->site_port);
        write_user(user,text);
        }
     if (user->high_ascii) write_user(user,"~CMƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
     else write_user(user,"~CM------------------------------------------------------------------------------\n");
     write_user(user," ~CGFor more information, use: ~CY.stat <username>\n");
     if (user->high_ascii) write_user(user,"~CMƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
     else write_user(user,"~CM------------------------------------------------------------------------------\n");
}

/*** Do the help ***/
void help(UR_OBJECT user)
{
int ret,len,i;
char filename[80];
char *c,*comword=NULL;

if (user->muzzled & SCUM) {
     if (user->high_ascii) write_user(user,"~CMƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
     else write_user(user,"~CM------------------------------------------------------------------------------\n");
     write_user(user,"~FG                  Commands available for level ~CTCRIM\n");
     if (user->high_ascii) write_user(user,"~CMƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
     else write_user(user,"~CM------------------------------------------------------------------------------\n");
     write_user(user,"~FM  [ ~CTScum~RS~FM ]\n");
     write_user(user,"~RS  quit      look      who        help       say        mode       rules\n");
     write_user(user,"~RS  ranks     time\n");
     if (user->high_ascii) write_user(user,"~CMƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
     else write_user(user,"~CM------------------------------------------------------------------------------\n");
     write_user(user,"~FG For help on a command, type ~CY.help <command>~FG.\n");
     if (user->high_ascii) write_user(user,"~CMƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
     else write_user(user,"~CM------------------------------------------------------------------------------\n");
     return;
     }
if (user->muzzled & JAILED) {
     if (user->high_ascii) write_user(user,"~CMƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
     else write_user(user,"~CM------------------------------------------------------------------------------\n");
     write_user(user,"~FG                  Commands available for level ~CTJAILED\n");
     if (user->high_ascii) write_user(user,"~CMƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
     else write_user(user,"~CM------------------------------------------------------------------------------\n");
     write_user(user,"~FM  [ ~CTJailed~RS~FM ]\n");
     write_user(user,"~RS  look      who        help       say        mode       rules       ranks\n");
     write_user(user,"~RS  time\n");
     if (user->high_ascii) write_user(user,"~CMƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
     else write_user(user,"~CM------------------------------------------------------------------------------\n");
     write_user(user,"~FG For help on a command, type ~CY.help <command>~FG.\n");
     if (user->high_ascii) write_user(user,"~CMƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
     else write_user(user,"~CM------------------------------------------------------------------------------\n");
     return;
     }
if (word_count<2) { help_commands(user); return; }
if (!strcmp(word[1],"nuts")) {  help_nuts(user);  return;  }

/* Check for any illegal crap in searched for filename so they cannot list
   out the /etc/passwd file for instance. */

c=word[1];
while(*c) {
	if (*c=='.' || *c++=='/') {
                sprintf(text,"Sorry, the %s command has not been invented yet.\n",word[1]);
                write_user(user,text);
                sprintf(text,"%s tried to to use %s to hack the system in the helpfiles.\n",user->name,word[1]);
		write_syslog(text,1);
		return;
		}
	}
i=0;
comword=word[1];
len=strlen(comword);
while(command[i][0]!='*') {
	if (!strncmp(command[i],comword,len)) {  com_num=i;  break;  }
	++i;
	}
if (com_num==-1 && !file_exists(word[1])) {
        sprintf(text,"~CB%% ~FRHelp: There is no help on the topic ~FB\"~FY%s~FB\"~FR.\n",word[1]);
	write_user(user,text);
	return;
	}
if (com_level[com_num]>user->level) {
        sprintf(text,"~CB%% ~FRYou do not have access to view the help to the %s command!\n",command[com_num]);
	write_user(user,text);
	return;
	}
sprintf(filename,"%s/%s",HELPFILES,word[1]);
if (!(ret=more(user,user->socket,filename))) {
        sprintf(text,"~CRSorry, could not find the help file ~FB\"~FY%s~FB\".\n",word[1]);
        write_user(user,text);
        }
if (ret==1) user->misc_op=2;
}

/*** Show the command available ***/
void help_commands(UR_OBJECT user)
{
int com,cnt,lev,cmds;
char temp[20], *center();

com=0;
if (user->high_ascii) write_user(user,"~CMƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
else write_user(user,"~CM------------------------------------------------------------------------------\n");
sprintf(text,help_header,level_name[user->level]);
write_user(user,center(text,78));
if (user->high_ascii) write_user(user,"\n~CMƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
else write_user(user,"\n~CM------------------------------------------------------------------------------\n");
cmds=0;
for(lev=NEW;lev<=user->level;++lev) {
     sprintf(text,help_levelname_style,level_name[lev]);
     write_user(user,center(text,70));
     write_user(user,"\n");
     com=0;  cnt=0;  text[0]='\0';
     while(command[com][0]!='*') {
          if (com_level[com]!=lev) {  com++;  continue;  }
          if (!cnt) write_user(user,"  ");
	  sprintf(temp,"~FT%-11.11s",command[com]);
          cmds++;
          strcat(text,temp);
          if (cnt==6) {
                        strcat(text,"\n");
			write_user(user,text);
			text[0]='\0';
			cnt=-1;
			}
		com++;	cnt++;
		}
	if (cnt) {
		strcat(text,"\n");
		write_user(user,text);
		}
	}
if (user->high_ascii) write_user(user,"~CMƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
else write_user(user,"~CM------------------------------------------------------------------------------\n");
sprintf(text,help_footer1,com,cmds);
write_user(user,text);
write_user(user,help_footer2);
if (user->high_ascii) write_user(user,"~CMƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
else write_user(user,"~CM------------------------------------------------------------------------------\n");
}

/*** Show info on the NUTS code ***/
void help_nuts(UR_OBJECT user)
{
if (user->high_ascii) write_user(user,"~CMƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
else write_user(user,"~CM------------------------------------------------------------------------------\n");
sprintf(text," ~CYM~CYoenuts version %s (C)1997,2004 Michael Irving ~CB(~CYM~CYoe~CB)\n",MNVERSION);
write_user(user,text);
write_user(user," ~FGBased on NUTS v3.3.3 (C)1996 Neil Robertson, Portions (C)1997 ~FRRed~FBdawg\n");
if (user->high_ascii) write_user(user,"~CMƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
else write_user(user,"~CM------------------------------------------------------------------------------\n");
write_user(user,"NUTS stands for Neils Unix Talk Server, a program which started out as a\n");
write_user(user,"university project in autumn 1992 and has progressed from thereon. In no\n");
write_user(user,"particular order thanks go to the following people who helped me develop or\n");
write_user(user,"debug this code in one way or another over the years:\n");
write_user(user,"   ~FTDarren Seryck, Steve Guest, Dave Temple, Satish Bedi, Tim Bernhardt,\n");
write_user(user,"   ~FTKien Tran, Jesse Walton, Pak Chan, Scott MacKenzie and Bryan McPhail.\n");
write_user(user,"Also thanks must go to anyone else who has emailed me with ideas and/or bug\n");
write_user(user,"reports and all the people who have used NUTS over the intervening years.\n");
write_user(user,"I know I've said this before but this time I really mean it - this is the final\n");
write_user(user,"version of NUTS 3. In a few years NUTS 4 may spring forth but in the meantime\n");
write_user(user,"that, as they say, is that. :)\n");
write_user(user,"If you wish to email me my address is '~FGneil@ogham.demon.co.uk~RS' and should\n");
write_user(user,"remain so for the forseeable future.\nNeil Robertson - November 1996.\n");
if (user->high_ascii) write_user(user,"~CMƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
else write_user(user,"~CM------------------------------------------------------------------------------\n");
write_user(user,"Moenuts is customized NUTS 3.3.3.  It was customized by Michael Irving.  I would\n");
write_user(user,"like to send out SPECIAL thanks to Reddawg, Nerf, Andy, Squirt, Arny, Addict and Curmitt\n");
write_user(user,"for helping me with many additions to this code which by myself wouldn't be\n");
write_user(user,"possible and or help with recovering pieces of code I had lost.\n");
if (user->high_ascii) write_user(user,"~CMƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
else write_user(user,"~CM------------------------------------------------------------------------------\n");
write_user(user,"~FG This version by ~FRM~CRo~CWe~FG, Portions by ~FRRed~FBdawg~FG, Andy, Squirt, Addict, Arny, Curmitt.\n");
if (user->high_ascii) write_user(user,"~CMƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
else write_user(user,"~CM------------------------------------------------------------------------------\n");
}

/**************************** MESSAGE BOARD COMMANDS **********************/
/*** Read the message board ***/
void read_board(UR_OBJECT user)
{
RM_OBJECT rm;
char filename[80],*name;
char *center();
int ret;

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (user->muzzled & SCUM) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
     	}
if (word_count<2) rm=user->room;
else {
	if ((rm=get_room(word[1]))==NULL) {
		write_user(user,nosuchroom);
		return;
		}
	if (!has_room_access(user,rm)) {
		write_user(user,"That room is currently private, you cannot read the board.\n");
		return;
		}
	}
sprintf(text,message_board_header,rm->recap);
write_user(user,center(text,78));
write_user(user,"\n\n");
sprintf(filename,"%s/%s.B",DATAFILES,rm->name);
if (!(ret=more(user,user->socket,filename))) {
     sprintf(text,read_no_messages,rm->recap);
     write_user(user,center(text,78));
	write_user(user,"\n\n");
	}
else if (ret==1) user->misc_op=2;
if (user->vis) name=user->recap; else name=invisname;
if (rm==user->room) {
     sprintf(text,user_read_board_prompt,name);
     write_room_except(user->room,text,user);
     }
}

/*** Write on the message board ***/
void write_board(UR_OBJECT user,char *inpstr,int done_editing)
{
FILE *fp;
int cnt,inp,newmsg;
char *ptr,*name,filename[80],*center();

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (user->muzzled & 1) {
	write_user(user,"You are muzzled, you cannot write on the board.\n");
	return;
	}
if (!done_editing) {
	if (word_count<2) {
          sprintf(text,write_edit_header);
          write_user(user,center(text,78));
		user->misc_op=3;
		editor(user,NULL);
		return;
		}
	ptr=inpstr;
	inp=1;
	}
else {
	ptr=user->malloc_start;  inp=0;
	}
if (user->muzzled & FROZEN) {
     write_user(user,user_write_end);
	return;
	}
sprintf(filename,"%s/%s.B",DATAFILES,user->room->name);
if (!(fp=fopen(filename,"a"))) {
	sprintf(text,"%s: cannot write to file.\n",syserror);
	write_user(user,text);
	sprintf(text,"ERROR: Couldn't open file %s to append in write_board().\n",filename);
	write_syslog(text,0);
	return;
	}
if (user->vis) name=user->recap; else name=invisname;
/* The posting time (PT) is the time its written in machine readable form, this
   makes it easy for this program to check the age of each message and delete
   as appropriate in check_messages() */
check_messages(user,3);
newmsg=user->room->mesg_cnt+1;
sprintf(text,"PT: %d %d\r~CGFrom   ~CW: ~RS%s        ~RS\n~CGWrote  ~CW:~FT %s\n",(int)(time(0)),newmsg,name,long_date(0));
fputs(text,fp);
fputs("~FM-------------------------------------------------------------------------------\n",fp);
cnt=0;
while(*ptr!='\0') {
	putc(*ptr,fp);
	if (*ptr=='\n') cnt=0; else ++cnt;
     if (cnt==77) { putc('\n',fp); cnt=0; }
	++ptr;
	}
if (inp) fputs("\n~FM-------------------------------------------------------------------------------\n\n",fp);
else fputs("~FM-------------------------------------------------------------------------------\n\n",fp);
fclose(fp);
write_user(user,user_write_end);
sprintf(text,room_write_end,name);
write_room_except(user->room,text,user);
user->room->mesg_cnt++;
}

/*** Wipe some messages off the board ***/
void wipe_board(UR_OBJECT user)
{
int cnt,cnt_um,valid,just_one,range,total;
int num,num_one,num_two,num_three,num_four,num_five,num_six;
char infile[80],line[82],id[82],*name;
FILE *infp,*outfp;
RM_OBJECT rm;

range=num_two=num_three=num_four=0;
if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
just_one=1;
total=1;
if (word_count<2 || ((num=atoi(word[1]))<1 && strcmp(word[1],"all") && strcmp(word[1],"top") && strcmp(word[1],"bottom"))) {
	print_useage(user,1);
	return;
	}
else {
	if (!strcmp(word[1],"top") && (num=atoi(word[2]))<1) {
		print_useage(user,1);
		return;
		}
	if (!strcmp(word[1],"bottom") && (num=atoi(word[2]))<1) {
		print_useage(user,1);
		return;
		}
	if (!strcmp(word[2],"-")) {
		(num_one=atoi(word[3]));
		range=1;
		just_one=0;
		total=(num_one-num)+1;
		if (total<2) {
			total=0;
			range=0;
			just_one=0;
			}
		goto GOT_IT;
		}
	if ((num_one=atoi(word[2]))<1) {
		just_one=1;
		range=0;
		total=1;
		goto GOT_IT;
		}
	if ((num_two=atoi(word[3]))<1) {
		just_one=0;
		range=0;
		total=2;
		num_three=0;
		num_four=0;
		num_five=0;
		goto GOT_IT;
		}
	if ((num_three=atoi(word[4]))<1) {
		just_one=0;
		range=0;
		total=3;
		num_four=0;
		num_five=0;
		goto GOT_IT;
		}
	if ((num_four=atoi(word[5]))<1) {
		just_one=0;
		range=0;
		total=4;
		num_five=0;
		goto GOT_IT;
		}
	if ((num_five=atoi(word[6]))<1) {
		just_one=0;
		range=0;
		total=5;
		goto GOT_IT;
		}
	if ((num_six=atoi(word[7]))!=0) {
		just_one=0;
		range=0;
		total=0;
		}
	}
GOT_IT:
if (!total && !just_one && !range) {
	print_useage(user,1);
	return;
	}
rm=user->room;
if (user->vis) name=user->recap; else name=invisname;
sprintf(infile,"%s/%s.B",DATAFILES,rm->name);
if (!(infp=fopen(infile,"r"))) {
     write_user(user,wipe_empty_board);
	return;
	}
if (!strcmp(word[1],"all")) {
	fclose(infp);
	unlink(infile);
     write_user(user,wipe_user_all_deleted);
     sprintf(text,wipe_room_all_deleted,name);
	write_room_except(rm,text,user);
     sprintf(text,"%s wiped the message board in the %s room.\n",user->recap,rm->recap);
	write_syslog(text,1);
	rm->mesg_cnt=0;
	return;
	}
if (!(outfp=fopen("tempfile","w"))) {
	sprintf(text,"%s: couldn't open tempfile.\n",syserror);
	write_user(user,text);
	write_syslog("ERROR: Couldn't open tempfile in wipe_board().\n",0);
	fclose(infp);
	return;
	}
if (!strcmp(word[1],"top")) {
	cnt=0; valid=1;
	fgets(line,82,infp); /* max of 80+newline+terminator = 82 */
	while(!feof(infp)) {
		if (cnt<=num) {
			if (*line=='\n') valid=1;
			sscanf(line,"%s",id);
			if (valid && !strcmp(id,"PT:")) {
				if (++cnt>num) fputs(line,outfp);
				valid=0;
				}
			}
		else fputs(line,outfp);
		fgets(line,82,infp);
		}
	fclose(infp);
	fclose(outfp);
	unlink(infile);
	if (cnt<num) {
		unlink("tempfile");
          sprintf(text,wipe_too_many,cnt);
		write_user(user,text);
          sprintf(text,wipe_room_all_deleted,name);
		write_room_except(rm,text,user);
		sprintf(text,"%s wiped all messages from the board in the %s.\n",user->name,rm->recap);
		write_syslog(text,1);
		rm->mesg_cnt=0;
		return;
		}
	if (cnt==num) {
		unlink("tempfile"); /* cos it'll be empty anyway */
          write_user(user,wipe_user_all_deleted);
		user->room->mesg_cnt=0;
          sprintf(text,"%s wiped all messages from the board in the %s.\n",user->name,rm->recap);
		}
	else {
		rename("tempfile",infile);
		check_messages(user,1);
          sprintf(text,wipe_deleted_top,num);
		write_user(user,text);
		user->room->mesg_cnt-=num;
          sprintf(text,"%s wiped %d messages from the board in the %s.\n",user->name,num,rm->recap);
		}
	write_syslog(text,1);
     sprintf(text,wipe_deleted_some,name,num);
	write_room_except(rm,text,user);
	return;
	}
cnt_um=0;
cnt=0;
valid=1;
fgets(line,82,infp); /* max of 80+newline+terminator = 82 */
while(!feof(infp)) {
	if (*line=='\n') valid=1;
	sscanf(line,"%s",id);
	if (valid && !strcmp(id,"PT:")) {
		cnt_um++;
		valid=0;
		}
	fgets(line,82,infp);
	}
fclose(infp);
sprintf(infile,"%s/%s.B",DATAFILES,rm->name);
if (!(infp=fopen(infile,"r"))) return;
if (!strcmp(word[1],"bottom")) {
	cnt=0;
	valid=1;
	total=num;
	num=cnt_um-total;
	num++;
	if (num<0) {
		fclose(infp);
		unlink(infile);
          sprintf(text,wipe_too_many,total);
		write_user(user,text);
          sprintf(text,wipe_room_all_deleted,name);
		write_room_except(rm,text,user);
		sprintf(text,"%s wiped all messages from the board in the %s.\n",user->name,rm->recap);
		write_syslog(text,1);
		rm->mesg_cnt=0;
		return;
		}
	fgets(line,82,infp); /* max of 80+newline+terminator = 82 */
	while(!feof(infp)) {
		if (*line=='\n') valid=1;
		sscanf(line,"%s",id);
		if (valid && !strcmp(id,"PT:")) {
			if (++cnt<num) fputs(line,outfp);
			valid=0;
			}
		fgets(line,82,infp);
		if (!valid && cnt<num) fputs(line,outfp);
		}
	fclose(infp);
	fclose(outfp);
	unlink(infile);
	if (cnt==total) {
		unlink("tempfile"); /* cos it'll be empty anyway */
          write_user(user,wipe_user_all_deleted);
		user->room->mesg_cnt=0;
		sprintf(text,"%s wiped all messages from the board in the %s.\n",user->name,rm->recap);
		}
	else {
		rename("tempfile",infile);
		check_messages(user,1);
          sprintf(text,wipe_deleted_bottom,total);
		write_user(user,text);
		user->room->mesg_cnt-=total;
		sprintf(text,"%s wiped %d messages from the board in the %s.\n",user->name,total,rm->recap);
		}
	write_syslog(text,1);
     sprintf(text,wipe_deleted_some,name,total);
	write_room_except(rm,text,user);
	return;
	}
if (just_one) {
	cnt=0;
	valid=1;
	if (cnt_um==1) {
		unlink(infile); /* cos it'll be empty anyway */
          write_user(user,wipe_user_all_deleted);
		user->room->mesg_cnt=0;
		sprintf(text,"%s wiped all messages from the board in the %s.\n",user->name,rm->recap);
		write_syslog(text,1);
		return;
		}
	if (num>cnt_um) {
          sprintf(text,wipe_missing_number,cnt_um,num);
		write_user(user,text);
		print_useage(user,1);
		return;
		}
	fgets(line,82,infp); /* max of 80+newline+terminator = 82 */
	while(!feof(infp)) {
		if (*line=='\n') valid=1;
		sscanf(line,"%s",id);
		if (valid && !strcmp(id,"PT:")) {
			if (++cnt!=num) fputs(line,outfp);
			valid=0;
			}
		fgets(line,82,infp);
		if (!valid && cnt!=num) fputs(line,outfp);
		}
	fclose(infp);
	fclose(outfp);
	unlink(infile);
	rename("tempfile",infile);
	check_messages(user,1);
     sprintf(text,wipe_user_one_message);
	write_user(user,text);
     sprintf(text,"%s wiped one message from the board in the %s.\n",user->name,rm->recap);
	write_syslog(text,1);
     sprintf(text,wipe_room_one_message,name);
	write_room_except(rm,text,user);
	return;
	}
if (range) {
	cnt=0;
	valid=1;
	if (cnt_um==total) {
		unlink(infile); /* cos it'll be empty anyway */
                write_user(user,wipe_user_all_deleted);
		user->room->mesg_cnt=0;
		sprintf(text,"%s wiped all messages from the board in the %s.\n",user->name,rm->recap);
		write_syslog(text,1);
		return;
		}
	if (num_one>cnt_um) {
          sprintf(text,wipe_missing_number,cnt_um,num_one);
		write_user(user,text);
		print_useage(user,1);
		return;
		}
	fgets(line,82,infp); /* max of 80+newline+terminator = 82 */
	while(!feof(infp)) {
		if (*line=='\n') valid=1;
		sscanf(line,"%s",id);
		if (valid && !strcmp(id,"PT:")) {
			cnt++;
			if (cnt<num || cnt>num_one) fputs(line,outfp);
			valid=0;
			}
		fgets(line,82,infp);
		if (!valid && (cnt<num || cnt>num_one)) fputs(line,outfp);
		}
	fclose(infp);
	fclose(outfp);
	unlink(infile);
	rename("tempfile",infile);
	check_messages(user,1);
     sprintf(text,wipe_user_delete_range,total);
	write_user(user,text);
     sprintf(text,"%s wiped %d messages from the board in the %s.\n",user->name,total,rm->recap);
	write_syslog(text,1);
     sprintf(text,wipe_room_all_deleted,name);
	write_room_except(rm,text,user);
	return;
	}
if (!just_one || !range) {
	cnt=0;
	valid=1;
	if (num>cnt_um) {
          sprintf(text,wipe_too_many,cnt_um,num);
		write_user(user,text);
		print_useage(user,1);
		return;
		}
	if (num_one>cnt_um) {
          sprintf(text,wipe_too_many,cnt_um,num_one);
		write_user(user,text);
		print_useage(user,1);
		return;
		}
	if (num_two>cnt_um ) {
          sprintf(text,wipe_too_many,cnt_um,num_two);
		write_user(user,text);
		print_useage(user,1);
		return;
		}
	if (num_three>cnt_um) {
          sprintf(text,wipe_too_many,cnt_um,num_three);
		write_user(user,text);
		print_useage(user,1);
		return;
		}
	if (num_four>cnt_um) {
          sprintf(text,wipe_too_many,cnt_um,num_four);
		write_user(user,text);
		print_useage(user,1);
		return;
		}
	if (cnt_um==total) {
		unlink(infile); /* cos it'll be empty anyway */
                write_user(user,wipe_user_all_deleted);
		user->room->mesg_cnt=0;
		sprintf(text,"%s wiped all messages from the board in the %s.\n",user->name,rm->recap);
		write_syslog(text,1);
		return;
		}
	fgets(line,82,infp); /* max of 80+newline+terminator = 82 */
	while(!feof(infp)) {
		if (*line=='\n') valid=1;
		sscanf(line,"%s",id);
		if (valid && !strcmp(id,"PT:")) {
			cnt++;
			switch(total) {
				case  2:
					if (cnt!=num && cnt!=num_one) fputs(line,outfp);
					break;
				case  3:
					if (cnt!=num && cnt!=num_one && cnt!=num_two) fputs(line,outfp);
					break;
				case  4:
					if (cnt!=num && cnt!=num_one && cnt!=num_two && cnt!=num_three) fputs(line,outfp);
					break;
				case  5:
					if (cnt!=num && cnt!=num_one && cnt!=num_two && cnt!=num_three && cnt!=num_four) fputs(line,outfp);
					break;
				}
			valid=0;
			}
		fgets(line,82,infp);
		switch(total) {
			case  2:
				if (!valid && (cnt!=num && cnt!=num_one)) fputs(line,outfp);
				break;
			case  3:
				if (!valid && (cnt!=num && cnt!=num_one && cnt!=num_two)) fputs(line,outfp);
				break;
			case  4:
				if (!valid && (cnt!=num && cnt!=num_one && cnt!=num_two && cnt!=num_three)) fputs(line,outfp);
				break;
			case  5:
				if (!valid && (cnt!=num && cnt!=num_one && cnt!=num_two && cnt!=num_three && cnt!=num_four)) fputs(line,outfp);
				break;
			}
		}
	fclose(infp);
	fclose(outfp);
	unlink(infile);
	rename("tempfile",infile);
	check_messages(user,1);
     sprintf(text,wipe_user_delete_range,total);
	write_user(user,text);
	sprintf(text,"%s wiped %d message from the board in the %s.\n",user->name,total,rm->recap);
	write_syslog(text,1);
     sprintf(text,wipe_deleted_some,name,total);
	write_room_except(rm,text,user);
	return;
	}
write_user(user,"~CRUnknown error in wipe\n");
print_useage(user,1);
}

/*** Search all the boards for the words given in the list. Rooms fixed to
	private will be ignore if the users level is less than gatecrash_level ***/
void search_boards(UR_OBJECT user)
{
RM_OBJECT rm;
FILE *fp;
char filename[80],line[82],buff[(MAX_LINES+1)*82],w1[81];
int w,cnt,message,yes,room_given;

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (word_count<2) {
	write_user(user,"Usage: search <word list>\n");  return;
	}
/* Go through rooms */
cnt=0;
for(rm=room_first;rm!=NULL;rm=rm->next) {
	sprintf(filename,"%s/%s.B",DATAFILES,rm->name);
	if (!(fp=fopen(filename,"r"))) continue;
	if (!has_room_access(user,rm)) {  fclose(fp);  continue;  }
	/* Go through file */
	fgets(line,81,fp);
	yes=0;  message=0;
	room_given=0;  buff[0]='\0';
	while(!feof(fp)) {
		if (*line=='\n') {
			if (yes) {  strcat(buff,"\n");  write_user(user,buff);  }
			message=0;  yes=0;  buff[0]='\0';
			}
		if (!message) {
			w1[0]='\0';
			sscanf(line,"%s",w1);
			if (!strcmp(w1,"PT:")) {
				message=1;
				strcpy(buff,remove_first(remove_first(line)));
				}
			}
		else strcat(buff,line);
		for(w=1;w<word_count;++w) {
			if (!yes && strstr(line,word[w])) {
				if (!room_given) {
                         sprintf(text,"~BM*** %s~BM ***\n\n",rm->recap);
					write_user(user,text);
					room_given=1;
					}
				yes=1;  cnt++;
				}
			}
		fgets(line,81,fp);
		}
	if (yes) {  strcat(buff,"\n");  write_user(user,buff);  }
	fclose(fp);
	}
if (cnt) {
	sprintf(text,"Total of %d matching messages.\n\n",cnt);
	write_user(user,text);
	}
else write_user(user,"No occurences found.\n");
}


/********************************** MAIL COMMANDS *************************/

/*** Read your mail ***/
void rmail(UR_OBJECT user)
{
FILE *infp,*outfp;
int ret;
char c,filename[80],line[DNL+1];

sprintf(filename,"%s/%s.M",USERFILES,user->name);
if (!(infp=fopen(filename,"r"))) {
     write_user(user,nomail_prompt);  return;
	}
/* Update last read / new mail received time at head of file */
if ((outfp=fopen("tempfile","w"))) {
	fprintf(outfp,"%d\r",(int)(time(0)));
	/* skip first line of mail file */
	fgets(line,DNL,infp);

	/* Copy rest of file */
	c=getc(infp);
	while(!feof(infp)) {  putc(c,outfp);  c=getc(infp);  }

	fclose(outfp);
	rename("tempfile",filename);
	}
user->read_mail=time(0);
fclose(infp);
count_messages(user);
if (user->high_ascii) {
     write_user(user,"~CT~BM⁄ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒø~RS\n");
     write_user(user,"~CT~BM≥                            Your Smail Box                                 ~FT≥~RS\n");
     write_user(user,"~CT~BM¿ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒŸ~RS\n");
     }
else {
     write_user(user,"\n~FM------------------------------------------------------------------------------\n");
     write_user(user,"~FG                          Your Smail Box\n");
     write_user(user,"~FM------------------------------------------------------------------------------\n");
     }
ret=more(user,user->socket,filename);
if (ret==1) user->misc_op=2;
}



/*** Send mail message ***/
void smail(UR_OBJECT user,char *inpstr,int done_editing)
{
UR_OBJECT u;
FILE *fp;
char filename[80],*center();

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (user->muzzled & 1) {
	write_user(user,"You are muzzled, you cannot mail anyone.\n");
	return;
	}
if (done_editing) {
	send_mail(user,user->mail_to,user->malloc_start);
	user->mail_to[0]='\0';
	return;
	}
if (word_count<2) {
	write_user(user,"Smail who?\n");
	return;
	}
if (user->muzzled & FROZEN) {
	word[1][0]=toupper(word[1][0]);
     sprintf(text,sendmail_prompt,word[1]);
	write_user(user,text);
	return;
	}
word[1][0]=toupper(word[1][0]);
u=NULL;
if (!(u=get_exact_user(word[1]))) {
        sprintf(filename,"%s/%s.D",USERFILES,word[1]);
        if (!(fp=fopen(filename,"r"))) {
                write_user(user,nosuchuser);
                return;
                }
        fclose(fp);
        }
if (u!=NULL) strcpy(word[1],u->name);
if (word_count>2) {
	/* One line mail */
	strcat(inpstr,"\n");
	send_mail(user,word[1],remove_first(inpstr));
	return;
	}
sprintf(text,smail_edit_header,word[1]);
write_user(user,center(text,78));
user->misc_op=4;
strcpy(user->mail_to,word[1]);
editor(user,NULL);
}

void dmail(UR_OBJECT user)
{
int cnt,cnt_um,valid,just_one,range,total;
int num,num_one,num_two,num_three,num_four,num_five,num_six;
char infile[80],line[82],id[82];
FILE *infp,*outfp;

range=num_two=num_three=num_four=0;
if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
just_one=1;
total=1;
if (word_count<2 || ((num=atoi(word[1]))<1 && strcmp(word[1],"all") && strcmp(word[1],"top") && strcmp(word[1],"bottom"))) {
	print_useage(user,0);
	return;
	}
else {
	if (!strcmp(word[1],"top") && (num=atoi(word[2]))<1) {
		print_useage(user,0);
		return;
		}
	if (!strcmp(word[1],"bottom") && (num=atoi(word[2]))<1) {
		print_useage(user,0);
		return;
		}
	if (!strcmp(word[2],"-")) {
		(num_one=atoi(word[3]));
		range=1;
		just_one=0;
		total=(num_one-num)+1;
		if (total<2) {
			total=0;
			range=0;
			just_one=0;
			}
		goto GOT_IT;
		}
	if ((num_one=atoi(word[2]))<1) {
		just_one=1;
		range=0;
		total=1;
		goto GOT_IT;
		}
	if ((num_two=atoi(word[3]))<1) {
		just_one=0;
		range=0;
		total=2;
		num_three=0;
		num_four=0;
		num_five=0;
		goto GOT_IT;
		}
	if ((num_three=atoi(word[4]))<1) {
		just_one=0;
		range=0;
		total=3;
		num_four=0;
		num_five=0;
		goto GOT_IT;
		}
	if ((num_four=atoi(word[5]))<1) {
		just_one=0;
		range=0;
		total=4;
		num_five=0;
		goto GOT_IT;
		}
	if ((num_five=atoi(word[6]))<1) {
		just_one=0;
		range=0;
		total=5;
		goto GOT_IT;
		}
	if ((num_six=atoi(word[7]))!=0) {
		just_one=0;
		range=0;
		total=0;
		}
	}
GOT_IT:
if (!total && !just_one && !range) {
	print_useage(user,0);
	return;
	}
sprintf(infile,"%s/%s.M",USERFILES,user->name);
if (!(infp=fopen(infile,"r"))) {
     write_user(user,dmail_nomail);
	return;
	}
if (!strcmp(word[1],"all")) {
	fclose(infp);
	unlink(infile);
     write_user(user,dmail_all);
	return;
	}
if (!(outfp=fopen("tempfile","w"))) {
	sprintf(text,"%s: couldn't open tempfile.\n",syserror);
	write_user(user,text);
	write_syslog("ERROR: Couldn't open tempfile in dmail().\n",0);
	fclose(infp);
	return;
	}
if (!strcmp(word[1],"top")) {
	cnt=0; valid=1;
	fgets(line,DNL,infp);
	fputs(line,outfp);
	fgets(line,82,infp); /* max of 80+newline+terminator = 82 */
	while(!feof(infp)) {
		if (cnt<=num) {
			if (*line=='\n') valid=1;
			sscanf(line,"%s",id);
               if (valid && (!strcmp(colour_com_strip(id),"From:"))) {
				if (++cnt>num) fputs(line,outfp);
				valid=0;
				}
			}
		else fputs(line,outfp);
		fgets(line,82,infp);
		}
	fclose(infp);
	fclose(outfp);
	unlink(infile);
	if (cnt<num) {
		unlink("tempfile");
          sprintf(text,dmail_too_many,cnt);
		write_user(user,text);
		return;
		}
	if (cnt==num) {
		unlink("tempfile"); /* cos it'll be empty anyway */
          write_user(user,dmail_all);
		return;
		}
	rename("tempfile",infile);
     sprintf(text,dmail_some,num);
	write_user(user,text);
	return;
	}
cnt_um=0;
cnt=0;
valid=1;
fgets(line,DNL,infp);
fgets(line,82,infp); /* max of 80+newline+terminator = 82 */
while(!feof(infp)) {
	if (*line=='\n') valid=1;
	sscanf(line,"%s",id);
     if (valid && (!strcmp(colour_com_strip(id),"From:"))) {
		cnt_um++;
		valid=0;
		}
	fgets(line,82,infp);
	}
fclose(infp);
sprintf(infile,"%s/%s.M",USERFILES,user->name);
if (!(infp=fopen(infile,"r"))) return;
if (!strcmp(word[1],"bottom")) {
	cnt=0;
	valid=1;
	total=num;
	num=cnt_um-total;
	num++;
	if (num<0) {
		fclose(infp);
		unlink(infile);
          sprintf(text,dmail_too_many,total);
		write_user(user,text);
		return;
		}
	fgets(line,DNL,infp);
	fputs(line,outfp);
	fgets(line,82,infp); /* max of 80+newline+terminator = 82 */
	while(!feof(infp)) {
		if (*line=='\n') valid=1;
		sscanf(line,"%s",id);
          if (valid && (!strcmp(colour_com_strip(id),"From:"))) {
			if (++cnt<num) fputs(line,outfp);
			valid=0;
			}
		fgets(line,82,infp);
		if (!valid && cnt<num) fputs(line,outfp);
		}
	fclose(infp);
	fclose(outfp);
	unlink(infile);
	if (cnt==total) {
		unlink("tempfile"); /* cos it'll be empty anyway */
                write_user(user,"~CRAll messages deleted.\n");
		return;
		}
	rename("tempfile",infile);
     sprintf(text,dmail_bottom,total);
	write_user(user,text);
	return;
	}
if (just_one) {
	cnt=0;
	valid=1;
	if (num>cnt_um) {
          sprintf(text,dmail_no_number,cnt_um,num);
		write_user(user,text);
		print_useage(user,0);
		return;
		}
	if (cnt_um==1) {
		unlink(infile); /* cos it'll be empty anyway */
          write_user(user,dmail_all);
		return;
		}
	fgets(line,DNL,infp);
	fputs(line,outfp);
	fgets(line,82,infp); /* max of 80+newline+terminator = 82 */
	while(!feof(infp)) {
		if (*line=='\n') valid=1;
		sscanf(line,"%s",id);
          if (valid && (!strcmp(colour_com_strip(id),"From:"))) {
			if (++cnt!=num) fputs(line,outfp);
			valid=0;
			}
		fgets(line,82,infp);
		if (!valid && cnt!=num) fputs(line,outfp);
		}
	fclose(infp);
	fclose(outfp);
	unlink(infile);
	rename("tempfile",infile);
     sprintf(text,dmail_one_message);
	write_user(user,text);
	return;
	}
if (range) {
	cnt=0;
	valid=1;
	if (cnt_um==total) {
		unlink(infile); /* cos it'll be empty anyway */
                write_user(user,"~CRAll messages deleted.\n");
		return;
		}
	if (num_one>cnt_um) {
          sprintf(text,dmail_no_number,cnt_um,num_one);
		write_user(user,text);
		print_useage(user,0);
		return;
		}
	fgets(line,DNL,infp);
	fputs(line,outfp);
	fgets(line,82,infp); /* max of 80+newline+terminator = 82 */
	while(!feof(infp)) {
		if (*line=='\n') valid=1;
		sscanf(line,"%s",id);
          if (valid && (!strcmp(colour_com_strip(id),"From:"))) {
			cnt++;
			if (cnt<num || cnt>num_one) fputs(line,outfp);
			valid=0;
			}
		fgets(line,82,infp);
		if (!valid && (cnt<num || cnt>num_one)) fputs(line,outfp);
		}
	fclose(infp);
	fclose(outfp);
	unlink(infile);
	rename("tempfile",infile);
     sprintf(text,dmail_some_messages,total);
	write_user(user,text);
	return;
	}
if (!just_one || !range) {
	cnt=0;
	valid=1;
	if (num>cnt_um) {
          sprintf(text,dmail_no_number,cnt_um,num);
		write_user(user,text);
		print_useage(user,0);
		return;
		}
	if (num_one>cnt_um) {
          sprintf(text,dmail_no_number,cnt_um,num_one);
		write_user(user,text);
		print_useage(user,0);
		return;
		}
	if (num_two>cnt_um ) {
          sprintf(text,dmail_no_number,cnt_um,num_two);
		write_user(user,text);
		print_useage(user,0);
		return;
		}
	if (num_three>cnt_um) {
          sprintf(text,dmail_no_number,cnt_um,num_three);
		write_user(user,text);
		print_useage(user,0);
		return;
		}
	if (num_four>cnt_um) {
          sprintf(text,dmail_no_number,cnt_um,num_four);
		write_user(user,text);
		print_useage(user,0);
		return;
		}
	if (cnt_um==total) {
		unlink(infile); /* cos it'll be empty anyway */
          write_user(user,dmail_all);
		return;
		}
	fgets(line,DNL,infp);
	fputs(line,outfp);
	fgets(line,82,infp); /* max of 80+newline+terminator = 82 */
	while(!feof(infp)) {
		if (*line=='\n') valid=1;
		sscanf(line,"%s",id);
          if (valid && (!strcmp(colour_com_strip(id),"From:"))) {
			cnt++;
			switch(total) {
				case  2:
					if (cnt!=num && cnt!=num_one) fputs(line,outfp);
					break;
				case  3:
					if (cnt!=num && cnt!=num_one && cnt!=num_two) fputs(line,outfp);
					break;
				case  4:
					if (cnt!=num && cnt!=num_one && cnt!=num_two && cnt!=num_three) fputs(line,outfp);
					break;
				case  5:
					if (cnt!=num && cnt!=num_one && cnt!=num_two && cnt!=num_three && cnt!=num_four) fputs(line,outfp);
					break;
				}
			valid=0;
			}
		fgets(line,82,infp);
		switch(total) {
			case  2:
				if (!valid && (cnt!=num && cnt!=num_one)) fputs(line,outfp);
				break;
			case  3:
				if (!valid && (cnt!=num && cnt!=num_one && cnt!=num_two)) fputs(line,outfp);
				break;
			case  4:
				if (!valid && (cnt!=num && cnt!=num_one && cnt!=num_two && cnt!=num_three)) fputs(line,outfp);
				break;
			case  5:
				if (!valid && (cnt!=num && cnt!=num_one && cnt!=num_two && cnt!=num_three && cnt!=num_four)) fputs(line,outfp);
				break;
			}
		}
	fclose(infp);
	fclose(outfp);
	unlink(infile);
	rename("tempfile",infile);
     sprintf(text,dmail_some_messages,total);
	write_user(user,text);
	return;
	}
write_user(user,"~CRUnknown error in wipe\n");
print_useage(user,1);
}

/*** Show list of people your mail is from without seeing the whole lot ***/
void mail_from(UR_OBJECT user)
{
FILE *fp;
int valid,cnt;
char id[ARR_SIZE],line[ARR_SIZE],filename[80],*colour_com_strip();

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
sprintf(filename,"%s/%s.M",USERFILES,user->name);
if (!(fp=fopen(filename,"r"))) {
     write_user(user,nomail_prompt);  return;
	}
if (user->high_ascii) {
     write_user(user,"~CT~BM⁄ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒø~RS\n");
     write_user(user,"~CT~BM≥                   You have mail from the following people                 ~FT≥~RS\n");
     write_user(user,"~CT~BM¿ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒŸ~RS\n");
     }
else {
     write_user(user,"\n~FM------------------------------------------------------------------------------\n");
     write_user(user,"~FG                  You have mail from the following people\n");
     write_user(user,"~FM------------------------------------------------------------------------------\n");
     }
valid=1;  cnt=0;
fgets(line,DNL,fp);
fgets(line,ARR_SIZE-1,fp);
while(!feof(fp)) {
	if (*line=='\n') valid=1;
	sscanf(line,"%s",id);
        if (valid && (!strcmp(colour_com_strip(id),"From:"))) {
		write_user(user,remove_first(line));
		cnt++;  valid=0;
		}
	fgets(line,ARR_SIZE-1,fp);
	}
fclose(fp);
if (user->high_ascii) write_user(user,"\n~CBƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
else write_user(user,"\n~FB-------------------------------------------------------------------------------\n");
sprintf(text,mailbox_total,cnt);
write_user(user,text);
if (user->high_ascii) write_user(user,"~CBƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n\n");
else write_user(user,"~FB-------------------------------------------------------------------------------\n\n");
}

void count_messages(UR_OBJECT user)
{
FILE *infp,*outfp;
char id[82],filename[80],line[82],name[80],dash[80];
char *colour_com_strip();
int mail_cnt,msg_num,valid;

mail_cnt=0;
sprintf(filename,"%s/%s.M",USERFILES,user->name);
if (!(infp=fopen(filename,"r"))) return;
if (!(outfp=fopen("tempfile","w"))) {
	write_syslog("ERROR: Couldn't open tempfile in check_messages().\n",0);
	fclose(infp);
	return;
	}
mail_cnt=0;
valid=1;
fgets(line,DNL,infp);
fputs(line,outfp);
fgets(line,82,infp); /* max of 80+newline+terminator = 82 */
while(!feof(infp)) {
	if (*line=='\n') valid=1;
	sscanf(line,"%s %s %s %d ",id,name,dash,&msg_num);
     if (valid && (!strcmp(colour_com_strip(id),"From:"))) {
		mail_cnt++;
		msg_num=mail_cnt;
		sprintf(line,"%s %s - %d\n",id,name,msg_num);
		fputs(line,outfp);
		valid=0;
		}
	else fputs(line,outfp);
	fgets(line,82,infp);
	}
fclose(infp);
fclose(outfp);
unlink(filename);
rename("tempfile",filename);
}

/********************************** CLONE COMMANDS ************************/

/*** Clone a user in another room ***/
void create_clone(UR_OBJECT user)
{
UR_OBJECT u;
RM_OBJECT rm;
int cnt;

/* Check room */
if (word_count<2) rm=user->room;
else {
	if ((rm=get_room(word[1]))==NULL) {
		write_user(user,nosuchroom);
                return;
		}
	}
/* If room is private then nocando */
if (!has_room_access(user,rm)) {
	write_user(user,"That room is currently private, you cannot create a clone there.\n");
	return;
	}
/* If room is personal and you are not invited in there */
if (((rm->access==FIXED_PERSONAL && user->invite_room!=rm)
    && user->level<OWNER) && strcmp(user->name,rm->owner)) {
        write_user(user,"~CRThat room is ~FTPERSONAL ~FRand you have no invite so you cannot create a clone there.\n");
	return;
	}
/* Count clones and see if user already has a copy there , no point having
   2 in the same room */
cnt=0;
for(u=user_first;u!=NULL;u=u->next) {
	if (u->type==CLONE_TYPE && u->owner==user) {
		if (u->room==rm) {
			sprintf(text,"You already have a clone in the %s.\n",rm->recap);
			write_user(user,text);
			return;
			}
		if (++cnt==max_clones) {
			write_user(user,"You already have the maximum number of clones allowed.\n");
			return;
			}
		}
	}
/* Create clone */
if ((u=create_user())==NULL) {
	sprintf(text,"%s: Unable to create copy.\n",syserror);
	write_user(user,text);
	write_syslog("ERROR: Unable to create user copy in clone().\n",0);
	return;
	}
u->type=CLONE_TYPE;
u->socket=user->socket;
u->room=rm;
u->owner=user;
strcpy(u->name,user->name);
strcpy(u->desc,user->desc);
strcpy(u->recap,user->recap);
strcpy(u->level_alias,user->level_alias);
strcpy(u->predesc,user->predesc);

if (rm==user->room) write_user(user,clone_here_prompt);
else {
	sprintf(text,clone_prompt,rm->recap);
	write_user(user,text);
	}
sprintf(text,"%s %s\n",user->recap,user->in_phrase);
write_room_except(rm,text,user);
}

/*** Destroy user clone ***/
void destroy_clone(UR_OBJECT user)
{
UR_OBJECT u,u2;
RM_OBJECT rm;
char *name;

/* Check room and user */
if (word_count<2) rm=user->room;
else {
	if ((rm=get_room(word[1]))==NULL) {
		write_user(user,nosuchroom);
          return;
		}
	}
if (word_count>2) {
	if ((u2=get_user(word[2]))==NULL) {
		write_user(user,notloggedon);
          return;
		}
	if (u2->level>=user->level && user->level<OWNER) {
		write_user(user,"You cannot destroy the clone of a user of an equal or higher level.\n");
		return;
		}
	}
else u2=user;
for(u=user_first;u!=NULL;u=u->next) {
	if (u->type==CLONE_TYPE && u->room==rm && u->owner==u2) {
		destruct_user(u);
		reset_access(rm);
          write_user(user,clone_user_destroy);
		if (user->vis) name=user->name; else name=invisname;
          sprintf(text,clone_room_destroy,u2->recap);
		write_room(rm,text);
		if (u2!=user) {
               sprintf(text,"\n~CB[~CYM~CYoenuts~CB]~CW: ~FM%s has destroyed your clone in the %s.\n",user->name,rm->recap);
			write_user(u2,text);
			}
		destructed=0;
		return;
		}
	}
if (u2==user) sprintf(text,"You do not have a clone in the %s.\n",rm->recap);
else sprintf(text,"%s does not have a clone the %s.\n",u2->recap,rm->recap);
write_user(user,text);
}

/*** Show users who own clones ***/
void myclones(UR_OBJECT user)
{
UR_OBJECT u;
int cnt;

cnt=0;
for(u=user_first;u!=NULL;u=u->next) {
	if (u->type!=CLONE_TYPE || u->owner!=user) continue;
     if (++cnt==1) {
     if (user->high_ascii) {
          write_user(user,"~CT~BM⁄ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒø~RS\n");
          write_user(user,"~CT~BM≥                     Rooms You Currently Have Clones In                     ~FT≥~RS\n");
          write_user(user,"~CT~BM¿ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒŸ~RS\n");
          }
     else {
          write_user(user,"\n~FM-------------------------------------------------------------------------------\n");
          write_user(user,"~FG                       Rooms You Currently Have Clones In\n");
          write_user(user,"~FM-------------------------------------------------------------------------------\n\n");
          }
     }
     sprintf(text,"Room     : %s\n",u->room->recap);
	write_user(user,text);
	}
if (!cnt) write_user(user,"You have no clones.\n");
else {
if (user->high_ascii) write_user(user,"~CMƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n\n");
else write_user(user,"~FM-------------------------------------------------------------------------------\n\n");
sprintf(text,"~FRYou have a total of %d clones.\n",cnt);
write_user(user,text);
if (user->high_ascii) write_user(user,"~CMƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n\n");
else write_user(user,"~FM-------------------------------------------------------------------------------\n\n");
	}
}

/*** Show all clones on the system ***/
void allclones(UR_OBJECT user)
{
UR_OBJECT u;
int cnt;

cnt=0;
for(u=user_first;u!=NULL;u=u->next) {
	if (u->type!=CLONE_TYPE) continue;
	if (++cnt==1) {
          if (user->high_ascii) write_user(user,"~CMƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
          else write_user(user,"~CM-------------------------------------------------------------------------------\n");
          sprintf(text," ~CTCurrent clones %s \n",long_date(1));
          write_user(user,text);
          if (user->high_ascii) write_user(user,"~CMƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
          else write_user(user,"~CM-------------------------------------------------------------------------------\n");
          }
     sprintf(text,all_clone_style,u->name,u->room);
	write_user(user,text);
	}
     if (user->high_ascii) write_user(user,"~CMƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
     else write_user(user,"~CM-------------------------------------------------------------------------------\n");
     if (!cnt) write_user(user," ~FRThere are no clones on the system.\n");
else {
     sprintf(text," ~FGTotal of %d clones.\n",cnt);
	write_user(user,text);
     }
if (user->high_ascii) write_user(user,"~CMƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n\n");
else write_user(user,"~CM-------------------------------------------------------------------------------\n\n");
}

/*** User swaps places with his own clone. All we do is swap the rooms the
	objects are in. ***/
void clone_switch(UR_OBJECT user)
{
UR_OBJECT u;
RM_OBJECT rm;

if (word_count<2) {
	write_user(user,"Usage: switch <room clone is in>\n");  return;
	}
if ((rm=get_room(word[1]))==NULL) {
	write_user(user,nosuchroom);  return;
	}
for(u=user_first;u!=NULL;u=u->next) {
	if (u->type==CLONE_TYPE && u->room==rm && u->owner==user) {
          write_user(user,clone_switch_prompt);
		u->room=user->room;
		user->room=rm;
		look(user);
		return;
		}
	}
write_user(user,"~CRYou do not have a clone in that room.\n");
}

/*** Make a clone speak ***/
void clone_say(UR_OBJECT user,char *inpstr)
{
RM_OBJECT rm;
UR_OBJECT u;

if (user->muzzled) {
	write_user(user,"You are muzzled, your clone cannot speak.\n");
	return;
	}
if (word_count<3) {
     write_user(user,"Usage: .csay <room clone is in> <message>\n");
	return;
	}
if ((rm=get_room(word[1]))==NULL) {
	write_user(user,nosuchroom);  return;
	}
for(u=user_first;u!=NULL;u=u->next) {
	if (u->type==CLONE_TYPE && u->room==rm && u->owner==user) {
		say(u,remove_first(inpstr));  return;
		}
	}
write_user(user,"You do not have a clone in that room.\n");
}

/*** Make a clone emote ***/
void clone_emote(UR_OBJECT user,char *inpstr)
{
RM_OBJECT rm;
UR_OBJECT u;

if (user->muzzled) {
	write_user(user,"You are muzzled, your clone cannot emote.\n");
	return;
	}
if (word_count<3) {
     write_user(user,"Usage: cemote <room clone is in> <message>\n");
	return;
	}
if ((rm=get_room(word[1]))==NULL) {
	write_user(user,nosuchroom);
     return;
	}
for(u=user_first;u!=NULL;u=u->next) {
	if (u->type==CLONE_TYPE && u->room==rm && u->owner==user) {
		emote(u,remove_first(inpstr));
          return;
          }
	}
write_user(user,"You do not have a clone in that room.\n");
}

/*** Set what a clone will hear, either all Speech , just bad language
	or nothing. ***/
void clone_hear(UR_OBJECT user)
{
RM_OBJECT rm;
UR_OBJECT u;

if (word_count<3
    || (strcmp(word[2],"all")
	    && strcmp(word[2],"swears")
	    && strcmp(word[2],"nothing"))) {
	write_user(user,"Usage: chear <room clone is in> all/swears/nothing\n");
	return;
	}
if ((rm=get_room(word[1]))==NULL) {
	write_user(user,nosuchroom);  return;
	}
for(u=user_first;u!=NULL;u=u->next) {
	if (u->type==CLONE_TYPE && u->room==rm && u->owner==user) break;
	}
if (u==NULL) {
	write_user(user,"You do not have a clone in that room.\n");
	return;
	}
if (!strcmp(word[2],"all")) {
	u->clone_hear=CLONE_HEAR_ALL;
	write_user(user,"Clone will now hear everything.\n");
	return;
	}
if (!strcmp(word[2],"swears")) {
	u->clone_hear=CLONE_HEAR_SWEARS;
	write_user(user,"Clone will now only hear swearing.\n");
	return;
	}
u->clone_hear=CLONE_HEAR_NOTHING;
write_user(user,"Clone will now hear nothing.\n");
}

/***************************** STAFF LEVEL COMMANDS ***********************/

/*** Kill a user ***/
void kill_user(UR_OBJECT user)
{
UR_OBJECT victim;
RM_OBJECT rm;
char *name;

if (word_count<2) {
	write_user(user,"Usage: kill <user>\n");
     return;
	}
if (!(victim=get_user(word[1]))) {
	write_user(user,notloggedon);
     return;
	}
if (user==victim) {
	write_user(user,"Trying to commit suicide this way is the sixth sign of madness.\n");
	return;
	}
if (victim->level>=user->level && user->level<OWNER) {
	write_user(user,"You cannot kill a user of equal or higher level than yourself.\n");
	sprintf(text,"%s tried to kill you!\n",user->recap);
	write_user(victim,text);
	return;
	}
sprintf(text,"%s KILLED %s.\n",user->name,victim->name);
write_syslog(text,1);
sprintf(text,"%s KILLED %s.\n",user->name,victim->name);
write_arrestrecord(victim,text,1);
write_user(user,kill_user_chant);
if (user->vis) name=user->recap; else name=invisname;
sprintf(text,kill_room_chant,name);
write_room_except(user->room,text,user);
sprintf(file,"%s",KILLEDFILE);
show_screen(victim);
write_user(victim,kill_victim_text);
sprintf(text,kill_room_text,victim->recap);
rm=victim->room;
write_room_except(rm,text,victim);
disconnect_user(victim);
write_room(NULL,kill_all_text);
}

/*** Removes a user silently ***/
void remove_user(UR_OBJECT user)
{
UR_OBJECT victim;
char *name;

if (word_count<2) {
	write_user(user,"Usage: remove <user>\n");
     return;
	}
if (!(victim=get_user(word[1]))) {
	write_user(user,notloggedon);
     return;
	}
if (user==victim) {
     write_user(user,"~CRTrying to commit suicide this way is a sign of madness!\n");
	return;
	}
if (victim->level>=user->level && user->level<OWNER) {
	write_user(user,"You cannot rmove a user of equal or higher level than yourself.\n");
        sprintf(text,"%s tried to boot you off %s!\n",user->recap,TALKERNAME);
	write_user(victim,text);
	return;
	}
sprintf(text,"%s removed %s.\n",user->name,victim->name);
write_syslog(text,1);
sprintf(text,"%s removed %s.\n",user->name,victim->name);
write_arrestrecord(victim,text,1);
if (user->vis) name=user->recap; else name=invisname;
/*
sprintf(text,"~CW[~FMWiZ~FW]: ~FM%s's~RS~CR connection was terminated by ~FM%s~RS~FY!!!\n",victim->recap,name);
write_level(WIZ,3,text,NULL);
*/
disconnect_user(victim);
}

/*** Promote a user ***/
void promote(UR_OBJECT user)
{
UR_OBJECT u;
RM_OBJECT rm;
char text2[80],*name;

if (user->temp_level>0) {
     write_user(user,"~CRYou are not permitted to use this command!\n");
     return;
     }
if (word_count<2) {
        write_user(user,"Usage: promote <user>\n");
        return;
	}
/* See if user is on atm */
if ((u=get_user(word[1]))!=NULL) {
     if (u->level==NEW && u->muzzled & SCUM) {
          sprintf(text,"~CGYou promote %s ~CGto level: ~CWNEWBIE~RS.\n",u->recap);
          write_user(user,text);
          if (user->vis) name=user->recap; else name=invisname;
                sprintf(text,"~CG%s~CG has promotes you to level: ~CWNEWBIE\n",name);
		write_user(u,text);
                sprintf(text,"%s PROMOTED %s to level NEW.\n",user->name,u->recap);
		write_syslog(text,1);
                sprintf(text,"%s PROMOTED %s to level NEW.\n",user->name,u->recap);
		write_arrestrecord(u,text,1);
		u->muzzled-=SCUM;
		return;
		}
     if (u==user) {
                write_user(user,"~CRTrying to get a higher promotion this way is useless...\n");
		return;
		}
     if (u->level>=user->level) {
		write_user(user,"You cannot promote a user to a level higher than your own.\n");
		return;
		}
     if (u->level==USER && user->level==WIZ) {
		write_user(user,"You cannot promote a user to the same level as your own.\n");
		return;
		}

/***************************************************************************

  These two blocks have no role in my talkers, but you may want to enable
  them for your own if you add more levels.             -> TIME PROMOTION **

 ***************************************************************************

     if (u->level==USER && user->level==WIZ && u->total_login<(86400)) {
          write_user(user,"Users cannot be promoted untill they have a total login time of one day.\n");
		return;
		}
     if (u->level==USER && user->level==GOD && u->total_login<(86400)) {
          write_user(user,"You cannot promote a user till their total login time is more than one day.\n");
		return;
          }

****************************************************************************/

	if (user->vis) name=user->recap; else name=invisname;
	u->level++;
        sprintf(text,promote_user_prompt,u->recap,level_name[u->level]);
	write_user(user,text);
	rm=user->room;
	user->room=NULL;
        sprintf(text,promote_prompt,name,u->recap,level_name[u->level]);
        write_room(NULL,text);
	user->room=rm;
	sprintf(text,"%s PROMOTED %s to level %s.\n",user->name,u->name,level_name[u->level]);
	write_syslog(text,1);
	sprintf(text,"%s PROMOTED %s to level %s.\n",user->name,u->name,level_name[u->level]);
	write_arrestrecord(u,text,1);
	return;
	}

/* Create a temp session, load details, alter , then save. This is inefficient
   but its simpler than the alternative */

if ((u=create_user())==NULL) {
	sprintf(text,"%s: unable to create temporary user object.\n",syserror);
	write_user(user,text);
	write_syslog("ERROR: Unable to create temporary user object in promote().\n",0);
	return;
	}
strcpy(u->name,word[1]);
if (!load_user_details(u)) {
	write_user(user,nosuchuser);
	destruct_user(u);
	destructed=0;
	return;
	}
if (u->level==NEW && u->muzzled & SCUM) {
        sprintf(text,"~CGYou promote %s~CB to level: ~CWNEWBIE\n",u->recap);
	write_user(user,text);
	if (user->vis) name=user->recap; else name=invisname;
        sprintf(text2,"~CG%s~CG has promoted you to level: ~CWNEWBIE\n",name);
	send_mail(user,word[1],text2);
        sprintf(text,"%s PROMOTED %s to level NEW.\n",user->name,u->name);
	write_syslog(text,1);
        sprintf(text,"%s PROMOTED %s to level NEW.\n",user->name,u->name);
	write_arrestrecord(u,text,1);
	u->muzzled-=SCUM;
	u->socket=-2;
	strcpy(u->site,u->last_site);
	save_user_details(u,0);
	destruct_user(u);
	destructed=0;
	return;
	}
if (u->level>=user->level) {
        write_user(user,"~CRYou cannot promote a user to a level higher than your own.\n");
	destruct_user(u);
	destructed=0;
	return;
	}

/*** See Note Above ***  -> TIME PROMOTION **

if (u->level==USER && user->level==WIZ && u->total_login<(86400)) {
	write_user(user,"You cannot promote a USER till total login time is more than 2 days.\n");
	destruct_user(u);
	destructed=0;
	return;
	}
if (u->level==USER && user->level==GOD && u->total_login<(86400)) {
	write_user(user,"You cannot promote a USER till total login time is more than 1 day.\n");
	destruct_user(u);
	destructed=0;
	return;
     }

*** See Note Above ***/

if (u->level==USER && user->level==WIZ) {
	write_user(user,"You cannot promote a user to the same level as your own.\n");
	destruct_user(u);
	destructed=0;
	return;
	}
if (u->level==WIZ && user->level==GOD) {
	write_user(user,"You cannot promote a user to the same level as your own. If they are not loged in.\n");
	destruct_user(u);
	destructed=0;
	return;
	}
u->level++;
u->socket=-2;
strcpy(u->site,u->last_site);
save_user_details(u,0);
sprintf(text,"~CGYou promote %s~CG to level: ~CW%s.\n",u->recap,level_name[u->level]);
write_user(user,text);
sprintf(text2,"~CGYou have been promoted to level: ~CW%s.\n",level_name[u->level]);
send_mail(user,word[1],text2);
sprintf(text,"%s PROMOTED %s to level %s.\n",user->name,word[1],level_name[u->level]);
write_syslog(text,1);
sprintf(text,"%s PROMOTED %s to level %s.\n",user->name,word[1],level_name[u->level]);
write_arrestrecord(u,text,1);
destruct_user(u);
destructed=0;
}

/*** Demote a user ***/
void demote(UR_OBJECT user)
{
UR_OBJECT u;
RM_OBJECT rm;
char text2[80],*name;

if (user->temp_level>0) {
     write_user(user,"~CRYou are not permitted to use this command!\n");
     return;
     }

if (word_count<2) {
	write_user(user,"Usage: demote <user>\n");
     return;
	}

/* See if user is on atm */

if ((u=get_user(word[1]))!=NULL) {
     if (u->level==NEW && u->muzzled & SCUM) {
                write_user(user,"You can not demote a user to level lower than~CR CRIM~RS.\n");
		return;
		}
     if (u->level==NEW) {
          sprintf(text,"~CRYou demote %s~CR to level: ~CWPond Scum\n",u->recap);
		write_user(user,text);
		if (user->vis) name=user->recap; else name=invisname;
          sprintf(text,"~CM%s has demoted you to level: ~CWPond Scum!\n",name);
		write_user(u,text);
          sprintf(text,"%s DEMOTED %s to level SCUM.\n",user->name,u->name);
		write_syslog(text,1);
          sprintf(text,"%s DEMOTED %s to level SCUM.\n",user->name,u->name);
		write_arrestrecord(u,text,1);
		u->muzzled+=SCUM;
		return;
		}
	if (u->level>=user->level) {
		write_user(user,"You cannot demote a user of an equal or higher level than yourself.\n");
		return;
		}
	if (user->vis) name=user->recap; else name=invisname;
	u->level--;
     sprintf(text,demote_user_prompt,u->recap,level_name[u->level]);
	write_user(user,text);
	rm=user->room;
	user->room=NULL;
     sprintf(text,demote_prompt,name,u->recap,level_name[u->level]);
     write_room(NULL,text);
	user->room=rm;
	sprintf(text,"%s DEMOTED %s to level %s.\n",user->name,u->name,level_name[u->level]);
	write_syslog(text,1);
	sprintf(text,"%s DEMOTED %s to level %s.\n",user->name,u->name,level_name[u->level]);
	write_arrestrecord(u,text,1);
	return;
	}
/* User not logged on */
if ((u=create_user())==NULL) {
	sprintf(text,"%s: unable to create temporary user object.\n",syserror);
	write_user(user,text);
	write_syslog("ERROR: Unable to create temporary user object in demote().\n",0);
	return;
	}
strcpy(u->name,word[1]);
if (!load_user_details(u)) {
	write_user(user,nosuchuser);
	destruct_user(u);
	destructed=0;
	return;
	}
     if (u->level==NEW && u->muzzled & SCUM) {
                write_user(user,"You can not demote a user to level lower than~CR CRIM~RS.\n");
		destruct_user(u);
		destructed=0;
		return;
		}
     if (u->level==NEW) {
          sprintf(text,"~CRYou demote %s to level: ~CWPond Scum~RS.\n",u->name);
		write_user(user,text);
		if (user->vis) name=user->recap; else name=invisname;
                sprintf(text2,"~CM%s has demoted you to level: ~CWPond Scum\n",name);
		send_mail(user,word[1],text2);
                sprintf(text,"%s DEMOTED %s to level SCUM.\n",user->name,u->name);
		write_syslog(text,1);
                sprintf(text,"%s DEMOTED %s to level SCUM.\n",user->name,word[1]);
		write_arrestrecord(u,text,1);
		u->muzzled+=SCUM;
		u->socket=-2;
		strcpy(u->site,u->last_site);
		save_user_details(u,0);
		destruct_user(u);
		destructed=0;
		return;
		}
if (u->level>=user->level) {
	write_user(user,"You cannot demote a user of an equal or higher level than yourself.\n");
	destruct_user(u);
	destructed=0;
	return;
	}
u->level--;
u->socket=-2;
strcpy(u->site,u->last_site);
save_user_details(u,0);
sprintf(text,"~CRYou demote %s to level: ~CW%s.\n",u->recap,level_name[u->level]);
write_user(user,text);
sprintf(text2,"~CRYou have been demoted to level: ~CW%s.\n",level_name[u->level]);
send_mail(user,word[1],text2);
sprintf(text,"%s DEMOTED %s to level %s.\n",user->name,word[1],level_name[u->level]);
write_syslog(text,1);
sprintf(text,"%s DEMOTED %s to level %s.\n",user->name,word[1],level_name[u->level]);
write_arrestrecord(u,text,1);
destruct_user(u);
destructed=0;
}

/*** List banned sites or users ***/
void listbans(UR_OBJECT user)
{
int i;
char filename[80];

if (!strcmp(word[1],"sites")) {
     if (user->high_ascii) {
          write_user(user,"~CT~BM⁄ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒø~RS\n");
          write_user(user,"~CT~BM≥                  Listing Of Banned Sites And Domains                      ~FT≥~RS\n");
          write_user(user,"~CT~BM¿ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒŸ~RS\n");
          }
     else {
          write_user(user,"~FM------------------------------------------------------------------------------\n");
          write_user(user,"~FG                      Listing of Banned Sites and Domains\n");
          write_user(user,"~FM------------------------------------------------------------------------------\n");
          }
	sprintf(filename,"%s/%s",DATAFILES,SITEBAN);
	switch(more(user,user->socket,filename)) {
		case 0:
          write_user(user,"~FRThere are currently no banned sites or domains.\n");
          write_user(user,"~FM-------------------------------------------------------------------------------\n");
		return;
		case 1: user->misc_op=2;
		}
          write_user(user,"~FM-------------------------------------------------------------------------------\n");
	return;
	}
if (!strcmp(word[1],"users")) {
     if (user->high_ascii) {
          write_user(user,"~CT~BM⁄ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒø~RS\n");
          write_user(user,"~CT~BM≥                    Listing Of Banned Users From This Talker                ~FT≥~RS\n");
          write_user(user,"~CT~BM¿ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒŸ~RS\n");
          }
     else {
          write_user(user,"~FM-------------------------------------------------------------------------------\n");
          write_user(user,"~FG                      Listing of Banned Users From This Talker\n");
          write_user(user,"~FM-------------------------------------------------------------------------------\n");
          }
     sprintf(filename,"%s/%s",DATAFILES,USERBAN);
	switch(more(user,user->socket,filename)) {
		case 0:
          write_user(user,"~FRThere are currently no banned users.\n\n");
          write_user(user,"~FM-------------------------------------------------------------------------------\n");
		return;
		case 1: user->misc_op=2;
		}
          write_user(user,"~FM-------------------------------------------------------------------------------\n");
	return;
	}
if (!strcmp(word[1],"swears")) {
     if (user->high_ascii) {
          write_user(user,"~CT~BM⁄ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒø~RS\n");
          write_user(user,"~CT~BM≥                       Listing Of Banned Swear Words                         ~FT≥~RS\n");
          write_user(user,"~CT~BM¿ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒŸ~RS\n");
          }
     else {
          write_user(user,"~FM-------------------------------------------------------------------------------\n");
          write_user(user,"~FG                          Listing of Banned Swear Words\n");
          write_user(user,"~FM-------------------------------------------------------------------------------\n");
          }
	i=0;
	while(swear_words[i][0]!='*') {
		write_user(user,swear_words[i]);
		write_user(user,"\n");
		++i;
		}
	if (!i) write_user(user,"There are no banned swear words.\n");
	if (ban_swearing) write_user(user,"\n");
	else write_user(user,"\n(Swearing ban is currently off)\n\n");
     write_user(user,"~FM-------------------------------------------------------------------------------\n");
	return;
	}
if (!strcmp(word[1],"new")) {
     if (user->high_ascii) {
          write_user(user,"~CT~BM⁄ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒø~RS\n");
          write_user(user,"~CT~BM≥        Listing of Banned Sites And Domains For New Users                   ~FT≥~RS\n");
          write_user(user,"~CT~BM¿ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒŸ~RS\n");
          }
     else {
          write_user(user,"~FM-------------------------------------------------------------------------------\n");
          write_user(user,"~FG            Listing of Banned Sites And Domains For New Users Only\n");
          write_user(user,"~FM-------------------------------------------------------------------------------\n");
          }
	sprintf(filename,"%s/%s",DATAFILES,NEWSITEBAN);
	switch(more(user,user->socket,filename)) {
		case 0:
          write_user(user,"~FRThere are currently no banned sites or domains for new users.\n");
          write_user(user,"~FM-------------------------------------------------------------------------------\n");
		return;
		case 1: user->misc_op=2;
		}
	return;
     write_user(user,"~FM-------------------------------------------------------------------------------\n");
	}
write_user(user,"Usage: listbans sites/new/users/swears\n");
}

/*** Ban a site/domain or user ***/
void ban(UR_OBJECT user)
{

if (word_count<3) {
     write_user(user,"Usage:  .ban site <site to be banned>\n");
     write_user(user,"   i.e. .ban site .theirsite.com\n");
     write_user(user,"   i.e. .ban site 198.123.241.  (omit last number)\n\n");
     write_user(user,"Usage:  .ban new <site to be banned>\n");
     write_user(user,"   i.e. .ban new .theirsite.com\n");
     write_user(user,"   i.e. .ban new 198.123.241.   (omit last number)\n\n");
     write_user(user,"Usage:  .ban user <username>\n\n");
     return;
     }
if (!strcmp(word[1],"site")) { ban_site(user); return; }
if (!strcmp(word[1],"user")) { ban_user(user); return; }
if (!strcmp(word[1],"new"))  { ban_new(user);  return; }

/* Neither site/user/new was specified, show usage! */
write_user(user,"Usage:  .ban site <site to be banned>\n");
write_user(user,"   i.e. .ban site .theirsite.com\n");
write_user(user,"   i.e. .ban site 198.123.241.     (omit last number)\n\n");
write_user(user,"Usage:  .ban new <site to be banned>\n");
write_user(user,"   i.e. .ban new .theirsite.com\n");
write_user(user,"   i.e. .ban new 198.123.241.      (omit last number)\n\n");
write_user(user,"Usage:  .ban user <username>\n\n");
}

void ban_site(UR_OBJECT user)
{
FILE *fp;
char filename[80],host[81],site[80];

gethostname(host,80);
if (!strcasecmp(word[2],host) || !strcasecmp(word[2],"localhost") || !strncmp(word[2],"127.",4)) {
	write_user(user,"You cannot ban the machine that this program is running on.\n");
	return;
	}
strtolower(word[2]);
if (strstr(word[2],"tdf")) {
        write_user(user,"You cannot ban that site totally!\n");
	return;
	}
sprintf(filename,"%s/%s",DATAFILES,SITEBAN);
/* See if ban already set for given site */
if ((fp=fopen(filename,"r"))) {
	fscanf(fp,"%s",site);
	while(!feof(fp)) {
		if (!strcmp(site,word[2])) {
			write_user(user,"That site/domain is already banned.\n");
			fclose(fp);  return;
			}
		fscanf(fp,"%s",site);
		}
	fclose(fp);
	}

/* Write new ban to file */
if (!(fp=fopen(filename,"a"))) {
	sprintf(text,"%s: Can't open file to append.\n",syserror);
	write_user(user,text);
	write_syslog("ERROR: Couldn't open file to append in ban_site().\n",0);
	return;
	}
fprintf(fp,"%s\n",word[2]);
fclose(fp);
write_user(user,"Site/domain banned.\n");
sprintf(text,"%s BANNED site/domain %s.\n",user->name,word[2]);
write_syslog(text,1);
}

void ban_new(UR_OBJECT user)
{
FILE *fp;
char filename[80],host[81],site[80];

gethostname(host,80);
if (!strcmp(word[2],host) || !strcmp(word[2],"localhost") || !strncmp(word[2],"127.",4)) {
	write_user(user,"You cannot ban the machine that this program is running on.\n");
	return;
	}
sprintf(filename,"%s/%s",DATAFILES,NEWSITEBAN);

/* See if ban already set for given site */
if ((fp=fopen(filename,"r"))) {
	fscanf(fp,"%s",site);
	while(!feof(fp)) {
		if (!strcmp(site,word[2])) {
			write_user(user,"That site/domain is already banned for new users.\n");
			fclose(fp);  return;
			}
		fscanf(fp,"%s",site);
		}
	fclose(fp);
	}

/* Write new ban to file */
if (!(fp=fopen(filename,"a"))) {
	sprintf(text,"%s: Can't open file to append.\n",syserror);
	write_user(user,text);
	write_syslog("ERROR: Couldn't open file to append in ban_new().\n",0);
	return;
	}
fprintf(fp,"%s\n",word[2]);
fclose(fp);
write_user(user,"Site/domain banned to new users.\n");
sprintf(text,"%s BANNED site/domain for NEW users only %s.\n",user->name,word[2]);
write_syslog(text,1);
}

void ban_user(UR_OBJECT user)
{
UR_OBJECT u;
FILE *fp;
char filename[80],filename2[80],p[20],name[USER_NAME_LEN+1];
int a,b,c,d,level;

word[2][0]=toupper(word[2][0]);
if (!strcmp(user->name,word[2])) {
	write_user(user,"Trying to ban yourself is the seventh sign of madness.\n");
	return;
	}
strtolower(word[2]);
word[2][0]=toupper(word[2][0]);
if (strstr(word[2],"Moe")) {
        write_user(user,"You cannot ban that user!\n");
        return;
        }
/* See if ban already set for given user */
sprintf(filename,"%s/%s",DATAFILES,USERBAN);
if ((fp=fopen(filename,"r"))) {
	fscanf(fp,"%s",name);
	while(!feof(fp)) {
		if (!strcmp(name,word[2])) {
			write_user(user,"That user is already banned.\n");
			fclose(fp);  return;
			}
		fscanf(fp,"%s",name);
		}
	fclose(fp);
	}

/* See if already on */
if ((u=get_user(word[2]))!=NULL) {
	if (u->level>=user->level) {
		write_user(user,"You cannot ban a user of equal or higher level than yourself.\n");
		return;
		}
	}
else {
	/* User not on so load up his data */
	sprintf(filename2,"%s/%s.D",USERFILES,word[2]);
	if (!(fp=fopen(filename2,"r"))) {
		write_user(user,nosuchuser);
                if (!(fp=fopen(filename,"a"))) {
                        sprintf(text,"%s: Can't open file to append.\n",syserror);
                        write_user(user,text);
                        write_syslog("ERROR: Couldn't open file to append in ban_user().\n",0);
                        return;
                        }
                fprintf(fp,"%s\n",word[2]);
                fclose(fp);
                sprintf(text,"The username \"%s\" does not exist on the system.  Adding anyways!\n",word[2]);
                write_user(user,text);
                sprintf(text,"%s Added %s to the userban list.\n",user->name,word[2]);
                write_syslog(text,1);
                return;
                }
	fscanf(fp,"%s\n%d %d %d %d %d",p,&a,&b,&c,&d,&level);
	fclose(fp);
	if (level>=user->level) {
		write_user(user,"You cannot ban a user of equal or higher level than yourself.\n");
		return;
		}
	}

/* Write new ban to file */
if (!(fp=fopen(filename,"a"))) {
	sprintf(text,"%s: Can't open file to append.\n",syserror);
	write_user(user,text);
	write_syslog("ERROR: Couldn't open file to append in ban_user().\n",0);
	return;
	}
fprintf(fp,"%s\n",word[2]);
fclose(fp);
write_user(user,"User banned.\n");
sprintf(text,"%s BANNED user %s.\n",user->name,word[2]);
write_syslog(text,1);
sprintf(text,"%s BANNED %s.\n",user->name,u->name);
write_arrestrecord(u,text,1);
if (u!=NULL) {
        write_user(u,"\n~BP~CRYou have been banned from here!\n\n");
	disconnect_user(u);
	}
}

/*** uban a site (or domain) or user ***/
void unban(UR_OBJECT user)
{
char *usage="Usage: unban site/new/user <site/user name>\n";

if (word_count<3) {
	write_user(user,usage);  return;
	}
if (!strcmp(word[1],"site")) { unban_site(user); return; }
if (!strcmp(word[1],"user")) { unban_user(user); return; }
if (!strcmp(word[1],"new"))  { unban_new(user);  return; }
write_user(user,usage);
}

void unban_site(UR_OBJECT user)
{
FILE *infp,*outfp;
char filename[80],site[80];
int found,cnt;

sprintf(filename,"%s/%s",DATAFILES,SITEBAN);
if (!(infp=fopen(filename,"r"))) {
	write_user(user,"That site/domain is not currently banned.\n");
	return;
	}
if (!(outfp=fopen("tempfile","w"))) {
	sprintf(text,"%s: Couldn't open tempfile.\n",syserror);
	write_user(user,text);
	write_syslog("ERROR: Couldn't open tempfile to write in unban_site().\n",0);
	fclose(infp);
	return;
	}
found=0;   cnt=0;
fscanf(infp,"%s",site);
while(!feof(infp)) {
	if (strcmp(word[2],site)) {
		fprintf(outfp,"%s\n",site);  cnt++;
		}
	else found=1;
	fscanf(infp,"%s",site);
	}
fclose(infp);
fclose(outfp);
if (!found) {
	write_user(user,"That site/domain is not currently banned.\n");
	unlink("tempfile");
	return;
	}
if (!cnt) {
	unlink(filename);  unlink("tempfile");
	}
else rename("tempfile",filename);
write_user(user,"Site ban removed.\n");
sprintf(text,"%s UNBANNED site %s.\n",user->name,word[2]);
write_syslog(text,1);
}


void unban_new(UR_OBJECT user)
{
FILE *infp,*outfp;
char filename[80],site[80];
int found,cnt;

sprintf(filename,"%s/%s",DATAFILES,NEWSITEBAN);
if (!(infp=fopen(filename,"r"))) {
	write_user(user,"That site/domain is not currently banned to new users.\n");
	return;
	}
if (!(outfp=fopen("tempfile","w"))) {
	sprintf(text,"%s: Couldn't open tempfile.\n",syserror);
	write_user(user,text);
	write_syslog("ERROR: Couldn't open tempfile to write in unban_new().\n",0);
	fclose(infp);
	return;
	}
found=0;   cnt=0;
fscanf(infp,"%s",site);
while(!feof(infp)) {
	if (strcmp(word[2],site)) {
		fprintf(outfp,"%s\n",site);  cnt++;
		}
	else found=1;
	fscanf(infp,"%s",site);
	}
fclose(infp);
fclose(outfp);
if (!found) {
	write_user(user,"That site/domain is not currently banned to new users.\n");
	unlink("tempfile");
	return;
	}
if (!cnt) {
	unlink(filename);  unlink("tempfile");
	}
else rename("tempfile",filename);
write_user(user,"Site ban for new users removed.\n");
sprintf(text,"%s UNBANNED site for new users %s.\n",user->name,word[2]);
write_syslog(text,1);
}

void unban_user(UR_OBJECT user)
{
FILE *infp,*outfp;
char filename[80],name[USER_NAME_LEN+1];
int found,cnt;

sprintf(filename,"%s/%s",DATAFILES,USERBAN);
if (!(infp=fopen(filename,"r"))) {
	write_user(user,"That user is not currently banned.\n");
	return;
	}
if (!(outfp=fopen("tempfile","w"))) {
	sprintf(text,"%s: Couldn't open tempfile.\n",syserror);
	write_user(user,text);
	write_syslog("ERROR: Couldn't open tempfile to write in unban_user().\n",0);
	fclose(infp);
	return;
	}
found=0;  cnt=0;
word[2][0]=toupper(word[2][0]);
fscanf(infp,"%s",name);
while(!feof(infp)) {
	if (strcmp(word[2],name)) {
		fprintf(outfp,"%s\n",name);  cnt++;
		}
	else found=1;
	fscanf(infp,"%s",name);
	}
fclose(infp);
fclose(outfp);
if (!found) {
	write_user(user,"That user is not currently banned.\n");
	unlink("tempfile");
	return;
	}
if (!cnt) {
	unlink(filename);  unlink("tempfile");
	}
else rename("tempfile",filename);
write_user(user,"User ban removed.\n");
sprintf(text,"%s UNBANNED user %s.\n",user->name,word[2]);
write_syslog(text,1);
}



/*** Set user visible or invisible ***/
void visibility(UR_OBJECT user,int vis)
{
if (vis) {
	if (user->vis) {
		write_user(user,"You are already visible.\n");  return;
		}
        sprintf(text,appear_prompt,user->recap);
        write_room(user->room,text);
	user->vis=1;
	return;
	}
if (!user->vis) {
	write_user(user,"You are already invisible.\n");  return;
	}
write_user(user,disapear_user_prompt);
sprintf(text,disapear_prompt,user->recap);
write_room_except(user->room,text,user);
user->vis=0;
}

/*** Site a user ***/
void site(UR_OBJECT user)
{
UR_OBJECT u;

if (word_count<2) {
	write_user(user,"Usage: site <user>\n");  return;
	}
/* User currently logged in */
if ((u=get_user(word[1]))) {
     if (strstr(u->site,u->site_port)) sprintf(text,site_style_dns_ip,u->recap,u->site,u->site_port);
     else sprintf(text,site_style_dns,u->recap,u->site);
	write_user(user,text);
	return;
	}
/* User not logged in */
if ((u=create_user())==NULL) {
	sprintf(text,"%s: unable to create temporary user object.\n",syserror);
	write_user(user,text);
	write_syslog("ERROR: Unable to create temporary user object in site().\n",0);
	return;
	}
strcpy(u->name,word[1]);
if (!load_user_details(u)) {
	write_user(user,nosuchuser);
	destruct_user(u);
	destructed=0;
	return;
	}
sprintf(text,site_style_offline,u->recap,u->last_site);
write_user(user,text);
destruct_user(u);
destructed=0;
}


/*** Shows wizard the record of a user ***/
void view_record(UR_OBJECT user)
{
UR_OBJECT u;
char filename[80], text2[ARR_SIZE], *center();
int cnt=0;

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (word_count<2) {
	sprintf(text,"Useage: record <user name>\n");
	write_user(user,text);
	return;
	}
if ((u=get_user(word[1]))!=NULL) {
     if (u->level>=user->level && user->level<OWNER) {
		write_user(user,"You cannot view the record a user of equal or higher level than yourself.\n");
		return;
		}
     sprintf(text2,"~CY~BMViewing %s~CY~BM's Record",u->name);
     cnt=(colour_com_count(user->recap)*3);
     if (user->high_ascii) {
          write_user(user,"~CT~BM⁄ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒø~RS\n");
          sprintf(text,"~CT~BM≥~FG~BM%-78.78s%-*.*s ~CT~BM≥~RS\n",center(text2,78),cnt,cnt," ");
          write_user(user,text);
          write_user(user,"~CT~BM¿ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒŸ~RS\n");
          }
     else {
          write_user(user,"~CT~BM.----------------------------------------------------------------------------.\n");
          sprintf(text,"~CT~BM|~CT~BM%-78.78s%-*.*s ~CT~BM|~RS\n",center(text2,78),cnt,cnt," ");
          write_user(user,text);
          write_user(user,"~CT~BM`----------------------------------------------------------------------------'\n");
          }
     sprintf(filename,"%s/%s.A",USERFILES,u->name);
	switch(more(user,user->socket,filename)) {
		case 0:
                sprintf(text,"%s ~CRdoes not have a record to view.\n",u->name);
		write_user(user,text);
		return;
		case 1: user->misc_op=2;
		}
     if (user->high_ascii) {
          write_user(user,"~CT~BM⁄ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒø~RS\n");
          write_user(user,"~CT~BM≥                           End Of User's Record                            ~FT≥~RS\n");
          write_user(user,"~CT~BM¿ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒŸ~RS\n");
          }
     else {
          write_user(user,"~CT~BM.----------------------------------------------------------------------------.~RS\n");
          write_user(user,"~CT~BM|~CY~BM                           End of User's Record.                            ~CT~BM|~RS\n");
          write_user(user,"~CT~BM`----------------------------------------------------------------------------'~RS\n\n");
          }
	return;
	}
/* User not logged on */
if ((u=create_user())==NULL) {
	sprintf(text,"%s: unable to create temporary user object.\n",syserror);
	write_user(user,text);
	write_syslog("ERROR: Unable to create temporary user object in view_record().\n",0);
	return;
	}
strcpy(u->name,word[1]);
if (!load_user_details(u)) {
	write_user(user,nosuchuser);
	destruct_user(u);
	destructed=0;
	return;
	}
if (u->level>=user->level && user->level<OWNER) {
	write_user(user,"~CRYou cannot view the record a user of equal or higher level than yourself.\n");
	destruct_user(u);
	destructed=0;
	return;
	}
     sprintf(text2,"~CY~BMViewing %s~CY~BM's Record",u->name);
     cnt=(colour_com_count(user->recap)*3);
     if (user->high_ascii) {
          write_user(user,"~CT~BM⁄ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒø~RS\n");
          sprintf(text,"~CT~BM≥~FG~BM%-78.78s%-*.*s ~CT~BM≥~RS\n",center(text2,78),cnt,cnt," ");
          write_user(user,text);
          write_user(user,"~CT~BM¿ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒŸ~RS\n");
          }
     else {
          write_user(user,"~CT~BM.----------------------------------------------------------------------------.\n");
          sprintf(text,"~CT~BM|~CT~BM%-78.78s%-*.*s ~CT~BM|~RS\n",center(text2,78),cnt,cnt," ");
          write_user(user,text);
          write_user(user,"~CT~BM`----------------------------------------------------------------------------'\n");
          }
     sprintf(filename,"%s/%s.A",USERFILES,u->name);
     switch(more(user,user->socket,filename)) {
	case 0:
          sprintf(text,"%s~CR does not have a record to view.\n",u->name);
          write_user(user,text);
          destruct_user(u);
          destructed=0;
          if (user->high_ascii) {
               write_user(user,"~CT~BM⁄ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒø~RS\n");
               write_user(user,"~CT~BM≥~CY~BM                         There is no record to view!                       ~CT~BM≥~RS\n");
               write_user(user,"~CT~BM¿ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒŸ~RS\n");
               }
          else {
               write_user(user,"~CT~BM.----------------------------------------------------------------------------.\n");
               write_user(user,"~CT~BM|~CY~BM                         There is no record to view!                        ~CT~BM|\n");
               write_user(user,"~CT~BM`----------------------------------------------------------------------------'\n\n");
               }
          return;
     case 1: user->misc_op=2;
     if (user->high_ascii) {
          write_user(user,"~CT~BM⁄ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒø~RS\n");
          write_user(user,"~CT~BM≥~CY~BM                           End Of User's Record                            ~CT~BM≥~RS\n");
          write_user(user,"~CT~BM¿ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒŸ~RS\n");
          }
     else {
          write_user(user,"~CT~BM.----------------------------------------------------------------------------.\n");
          write_user(user,"~CT~BM|~CY~BM                           End Of User's Record.                            ~CT~BM|\n");
          write_user(user,"~CT~BM`----------------------------------------------------------------------------'\n\n");
          }
	destruct_user(u);
	destructed=0;
	return;
	}
destruct_user(u);
destructed=0;
}

/*** Wake up some sleepy herbert ***/
void wake(UR_OBJECT user)
{
UR_OBJECT u;
char *name;

if (word_count<2) {
	write_user(user,"Usage: wake <user>\n");  return;
	}
if (user->muzzled & 1) {
	write_user(user,"You are muzzled, you cannot wake anyone.\n");  return;
	}
if (!(u=get_user(word[1]))) {
	write_user(user,notloggedon);  return;
	}
if (u==user) {
	write_user(user,"Trying to wake yourself up is the eighth sign of madness.\n");
	return;
	}
if (u->malloc_start!=NULL) {
	sprintf(text,"%s is using the editor at the moment.\n",u->recap);
	write_user(user,text);
	return;
	}
if (user->vis) name=user->recap; else name=invisname;
sprintf(text,wake_victim_prompt,name);
write_user(u,text);
sprintf(text,wake_user_prompt,u->recap);
write_user(user,text);
}

/*** Shout something to other wizes and OWNERs. If the level isnt given it
     defaults to WIZ level. ***/
void wizshout(UR_OBJECT user,char *inpstr)
{
int lev;

if (user->muzzled) {
	write_user(user,"You are amuzzled, shame on you!\n");  return;
	}
if (word_count<2) {
	write_user(user,"Usage: wiztell [<superuser level>] <message>\n");
	return;
	}
strtoupper(word[1]);
if ((lev=get_level(word[1]))==-1) lev=WIZ;
else {
     if (lev<WIZ || word_count<3) {
          write_user(user,"Usage: .wiztell [<superuser level>] <message>\n");
		return;
		}
	if (lev>user->level) {
		write_user(user,"You cannot specifically wiztell to users of a higher level than yourself.\n");
		return;
		}
	inpstr=remove_first(inpstr);
        sprintf(text,"~CB[~FGTo Level ~FT%s~CB]~FM: ~RS%s\n",level_name[lev],inpstr);
	write_user(user,text);
        sprintf(text,"~CB[~FGTo Level ~FT%s~CB]~FM: ~FT[ ~RS%s~FT ]~RS %s\n",level_name[lev],user->recap,inpstr);
	write_level(lev,5,text,user);
	return;
	}
sprintf(text,wizshout_style,user->recap,inpstr);
write_user(user,text);
write_level(WIZ,5,text,user);
record_wiz(user,text);
}

/*** Emote something to all wizs***/
void wemote(UR_OBJECT user,char *inpstr)
{
int lev;

if (user->muzzled) {
	write_user(user,"You are amuzzled, shame on you!\n");  return;
	}
if (word_count<2) {
	write_user(user,"Usage: wemote [<superuser level>] <message>\n");
	return;
	}
strtoupper(word[1]);
if ((lev=get_level(word[1]))==-1) lev=WIZ;
else {
     if (lev<WIZ || word_count<3) {
          write_user(user,"Usage: .wemote [<superuser level>] <message>\n");
		return;
		}
	if (lev>user->level) {
		write_user(user,"You cannot specifically wiztell to users of a higher level than yourself.\n");
		return;
		}
	inpstr=remove_first(inpstr);
	sprintf(text,"~FG[~CYto level ~CT%s~FG]~CW:~RS %s %s\n",level_name[lev],user->recap,inpstr);
	write_user(user,text);
        sprintf(text,"~FG[~CYto level ~CT%s~FG]~CW:~RS %s %s\n",level_name[lev],user->recap,inpstr);
	write_level(lev,5,text,user);
	return;
	}
sprintf(text,wizemote_style,user->recap,inpstr);
write_user(user,text);
write_level(WIZ,5,text,user);
record_wiz(user,text);
}

/*** Broadcast an important message ***/
void bcast(UR_OBJECT user,char *inpstr)
{
UR_OBJECT u;
char *center();

if (word_count<2) {
	write_user(user,"Usage: bcast <message>\n");
	return;
	}
if (user->muzzled) {
	write_user(user,"You are muzzled, you cannot broadcast anything.\n");
	return;
	}
force_listen=1;
if (inpstr[0]=='.') {
	inpstr++;
	if (!inpstr[0]) {
		write_user(user,"Usage: bcast <message>\n");
		return;
		}
	for(u=user_first;u!=NULL;u=u->next) {
	   if (u->login>0) continue;
           if (u->type==CLONE_TYPE) continue;
           if (u->ignall) continue;
	   if (u->high_ascii) {
                write_user(u,"\n                                    ~CY,,, \n");
                write_user(u,"                                   ~CM(~CBo o~CM) \n");
                write_user(u,"~FG-------------------------------~CMoOO~RS~FG--~FT(_~FT)~RS~FG--~CMOOo~RS~FG----------------------------------\n");
		write_user(u,center(inpstr,78));
		write_user(u,"\n~FG------------------------------------------------------------------------------\n\n");
		}
	   else {
                write_user(u,"\n                                    ~CY,,, \n");
                write_user(u,"                                   ~CM(~CBo o~CM) \n");
                write_user(u,"~FG-------------------------------~CMoOO~RS~FG--~FT(_~FT)~RS~FG--~CMOOo~RS~FG----------------------------------\n");
		write_user(u,center(inpstr,78));
		write_user(u,"\n~FG------------------------------------------------------------------------------\n\n");
		}
	    }
	}
else {
     write_level(OWNER,4,"\07\n",NULL);
     if (!user->vis || user->hidden) sprintf(text,"~FB-~FM=~CM[ ~FTBroadcast Message~CM ]~FM=~FB-\n");
     else sprintf(text,"~FB-~FM=~CM[ ~FTBroadcast Message From~FG %s~RS~CM ]~FM=~FB-\n",user->recap);
     write_level(OWNER,4,center(text,78),NULL);
     write_level(OWNER,4,"~CM------------------------------------------------------------------------------\n",NULL);
     write_level(OWNER,4,center(inpstr,78),NULL);
     write_level(OWNER,4,"\n~CM------------------------------------------------------------------------------\n\n",NULL);
	}
}

/*** Mash a user into a silly potato head <grin> ***/
void gag(UR_OBJECT user)
{
UR_OBJECT u;

if (word_count<2) {
     	write_user(user,"Usage: mash <user name>\n");
     	return;
	}
if (!(u=get_user(word[1]))) {
	write_user(user,notloggedon);
	return;
	}
if (u==user) {
        write_user(user,"You are a silly potato head for trying to mash yourself!!\n");
	return;
	}
if (u->level>=user->level) {
          write_user(user,"Trying to mash someone of a equal or higher level than yourself isn't nice!\n");
          sprintf(text,"~FR%s thought of mashing you!\n",user->recap);
     	write_user(u,text);
     	return;
	}
if (u->gaged) {
     sprintf(text,mash_forgive,u->recap);
     write_room(NULL,text);
     strcpy(u->desc,"~CGhas been forgiven...");
     sprintf(text,"%s unmashed %s.\n",user->name,u->name);
	write_syslog(text,1);
     sprintf(text,"%s unmashed %s.\n",user->name,u->name);
	write_arrestrecord(u,text,1);
	u->gaged=0;
     u->mashed=u->gaged;
	return;
	}
sprintf(text,mash_prompt,u->recap);
write_room(NULL,text);
sprintf(text,"%s mashed %s.\n",user->name,u->name);
write_syslog(text,1);
sprintf(text,"%s mashed %s.\n",user->name,u->name);
write_arrestrecord(u,text,1);
u->gaged=1;
strcpy(u->desc,"~CRis being punished...");
u->mashed=u->gaged;
}

/*** Muzzle an annoying user so he cant speak, emote, echo, write, smail
     or bcast. Muzzles have levels from WIZ to OWNER so for instance a WIZ
     cannot remove a muzzle set by a OWNER.  ***/

void muzzle(UR_OBJECT user)
{
UR_OBJECT u;

if (word_count<2) {
	write_user(user,"Usage: muzzle <user>\n");  return;
	}
if ((u=get_user(word[1]))!=NULL) {
	if (u==user) {
		write_user(user,"Trying to muzzle yourself is the ninth sign of madness.\n");
		return;
		}
	if (u->level>=user->level) {
          write_user(user,"~FRYou cannot muzzle a user of equal or higher level than yourself.\n");
          sprintf(text,"~FR%s thought about muzzling you.\n",user->recap);
		write_user(u,text);
		return;
		}
	if (u->muzzled & 1) {
		sprintf(text,"%s is already muzzled.\n",u->recap);
		write_user(user,text);
          return;
		}
     sprintf(text,muzzle_user_prompt,u->recap);
	write_user(user,text);
     write_user(u,muzzle_victim_prompt);
	sprintf(text,"%s muzzled %s.\n",user->name,u->name);
	write_syslog(text,1);
	sprintf(text,"%s muzzled %s.\n",user->name,u->name);
	write_arrestrecord(u,text,1);
	u->muzzled++;
	return;
	}
/* User not logged on */
if ((u=create_user())==NULL) {
	sprintf(text,"%s: unable to create temporary user object.\n",syserror);
	write_user(user,text);
	write_syslog("ERROR: Unable to create temporary user object in muzzle().\n",0);
	return;
	}
strcpy(u->name,word[1]);
if (!load_user_details(u)) {
	write_user(user,nosuchuser);
	destruct_user(u);
	destructed=0;
	return;
	}
if (u->level>=user->level) {
	write_user(user,"You cannot muzzle a user of equal or higher level than yourself.\n");
	destruct_user(u);
	destructed=0;
	return;
	}
if (u->muzzled & 1) {
	sprintf(text,"%s is already muzzled.\n",u->recap);
	write_user(user,text);
	destruct_user(u);
	destructed=0;
	return;
	}
u->socket=-2;
u->muzzled++;
strcpy(u->site,u->last_site);
save_user_details(u,0);
sprintf(text,muzzle_user_prompt,u->recap);
write_user(user,text);
send_mail(user,word[1],muzzle_victim_prompt);
sprintf(text,"%s muzzled %s.\n",user->name,u->name);
write_syslog(text,1);
sprintf(text,"%s muzzled %s.\n",user->name,u->name);
write_arrestrecord(u,text,1);
destruct_user(u);
destructed=0;
}

/*** Umuzzle the bastard now he's apologised and grovelled enough via email ***/
void unmuzzle(UR_OBJECT user)
{
UR_OBJECT u;

if (word_count<2) {
	write_user(user,"Usage: unmuzzle <user>\n");  return;
	}
if ((u=get_user(word[1]))!=NULL) {
	if (u==user) {
		write_user(user,"Trying to unmuzzle yourself is the tenth sign of madness.\n");
		return;
		}
	if (!(u->muzzled & 1)) {
		sprintf(text,"%s is not muzzled.\n",u->recap);
		write_user(user,text);
		return;
		}
     sprintf(text,unmuzzle_user_prompt,u->recap);
	write_user(user,text);
     write_user(u,unmuzzle_victim_prompt);
	sprintf(text,"%s unmuzzled %s.\n",user->name,u->name);
	write_syslog(text,1);
	sprintf(text,"%s unmuzzled %s.\n",user->name,u->name);
	write_arrestrecord(u,text,1);
	u->muzzled--;
	return;
	}
/* User not logged on */
if ((u=create_user())==NULL) {
	sprintf(text,"%s: unable to create temporary user object.\n",syserror);
	write_user(user,text);
	write_syslog("ERROR: Unable to create temporary user object in unmuzzle().\n",0);
	return;
	}
strcpy(u->name,word[1]);
if (!load_user_details(u)) {
	write_user(user,nosuchuser);
	destruct_user(u);
	destructed=0;
	return;
	}
if (!(u->muzzled & 1)) {
	sprintf(text,"%s is not muzzled.\n",u->recap);
	write_user(user,text);
	destruct_user(u);
	destructed=0;
	return;
	}
u->socket=-2;
u->muzzled--;
strcpy(u->site,u->last_site);
save_user_details(u,0);
sprintf(text,unmuzzle_user_prompt,u->recap);
write_user(user,text);
send_mail(user,word[1],unmuzzle_victim_prompt);
sprintf(text,"%s unmuzzled %s.\n",user->name,u->name);
write_syslog(text,1);
sprintf(text,"%s unmuzzled %s.\n",user->name,u->name);
write_arrestrecord(u,text,1);
destruct_user(u);
destructed=0;
}

/* Thow the user in the brink!! */

void arrest(UR_OBJECT user)
{
UR_OBJECT u;
RM_OBJECT rm;

if (word_count<2) {
	write_user(user,"Usage: arrest <user>\n");
     return;
	}
if ((u=get_user(word[1]))!=NULL) {
	if (u==user) {
		write_user(user,"Trying to arrest yourself is the ninth sign of madness.\n");
		return;
          }
	if (u->level>=user->level) {
		write_user(user,"You cannot arrest a user of equal or higher level than yourself.\n");
		sprintf(text,"%s thought about arresting you.\n",user->name);
		write_user(u,text);
		return;
		}
	if (u->muzzled & JAILED) {
		sprintf(text,"%s is already under arrest.\n",u->name);
		write_user(user,text);  return;
		}
     sprintf(text,arrest_user_prompt,u->name);
	write_user(user,text);
        sprintf(text,arrest_room_prompt,user->recap,u->recap);
        write_room(NULL,text);
	sprintf(text,"%s arrested %s.\n",user->name,u->name);
	write_syslog(text,1);
	sprintf(text,"%s arrested %s.\n",user->name,u->name);
	write_arrestrecord(u,text,1);
	u->muzzled+=JAILED;
	if ((rm=get_room(jail_room))) move_user(u,rm,2);
     else write_room(u->room,"~CRACK!  The Jail Musta Got Blown Up!  The Convict Will Have To Remain Here!\n~CRHowever, they will have limited command access...\n");
	return;
	}
/* User not logged on */
if ((u=create_user())==NULL) {
	sprintf(text,"%s: unable to create temporary user object.\n",syserror);
	write_user(user,text);
	write_syslog("ERROR: Unable to create temporary user object in arrest().\n",0);
	return;
	}
strcpy(u->name,word[1]);
if (!load_user_details(u)) {
	write_user(user,nosuchuser);
	destruct_user(u);
	destructed=0;
	return;
	}
if (u->level>=user->level) {
	write_user(user,"You cannot muzzle a user of equal or higher level than yourself.\n");
	destruct_user(u);
	destructed=0;
	return;
	}
if (u->muzzled & JAILED) {
	sprintf(text,"%s is already under arrest.\n",u->name);
	write_user(user,text);
	destruct_user(u);
	destructed=0;
	return;
	}
u->socket=-2;
u->muzzled+=JAILED;
strcpy(u->site,u->last_site);
save_user_details(u,0);
sprintf(text,arrest_user_prompt,u->name);
write_user(user,text);
send_mail(user,word[1],arrest_smail_message);
sprintf(text,"%s arrested %s.\n",user->name,u->name);
write_syslog(text,1);
sprintf(text,"%s arrested %s.\n",user->name,u->name);
write_arrestrecord(u,text,1);
destruct_user(u);
destructed=0;
}

/*** Unarrest the bastard now he's grovelled enough via email ***/
void unarrest(UR_OBJECT user)
{
UR_OBJECT u;
RM_OBJECT rm;

if (word_count<2) {
	write_user(user,"Usage: unarrest <user>\n");
        return;
	}
if ((u=get_user(word[1]))!=NULL) {
	if (u==user) {
		write_user(user,"Trying to unarrest yourself is the tenth sign of madness.\n");
		return;
		}
	if (!(u->muzzled & JAILED)) {
		sprintf(text,"%s is not under arrest.\n",u->name);
		write_user(user,text);
		return;
		}
     sprintf(text,unarrest_user_prompt,u->name);
	write_user(user,text);
     write_user(u,unarrest_victim_prompt);
	sprintf(text,"%s unarrested %s.\n",user->name,u->name);
	write_syslog(text,1);
	sprintf(text,"%s unarrested %s.\n",user->name,u->name);
	write_arrestrecord(u,text,1);
	u->muzzled-=JAILED;
	rm=room_first;
	move_user(u,rm,2);
	return;
	}

/* User not logged on */
if ((u=create_user())==NULL) {
	sprintf(text,"%s: unable to create temporary user object.\n",syserror);
	write_user(user,text);
	write_syslog("ERROR: Unable to create temporary user object in unarrest().\n",0);
	return;
	}
strcpy(u->name,word[1]);
if (!load_user_details(u)) {
	write_user(user,nosuchuser);
	destruct_user(u);
	destructed=0;
	return;
	}
if (!(u->muzzled & JAILED)) {
	sprintf(text,"%s is not under arrest.\n",u->name);
	write_user(user,text);
	destruct_user(u);
	destructed=0;
	return;
	}
u->socket=-2;
u->muzzled-=JAILED;
strcpy(u->site,u->last_site);
strcpy(u->login_room,room_first->name);
save_user_details(u,0);
sprintf(text,unarrest_user_prompt,u->name);
write_user(user,text);
send_mail(user,word[1],unarrest_victim_prompt);
sprintf(text,"%s unarrested %s.  Login Room Set to \"%s\"\n",user->name,u->name,u->login_room);
write_syslog(text,1);
sprintf(text,"%s unarrested %s.  Login Room Set To \"%s\"\n",user->name,u->name,u->login_room);
write_arrestrecord(u,text,1);
destruct_user(u);
destructed=0;
}


/**************************** HIGH LEVEL COMMANDS *************************/

void freeze(UR_OBJECT user)
{
UR_OBJECT u;

if (word_count<2) {
	write_user(user,"Usage: freeze <user>\n");  return;
	}
if ((u=get_user(word[1]))!=NULL) {
	if (u==user) {
		write_user(user,"Trying to freeze yourself is the ninth sign of madness.\n");
		return;
		}
	if (u->level>=user->level) {
		write_user(user,"You cannot freeze a user of equal or higher level than yourself.\n");
		sprintf(text,"%s thought about freezing you.\n",user->name);
		write_user(u,text);
		return;
		}
	if (u->muzzled & FROZEN) {
		sprintf(text,"%s is already frozen.\n",u->name);
		write_user(user,text);
          return;
		}
     sprintf(text,"~CMYou freeze~FT: ~CW%s.\n",u->name);
	write_user(user,text);
	sprintf(text,"%s freeze's %s.\n",user->name,u->name);
	write_syslog(text,1);
	sprintf(text,"%s freeze's %s.\n",user->name,u->name);
	write_arrestrecord(u,text,1);
	u->muzzled+=FROZEN;
	return;
	}
/* User not logged on */
if ((u=create_user())==NULL) {
	sprintf(text,"%s: unable to create temporary user object.\n",syserror);
	write_user(user,text);
	write_syslog("ERROR: Unable to create temporary user object in freeze().\n",0);
	return;
	}
strcpy(u->name,word[1]);
if (!load_user_details(u)) {
	write_user(user,nosuchuser);
	destruct_user(u);
	destructed=0;
	return;
	}
if (u->level>=user->level) {
	write_user(user,"You cannot freeze a user of equal or higher level than yourself.\n");
	destruct_user(u);
	destructed=0;
	return;
	}
if (u->muzzled & FROZEN) {
	sprintf(text,"%s is already frozen.\n",u->name);
	write_user(user,text);
	destruct_user(u);
	destructed=0;
	return;
	}
u->socket=-2;
u->muzzled+=FROZEN;
strcpy(u->site,u->last_site);
save_user_details(u,0);
sprintf(text,"~CMYou have frozen: ~CW%s.\n",u->name);
write_user(user,text);
sprintf(text,"%s freeze's %s.\n",user->name,u->name);
write_syslog(text,1);
sprintf(text,"%s freezes's %s.\n",user->name,u->name);
write_arrestrecord(u,text,1);
destruct_user(u);
destructed=0;
}


/*** Umuzzle the bastard now he's apologised and grovelled enough via email ***/
void unfreeze(UR_OBJECT user)
{
UR_OBJECT u;

if (word_count<2) {
	write_user(user,"Usage: unfreeze <user>\n");  return;
	}
if ((u=get_user(word[1]))!=NULL) {
	if (u==user) {
		write_user(user,"Trying to unfreeze yourself is the tenth sign of madness.\n");
		return;
		}
	if (!(u->muzzled & FROZEN)) {
		sprintf(text,"%s is not frozen.\n",u->name);
		write_user(user,text);
		return;
		}
        sprintf(text,"~CGYou have released %s~CG from being frozen.\n",u->name);
	write_user(user,text);
	sprintf(text,"%s unfreeze's %s.\n",user->name,u->name);
	write_syslog(text,1);
	sprintf(text,"%s unfreeze's %s.\n",user->name,u->name);
	write_arrestrecord(u,text,1);
	u->muzzled-=FROZEN;
	return;
	}
/* User not logged on */
if ((u=create_user())==NULL) {
	sprintf(text,"%s: unable to create temporary user object.\n",syserror);
	write_user(user,text);
	write_syslog("ERROR: Unable to create temporary user object in unfreeze().\n",0);
	return;
	}
strcpy(u->name,word[1]);
if (!load_user_details(u)) {
	write_user(user,nosuchuser);
	destruct_user(u);
	destructed=0;
	return;
	}
if (!(u->muzzled & FROZEN)) {
	sprintf(text,"%s is not frozen.\n",u->name);
	write_user(user,text);
	destruct_user(u);
	destructed=0;
	return;
	}
u->socket=-2;
u->muzzled-=FROZEN;
strcpy(u->site,u->last_site);
save_user_details(u,0);
sprintf(text,"~CGYou unfreeze %s~CG.\n",u->name);
write_user(user,text);
sprintf(text,"%s unfreeze's %s.\n",user->name,u->name);
write_syslog(text,1);
sprintf(text,"%s unfreeze's %s.\n",user->name,u->name);
write_arrestrecord(u,text,1);
destruct_user(u);
destructed=0;
}

/*** Change users password. Only OWNER and above can change another users
	password and they do this by specifying the user at the end. When this is
	done the old password given can be anything, the wiz doesnt have to know it
	in advance. ***/
void change_pass(UR_OBJECT user)
{
UR_OBJECT u;
char *name;

if (word_count<3) {
     if (user->level<ARCH) write_user(user,"Usage: passwd <old password> <new password>\n");
     else write_user(user,"Usage: passwd <old password> <new password> [<user>]\n");
     return;
     }
if (strlen(word[2])<3) {
	write_user(user,"New password too short.\n");
	return;
	}
if (strlen(word[2])>PASS_LEN) {
	write_user(user,"New password too long.\n");
	return;
	}
/* Change own password */
if (word_count==3) {
	if (strcmp((char *)crypt(word[1],"NU"),user->pass)) {
		write_user(user,"Old password incorrect.\n");
		return;
		}
	if (!strcmp(word[1],word[2])) {
		write_user(user,"Old and new passwords are the same.\n");
		return;
		}
	strcpy(user->pass,(char *)crypt(word[2],"NU"));
	save_user_details(user,0);
	cls(user);
     write_user(user,"Your password has been changed.\n");
	return;
	}
/* Change someone elses */
if (user->level<ARCH) {
	write_user(user,"You are not a high enough level to use the user option.\n");
	return;
	}
word[3][0]=toupper(word[3][0]);
if (!strcmp(word[3],user->name)) {
	/* security feature  - prevents someone coming to a wizes terminal and
	   changing his password since he wont have to know the old one */
	write_user(user,"You cannot change your own password using the <user> option.\n");
	return;
	}
if ((u=get_user(word[3]))) {
	if (u->level>=user->level) {
		write_user(user,"You cannot change the password of a user of equal or higher level than yourself.\n");
		return;
		}
	strcpy(u->pass,(char *)crypt(word[2],"NU"));
	cls(user);
        sprintf(text,"%s~CG's password has been changed.\n",u->name);
	write_user(user,text);
	if (user->vis) name=user->name; else name=invisname;
        sprintf(text,"~CMYour password has been changed by %s~CM!\n",name);
	write_user(u,text);
	sprintf(text,"%s changed %s's password.\n",user->name,u->name);
	write_syslog(text,1);
	return;
	}
if ((u=create_user())==NULL) {
	sprintf(text,"%s: unable to create temporary user object.\n",syserror);
	write_user(user,text);
	write_syslog("ERROR: Unable to create temporary user object in change_pass().\n",0);
	return;
	}
strcpy(u->name,word[3]);
if (!load_user_details(u)) {
	write_user(user,nosuchuser);
	destruct_user(u);
	destructed=0;
	return;
	}
if (u->level>=user->level) {
	write_user(user,"You cannot change the password of a user of equal or higher level than yourself.\n");
	destruct_user(u);
	destructed=0;
	return;
	}
strcpy(u->pass,(char *)crypt(word[2],"NU"));
save_user_details(u,0);
destruct_user(u);
destructed=0;
cls(user);
sprintf(text,"%s's password changed to \"%s\".\n",word[3],word[2]);
write_user(user,text);
sprintf(text,"%s changed %s's password.\n",user->name,u->name);
write_syslog(text,1);
}

/*** Temporarily Change System Configuration Options Without Rebooting ***/
void changesys(UR_OBJECT user)
{
int temp,val;
char *fortune[]={"QOTD","Fortune","Offens"};

if (word_count<2) {
  	sprintf(text,"MAX Users                 : %-3.3d users.      Usage: change max <10-100>\n",max_users);
	write_user(user,text);
	sprintf(text,"MAX Clones                : %-1.1d clones.       Usage: change clones <1-6>\n",max_clones);
	write_user(user,text);
	sprintf(text,"User Idle Time Out        : %-4.4d seconds.   Usage: change idle <600-7200>\n",user_idle_time);
	write_user(user,text);
	sprintf(text,"Message Life Time         : %-2.2d days.        Usage: change life <1-15>\n",mesg_life);
	write_user(user,text);
	sprintf(text,"Time Out Afks             : %-4.4s            Usage: change afks <yes/no>\n",noyes2[time_out_afks]);
	write_user(user,text);
	sprintf(text,"Allow Suicideing          : %-3.3s             Usage: change suicide yes/no\n",noyes2[sys_allow_suicide]);
	write_user(user,text);
	sprintf(text,"Allow Auto Promote        : %-3.3s             Usage: change autopromote on/off\n",offon[sys_allow_autopromote]);
	write_user(user,text);
	sprintf(text,"Fortne Or Quote-Of-The-Day: %-7.7s         Usage: change fortune 0,1,2\n",fortune[SYS_FORTUNE]);
	write_user(user,text);
	return;
	}
if (!strcmp(word[1],"max")) {
	temp=max_users;
	if (word_count<3 || !isnumber(word[2])) {
		write_user(user,"Usage: change max <10-100>\n");
		return;
		}
	val=atoi(word[2]);
	if (val<10) {
                sprintf(text,"The Value %d is to ~CRLow~RS for Max users. Range: 10-99.\n",val);
		write_user(user,text);
		return;
		}
	if (val>100) {
                sprintf(text,"The Value %d is to ~CRHigh~RS for Max users. Range: 10-99.\n",val);
		write_user(user,text);
		return;
		}
	max_users=val;
        sprintf(text,"~CMYou changed Max users from:~RS %d ~CMto~RS %d.\n",temp,max_users);
	write_user(user,text);
	sprintf(text,"%s Changed Max users from: %d to %d.\n",user->name,temp,max_users);
	write_syslog(text,1);
	return;
	}
if (!strcmp(word[1],"clones")) {
	temp=max_clones;
	if (word_count<3 || !isnumber(word[2])) {
		write_user(user,"Usage: change clones <1-6>\n");
		return;
		}
	val=atoi(word[2]);
	if (val<1) {
                sprintf(text,"The Value %d is to ~CRLow~RS for Max clones. Range: 1-6.\n",val);
		write_user(user,text);
		return;
		}
	if (val>6) {
                sprintf(text,"The Value %d is to ~CRHigh~RS for Max clones. Range: 1-6.\n",val);
		write_user(user,text);
		return;
		}
	max_clones=val;
        sprintf(text,"~CMYou changed Max clones from:~RS %d ~CMto~RS %d.\n",temp,max_clones);
	write_user(user,text);
	sprintf(text,"%s Changed Max clones from: %d to %d.\n",user->name,temp,max_clones);
	write_syslog(text,1);
	return;
	}
if (!strcmp(word[1],"idle")) {
	temp=user_idle_time;
	if (word_count<3 || !isnumber(word[2])) {
		write_user(user,"Usage: change idle <600-7200>\n");
		return;
		}
	val=atoi(word[2]);
	if (val<600) {
                sprintf(text,"The Value %d is to ~CRLow~RS for User Idle Time Out. Range: 600-7200.\n",val);
		write_user(user,text);
		return;
		}
	if (val>7200) {
                sprintf(text,"The Value %d is to ~CRHigh~RS for User Idle Time Out. Range: 600-7200.\n",val);
		write_user(user,text);
		return;
		}
	user_idle_time=val;
        sprintf(text,"~CMYou changed User Idle Time Out from:~RS %d sec. ~CMto~RS %d sec.\n",temp,user_idle_time);
	write_user(user,text);
	sprintf(text,"%s Changed User Idle Time Out from: %d to %d.\n",user->name,temp,user_idle_time);
	write_syslog(text,1);
	return;
	}
if (!strcmp(word[1],"life")) {
	temp=mesg_life;
	if (word_count<3 || !isnumber(word[2])) {
		write_user(user,"Usage: change life <1-15>\n");
		return;
		}
	val=atoi(word[2]);
	if (val<1) {
                sprintf(text,"The Value %d is to ~CRLow~RS for Message Life Time. Range: 1-15.\n",val);
		write_user(user,text);
		return;
		}
	if (val>15) {
                sprintf(text,"The Value %d is to ~CRHigh~RS for Message Life Time. Range: 1-15.\n",val);
		write_user(user,text);
		return;
		}
	mesg_life=val;
        sprintf(text,"~CMYou changed Message Life Time from:~RS %d days ~CMto~RS %d days.\n",temp,mesg_life);
	write_user(user,text);
	sprintf(text,"%s Changed Message Life Time from: %d to %d.\n",user->name,temp,mesg_life);
	write_syslog(text,1);
	return;
	}
if (!strcmp(word[1],"afks")) {
	if (word_count<3) {
		write_user(user,"Usage: change afks <yes/no>\n");
		return;
		}
	if (!strcasecmp(word[2],"yes") && (time_out_afks!=0)) {
                sprintf(text,"Time Out Afks is set to ~CRYES~RS Usage: change afks <yes/no>.\n");
		write_user(user,text);
		return;
		}
	if (!strcasecmp(word[2],"no") && (time_out_afks==0)) {
                sprintf(text,"Time Out Afks is set to ~CRNO~RS Usage: change afks <yes/no>.\n");
		write_user(user,text);
		return;
		}
	if (!strcasecmp(word[2],"yes") && (time_out_afks==0)) {
		time_out_afks=1;
                sprintf(text,"~CMYou changed Time Out Afks from: ~RSNO ~CMto ~RSYES.\n");
		write_user(user,text);
		sprintf(text,"%s changed Time Out Afks from: NO to YES.\n",user->name);
		write_syslog(text,1);
		return;
		}
	if (!strcasecmp(word[2],"no") && (time_out_afks!=0)) {
		time_out_afks=0;
                sprintf(text,"~CMYou Time Out Afks from: ~RSYES ~CMto ~RSNO.\n");
		write_user(user,text);
		sprintf(text,"%s Changed Time Out Afks from: YES to NO.\n",user->name);
		write_syslog(text,1);
		return;
		}
	write_user(user,"Usage: change afks <yes/no>\n");
	return;
	}
if (!strcmp(word[1],"suicide")) {
	if (word_count<3) {
		write_user(user,"Usage: change suicide <yes/no>\n");
		return;
		}
	if (!strcasecmp(word[2],"yes") && !sys_allow_suicide) {
                write_user(user,"~CYSuicideing Is Now Allowed\n");
		sys_allow_suicide=1;
		sprintf(text,"%s Turned Suicideing ON.\n",user->name);
		write_syslog(text,1);
		return;
		}
	if (!strcasecmp(word[2],"no") && sys_allow_suicide) {
                write_user(user,"~CGSuicideing Is No Longer Allowed\n");
		sys_allow_suicide=0;
		sprintf(text,"%s Turned Suicideing OFF.\n",user->name);
		write_syslog(text,1);
		return;
		}
	if (!strcasecmp(word[2],"yes") && sys_allow_suicide) {
                write_user(user,"~CRSuicideing is already allowed.  Use .change suicide no to disallow!\n");
		return;
		}
	if (!strcasecmp(word[2],"no") && !sys_allow_suicide) {
                write_user(user,"~CRSuicideing is already disabled.  Use .change suicide yes to allow it!\n");
		return;
		}
	write_user(user,"Usage: .change suicide Yes/No\n");
	return;
	}
if (!strcasecmp(word[1],"autopromote")) {
	if (word_count<3) {
		write_user(user,"Usage: change autopromote <on/off>\n");
		return;
		}
	if (!strcasecmp(word[2],"on") && !sys_allow_autopromote) {
                write_user(user,"~CYAutopromote Has Been Turned On.\n");
		sys_allow_autopromote=1;
		sprintf(text,"%s Turned Auto-Promote ON.\n",user->name);
		write_syslog(text,1);
		return;
		}
	if (!strcasecmp(word[2],"off") && sys_allow_autopromote) {
                write_user(user,"~CGAutopromote Has Been Turned Off.\n");
		sys_allow_autopromote=0;
		sprintf(text,"%s Turned Auto-Promote OFF.\n",user->name);
		write_syslog(text,1);
		return;
		}
	if (!strcasecmp(word[2],"on") && sys_allow_autopromote) {
                write_user(user,"~CRAutopromote Is Already ON.  Use .change autopromote off to disable it!\n");
		return;
		}
	if (!strcasecmp(word[2],"off") && !sys_allow_autopromote) {
                write_user(user,"~CRAutopromote Is Already OFF.  Use .change autopromote on to enable it!\n");
		return;
		}
	write_user(user,"Usage: .change autopromote <on/off>\n");
	return;
	}
if (!strcmp(word[1],"fortune")) {
	if (word_count<3) {
		write_user(user,"Usage: change fortune <0/1/2>\n");
		return;
		}
	if (!strcasecmp(word[2],"0")) {
                write_user(user,"~CYUse Quote Of The Day.\n");
		SYS_FORTUNE=0;
		sprintf(text,"%s Switched To Quote Of The Day.\n",user->name);
		write_syslog(text,1);
		return;
		}
	if (!strcasecmp(word[2],"1")) {
                write_user(user,"~CGUse System Fortune.\n");
		SYS_FORTUNE=1;
		sprintf(text,"%s Switched To Fortunes.\n",user->name);
		write_syslog(text,1);
		return;
		}
	if (!strcasecmp(word[2],"2")) {
                write_user(user,"~CGUse System Fortune (Offensive Mode).\n");
		SYS_FORTUNE=2;
		sprintf(text,"%s Switched To Offensive Fortunes.\n",user->name);
		write_syslog(text,1);
		return;
		}
	write_user(user,"Usage: .change fortune <0/1/2>\n");
	return;
	}
write_user(user,"Type .change for Useage\n");
}

/*** Switch system logging on and off ***/
void logging(UR_OBJECT user)
{
if (!ALLOW_SYSLOG_TOGGLE) {
     write_user(user,"~CREvent Logging Cannot Be Turned Off At This Time!\n");
     return;
     }
if (system_logging) {
     write_user(user,"Events will no longer be logged in the System Log.\n");
	sprintf(text,"%s switched system logging OFF.\n",user->name);
	write_syslog(text,1);
	system_logging=0;
	return;
	}
system_logging=1;
write_user(user,"Events will now be logged in the System Log.\n");
sprintf(text,"%s switched system logging ON.\n",user->name);
write_syslog(text,1);
}


/*** Set minlogin level ***/
void minlogin(UR_OBJECT user)
{
UR_OBJECT u,next;
char *usage="Usage: minlogin [none]/[<user level>] [<-n>]\n";
char levstr[5],*name;
int lev,cnt;

if (word_count<2) {
	write_user(user,usage);
     return;
	}
strtoupper(word[1]);
if ((lev=get_level(word[1]))==-1) {
	if (strcmp(word[1],"NONE")) {
		write_user(user,usage);  return;
		}
	lev=-1;
	strcpy(levstr,"NONE");
	}
else strcpy(levstr,level_name[lev]);
if (lev>user->level) {
	write_user(user,"You cannot set minlogin to a higher level than your own.\n");
	return;
	}
if (minlogin_level==lev) {
	write_user(user,"It is already set to that.\n");
     return;
	}
minlogin_level=lev;
sprintf(text,"~CMMinlogin level set to: ~CW%s.\n",levstr);
write_user(user,text);
if (user->vis) name=user->name; else name=invisname;
sprintf(text,"%s~CM has set the minlogin level to: ~OL%s.\n",name,levstr);
write_room_except(NULL,text,user);
sprintf(text,"%s set the minlogin level to %s.\n",user->name,levstr);
write_syslog(text,1);

/* Now boot off anyone below that level unless -n is specified */

if (!strcasecmp(word[2],"-n")) return;

cnt=0;
u=user_first;
while(u) {
	next=u->next;
	if (!u->login && u->type!=CLONE_TYPE && u->level<lev) {
          write_user(u,"\n~CMYour level is now below the minlogin level, disconnecting you...\n");
		disconnect_user(u);
		++cnt;
		}
	u=next;
	}
sprintf(text,"Total of %d users were disconnected.\n",cnt);
destructed=0;
write_user(user,text);
}

/*** Free a hung socket ***/
void clearline(UR_OBJECT user)
{
UR_OBJECT u;
int sock;

if (word_count<2 || !isnumber(word[1])) {
	write_user(user,"Usage: clearline <line>\n");  return;
	}
sock=atoi(word[1]);

/* Find line amongst users */
for(u=user_first;u!=NULL;u=u->next)
	if (u->type!=CLONE_TYPE && u->socket==sock) goto FOUND;
write_user(user,"That line is not currently active.\n");
return;

FOUND:
if (!u->login) {
	write_user(user,"You cannot clear the line of a logged in user.\n");
	return;
	}
write_user(u,"\n\nThis line is being cleared.\n\n");
disconnect_user(u);
sprintf(text,"%s cleared line %d.\n",user->name,sock);
write_syslog(text,1);
sprintf(text,"Line %d cleared.\n",sock);
write_user(user,text);
destructed=0;
no_prompt=0;
}


/*** Change whether a rooms access is fixed or not ***/
void change_room_fix(UR_OBJECT user,int fix)
{
RM_OBJECT rm;
char *name;

if (word_count<2) rm=user->room;
else {
	if ((rm=get_room(word[1]))==NULL) {
		write_user(user,nosuchroom);  return;
		}
	}
if (user->vis) name=user->name; else name=invisname;
if (fix) {
	if (rm->access & FIXED) {
		if (rm==user->room)
			write_user(user,"This room's access is already fixed.\n");
		else write_user(user,"That room's access is already fixed.\n");
		return;
		}
	sprintf(text,"Access for room %s is now ~FRFIXED.\n",rm->recap);
	write_user(user,text);
	if (user->room==rm) {
		sprintf(text,"%s has ~FRFIXED~RS access for this room.\n",name);
		write_room_except(rm,text,user);
		}
	else {
		sprintf(text,"This room's access has been ~FRFIXED.\n");
		write_room(rm,text);
		}
	sprintf(text,"%s FIXED access to room %s.\n",user->name,rm->recap);
	write_syslog(text,1);
	rm->access+=2;
	return;
	}
if (!(rm->access & FIXED)) {
	if (rm==user->room)
		write_user(user,"This room's access is already unfixed.\n");
	else write_user(user,"That room's access is already unfixed.\n");
	return;
	}
sprintf(text,"~FTAccess for room %s~RS~FT is now ~FGUNFIXED.\n",rm->recap);
write_user(user,text);
if (user->room==rm) {
	sprintf(text,"%s has ~FGUNFIXED~RS access for this room.\n",name);
	write_room_except(rm,text,user);
	}
else {
	sprintf(text,"This room's access has been ~FGUNFIXED.\n");
	write_room(rm,text);
	}
sprintf(text,"%s UNFIXED access to room %s.\n",user->name,rm->recap);
write_syslog(text,1);
rm->access-=2;
reset_access(rm);
}

/*** View the login log ***/
void viewloginlog(UR_OBJECT user)
{
FILE *fp;
char c,*emp="\nThe login log is empty.\n";
int lines,cnt,cnt2;

if (word_count==1) {
     if (user->high_ascii) {
          write_user(user,"~CT~BM⁄ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒø~RS\n");
          write_user(user,"~CT~BM≥                           Viewing The Login Log                           ~FT≥~RS\n");
          write_user(user,"~CT~BM¿ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒŸ~RS\n");
          }
     else {
          write_user(user,"~FM------------------------------------------------------------------------------\n");
          write_user(user,"~FG                       Viewing the Login Log\n");
          write_user(user,"~FM------------------------------------------------------------------------------\n");
          }
	switch(more(user,user->socket,LOGINLOG)) {
		case 0: write_user(user,emp);  return;
		case 1: user->misc_op=2;
		}
	return;
	}
if ((lines=atoi(word[1]))<1) {
	write_user(user,"Usage: viewlog [<lines from the end>]\n");  return;
	}
/* Count total lines */
if (!(fp=fopen(LOGINLOG,"r"))) {  write_user(user,emp);  return;  }
cnt=0;

c=getc(fp);
while(!feof(fp)) {
	if (c=='\n') ++cnt;
	c=getc(fp);
	}
if (cnt<lines) {
	sprintf(text,"There are only %d lines in the login log.\n",cnt);
	write_user(user,text);
	fclose(fp);
     write_user(user,"~FM-------------------------------------------------------------------------------\n");
	return;
	}
if (cnt==lines) {
     if (user->high_ascii) {
          write_user(user,"~CT~BM⁄ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒø~RS\n");
          write_user(user,"~CT~BM≥                           Viewing The Login Log                           ~FT≥~RS\n");
          write_user(user,"~CT~BM¿ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒŸ~RS\n");
          }
     else {
          write_user(user,"~FM------------------------------------------------------------------------------\n");
          write_user(user,"~FG                       Viewing the Login Log\n");
          write_user(user,"~FM------------------------------------------------------------------------------\n");
          }
	fclose(fp);  more(user,user->socket,LOGINLOG);  return;
	}

/* Find line to start on */
fseek(fp,0,0);
cnt2=0;
c=getc(fp);
while(!feof(fp)) {
	if (c=='\n') ++cnt2;
	c=getc(fp);
	if (cnt2==cnt-lines) {
          if (user->high_ascii) {
               write_user(user,"~CT~BM⁄ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒø~RS\n");
               sprintf(text,"~CT~BM≥                    Viewing Last %-3d lines of the login log.               ~FT≥~RS\n",lines);
               write_user(user,text);
               write_user(user,"~CT~BM¿ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒŸ~RS\n");
               }
          else {
               write_user(user,"~FM------------------------------------------------------------------------------\n");
               sprintf(text,"~FG                 Viewing Last %d lines of the Login log\n",lines);
               write_user(user,text);
               write_user(user,"~FM------------------------------------------------------------------------------\n\n");
               }
		user->filepos=ftell(fp)-1;
		fclose(fp);
		if (more(user,user->socket,LOGINLOG)!=1) user->filepos=0;
		else user->misc_op=2;
		return;
		}
	}
if (user->high_ascii) write_user(user,"~CMƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
else write_user(user,"~CM------------------------------------------------------------------------------\n");
fclose(fp);
sprintf(text,"%s: Line count error.\n",syserror);
write_user(user,text);
write_syslog("ERROR: Line count error in viewloginlog().\n",0);
if (user->high_ascii) write_user(user,"~CMƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n\n");
else write_user(user,"~CM------------------------------------------------------------------------------\n\n");
}

/*** View the system log ***/
void viewsyslog(UR_OBJECT user)
{
FILE *fp;
char c,*emp="~FRThe system log is empty.\n";
int lines,cnt,cnt2;

if (word_count==1) {
          if (user->high_ascii) {
               write_user(user,"~CT~BM⁄ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒø~RS\n");
               write_user(user,"~CT~BM≥                    Viewing The System Log                                 ~FT≥~RS\n");
               write_user(user,"~CT~BM¿ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒŸ~RS\n");
               }
          else {
               write_user(user,"~FM------------------------------------------------------------------------------\n");
               write_user(user,"~FG                      Viewing the System Log\n");
               write_user(user,"~FM------------------------------------------------------------------------------\n");
               }
     switch(more(user,user->socket,SYSLOG)) {
		case 0: write_user(user,emp);  return;
		case 1: user->misc_op=2;
		}
	return;
	}
if ((lines=atoi(word[1]))<1) {
	write_user(user,"Usage: viewsys [<lines from the end>]\n");  return;
	}

/* Count total lines */
if (!(fp=fopen(SYSLOG,"r"))) {  write_user(user,emp);  return;  }
cnt=0;

c=getc(fp);
while(!feof(fp)) {
	if (c=='\n') ++cnt;
	c=getc(fp);
	}
if (cnt<lines) {
     sprintf(text,"~FRThere are only %d lines in the system log.\n",cnt);
       write_user(user,text);
       fclose(fp);
       if (user->high_ascii) write_user(user,"~CMƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n\n");
       else write_user(user,"~CM------------------------------------------------------------------------------\n\n");
       return;
       }
if (cnt==lines) {
          if (user->high_ascii) {
               write_user(user,"~CT~BM⁄ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒø~RS\n");
               write_user(user,"~CT~BM≥                    Viewing The System Log                                ~FT≥~RS\n");
               write_user(user,"~CT~BM¿ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒŸ~RS\n");
               }
          else {
               write_user(user,"~FM------------------------------------------------------------------------------\n");
               write_user(user,"~FG                      Viewing the System Log\n");
               write_user(user,"~FM------------------------------------------------------------------------------\n");
               }
	fclose(fp);  more(user,user->socket,SYSLOG);
     if (user->high_ascii) write_user(user,"~CMƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n\n");
     else write_user(user,"~CM------------------------------------------------------------------------------\n\n");
     return;
	}

/* Find line to start on */
fseek(fp,0,0);
cnt2=0;
c=getc(fp);
while(!feof(fp)) {
	if (c=='\n') ++cnt2;
	c=getc(fp);
	if (cnt2==cnt-lines) {
          if (user->high_ascii) {
               write_user(user,"~CT~BM⁄ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒø~RS\n");
               sprintf(text,"~CT~BM≥                    Viewing Last %-3d lines of the login log.               ~FT≥~RS\n",lines);
               write_user(user,text);
               write_user(user,"~CT~BM¿ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒŸ~RS\n");
               }
          else {
               write_user(user,"~FM------------------------------------------------------------------------------\n");
               sprintf(text,"~FG                 Viewing Last %d lines of the Login log\n",lines);
               write_user(user,text);
               write_user(user,"~FM------------------------------------------------------------------------------\n");
               }
		user->filepos=ftell(fp)-1;
		fclose(fp);
		if (more(user,user->socket,SYSLOG)!=1) user->filepos=0;
		else user->misc_op=2;
		return;
		}
	}
if (user->high_ascii) write_user(user,"~CMƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n\n");
else write_user(user,"~CM------------------------------------------------------------------------------\n\n");
fclose(fp);
sprintf(text,"%s: Line count error.\n",syserror);
write_user(user,text);
write_syslog("ERROR: Line count error in viewsyslog().\n",0);
}

/*** Switch swearing ban on and off ***/
void swban(UR_OBJECT user)
{
if (!ban_swearing) {
	write_user(user,"Swearing ban ~FGON.\n");
	sprintf(text,"%s switched swearing ban ON.\n",user->name);
	write_syslog(text,1);
	ban_swearing=1;  return;
	}
write_user(user,"Swearing ban ~FROFF.\n");
sprintf(text,"%s switched swearing ban OFF.\n",user->name);
write_syslog(text,1);
ban_swearing=0;
}

/*** This command can be disabled by changing ALLOW_SUICIDE to Zero in .h
     or by using the .change suicide command while the talker is running  ***/
void suicide(UR_OBJECT user)
{
if (!sys_allow_suicide) {
     write_user(user,"~CRSuicide Has Been Disabled!\n");
     return;
     }
if (word_count<2) {
	write_user(user,"Usage: suicide <your password>\n");
     return;
	}
if (strcmp((char *)crypt(word[1],"NU"),user->pass)) {
	write_user(user,"Password incorrect.\n");
     return;
	}
write_user(user,"\n~BP~CY~BR *** WARNING - This will delete your account! *** \n\nAre you sure about this (y/n)? ");
user->misc_op=6;
no_prompt=1;
}

/*** NUKE a user ***/
void delete_user(UR_OBJECT user,int this_user)
{
UR_OBJECT u;
char filename[80],name[USER_NAME_LEN+1];

if (this_user) {
	/* User structure gets destructed in disconnect_user(), need to keep a
	   copy of the name */
	strcpy(name,user->name);
        write_user(user,"\n~CRYour Account Has Been Deleted\n");
        sprintf(text,suicide_prompt,user->recap);
        write_room(NULL,text);
	sprintf(text,"%s suicided. ** Arrest Record Kept for viewing! **\n",name);
	write_syslog(text,1);
	disconnect_user(user);
	sprintf(filename,"%s/%s.D",USERFILES,name);
	unlink(filename);
	sprintf(filename,"%s/%s.M",USERFILES,name);
	unlink(filename);
	sprintf(filename,"%s/%s.P",USERFILES,name);
	unlink(filename);
	clean_userlist(name);
	return;
	}
if (word_count<2) {
	write_user(user,"Usage: nuke <user>\n");
        return;
	}
word[1][0]=toupper(word[1][0]);
if (!strcmp(word[1],user->name)) {
	write_user(user,"Trying to nuke yourself is the eleventh sign of madness.\n");
	return;
	}
if (get_user(word[1])!=NULL) {
	/* Safety measure just in case. Will have to .kill them first */
        write_user(user,"You cannot nuke a user who is currently logged on.\n");
	return;
	}
if ((u=create_user())==NULL) {
	sprintf(text,"%s: unable to create temporary user object.\n",syserror);
	write_user(user,text);
	write_syslog("ERROR: Unable to create temporary user object in delete_user().\n",0);
	return;
	}
strcpy(u->name,word[1]);
if (!load_user_details(u)) {
	write_user(user,nosuchuser);
	destruct_user(u);
	destructed=0;
	return;
	}
if (u->level>=user->level) {
	write_user(user,"You cannot nuke a user of an equal or higher level than yourself.\n");
	destruct_user(u);
	destructed=0;
	return;
	}
destruct_user(u);
destructed=0;
clean_userlist(u->name);
sprintf(filename,"%s/%s.D",USERFILES,word[1]);
unlink(filename);
sprintf(filename,"%s/%s.M",USERFILES,word[1]);
unlink(filename);
sprintf(filename,"%s/%s.P",USERFILES,word[1]);
unlink(filename);
sprintf(filename,"%s/%s.E",USERFILES,word[1]);
unlink(filename);
sprintf(text,"~BP~CRUser \"~RS%s~CR\" deleted, Arrest Record Kept For Review!\n",word[1]);
write_user(user,text);
sprintf(text,"%s Deleted %s. *Note: Arrest Record Kept*\n",user->name,word[1]);
write_syslog(text,1);
}

/*** Shutdown talker interface func. Countdown time is entered in seconds so
	we can specify less than a minute till reboot. ***/
void shutdown_com(UR_OBJECT user)
{
if (rs_which==1) {
	write_user(user,"The reboot countdown is currently active, you must cancel it first.\n");
	return;
	}
if (!strcmp(word[1],"cancel")) {
	if (!rs_countdown || rs_which!=0) {
		write_user(user,"The shutdown countdown is not currently active.\n");
		return;
		}
	if (rs_countdown && !rs_which && rs_user==NULL) {
		write_user(user,"Someone else is currently setting the shutdown countdown.\n");
		return;
		}
        write_room(NULL,"~CB[~CYM~CYoenuts~CB]~CW:~RS~FG Shutdown cancelled.\n");
	sprintf(text,"%s cancelled the shutdown countdown.\n",user->name);
	write_syslog(text,1);
	rs_countdown=0;
	rs_announce=0;
	rs_which=-1;
	rs_user=NULL;
	return;
	}
if (word_count>1 && !isnumber(word[1])) {
	write_user(user,"Usage: shutdown [<secs>/cancel]\n");  return;
	}
if (rs_countdown && !rs_which) {
	write_user(user,"The shutdown countdown is currently active, you must cancel it first.\n");
	return;
	}
if (word_count<2) {
	rs_countdown=0;
	rs_announce=0;
	rs_which=-1;
	rs_user=NULL;
	}
else {
	rs_countdown=atoi(word[1]);
	rs_which=0;
	}
write_user(user,"\n~BP~CY~BR *** WARNING - This will shutdown the talker! *** ~RS\n\nAre you sure about this (y/n)? ");
user->misc_op=1;
no_prompt=1;
}

/*** Reboot talker interface func. ***/
void reboot_com(UR_OBJECT user)
{
if (!rs_which) {
	write_user(user,"The shutdown countdown is currently active, you must cancel it first.\n");
	return;
	}
if (!strcmp(word[1],"cancel")) {
	if (!rs_countdown) {
		write_user(user,"The reboot countdown is not currently active.\n");
		return;
		}
	if (rs_countdown && rs_user==NULL) {
		write_user(user,"Someone else is currently setting the reboot countdown.\n");
		return;
		}
        write_room(NULL,"~CB[~CYM~CYoenuts~CB]~CW:~RS~FG Reboot cancelled.\n");
	sprintf(text,"%s cancelled the reboot countdown.\n",user->name);
	write_syslog(text,1);
	rs_countdown=0;
	rs_announce=0;
	rs_which=-1;
	rs_user=NULL;
	return;
	}
if (word_count>1 && !isnumber(word[1])) {
	write_user(user,"Usage: reboot [<secs>/cancel]\n");  return;
	}
if (rs_countdown) {
	write_user(user,"The reboot countdown is currently active, you must cancel it first.\n");
	return;
	}
if (word_count<2) {
	rs_countdown=0;
	rs_announce=0;
	rs_which=-1;
	rs_user=NULL;
	}
else {
	rs_countdown=atoi(word[1]);
	rs_which=1;
	}
write_user(user,"~BP~CB[~CRWarning~CB] ~CT- ~CRThis will reboot the talker.\n\n~CRAre you sure about this? ~CB(~CYy~CB/~CYn~CB)~CW: ");
user->misc_op=7;
no_prompt=1;
}

/*** Shutdown the talker ***/
void talker_shutdown(UR_OBJECT user,char *str,int reboot)
{
UR_OBJECT u,next;
int i;
char *ptr;
char *args[]={ progname,confile,NULL };

if (user!=NULL) ptr=user->name; else ptr=str;
if (save_hist()) write_room(NULL,"~CGPoker history has been saved...\n");
else write_room(NULL,"~CRERROR: ~CMCould not save poker history...\n");
if (reboot) {
     write_room(NULL,"\07\n~CB[~CYM~CYoenuts~CB]~CW: ~FMRebooting now!!\n\n");
     sprintf(text,"*** REBOOT initiated by %s ***\n",ptr);
     }
else {
     write_room(NULL,"\07\n~CB[~CYM~CYoenuts~CB]~CW:~FM Shutting down now!!\n\n");
     sprintf(text,"Shutdown Initiated By %s\n",ptr);
     }
write_syslog(text,1);
u=user_first;
while (u!=NULL) {
	next=u->next;
	if (!u->login || !u->type==CLONE_TYPE) write_user(u,text);
	disconnect_user(u);
	u=next;
	}
for(i=0;i<3;++i) close(listen_sock[i]);
if (reboot) {

	/* If someone has changed the binary or the config filename while this
	   prog has been running this won't work */

	execvp(progname,args);

	/* If we get this far it hasn't worked */

	sprintf(text,"\n!! REBOOT FAILED !! %s: %s\n\n",long_date(1),strerror(errno));
	write_syslog(text,0);
	exit(12);
	}

/* sprintf(text,"Shutdown Completed %s!\n",long_date(1)); */

sprintf(text,"|> Moenuts Shutdown Complete!\n");
write_syslog(text,1);
unlink(PIDFILE); /* Remove the PID file seeing we don't need it no more, cuz the talker is dead */
exit(0);
}

/**************************** EVENT FUNCTIONS **************************/

void do_events(int sig)
{
int x=0;
set_date_time();
check_reboot_shutdown();
check_idle_and_timeout();
credit_user_time();
check_messages(NULL,0);
if (ATMOSPHERES>0) check_atmospheres();
x=webwho();
reset_alarm();
}

void reset_alarm(void)
{
SIGNAL(SIGALRM,do_events);
alarm(heartbeat);
}

/*** See if timed reboot or shutdown is underway ***/
void check_reboot_shutdown(void)
{
int secs;
char *w[]={ "Shutdown","Rebooting","Seamless reboot" };

if (rs_user==NULL) return;
rs_countdown-=heartbeat;
if (rs_countdown<=0) {
	if (rs_which==1 || !rs_which) talker_shutdown(rs_user,NULL,rs_which);
	else {
		rs_countdown=0;  rs_which=-1;
		do_reboot(rs_user);
		return;
		}
	}
/* Print countdown message every minute unless we have less than 1 minute
   to go when we print every 10 secs */
secs=(int)(time(0)-rs_announce);
if (rs_countdown>=60 && secs>=60) {
        sprintf(text,"~CB[~CYM~CYoenuts~CB]~CW: %s in %d minutes, %d seconds.\n",w[rs_which],rs_countdown/60,rs_countdown%60);
	write_room(NULL,text);
	rs_announce=time(0);
	}
if (rs_countdown<60 && secs>=10) {
        sprintf(text,"~CB[~CYM~CYoenuts~CB]~CW: %s in %d seconds.\n",w[rs_which],rs_countdown);
	write_room(NULL,text);
	rs_announce=time(0);
	}
}

/*** login_time_out is the length of time someone can idle at login,
     user_idle_time is the length of time they can idle once logged in.
     Also ups users total login time. ***/
void check_idle_and_timeout(void)
{
UR_OBJECT user,next;
int tm;

/* Use while loop here instead of for loop for when user structure gets
   destructed, we may lose ->next link and crash the program */

user=user_first;
while(user) {
	next=user->next;
	if (user->type==CLONE_TYPE) {  user=next;  continue;  }
	user->total_login+=heartbeat;
	if (user->level>time_out_maxlevel) {  user=next;  continue;  }
	tm=(int)(time(0) - user->last_input);
	if (user->login>0 && tm>=login_idle_time) {
          write_user(user,login_timeout);
		disconnect_user(user);
		user=next;
		continue;
		}
	if (user->warned==1) {
		if (tm<user_idle_time-120) {  user->warned=0;  continue;  }
		if (tm>=user_idle_time) {
			if (kill_idle_users) {
                    write_user(user,idle_user_timeout);
                    sprintf(text,idle_kill_prompt1,user->recap);
				write_room(NULL,text);
                    write_room(NULL,idle_kill_prompt2);
				disconnect_user(user);
				}
			else if (user->warned==1) {
                    write_user(user,idle_user_drift);
				user->afk=1;
				strcpy(user->afk_mesg,"appears to be sleeping!");
                    sprintf(text,idle_room_drift,user->recap);
				write_room_except(user->room,text,user);
				user->warned=2;
				}
			user=next;
			continue;
			}
		}
	if ((!user->afk || (user->afk && time_out_afks))
	    	&& !user->login
		&& !user->warned
		&& tm>=user_idle_time-120) {
		if (kill_idle_users) {
               write_user(user,idle_user_warning);
               sprintf(text,idle_room_drift);
			write_room(user->room,text);
			}
		user->warned=1;
		}
	user=next;
	}
}

/*** Remove any expired messages from boards unless force = 2,
     in which case just do a recount. ***/
void check_messages(UR_OBJECT user,int force)
{
RM_OBJECT rm;
FILE *infp,*outfp=NULL;
char id[82],filename[80],line[82],color[80],from[80],name[80];
int valid,pt,write_rest,pth,shit;
int board_cnt,old_cnt,bad_cnt,tmp,msg_num;
static int done=0;

switch(force) {
	case 0:
	if (mesg_check_hour==thour && mesg_check_min==tmin) {
		if (done) return;
		}
	else {  done=0;  return;  }
	break;

	case 1:
	shit=1;
	}
done=1;
board_cnt=0;
old_cnt=0;
bad_cnt=0;

for(rm=room_first;rm!=NULL;rm=rm->next) {
	tmp=rm->mesg_cnt;
	rm->mesg_cnt=0;
	sprintf(filename,"%s/%s.B",DATAFILES,rm->name);
	if (!(infp=fopen(filename,"r"))) continue;
	if (force<2) {
		if (!(outfp=fopen("tempfile","w"))) {
			if (force) fprintf(stderr,"NUTS: Couldn't open tempfile.\n");
			write_syslog("ERROR: Couldn't open tempfile in check_messages().\n",0);
			fclose(infp);
			return;
			}
		}
	board_cnt++;
	/* We assume that once 1 in date message is encountered all the others
	   will be in date too , hence write_rest once set to 1 is never set to
	   0 again */
	valid=1; write_rest=0;
	fgets(line,82,infp); /* max of 80+newline+terminator = 82 */
	while(!feof(infp)) {
		if (*line=='\n') valid=1;
		sscanf(line,"%s %d %d %s %s %s",id,&pt,&msg_num,color,from,name);
		pth=pt;
		if (!write_rest) {
			if (valid && !strcmp(id,"PT:")) {
				if (force==2 || force==3) rm->mesg_cnt++;
				else {
					/* 86400 = num. of secs in a day */
					if ((int)time(0) - pt < mesg_life*86400) {
						rm->mesg_cnt++;
						msg_num=rm->mesg_cnt;
						sprintf(line,"%s %d %d\r%s %s %s \n",id,pth,msg_num,color,from,name);
						fputs(line,outfp);
						write_rest=1;
						}
					else old_cnt++;
					}
				valid=0;
				}
			}
		else {
			if (valid && !strcmp(id,"PT:")) {
				rm->mesg_cnt++;
				msg_num=rm->mesg_cnt;
				sprintf(line,"%s %d %d\r%s %s %s \n",id,pth,msg_num,color,from,name);
				fputs(line,outfp);
				valid=0;
				}
			else fputs(line,outfp);
			}
		fgets(line,82,infp);
		}
	fclose(infp);
	if (force<2) {
		fclose(outfp);
		unlink(filename);
		if (!write_rest) unlink("tempfile");
		else rename("tempfile",filename);
		}
	if (rm->mesg_cnt!=tmp) bad_cnt++;
	}
if (com_num==WIPE || com_num==DMAIL) return;
switch(force) {
	case 0:
        write_room(NULL,"~CB[~CYM~CYoenuts~CB]~CW: ~FGRoutine Backup Is Taking Place...");
	if (bad_cnt) sprintf(text,"CHECK_MESSAGES: %d files checked, %d had an incorrect message count, %d messages deleted.\n",board_cnt,bad_cnt,old_cnt);
	else sprintf(text,"CHECK_MESSAGES: %d files checked, %d messages deleted.\n",board_cnt,old_cnt);
	write_syslog(text,1);
	sprintf(filename,"%s/%s.%02d%02d.%02d%02d",LOGFILES,SYSLOG,tmonth+1,tmday,thour,tmin);
	rename(SYSLOG,filename);
	sprintf(text,"SYSLOG Renamed to: %s/%s.%02d%02d.%02d%02d\n",LOGFILES,SYSLOG,tmonth+1,tmday,thour,tmin);
	write_syslog(text,1);
	sprintf(filename,"%s/%s.%02d%02d.%02d%02d",LOGFILES,LOGINLOG,tmonth+1,tmday,thour,tmin);
	rename(LOGINLOG,filename);
	sprintf(text,"LOGINLOG Renamed to: %s/%s.%02d%02d.%02d%02d\n",LOGFILES,LOGINLOG,tmonth+1,tmday,thour,tmin);
	write_syslog(text,1);
	write_loginlog(text,1);
	if (!BACKUPFILES) write_syslog("Backup Of Talker Files Disabled!\n",1);
	else backup_talker(); /* Backup Talker When Logfiles Are Backed Up */
        write_room(NULL,"  ~RS~FB-~CB=~CT] ~CYDone ~CT[~CB=~FB-\n");
	break;

	case 1:
        printf("|> %d board files checked, %d out of date messages found.\n",board_cnt,old_cnt);
	break;

	case 2:
        sprintf(text,"~FT%d board files checked, %d had an incorrect message count.\n",board_cnt,bad_cnt);
	write_user(user,text);
	sprintf(text,"%s forced a recount of the message boards.\n",user->name);
	write_syslog(text,1);
	}
}

/*** print out greeting in large letters ***/
void greet(UR_OBJECT user,char *inpstr)
{
char pbuff[ARR_SIZE],temp[8], *name;
int slen,lc,c,i,j;
char *clr[]={"~CM","~CG","~CM","~CY","~CR"};

if (user->muzzled & 1) {
   write_user(user,"You are muzzled, you cannot greet.\n");  return;
   }
if (word_count<2) {
   write_user(user,"Usage: greet <message>\n"); return;
   }
if (ban_swearing && contains_swearing(inpstr)) {
    write_user(user,noswearing);
    return;
    }
if (strlen(inpstr)>11) {
    write_user(user,"You can only have up to 11 letters in the greet.\n");
    return;
    }
if (user->vis) name=user->recap; else name=invisname;

if (!user->hidden) {
     sprintf(text,greet_style,name);
	write_room(user->room,text);
	}
slen=strlen(inpstr);
if (slen>11) slen=11;
for (i=0; i<5; ++i) {
   pbuff[0] = '\0';
   temp[0]='\0';
   for (c=0; c<slen; ++c) {
     /* check to see if it's a character a-z */
     if (isupper(inpstr[c]) || islower(inpstr[c])) {
       lc = tolower(inpstr[c]) - 'a';
       if ((lc >= 0) && (lc < 27)) {
         for (j=0;j<5;++j) {
	   if(biglet[lc][i][j]) {
	     sprintf(temp,"%s#",clr[rand()%5]);
	     strcat(pbuff,temp);
	     }
	   else strcat(pbuff," ");
	   }
         strcat(pbuff,"  ");
         }
       }
     /* check if it's a character from ! to @ */
     if (isprint(inpstr[c])) {
       lc = inpstr[c] - '!';
       if ((lc >= 0) && (lc < 32)) {
         for (j=0;j<5;++j) {
	   if(bigsym[lc][i][j]) {
	     sprintf(temp,"%s#",clr[rand()%5]);
	     strcat(pbuff,temp);
	     }
	   else strcat(pbuff," ");
	   }
         strcat(pbuff,"  ");
         }
       }
     }
   sprintf(text,"%s\n",pbuff);
   write_room(user->room,text);
   }
write_room(user->room,"\n");
if (!user->hidden) {
	strtoupper(inpstr);
        sprintf(text,"%s announces: %s",name,inpstr);
	record(user->room,text);
	}
}

/* adds a name to the userlist */
void add_userlist(char *name)
{
FILE *fp;
char filename[80];

if (in_userlist(name)) return; /* Why bother adding it if it's there already? */
sprintf(filename,"%s/%s",USERFILES,USERLIST);
if ((fp=fopen(filename,"a"))) {
	fprintf(fp,"%s\n",name);
	fclose(fp);
	}
}

/* takes a name out of the userlist file */
void clean_userlist(char *name)
{
char filename[80], check[USER_NAME_LEN+1];
FILE *fpi,*fpo;

sprintf(filename,"%s/%s",USERFILES,USERLIST);
if (!(fpi=fopen(filename,"r"))) return;
if (!(fpo=fopen("templist","w"))) { fclose(fpi);  return; }

name[0]=toupper(name[0]);
fscanf(fpi,"%s",check);
while(!(feof(fpi))) {
	check[0]=toupper(check[0]);
	if (strcasecmp(name,check)) fprintf(fpo,"%s\n",check);
	fscanf(fpi,"%s",check);
	}
fclose(fpi);
fclose(fpo);
unlink(filename);
rename("templist",filename);
}

/* checks a name to see if it's in the userlist - incase of a bug, or userlst
   gets buggered up somehow */
int in_userlist(char *name)
{
char filename[80], check[USER_NAME_LEN+1];
FILE *fp;

sprintf(filename,"%s/%s",USERFILES,USERLIST);
if (!(fp=fopen(filename,"r"))) return 0;

name[0]=toupper(name[0]);
fscanf(fp,"%s",check);
while(!(feof(fp))) {
	check[0]=toupper(check[0]);
	if (!strcasecmp(name,check)) {
		fclose(fp);
          	return 1;
		}
	fscanf(fp,"%s",check);
	}
fclose(fp);
return 0;
}

/* Display all the people logged on from the same site as user */
void samesite(UR_OBJECT user,int stage)
{
UR_OBJECT u,u_loop;
int found,cnt,same,on=0;
char filename[80],name[USER_NAME_LEN+1];
FILE *fpi;

if (!stage) {
  if (word_count<2) {
    write_user(user,"Usage: samesite user/site [all]\n");
    return;
    }
  strtolower(word[1]); strtolower(word[2]);
  if (word_count==3 && !strcmp(word[2],"all")) user->samesite_all_store=1;
  else user->samesite_all_store=0;
  if (!strcmp(word[1],"user")) {
    write_user(user,"\nEnter the name of the user to be checked against: ");
    user->misc_op=9;
    return;
    }
  if (!strcmp(word[1],"site")) {
    write_user(user,"\n~CRNOTE:~RS Partial site strings can be given, but NO wildcards.\n");
    write_user(user,"Enter the site to be checked against: ");
    user->misc_op=10;
    return;
    }
  write_user(user,"Usage: samesite user/site [all]\n");
  return;
  }

/* check for users of same site - user supplied */
if (stage==1) {
  /* check just those logged on */
  if (!user->samesite_all_store) {
    found=cnt=same=0;
    if ((u=get_user(user->samesite_check_store))==NULL) {
      write_user(user,notloggedon);
      return;
      }
    for (u_loop=user_first;u_loop!=NULL;u_loop=u_loop->next) {
      cnt++;
      if (u_loop==u) continue;
      if (strstr(u->site,u_loop->site)) {
	same++;
	if (++found==1) {
          sprintf(text,"\n~CT-=+=- ~FYUsers logged in with the same site as ~FG%s~FT -=+=-\n\n",u->name);
	  write_user(user,text);
          }
	sprintf(text,"    %s %s\n",u_loop->name,u_loop->desc);
	if (!u_loop->vis) text[3]='*';
	write_user(user,text);
        }
      }

    if (!found) {
      sprintf(text,"~FMNo users currently logged on have that same site as ~CG%s~FM.\n",u->name);
      write_user(user,text);
      }
    else {
      sprintf(text,"\n~FMOut of ~CM%d~FM users, ~CM%d~FM had the site as ~CG%s~RS ~FG(%s)\n\n",cnt,same,u->name,u->site);
      write_user(user,text);
      }
    return;
    }

  /* check all the users..  First, load the name given */
  if (!(u=get_user(user->samesite_check_store))) {
    if ((u=create_user())==NULL) {
      sprintf(text,"%s: unable to create temporary user session.\n",syserror);
      write_user(user,text);
      write_syslog("ERROR: Unable to create temporary user session in samesite() - stage 1/all.\n",0);
      return;
      }
    strcpy(u->name,user->samesite_check_store);
    if (!load_user_details(u)) {
      destruct_user(u); destructed=0;
      sprintf(text,"Sorry, unable to load user file for %s.\n",user->samesite_check_store);
      write_user(user,text);
      write_syslog("ERROR: Unable to load user details in samesite() - stage 1/all.\n",0);
      return;
      }
    on=0;
    }
  else on=1;
  /* open userlist to check against all users */
  sprintf(filename,"%s/%s",USERFILES,USERLIST);
  if (!(fpi=fopen(filename,"r"))) {
    write_syslog("ERROR: Unable to open userlist in samesite() - stage 1/all.\n",0);
    write_user(user,"~CRSorry, you are unable to use the ~CMall~CR option at this time.\n");
    return;
    }
  found=cnt=same=0;
  fscanf(fpi,"%s",name);
  while (!feof(fpi)) {
    name[0]=toupper(name[0]);
    /* create a user object if user not already logged on */
    if ((u_loop=create_user())==NULL) {
      write_syslog("ERROR: Unable to create temporary user session in samesite().\n",0);
      goto SAME_SKIP1;
      }
    strcpy(u_loop->name,name);
    if (!load_user_details(u_loop)) {
      destruct_user(u_loop); destructed=0;
      goto SAME_SKIP1;
      }
    cnt++;
    if ((on && !strcmp(u->site,u_loop->last_site)) || (!on && !strcmp(u->last_site,u_loop->last_site))) {
      same++;
      if (++found==1) {
        sprintf(text,"\n~CT-=+=- ~CYAll users from the same site as ~CG%s~CT -=+=-\n\n",u->name);
	write_user(user,text);
        }
      sprintf(text,"    %s %s\n",u_loop->name,u_loop->desc);
      write_user(user,text);
      destruct_user(u_loop);
      destructed=0;
      goto SAME_SKIP1;
      }
    destruct_user(u_loop);
    destructed=0;
  SAME_SKIP1:
    fscanf(fpi,"%s",name);
    }
  fclose(fpi);
  if (!found) {
    sprintf(text,"There was no users with the same site as %s found.\n",u->name);
    write_user(user,text);
    }
  else {
    if (!on) sprintf(text,"\n~FMOut of ~CM%d~FM users, ~CM%d~FM had the site as ~CG%s~RS ~CB(~CY%s~CB)\n\n",cnt,same,u->name,u->last_site);
    else sprintf(text,"\n~FMOut of ~CM%d~FM users, ~CM%d~CM had the site as ~CG%s ~CB(~CY%s~CB)\n\n",cnt,same,u->name,u->site);
    write_user(user,text);
    }
  if (!on) { destruct_user(u);  destructed=0; }
  return;
  } /* end of stage 1 */

/* check for users of same site - site supplied */
if (stage==2) {
  /* check just those logged on */
  if (!user->samesite_all_store) {
    found=cnt=same=0;
    for (u=user_first;u!=NULL;u=u->next) {
      cnt++;
      if (!strstr(u->site,user->samesite_check_store)) continue;
      same++;
      if (++found==1) {
        sprintf(text,"\n~CT-=+=- ~CYUsers logged in from the same site as ~CG%s~CT -=+=-\n\n",user->samesite_check_store);
	write_user(user,text);
        }
      sprintf(text,"    %s %s~RS\n",u->name,u->desc);
      if (!u->vis) text[3]='*';
      write_user(user,text);
      }
    if (!found) {
      sprintf(text,"~FMThere are no users currently logged in with the same site as %s.\n",user->samesite_check_store);
      write_user(user,text);
      }
    else {
      sprintf(text,"\n~FMOut of ~CM%d~FM users, ~CM%d~FM had the site as ~CM%s\n\n",cnt,same,user->samesite_check_store);
      write_user(user,text);
      }
    return;
    }
  /* check all the users.. */
  /* open userlist to check against all users */
  sprintf(filename,"%s/%s",USERFILES,USERLIST);
  if (!(fpi=fopen(filename,"r"))) {
    write_syslog("ERROR: Unable to open userlist in samesite() - stage 2/all.\n",0);
    write_user(user,"~FMSorry, you are unable to use the ~CRall~FM option at this time.\n");
    return;
    }
  found=cnt=same=0;
  fscanf(fpi,"%s",name);
  while (!feof(fpi)) {
    name[0]=toupper(name[0]);
    /* create a user object if user not already logged on */
    if ((u_loop=create_user())==NULL) {
      write_syslog("ERROR: Unable to create temporary user session in samesite() - stage 2/all.\n",0);
      goto SAME_SKIP2;
      }
    strcpy(u_loop->name,name);
    if (!load_user_details(u_loop)) {
      destruct_user(u_loop); destructed=0;
      goto SAME_SKIP2;
      }
    cnt++;
    if (strstr(u_loop->last_site,user->samesite_check_store)) {
      same++;
      if (++found==1) {
        sprintf(text,"\n~CT-=+=- ~CYAll users that have the site ~CG%s~CT -=+=-\n\n",user->samesite_check_store);
	write_user(user,text);
        }
      sprintf(text,"    %s %s ~RS~CB(~CG%s~CB)\n",u_loop->name,u_loop->desc,u_loop->last_site);
      write_user(user,text);
      destruct_user(u_loop);
      destructed=0;
      goto SAME_SKIP2;
      }
    destruct_user(u_loop);
    destructed=0;
  SAME_SKIP2:
    fscanf(fpi,"%s",name);
    }
  fclose(fpi);
  if (!found) {
    sprintf(text,"~FMNo users have the same site as ~CG%s.\n",user->samesite_check_store);
    write_user(user,text);
    }
  else {
    if (!on) sprintf(text,"\n~FMOut of ~CM%d~FM users, ~CM%d~FM had the site as ~CG%s~RS\n\n",cnt,same,user->samesite_check_store);
    else sprintf(text,"\nOut of ~CM%d~FM users, ~CM%d~FM had the site as ~CG%s~RS\n\n",cnt,same,user->samesite_check_store);
    write_user(user,text);
    }
  return;
  } /* end of stage 2 */
}

/** count number of users listed in USERLIST for a global count **/
void count_users(void)
{
int cnt;
char filename[80],name[USER_NAME_LEN+1];
FILE *fp;

cnt=0;
sprintf(filename,"%s/%s",USERFILES,USERLIST);
if (!(fp=fopen(filename,"r"))) {
	user_count=0;  return;
	}
fscanf(fp,"%s",name);
while (!feof(fp)) {
	cnt++;
	fscanf(fp,"%s",name);
	}
fclose(fp);
user_count=cnt;
}

/** Send A Picture To A User **/
void pictell(UR_OBJECT user,char *inpstr)
{
UR_OBJECT u;
RM_OBJECT rm;
RM_OBJECT urm;
FILE *fp;
char filename[80],line[256];
char *name,*c;

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (user->muzzled & 1) {
     write_user(user,"You are muzzled, you cannot pictell anyone.\n");
	return;
	}
if (word_count<3) {
     write_user(user,"Usage:  .ptell <user> <picture>\n");
     return;
	}
if (!(u=get_user(word[1]))) {
	write_user(user,notloggedon);
	return;
	}
if (u==user) {
     write_user(user,"Use .vpic <picture> to view a pic!\n");
	return;
	}
if ((u->hidden && (!(u->vis))) && user->level<OWNER) { write_user(user,notloggedon); return; }
if (user->gaged) {
        write_user(user,"~CW>>>>~RS Silly POTATO HEAD\n");
	return;
	}
if (u->afk) {
     if (u->afk_mesg[0]) {
      sprintf(text,"~CR%s ~FGis ~FMAFK: %s\n",u->recap,u->afk_mesg);
	}
     else {
	 sprintf(text,"%s is AFK at the moment, They cannot be sent pictures!\n",u->recap);
	write_user(user,text);
	return;
	}
     }
if (u->malloc_start!=NULL) {
        sprintf(text,"%s is using the editor at the moment and cannot be sent pictures!\n",u->recap);
	write_user(user,text);
	return;
	}
if (u->ignall && user->level<GOD) {
        sprintf(text,"~CT%s is ignoring everyone at the moment.\n",u->recap);
	write_user(user,text);
	return;
	}
if ((u->ignore & ROOM_PICTURE) && user->level<GOD) {
        sprintf(text,"~CT%s is ignoring pictures at the moment.\n",u->recap);
	write_user(user,text);
	return;
	}
if (!strcmp(user->name,u->ignuser) && user->level<GOD) {
        sprintf(text,"~CT%s is ignoring you at the moment.\n",u->recap);
	write_user(user,text);
	return;
	}
rm=user->room;
urm=u->room;
if ((rm!=urm) && (u->ignore & OUT_ROOM_TELLS) && user->level<GOD) {
        sprintf(text,"~CT%s is ignoring pictures outside the room.\n",u->recap);
	write_user(user,text);
	return;
	}
if (user->muzzled & FROZEN) {
	inpstr=remove_first(inpstr);
        sprintf(text,"\n~CM~BM[ ~FT~BMYou Send ~FW~BM%s~FG~BM: %s ~CM~BM]~RS\n",u->recap,inpstr);
	write_user(user,text);
	return;
	}
inpstr=remove_first(inpstr);

/* Check for any illegal crap in searched for filename so they cannot list
   out the /etc/passwd file for instance. */

c=word[1];
while(*c) {
	if (*c=='.' || *c++=='/') {
                write_user(user,"~CM** ~FRInvalid Filename ~FM**\n");
		sprintf(text,"%s tried to to use %s to hack the system in Pictell.\n",user->name,word[1]);
		write_syslog(text,1);
		return;
		}
	}
sprintf(filename,"%s/%s",PICFILES,inpstr);
if (!(fp=fopen(filename,"r"))) {
     sprintf(text,"~CWPictell~CB:~CT %s ~CMcould not be found...\n",inpstr);
     write_user(user,text);
     return;
     }
else {
     sprintf(text,"\n~CM~BM[ ~FT~BMYou Send ~FW~BM%s~FG~BM: %s ~CM~BM]~RS\n",u->recap,inpstr);
     write_user(user,text);
     if (user->vis) name=user->recap; else name=invisname;
     sprintf(text,"\n~CM~BM[ ~FT~BM%s sends you~FG~BM: %s ~CM~BM]~RS\n\n",name,inpstr);
     write_user(u,text);

     /* show the file */

     fgets(line,255,fp);
	while(!feof(fp)) {
          write_user(u,line);
		fgets(line,255,fp);
		}
	fclose(fp);
	}
write_user(user,"~CGPicture Has Been sent...\n");
}

/** Send A Picture To User's Current Room **/
void roompic(UR_OBJECT user,char *inpstr)
{
FILE *fp;
char filename[80],line[256];
char *name,*c;

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (user->muzzled & 1) {
     write_user(user,"You are muzzled, you cannot send pictures!\n");
	return;
	}
if (word_count<2) {
     write_user(user,"Usage:  .rpic <picture>\n");  return;
	}

/* Check for any illegal crap in searched for filename so they cannot list
   out the /etc/passwd file for instance. */

c=word[1];
while(*c) {
	if (*c=='.' || *c++=='/') {
                write_user(user,"~CM** ~FRInvalid Filename~FM **\n");
		sprintf(text,"%s tried to to use %s to hack the system in Room Picture.\n",user->name,word[1]);
		write_syslog(text,1);
		return;
		}
	}
if (user->muzzled & FROZEN) {
	inpstr=remove_first(inpstr);
     sprintf(text,"\n~CM~BM[ ~FT~BMYou Send The Room~FW~BM:~FG~BM %s ~CM~BM]~RS\n",inpstr);
	write_user(user,text);
	return;
	}
     sprintf(filename,"%s/%s",PICFILES,inpstr);
     if (!(fp=fopen(filename,"r"))) {
          sprintf(text,"~CBRoompic~CB:~FT %s ~RMcould not be found...\n",inpstr);
          write_user(user,text);
          piclist(user);
          return;
          }
     else {
          sprintf(text,"\n~CM~BM[ ~FT~BMYou Send The Room~FW~BM:~FG~BM %s ~CM~BM]~RS\n",inpstr);
          write_user(user,text);
          if (user->vis) name=user->name; else name=invisname;
          sprintf(text,"\n~CM~BM[ ~FT~BM%s sends everyone in the room~FG~BM: ~CT%s ~CM~BM]~RS\n\n",name,inpstr);
          write_room_except(user->room,text,user);

          /* show the file */

          fgets(line,255,fp);
          while(!feof(fp)) {
               write_room(user->room,line);
               fgets(line,255,fp);
               }
          fclose(fp);
          }
}

/*** Show the piclist to the user who requests it ***/
void piclist(UR_OBJECT user)
{
char filename[80];

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
sprintf(filename,"%s/%s",PICFILES,PICLISTFILE);
switch(more(user,user->socket,filename)) {
        case 0: write_user(user,"~CRI was unable to find a pictures list.  Sorry...\n"); return;
        case 1: user->misc_op=2;
        }
}

/** Viewpic (C)1997 Mike Irving, All rights Reserved **/
void viewpic(UR_OBJECT user,char *inpstr)
{
FILE *fp;
char filename[80],line[256];
char *c;

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (word_count<2) {
     write_user(user,"Usage:  .vpic <picture>\n");  return;
	}

/* Check for any illegal crap in searched for filename so they cannot list
   out the /etc/passwd file for instance. */
c=word[1];
while(*c) {
	if (*c=='.' || *c++=='/') {
                write_user(user,"~CM~BM** ~FR~BMInvalid Picture Name~CM~BM **\n");
		sprintf(text,"%s tried to to use %s to hack the system in View Pic.\n",user->name,word[1]);
		write_syslog(text,1);
		return;
		}
	}
     sprintf(filename,"%s/%s",PICFILES,inpstr);
     if (!(fp=fopen(filename,"r"))) {
          sprintf(text,"~CMViewpic~FB:~FT %s ~FMcould not be found...\n",inpstr);
          write_user(user,text);
          piclist(user);
          return;
          }
     else {
          sprintf(text,"\n~CM~BM[ ~FT~BMYou View The Following Picture~FW~BM:~FG~BM %s ~FM~BM]~RS\n",inpstr);
          write_user(user,text);

          /* show the file */

          fgets(line,255,fp);
          while(!feof(fp)) {
               write_user(user,line);
               fgets(line,255,fp);
               }
          fclose(fp);
          }
}

/*** Make User A Room ***/
void makeroom(UR_OBJECT user)
{
RM_OBJECT room;
int i;

if (!strcasecmp(user->roomname,"none")) return;

room=get_room(user->roomname);

if (room==NULL) {
  room=create_room();
  if (room==NULL) return;
  strcpy(room->name,user->roomname);
  strncpy(room->label,user->name,4);
  room->link[0]=room_first;
  room->access=FIXED_PERSONAL;
  strcpy(room->map,"users");
  strncpy(room->owner,user->name,USER_NAME_LEN);
  strncpy(room->topic,user->roomtopic,TOPIC_LEN);
  }
if (room==user->room) {
  write_user(user,"You are already in your home room.\n");
  return;
  }
sprintf(text,user_room_prompt,user->roomname);
write_user(user,text);
if (user->hideroom==1) { room->hidden=1; write_user(user,"~CB[~CYM~CYoenuts~CB]~CT: ~CRMaking Your Room Hidden To Others...\n"); }
if (!(load_atmospheres(room))) {
	write_user(user,"~CB[~CYM~CYoenuts~CB]~CT: ~CGLoading the atmospheres for your room...\n");
	for(i=0;i<ATMOSPHERES;++i) room->atmos[i][0]='\0';
	}
if (!load_room_preferences(room)) { write_user(user,"~CB[~CYM~CYoenuts~CB]~CW: ~CTLoading your room preferences failed, defaults created.\n"); }
}

/*** Give a user a room. ***/
void giveroom(UR_OBJECT user)
{
UR_OBJECT u;
RM_OBJECT rm;

if (word_count<2) {
        write_user(user,"Usage: giveroom <user> <roomname>\n");
        return;
        }
word[1][0]=toupper(word[1][0]);
if ((u=get_user(word[1]))) {
        if (u==user && user->level<OWNER) {
                write_user(user,"You cannot give yourself a room.\n");
                return;
                }
	if (word_count<3) {
                sprintf(text,"~CMGive %s ~RS~CM a room called what?!?\n",u->recap);
		write_user(user,text);
        	write_user(user,"Usage: giveroom <user> <roomname>\n");
		return;
        	}
     if (!strcasecmp(word[2],"none")) {
                sprintf(text,"~CW-> ~FRYou have taken ~FM%s~RS~CR's room away.\n",u->recap);
		write_user(user,text);
                write_user(u,"~CW-> ~FRYour room has been taken away!\n");
		if ((rm=get_room(u->roomname))) destruct_room(rm);
		strcpy(u->roomname,"None");
		strcpy(u->roomgiver,user->name); /* Set To Taker's Name */
		}
	else {
                sprintf(text,"~CW-> ~FGYou have given ~FM%s~RS~CG a room called ~FT%s.\n",u->recap,word[2]);
		write_user(user,text);
                u->roombanned=0;
		strncpy(u->roomname,word[2],ROOM_NAME_LEN);
		strcpy(u->roomgiver,user->name);
		makeroom(u);
		}
	return;
	}
if (!(u=get_user(word[1]))) {
	if ((u=create_user())==NULL) {
		sprintf(text,"%s: unable to create temporary user object.\n",syserror);
		write_user(user,text);
                write_syslog("ERROR: Unable to create temporary user object in giveroom().\n",0);
		return;
		}
	strcpy(u->name,word[1]);
	if (!load_user_details(u)) {
		write_user(user,nosuchuser);
		destruct_user(u);
		destructed=0;
		return;
		}
        }
if (u==user && user->level<OWNER) {
        write_user(user,"You cannot give yourself a room.\n");
        return;
        }
u->roombanned=0;
strncpy(u->roomname,word[2],ROOM_NAME_LEN);
strcpy(u->roomgiver,user->name);
strcpy(u->site,u->last_site);
if (!strcasecmp(u->roomname,"None")) {
        sprintf(text,"~CMSorry %s~CM...  ~FR-=[ ~CREviction Notice ~FR]=-\n~CRYour room, ~FM%s~FR, has been taken away for reasons unknown...  ~FBPay your rent?\n",u->recap,u->roomname);
	send_mail(user,word[1],text);
        sprintf(text,"~CW-> ~CRYou have taken %s~RS~CR's room away.\n",u->recap);
	write_user(user,text);
	}
else {
        sprintf(text,"~CYHey %s~CY!!!  ~CG-=[ ~CTCongratulations!!! ~CG]=-\n~CGYou have been given a personal room called ~FT%s.\n",u->recap,u->roomname);
	send_mail(user,word[1],text);
        sprintf(text,"~CW-> ~FGYou have given ~FM%s~RS~CG a room called ~FT%s.\n",u->recap,u->roomname);
        write_user(user,text);
	}
save_user_details(u,0);
destruct_user(u);
destructed=0;
}

/*** Room kill ***/
void rmkill(UR_OBJECT user)
{
UR_OBJECT u;
RM_OBJECT rm;

if (word_count<2) {
        write_user(user,"Usage: rmkill <user>\n");
	return;
        }
if (strcmp(user->name,user->room->owner)) {
        write_user(user,"This is not your room, you cannot do that.\n");
	return;
        }
word[1][0]=toupper(word[1][0]);
if (!(u=get_user(word[1]))) {
	write_user(user,notloggedon);  return;
	}
rm=room_first;
if (!rm) {
        write_user(user,"~CB[~CYM~CYoenuts~CB]~CW: ~FR? WTF?!~CR  The main room disapeared!!!\n");
        return;
        }
if (u==user) {
        sprintf(text,"~CT!! %s~CG kicks themself out of their own room!\n~CT!! ~FBWOW!!  ~FMSuch Tallent They Have! :-)\n",user->recap);
	write_room(NULL,text);
	move_user(user,rm,2);
        return;
        }
if (u->room!=user->room) {
        sprintf(text,"~CRYou look around and realize %s~RS~CR isn't in this room with you.\n",u->recap);
        write_user(user,text);
        return;
        }
move_user(u,rm,2);
sprintf(text,"~CT!! ~FY%s yells: ~FR\"AND STAY OUT!!!\" ~FYto ~FM%s\n",user->recap,u->recap);
write_user(user,text);
}

/*** Ban a user from a room ***/
void rmban(UR_OBJECT user)
{
UR_OBJECT u;

if (word_count<2) {
        write_user(user,"Usage: rmban <user>\n");  return;
        }
word[1][0]=toupper(word[1][0]);
if ((u=get_user(word[1]))) {
        if (u==user) {
                write_user(user,"You cannot ban yourself from your room.\n");
                return;
                }
        if (u->roombanned) {
                sprintf(text,"%s is already banned from their room.",u->recap);
                write_user(user,text);
                return;
                }
	u->roombanned=1;
         sprintf(text,"You have banned %s from their room. (~FT%s~RS)\n",u->name,u->roomname);
         write_user(user,text);
	if (strcasecmp(u->roomname,"None")) {
          sprintf(text,"%s has banned you from your room.\n",user->recap);
        	write_user(u,text);
        	}
	return;
     }
if (!(u=get_user(word[1]))) {
	if ((u=create_user())==NULL) {
		sprintf(text,"%s: unable to create temporary user object.\n",syserror);
		write_user(user,text);
		write_syslog("ERROR: Unable to create temporary user object in examine().\n",0);
		return;
		}
	strcpy(u->name,word[1]);
	if (!load_user_details(u)) {
		write_user(user,nosuchuser);
		destruct_user(u);
		destructed=0;
		return;
		}
        }
if (u==user) {
        write_user(user,"You cannot ban yourself from your room.\n");
        return;
        }
if (u->roombanned) {
        sprintf(text,"%s is already banned from their room.",u->recap);
        write_user(user,text);
        return;
        }
u->roombanned=1;
sprintf(text,"You have banned %s from their room.  (~FT%s~RS)\n",u->name,u->roomname);
write_user(user,text);
strcpy(u->site,u->last_site);
save_user_details(u,0);
if (strcasecmp(u->roomname,"None")) send_mail(user,word[1],"~CRYou have been banned from your personal room.\n");
destruct_user(u);
destructed=0;
}

/*** Unban a user from a room ***/
void rmunban(UR_OBJECT user)
{
UR_OBJECT u;

if (word_count<2) {
        write_user(user,"Usage: rmban <user>\n");  return;
        }
word[1][0]=toupper(word[1][0]);
if ((u=get_user(word[1]))) {
        if (u==user) {
                write_user(user,"You cannot un-ban yourself from your room.\n");
                return;
                }
        if (!u->roombanned) {
                sprintf(text,"%s is not banned from their room.",u->recap);
                write_user(user,text);
                return;
                }
	u->roombanned=0;
        sprintf(text,"%s is no longer banned from their room. (~FT%s~RS)\n",u->name,u->roomname);
        write_user(user,text);
	if (strcasecmp(u->roomname,"None")) {
	        sprintf(text,"%s has removed your room ban.\n",user->recap);
        	write_user(u,text);
        	}
	return;
        }
if (!(u=get_user(word[1]))) {
	if ((u=create_user())==NULL) {
		sprintf(text,"%s: unable to create temporary user object.\n",syserror);
		write_user(user,text);
		write_syslog("ERROR: Unable to create temporary user object in examine().\n",0);
		return;
		}
	strcpy(u->name,word[1]);
	if (!load_user_details(u)) {
		write_user(user,nosuchuser);
		destruct_user(u);
		destructed=0;
		return;
		}
        }
if (u==user) {
        write_user(user,"You cannot un-ban yourself from your room.\n");
        return;
        }
if (!u->roombanned) {
        sprintf(text,"%s is not banned from their room.",u->recap);
        write_user(user,text);
        return;
        }
u->roombanned=0;
sprintf(text,"~CRYou have un-banned ~CM%s~CR from their room.  ~CB(~CG%s~CB)\n",u->name,u->roomname);
write_user(user,text);
strcpy(u->site,u->last_site);
save_user_details(u,0);
if (strcasecmp(u->roomname,"None")) send_mail(user,word[1],"~CMYour room ban has been lifted.\n");
destruct_user(u);
destructed=0;
return;
}

/** Goto User's Room **/
void goto_myroom(UR_OBJECT user)
{
RM_OBJECT rm;

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if ((rm=get_room(user->roomname))==NULL) {
	write_user(user,nosuchroom);
	return;
	}
if (user->roombanned) {
	write_user(user,"You are banned from your room, you cannot enter it!\n");
	return;
	}
if (rm==user->room) {
        write_user(user,"~CGYou are already in your room!\n");
	return;
	}
move_user(user,rm,3);
}

/*** Toggle hidden attribute for a user ***/
void toggle_hidden(UR_OBJECT user)
{
if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (user->muzzled & SCUM) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (user->hidden) {
        write_user(user,"~CRYou will be no longer hidden from others.\n");
	user->hidden=0;
	}
else {
	user->hidden=1;
        write_user(user,"~CGYou will now be hidden from others.\n");
	}
}

/*** Shackle a user to their current room so they cannot move ***/
void shackle(UR_OBJECT user)
{
UR_OBJECT u;
char *name, *rname;

if (user->vis) name=user->recap; else name=invisname;

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (user->muzzled & 1) {
        write_user(user,"You are not permitted to shackle anyone while muzzled!\n");
	return;
	}
if (word_count<2) {
     write_user(user,"Usage:  shackle <user> \n");
     return;
     }
if (!(u=get_user(word[1]))) {
	write_user(user,notloggedon);
        return;
	}
if (u->vis) rname=u->recap; else rname=invisname;
if (u->shackled) {
        sprintf(text,"~CT%s has already been shackled by a %s!\n",u->recap,level_name[u->shackle_level]);
	write_user(user,text);
	return;
	}
if (u==user) {
     write_user(user,"Trying to shackle yourself is universally stupid!\n");
     return;
     }
if (user->level<=u->level) {
        write_user(user,"~CRYou cannot shackle someone of equal or greater level than youself\n");
        sprintf(text,"~CR%s thought about shackleing you to this room.\n",user->recap);
	write_user(u,text);
	sprintf(text,"%s attempted to SHACKLE %s.\n",user->name,u->name);
	write_syslog(text,1);
	return;
	}
if (user->muzzled & FROZEN) {
     sprintf(text,"~CRYou shackle %s so they cannot leave.\n",rname);
     write_user(user,text);
     return;
	}
sprintf(text,"~CRYou shackle %s so they cannot leave.\n",rname);
write_user(user,text);
sprintf(text,"~CR%s orders the guards to shakle you to the room so you cannot leave.\n",name);
write_user(u,text);
sprintf(text,"~CR%s orders the guards to shackle %s so they cannot leave this room.\n",name,rname);
write_room_except(u->room,text,u);
u->shackled=1;
u->shackle_level=user->level;
sprintf(text,"%s was shackled by %s.  Shackle Level = %s\n",u->name,user->name,level_name[user->level]);
write_syslog(text,1);
write_arrestrecord(u,text,1);
}

/*** Unshackle a user so they can move about again ***/
void unshackle(UR_OBJECT user)
{
UR_OBJECT u;
char *name, *rname;

if (user->vis) name=user->recap; else name=invisname;

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (user->muzzled & 1) {
        write_user(user,"You are not permitted to un-shackle anyone while muzzled!\n");
	return;
	}
if (word_count<2) {
        write_user(user,"Usage:  unshackle <user> \n");
        return;
	}
if (!(u=get_user(word[1]))) {
	write_user(user,notloggedon);
        return;
	}
if (u->vis) rname=u->recap; else rname=invisname;
if (!u->shackled) {
        sprintf(text,"~CT%s is not currently shackled.\n",u->recap);
	write_user(user,text);
	return;
	}
if (u==user) {
     write_user(user,"~FRYou try to remove your own shackles, but to no avail!\n");
     return;
     }
if (user->level<=u->level) {
        write_user(user,"~CRYou cannot un-shackle someone of equal or greater level than youself\n");
        sprintf(text,"~CR%s wanted to un-shackle you.\n",user->name);
	write_user(u,text);
	sprintf(text,"%s attempted to remove %s's shackles.\n",user->name,u->name);
	write_syslog(text,1);
	return;
	}
if (user->muzzled & FROZEN) {
     sprintf(text,"~CRYou release %s from their shackles.\n",rname);
     write_user(user,text);
     return;
     }
sprintf(text,"~CRYou release %s from their shackles.\n",rname);
write_user(user,text);
sprintf(text,"~CR%s orders the guards to release you from your shackles.\n",name);
write_user(u,text);
sprintf(text,"~CR%s orders the guards to release %s from their shackles.\n",name,rname);
write_room_except(u->room,text,u);
u->shackled=0;
u->shackle_level=0;
sprintf(text,"%s was unshackled by %s (%s)\n",u->name,user->name,level_name[user->level]);
write_syslog(text,1);
write_arrestrecord(u,text,1);
}

/*** Make someone Invisible ***/
void makeinvis(UR_OBJECT user)
{
UR_OBJECT u;
char *name, *rname;

if (user->vis) name=user->recap; else name=invisname;

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (user->muzzled & 1) {
        write_user(user,"You are not permitted to use this command.\n");
	return;
	}
if (word_count<2) {
        write_user(user,"Usage:  makeinvis <user> \n");
        return;
	}
if (!(u=get_user(word[1]))) {
	write_user(user,notloggedon);
        return;
	}
if (u->vis) rname=u->recap; else rname=invisname;
if (u==user) {
     visibility(user,0);
     return;
     }
if (user->level<=u->level) {
        write_user(user,"~CRYou cannot make someone of equal or greater level than youself invisible.\n");
        sprintf(text,"~CR%s thought about making you disapear.\n",user->name);
	write_user(u,text);
	sprintf(text,"%s attempted to make %s invisible.\n",user->name,u->name);
	write_syslog(text,1);
	return;
	}
if (!u->vis) {
     write_user(user,"~CTThat user is already invisible.\n");
     return;
     }
if (user->muzzled & FROZEN) {
     sprintf(text,"~CTYou cast an ancient spell and makes %s disappear.\n",rname);
     write_user(user,text);
     return;
     }
sprintf(text,"~CTYou cast a ancient spell and makes %s disappear.\n",rname);
write_user(user,text);
visibility(u,0);
sprintf(text,"%s was made invisible by %s.  Level = %s\n",u->name,user->name,level_name[user->level]);
write_syslog(text,1);
write_arrestrecord(u,text,1);
}

/*** Make a user visible again ***/
void makevis(UR_OBJECT user)
{
UR_OBJECT u;
char *name, *rname;

if (user->vis) name=user->recap; else name=invisname;

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (user->muzzled & 1) {
     write_user(user,"You are not permitted to use this command.\n");
	return;
	}
if (word_count<2) {
     write_user(user,"Usage:  makevis <user> \n");
     return;
	}
if (!(u=get_user(word[1]))) {
	write_user(user,notloggedon);
     return;
	}
if (u->vis) rname=u->recap; else rname=invisname;
if (u==user) {
     visibility(user,1);
     return;
     }
if (u->vis) {
     write_user(user,"~CTThat user is already visible.\n");
     return;
     }
if (user->level<=u->level) {
        write_user(user,"~CRYou cannot make someone of equal or greater level than youself visible\n");
        sprintf(text,"~CR%s thought about making you visible.\n",user->name);
	write_user(u,text);
	sprintf(text,"%s attempted to make %s visible.\n",user->name,u->name);
	write_syslog(text,1);
	return;
	}
if (user->muzzled & FROZEN) {
     sprintf(text,"~CTYou cast a ancient spell and makes %s re-appear.\n",rname);
     write_user(user,text);
     return;
	}
sprintf(text,"~CTYou cast a ancient spell and makes %s re-appear.\n",rname);
write_user(user,text);
visibility(u,1);
sprintf(text,"%s was made visible by %s.  Level = %s\n",u->name,user->name,level_name[user->level]);
write_syslog(text,1);
write_arrestrecord(u,text,1);
}

/** ShowScreen (C)1998 Mike Irving, All rights Reserved **/
int show_screen(UR_OBJECT user)
{
FILE *fp;
char filename[80],line[ARR_SIZE+1];
int retcode,ansi,view;
retcode=0; ansi=0; view=0;

sprintf(filename,"%s/%s.%s",SCREENFILES,file,fileext[1]);
ansi=file_exists(filename);
if (user->high_ascii && ansi) {
	sprintf(filename,"%s/%s.%s",SCREENFILES,file,fileext[user->high_ascii]);
	view=1;
	}
else sprintf(filename,"%s/%s.%s",SCREENFILES,file,fileext[0]);

if (view) {
	retcode=viewfile(user,filename);
	return retcode;
	}
if (!(fp=fopen(filename,"r"))) return 0;

fgets(line,ARR_SIZE,fp);
while(!feof(fp)) {
	line[strlen(line)-1]=0;
	strcat(line,"\n");
        write_user(user,line);
        fgets(line,ARR_SIZE,fp);
        }
fclose(fp);
return 1;
}

void show_version(UR_OBJECT user)
{
     int sz=0;
     count_users(); /* Count Number Of Users With Accounts Here */

     if (user->high_ascii) write_user(user,"\n~CBƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
     else write_user(user,"~CB----------------------------------------------------------------------------\n");
     write_user(user,"~CT        Moenuts (C)1997 - 2003, Michael Irving, All Rights Reserved.\n");
     if (user->high_ascii) write_user(user,"~CBƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
     else write_user(user,"~CB----------------------------------------------------------------------------\n");
     sprintf(text,"~CMNUTS version      ~FT: ~FGv%s\n",VERSION);
     write_user(user,text);
     sprintf(text,"~CMMoenuts version   ~FT: ~FGv%s compiled at %s on %s\n",MNVERSION,COMPILE_TIME,COMPILE_DATE);
     write_user(user,text);
     sz=(strlen(COMPILE_FULL)-8);
     sprintf(text,"~CMMoenuts host info ~FT: ~FG%*.*s\n",sz,sz,COMPILE_FULL);
     write_user(user,text);
     if (user->level==OWNER) {
          sprintf(text,"~CMExecutable/Config ~FT: ~FG%s %s\n",progname,confile);
          write_user(user,text);
          }
     sprintf(text,"~CMNumber of Users   ~FT: ~FG%d\n",user_count);
     write_user(user,text);
     sprintf(text,"~CMGame  Library     ~FT: ~FG%s\n",GAMELIBID);
     write_user(user,text);
     sprintf(text,"~CMQuote Library     ~FT: ~FG%s\n",QUOTELIBID);
     write_user(user,text);
     sprintf(text,"~CMAFK   Library     ~FT: ~FG%s\n",AFKLIBID);
     write_user(user,text);
     sprintf(text,"~CMAtmosphere Library~FT: ~FG%s\n",ATMOSLIBID);
     write_user(user,text);
     sprintf(text,"~CMPrompts Library   ~FT: ~FG%s\n",PROMPT_VERSION);
     write_user(user,text);
     if (user->high_ascii) write_user(user,"~CBƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
     else write_user(user,"~CB----------------------------------------------------------------------------\n");
}

/* Autopromote Routines */
void autopromote(UR_OBJECT user)
{
FILE *fp;
int genderset;
char filename[80],*center();

if (!sys_allow_autopromote) return;

genderset=0;
if (user->prompt & 4 && !genderset) genderset=1;
if (user->prompt & 8 && !genderset) genderset=1;

if (!user->level==NEW) return;
sprintf(filename,"%s/%s.P",USERFILES,user->name);
if(!(fp=fopen(filename,"r"))) {
	write_user(user,"You still need to set the following items:\n");
	write_user(user,"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
        if (!user->accreq) write_user(user,"Use: .accreq <your VALID email address>\n");
        if (!strcmp(user->desc,DEFAULTDESC)) write_user(user,"Use: .desc <a short description which goes beside your name>\n");
        if (user->age==0) write_user(user,"Use: .set age <1 - 99> to set your REAL age. (Use .set fakeage to set the age ppl will see)\n");
        if (!genderset)  write_user(user,"Use: .set gender m/f to set your gender.\n");
        write_user(user,"Use: .entpro, and enter a small profile of yourself.\n");
        return;
        }
else {
     fclose(fp);
     if ((user->accreq) && (strcmp(user->desc,DEFAULTDESC)) && (!user->age==0) && genderset) {
          user->ignall=user->ignall_store;
          user->level=USER;
	  strncpy(user->level_alias,level_name[user->level],USER_ALIAS_LEN);
          sprintf(text,"\n~CG%s has been auto-promoted to level: ~FW%s!",user->name,level_name[user->level]);
          write_room(NULL,text);
          write_room(NULL,"\n                                       ~CY,,, \n");
          write_room(NULL,"                                      ~CM(~CBo o~CM) \n");
          write_room(NULL,"----------------------------------~CMoOO~RS--~FT(_)~RS--~CMOOo~RS-------------------------------\n");
          sprintf(text,autopromote_style,user->name,TALKERNAME);
          write_room(NULL,center(text,80));
          write_room(NULL,"\n------------------------------------------------------------------------------\n\n");
	  /* We save the user's details just incase the talker crashes and they get DC'd before the userfile gets saved */
  	  save_user_details(user,1);
	  if (strcasecmp(newbie_room,"none")) move_user(user,room_first,0); /* Move Newbie To The First (Main) Room */
  	  return;
          }
	  else {
                write_user(user,"~CYYou still need to set the following items:\n");
		write_user(user,"~FY~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
                if (!user->accreq) write_user(user,"Use: .accreq <your real VALID email address>\n");
                if (!strcmp(user->desc,DEFAULTDESC)) write_user(user,"Use: .desc <a short description> which goes beside your name\n");
                if (user->age==0) write_user(user,"Use: .set age <1 - 99> to set your REAL age. (Use: .set fakeage to set the age everyone else sees.)\n");
                if (!genderset) write_user(user,"Use: .set gender m/f  to set your gender.\n");
		}
     }
}

/* Who List Styles */
void hopewho(UR_OBJECT user)
{
UR_OBJECT u;
UR_OBJECT cu=NULL;
RM_OBJECT rm;

int cnt,total,invis,mins,idle,logins,hidden;
char line[USER_NAME_LEN+USER_DESC_LEN*2];
char rname[ROOM_RECAP_LEN+1],levelname[20];
char gender[10],text2[ARR_SIZE+1];

     cnt=0;  total=0;  invis=0;  logins=0;  hidden=0;
     write_user(user,"\n\n");
     sprintf(text2,"People At %s %s",TALKERNAME,long_date(1));
     if (user->high_ascii) {
        write_user(user,"~FB-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-\n");
           sprintf(text,"~FB-==-~CT%-67.67s~RS~FB-==-\n",center(text2,66));
        write_user(user,text);
        write_user(user,"~FB-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-\n");
        write_user(user,"~FB∫    ~CTRoom Name:   ~RS~FB∫~CYG~RS~FB∫ ~CMRank    ~RS~FB∫~CGTime~FW:~FRIdle~RS~FB∫ ~FTName and Description\n");
        write_user(user,"~FB«ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ◊ƒ◊ƒƒƒƒƒƒƒƒƒ◊ƒƒƒƒƒƒƒƒƒ◊ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
        }
     else {
        write_user(user,"~FB_.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._\n");
           sprintf(text,"~FB_.-[ ~CT%-67.67s ~RS~FB]-._\n",center(text2,66));
        write_user(user,text);
        write_user(user,"~FB_.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._\n");
        write_user(user,"~FB|    ~CTRoom Name:   ~RS~FB|~CYG~RS~FB| ~CMRank    ~RS~FB|~CGTime~FW:~FRIdle~RS~FB| ~FTName and Description\n");
        write_user(user,"~FB`.-._.-._.-._.-._.+._.+._.-._.-._.-._.-._.-.+.-._.-._.-._.-._.-._.-._.-._.-._\n");
        }
     for(rm=room_first;rm!=NULL;rm=rm->next) {
     for(u=user_first;u!=NULL;u=u->next) {
          if (u->login) {logins++; continue; }
          if (u->room->hidden && user->level<OWNER) continue;
          if (u->type==CLONE_TYPE) cu=get_user(u->name);
	  if (!(u->room==rm)) continue;
          mins=(int)(time(0) - u->last_login)/60;
          idle=(int)(time(0) - u->last_input)/60;
          if (u->type==CLONE_TYPE) {
               mins=(int)(time(0) - cu->last_login)/60;
               idle=(int)(time(0) - cu->last_input)/60;
               }
          if (!u->level && u->muzzled & SCUM) strcpy(levelname,"Scum");
          else strcpy(levelname,u->level_alias);
          if (u->type==CLONE_TYPE) {
               if (!cu->level && cu->muzzled & SCUM) strcpy(levelname,"Scum");
               else strcpy(levelname,cu->level_alias);
               }
          if (u->muzzled & JAILED) strcpy(levelname,"Jailed");
          strcpy(gender,"~CGN");
          if (u->prompt & FEMALE) strcpy(gender,"~CMF");
          if (u->prompt & MALE)   strcpy(gender,"~CTM");
          if (u->type==CLONE_TYPE) {
               strcpy(gender,"Neuter");
               if (cu->prompt & FEMALE) strcpy(gender,"~CMF");
               if (cu->prompt & MALE)   strcpy(gender,"~CTM");
               }
          ++total;
          if (u->login) logins++;
          if (u->hidden && user->level<OWNER) { total--; hidden++; continue; }
          if (!u->vis) {
               --total; ++invis;
               if (u->level>user->level) continue;
               }
          strcpy(rname,u->room->recap);
          sprintf(line," %s %s",u->recap,u->desc);
          if (u->afk) sprintf(line," %s is away from the keyboard.",u->recap);
          else if (idle>10) sprintf(line," %s appears to be sleeping.",u->recap);
          else if (u->malloc_start!=NULL) sprintf(line," %s is using the text editor",u->recap);
          if (u->type==CLONE_TYPE) {
                if (cu->afk) sprintf(line," %s is away from the keyboard.",u->recap);
                else if (idle>10) sprintf(line," %s appears to be sleeping.",u->recap);
                else if (cu->malloc_start!=NULL) sprintf(line," %s is using the editor.",u->recap);
                }
	  /* Process User Status */
          if (!u->vis) line[0]='!';
          if (u->hidden) line[0]='#';
          if (u->room->hidden) line[0]='&';
          cnt=colour_com_count(rname);
          if (user->high_ascii) sprintf(text,"~RS~FB∫ ~CT%-*.*s ~RS~FB∫~CY%s~RS~FB∫ ~CM%-7.7s ~RS~FB∫~CG%4d~FW:~FR%-4d~RS~FB∫~RS%s\n",15+cnt*3,15+cnt*3,rname,gender,levelname,mins,idle,line);
          else sprintf(text,"~CT  %-*.*s ~RS~FB|~CY%s~RS~FB| ~CM%-7.7s ~RS~FB|~CG%4d~FW:~FR%-4d~RS~FB|~RS%s\n",15+cnt*3,15+cnt*3,rname,gender,levelname,mins,idle,line);
          write_user(user,text);
          continue;
          }
     }
     if (user->high_ascii) write_user(user,"~FB-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-\n");
     else write_user(user,"~FB_.-._.-._.-._.-._.-._.+._.-._.-._;-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._\n");
     if (user->level>WIZ) {
         sprintf(text,"  ~FRThere are %d invisible and %d people at the login stage.\n",invis,logins);
         write_user(user,text);
         }
    sprintf(text,"  ~CBThere %s currently %d %s in the %s.\n",(total==1?"is":"are"),total,(total==1?"person":"people"),SHORTNAME);
    write_user(user,text);
    if (user->high_ascii) write_user(user,"~FB-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-\n\n");
    else write_user(user,"~FB_.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._\n\n");
}

void stairwho(UR_OBJECT user)
{
UR_OBJECT u;
UR_OBJECT cu=NULL;
RM_OBJECT rm;

int rncnt, cnt,total,invis,mins,idle,logins,hidden;
char line[USER_NAME_LEN+USER_DESC_LEN*2];
char rname[ROOM_NAME_LEN+1],levelname[20];
char status[6], gender[7],*center();

     total=0;  invis=0;  logins=0;  hidden=0;/* Init Vars to 0 */
     write_user(user,"\n");
     sprintf(text,"~FMPeople roaming %s %s",TALKERNAME,long_date(1));
     write_user(user,center(text,75));
     write_user(user,"\n\n");
     if (user->high_ascii) {
          write_user(user,"~CB⁄ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ¬ƒƒƒ¬ƒƒƒƒƒ¬ƒƒƒƒƒƒƒƒƒƒ¬ƒƒƒƒ¬ƒƒƒƒ¬ƒø\n");
          write_user(user,"~CB≥   ~FTName And Description                  ~FB≥~FTm/f~FB≥~FTLevel~FB≥~FTRoom      ~FB≥~FTMins~FB≥~FTIdle~FB≥~FTS~FB≥\n");
          write_user(user,"~CB√ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ¡¬ƒ¬¡ƒƒƒƒƒ≈ƒƒƒƒƒƒƒƒƒƒ≈ƒƒƒƒ≈ƒƒƒƒ≈ƒ¥\n");
          }
     else {
          write_user(user,"~CB+--------------------------------------------------------------------------+\n");
          write_user(user,"~CB|   ~FTName and Description                  ~FB|~FTm/f~FB|~FTLevel~FB|~FTRoom      ~FB|~FTMins~FB|~FTIdle~FB|~FTS~FB|\n");
          write_user(user,"~CB+------------------------------------------+-+------+----------+----+----+-+\n");
          }
     for(rm=room_first;rm!=NULL;rm=rm->next) {
     for(u=user_first;u!=NULL;u=u->next) {
          if (u->login) {logins++; continue; }
          if (u->room->hidden && user->level<OWNER) continue;
          if (u->type==CLONE_TYPE) cu=get_user(u->name);
	  if (!(u->room==rm)) continue;
          mins=(int)(time(0) - u->last_login)/60;
          idle=(int)(time(0) - u->last_input)/60;
          if (u->type==CLONE_TYPE) {
               mins=(int)(time(0) - cu->last_login)/60;
               idle=(int)(time(0) - cu->last_input)/60;
               }
          if (!u->level && u->muzzled & SCUM) strcpy(levelname,"Scum");
          else strcpy(levelname,u->level_alias);
          if (u->type==CLONE_TYPE) {
               if (!cu->level && cu->muzzled & SCUM) strcpy(levelname,"Scum");
               else strcpy(levelname,cu->level_alias);
               }
          if (u->muzzled & JAILED) strcpy(levelname,"Jailed");
          strcpy(gender,"Neuter");
          if (u->prompt & FEMALE) strcpy(gender,"Female");
          if (u->prompt & MALE)   strcpy(gender,"Male  ");
          if (u->type==CLONE_TYPE) {
               strcpy(gender,"Neuter");
               if (cu->prompt & FEMALE) strcpy(gender,"Female");
               if (cu->prompt & MALE)   strcpy(gender,"Male  ");
               }
          ++total;
          if (u->login) logins++;
          if (u->hidden && user->level<OWNER) { total--; hidden++; continue; }
          if (!u->vis) {
               --total;
               ++invis;
               if (u->level>user->level) continue;
               }
          sprintf(line,"  %s %s",u->recap,u->desc);
          if (!u->vis) line[0]='!';
          if (u->hidden) line[1]='#';
          if (u->room->hidden) line[1]='&';
          strcpy(rname,u->room->recap);
          if (u->afk) strcpy(status,"A");
          else if (u->malloc_start!=NULL) strcpy(status,"E");
          else if (idle) strcpy(status,"i");
          else if (!idle) strcpy(status,"a");
          if (u->type==CLONE_TYPE) {
               if (cu->afk) strcat(status,"A");
               else if (cu->malloc_start!=NULL) strcpy(status,"E");
               else if (idle) strcpy(status,"i");
               else if (!idle) strcpy(status,"a");
               }
          /* Count number of colour coms to be taken account of when formatting */
          cnt=colour_com_count(line);
	  rncnt=colour_com_count(rname);
          if (user->high_ascii) sprintf(text,"~CB\263~RS %-*.*s ~RS~CB\263~FM%-1.1s~FB\263~FR%-6.6s~FB\263~FT%-*.*s~FB\263~FY%4.4d~FB\263~FR%4.4d~FB\263~FG%-1.1s~FB\263\n",40+cnt*3,40+cnt*3,line,gender,levelname,10+rncnt*3,10+rncnt*3,rname,mins,idle,status);
          else sprintf(text,"~CB|~RS %-*.*s ~RS~CB|~FM%-1.1s~FB|~FR%-6.6s~FB|~FT%-*.*s~FB|~FY%4.4d~FB|~FR%4.4d~FB|~FG%-1.1s~FB|\n",40+cnt*3,40+cnt*3,line,gender,levelname,10+rncnt*3,10+rncnt*3,rname,mins,idle,status);
          write_user(user,text);
          continue;
          }
       }
     if (user->high_ascii) write_user(user,"~CB¿ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ¡ƒ¡ƒƒƒƒƒƒ¡ƒƒƒƒƒƒƒƒƒƒ¡ƒƒƒƒ¡ƒƒƒƒ¡ƒŸ\n");
     else write_user(user,"~CB+------------------------------------------+-+------+----------+----+----+-+\n");
     if (user->level>WIZ) {
         sprintf(text,"  ~FRThere are %d invisible users and %d logins.\n",invis,logins);
         write_user(user,text);
         }
    sprintf(text,"  ~FGThere are %d people roaming around.\n",total);
    write_user(user,text);
    if (user->high_ascii) write_user(user,"~CBƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
    else write_user(user,"~CB----------------------------------------------------------------------------\n");
}


/*** Show People Connected ***/
void showpeople(UR_OBJECT user,int style)
{
UR_OBJECT u;

int total,invis,mins,idle,logins, hidden;
char portstr[16],idlestr[6],sockstr[3],levelname[20];
char gender[7];

total=0;  invis=0;  logins=0;  hidden=0;
sprintf(text,"\n     People Connected To %s %s \n\n",SHORTNAME,long_date(1));
write_user(user,text);
if (user->high_ascii) write_user(user,"~FRƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
else write_user(user,"~FR------------------------------------------------------------------------------\n");
write_user(user," ~CTName            ~FW:  ~FMLevel ~FBLine ~FYVisi ~FRIdle ~FGMins ~FTPort ~FBConnected From\n");
if (user->high_ascii) write_user(user,"~FTƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
else write_user(user,"~FT------------------------------------------------------------------------------\n");
for(u=user_first;u!=NULL;u=u->next) {
     if (u->type==CLONE_TYPE) continue;
     if (!u->login) { if (u->room->hidden && user->level<OWNER) continue; }
     mins=(int)(time(0)-u->last_login)/60;
     idle=(int)(time(0)-u->last_input)/60;
     if (!u->level && u->muzzled & SCUM) strcpy(levelname,"Scum");
     else {
          strcpy(levelname,level_name[u->level]);
          if (u->prompt & FEMALE) strcpy(levelname,level_name_fem[u->level]);
          }
     if (u->muzzled & JAILED) strcpy(levelname,"Jailed");
     strcpy(gender,"Neuter");
     if (u->prompt & FEMALE) strcpy(gender,"Female");
     if (u->prompt & MALE)   strcpy(gender,"Male  ");
     sprintf(portstr,"%d",u->port);
     if (u->login) {
          sprintf(text," [Login stage %d] :   -    %2.2d    -  %4.4d    -  %s  %s\n",4 - u->login,u->socket,idle,portstr,u->site);
          write_user(user,text);
          logins++;
          continue;
          }
     ++total;
     if (!u->vis) {
          if (u->hidden && user->level<OWNER) { total--; continue; }
          total--;
          if (u->level>user->level) continue;
          invis++;
		}
     if (u->afk) strcpy(idlestr," AFK");
     else sprintf(idlestr,"%4.4d",idle);
     sprintf(sockstr,"%2.2d",u->socket);
     sprintf(text," %-15.15s :  %-5.5s %-4.4s %-4.4s %-4.4s %4.4d %-4.4s %s\n",colour_com_strip(u->recap),levelname,sockstr,noyes1[u->vis],idlestr,mins,portstr,u->site);
     write_user(user,text);
     continue;
     }
   if (user->high_ascii) write_user(user,"~FTƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
   else write_user(user,"~FT------------------------------------------------------------------------------\n");
   sprintf(text," ~CTVisible: ~FY%-4d ~FTInvisible: ~FY%-4d ~FTLogins: ~FY%d\n",total,invis,logins);
   write_user(user,text);
   if (user->high_ascii) write_user(user,"~FRƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
   else write_user(user,"~FR------------------------------------------------------------------------------\n");
}

void shortwho(UR_OBJECT user)
{
int ret,cnt,invis,logins,hidden;
char line[USER_NAME_LEN+USER_DESC_LEN*2];
UR_OBJECT u;

cnt=0; ret=0; invis=0; logins=0; hidden=0;
write_user(user,"\n");
sprintf(text,"~FM-~CM=~CR[ ~RSPeople in the %s %s ~CR]~CM=~FM-",TALKERNAME,long_date(1));
write_user(user,center(text,80));
if (user->high_ascii) write_user(user,"\n~CMƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
else write_user(user,"\n~CM------------------------------------------------------------------------------\n");
for(u=user_first;u!=NULL;u=u->next) {
     if (u->type==CLONE_TYPE) continue;
     if (u->login) { logins++; continue; }
     if (u->room->hidden && user->level<OWNER) continue;
     if (u->hidden) hidden++;
     if (u->hidden && user->level<OWNER) continue;
     if (!u->vis) invis++;
     if (!u->vis && (u->level>user->level)) continue;
     cnt++;
     if (u->hidden) cnt--;
     if (u->room->hidden) sprintf(line,"   ~CY&~CR%-15.15s",colour_com_strip(u->recap));
     else sprintf(line,"    ~CT%-15.15s",colour_com_strip(u->recap));
     if (!u->vis) line[2]='!';
     if (u->hidden) line[1]='#';
     write_user(user,line);
     ret++;
     if (ret==4) { ret=0; write_user(user,"\n"); }
     }
if (ret>0 && ret<4) write_user(user,"\n");
if (user->high_ascii) write_user(user,"~CMƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
else write_user(user,"~CM------------------------------------------------------------------------------\n");
if (user->level>=WIZ) {
     sprintf(text,"   ~CBThere are ~FM%d ~FBinvisible and ~FM%d ~FBlogins.\n",invis,logins);
     write_user(user,text);
     }
if (user->level==OWNER) {
     sprintf(text,"   ~CRThere are ~FM%d ~FRhidden users.\n",hidden);
     write_user(user,text);
     }
sprintf(text,"   ~CGThere are ~FM%d~FG users online!\n",cnt);
write_user(user,text);
if (user->high_ascii) write_user(user,"~CMƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
else write_user(user,"~CM------------------------------------------------------------------------------\n");
}

void byroom(UR_OBJECT user)
{
UR_OBJECT u;
UR_OBJECT cu;
RM_OBJECT rm;

int cnt,total,invis,mins,idle,logins,hidden;
char line[USER_NAME_LEN+USER_DESC_LEN*2];
char rname[ROOM_NAME_LEN+1],levelname[20];
char gender[7],text2[ARR_SIZE+1];

total=0;  invis=0;  logins=0;  hidden=0;

/*** Print Who List Header ***/

     write_user(user,"\n\n");
     sprintf(text2,"People At %s %s",TALKERNAME,long_date(1));
     if (user->high_ascii) {
        write_user(user,"~FB-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-\n");
           sprintf(text,"~FB-==-~CT%-67.67s~RS~FB-==-\n",center(text2,66));
        write_user(user,text);
        write_user(user,"~FB-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-\n");
        }
     else {
        write_user(user,"~FB_.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._\n");
           sprintf(text,"~FB_.-[ ~CT%-67.67s ~RS~FB]-._\n",center(text2,66));
        write_user(user,text);
        write_user(user,"~FB_.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._\n");
        }
for(rm=room_first;rm!=NULL;rm=rm->next) {
     if (rm->hidden && user->level<OWNER) continue;
     if (inroom(rm)) {  /* Check To See If Room Is Empty */
        sprintf(text,"~FB-~CB=~CT+  ~CG%-67.67s  ~CT+~CB=~FB-\n",center(rm->recap,66));
	write_user(user,text);
	cu=NULL;
        for(u=user_first;u!=NULL;u=u->next) {
	  if (u->login) { logins++; continue; }
//          if (u->type==CLONE_TYPE) cu=get_user(u->name);
	  if (u->type == CLONE_TYPE) cu = u->owner;
//  	  if (u->type==CLONE_TYPE && cu->room!=rm) continue;
	  if (u->room!=rm) continue;
          mins=(int)(time(0) - u->last_login)/60;
          idle=(int)(time(0) - u->last_input)/60;
          if (u->type==CLONE_TYPE) {
               mins=(int)(time(0) - cu->last_login)/60;
               idle=(int)(time(0) - cu->last_input)/60;
               }
          if (!u->level && u->muzzled & SCUM) strcpy(levelname,"Scum");
          else strcpy(levelname,u->level_alias);
          if (u->type==CLONE_TYPE) {
               if (!cu->level && cu->muzzled & SCUM) strcpy(levelname,"Scum");
               else strcpy(levelname,cu->level_alias);
               }
          if (u->muzzled & JAILED) strcpy(levelname,"Jailed");
          strcpy(gender,"~CGN");
          if (u->prompt & FEMALE) strcpy(gender,"~CMF");
          if (u->prompt & MALE)   strcpy(gender,"~CTM");
          if (u->type==CLONE_TYPE) {
               strcpy(gender,"Neuter");
               if (cu->prompt & FEMALE) strcpy(gender,"~CMF");
               if (cu->prompt & MALE)   strcpy(gender,"~CTM");
               }
          ++total;
          if (u->login) logins++;
          if (u->hidden && user->level<OWNER) { total--; hidden++; continue; }
          if (!u->vis) {
               --total; ++invis;
               if (u->level>user->level) continue;
               }
          strcpy(rname,u->room->recap);
          sprintf(line," %s %s",u->recap,u->desc);
          if (u->afk) sprintf(line," %s ~RS~FRis away from the keyboard.",u->recap);
          else if (idle>10) sprintf(line," %s ~RS~FMappears to be sleeping.",u->recap);
          else if (u->malloc_start!=NULL) sprintf(line," %s ~RS~FTis using the text editor",u->recap);
          if (u->type==CLONE_TYPE) {
                if (cu->afk) sprintf(line," %s ~RS~FRis away from the keyboard.",u->recap);
                else if (idle>10) sprintf(line," %s ~RS~FRappears to be sleeping.",u->recap);
                else if (cu->malloc_start!=NULL) sprintf(line," %s ~RS~FTis using the editor.",u->recap);
                }
	  /* Process User Status */
          if (!u->vis) line[0]='!';
          if (u->hidden) line[0]='#';
          cnt=colour_com_count(line);
          if (user->high_ascii) sprintf(text,"~FB-~CB=~CT+ ~CY%s ~FR: ~CM%-12.12s ~FR:~CG%5d~FW:~FR%-5d~RS~FR:~RS%-*.*s ~RS~CT+~CB=~FB-\n",gender,levelname,mins,idle,39+cnt*3,39+cnt*3,line);
          else sprintf(text,"~FB-~CB=~CT+ ~FY%s ~RS~FR: ~CM%-12.12s ~RS~FR:~CG%5d~FW:~FR%-5d~RS~FR:~RS%-*.*s ~RS~CT+~CB=~FB-\n",gender,levelname,mins,idle,39+cnt*3,39+cnt*3,line);
	  write_user(user,text);
          continue;
          }
	}
    }

/*** Display Footer ***/

     if (user->high_ascii) write_user(user,"~FB-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-\n");
     else write_user(user,"~FB_.-._.-._.-._.-._.-._.+._.-._.-._;-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._\n");
     if (user->level>WIZ) {
         sprintf(text,"  ~FRThere are %d invisible and %d people at the login stage.\n",invis,logins);
         write_user(user,text);
         }
    sprintf(text,"  ~CBThere %s currently %d %s in the %s.\n",(total==1?"is":"are"),total,(total==1?"person":"people"),SHORTNAME);
    write_user(user,text);
    if (user->high_ascii) write_user(user,"~FB-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-\n\n");
    else write_user(user,"~FB_.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._\n\n");
}

/* Returns the number of users in a room, Including Clones */
int inroom(RM_OBJECT room)
{
UR_OBJECT u;
int users=0;

for(u=user_first;u!=NULL;u=u->next) {
        if (u->login) continue;
	if (u->room==room) users++;
	}
return users;
}

void camwho(UR_OBJECT user)
{
  hopewho(user);
}

void ncohwho(UR_OBJECT user)
{
  hopewho(user);
}

void dcovewho(UR_OBJECT user)
{
  hopewho(user);
}

/*** Reloads room description for room the user is in ***/
void setrank(UR_OBJECT user,char *inpstr)
{
UR_OBJECT u;
char *name;

if (user->vis) name=user->recap; else name=invisname;

if (word_count<2) {
        write_user(user,"Usage:  setrank <user> <alias>\n");
        return;
        }
if (!(u=get_user(word[1]))) {
        write_user(user,notloggedon);
        return;
	}
if (u->hidden && user->level<OWNER) {
        write_user(user,notloggedon);
        return;
        }
if ((user->level<=u->level) && (u!=user)) {
        write_user(user,"You cannot cannot change the rank of anyone else the same or higher level\n");
        return;
        }
inpstr=remove_first(inpstr);
inpstr=colour_com_strip(inpstr);
if (strlen(inpstr)>USER_ALIAS_LEN) {
	write_user(user,"Alias Length Too Long!\n");
	return;
	}
strncpy(u->level_alias,inpstr,USER_ALIAS_LEN);
sprintf(text,"~CW-> ~FM%s has set your rank to ~FT\"%s\" \n",name,u->level_alias);
write_user(u,text);
sprintf(text,"~CMYou set %s's rank to %s\n",u->name,u->level_alias);
write_user(user,text);
}

void execall(UR_OBJECT user,char *inpstr)
{
UR_OBJECT u;
char temp[ARR_SIZE], cmd[ARR_SIZE], text2[ARR_SIZE+2];
int cnt;
cnt=0;

memset(temp,0,sizeof(temp)-1);
memset(text2,0,sizeof(text2)-1);
memset(cmd,0,sizeof(cmd)-1);
if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (user->muzzled & 1) {
	write_user(user,"You are muzzled and are not permitted to use the \"all\" command.\n");
	return;
	}
if (word_count<2) {
	write_user(user,"Execute A Command On Everybody!\nUsage: .execall <command> <args>\n");
	return;
	}
strcpy(cmd,word[1]);
inpstr=remove_first(inpstr);
strncpy(temp,inpstr,sizeof(temp)-1);
for(u=user_first;u!=NULL;u=u->next) {
	if (u->type==CLONE_TYPE) continue;
	if (u->afk) continue;
	if (u->hidden && user->level<OWNER) continue;
	if (u==user) continue;
	cnt++;
//	inpstr[0]='\0';
//	sprintf(inpstr,"%s %s %s",cmd,u->name,temp);
	memset(text2,0,sizeof(text2)-1);
	snprintf(text2,(sizeof(text2)-1),"%s %s %s",cmd,u->name,temp);
	clear_words();
	word_count=wordfind(text2);
	exec_com(user,text2);
	}
}

void atmos_assign(RM_OBJECT rm)
{
int i=0;
for(i=0;i<ATMOSPHERES;++i) strcpy(rm->atmos[i],atmos[i]);
}

/***************************************************************************
    Moesoft Craps! v1.00 for Moenuts v1.52+  (C)1995-1998 Michael Irving
***************************************************************************/

void craps(UR_OBJECT user,int gamestage)
{
int die1,die2,i,j,bank,total,lasttotal,roll;
i=0; die1=0; die2=0; roll=0;  total=0;

if (!gamestage) {
    bank=user->bank_balance;
    if (bank>0) withdrawlbank(user,bank,1);
    user->cpot=bank;
    if (user->cpot<10) {
          craps_poor(user);
	  depositbank(user,user->cpot,1);
          user->misc_op=0;
	  user->cpot=0;
	  craps_endgame(user);
          return;
          }
    if (user->cpot>1000000) {
          craps_getout(user);
	  depositbank(user,user->cpot,0);
	  user->cpot=0;
          user->cwager=0;
	  craps_endgame(user);
          return;
          }
     i=0; die1=0; die2=0; roll=0;
     sprintf(text,"\n~CYMinimum Wager ~FB= ~FM$%-6d  ~FRMaximum Wager ~FB= ~FM$%-6d\n",CRAPS_MINWAGER,bank);
     write_user(user,text);
     sprintf(text,"~FGYou have ~CT$~FM%d ~RS~FGin your pot.\n~FMHow much do you wish to wager? ",bank);
     write_user(user,text);
     user->misc_op=11;
     return;
     }
for(i=1;i<3;++i) {
     roll=i;  die1=0;  die2=0;  j=0;
     if (roll<2) write_user(user,"\n~CMYou pick up the dice, shake them around a couple times then blow\n~CMon them for luck, then throw them down the craps table...\n\n");
     else write_user(user,"\n~CMYou pick the dice up again for your second roll...\n\n");
     while(die1<1) { die1=(rand()%6)+1; }
     while(die2<1) { die2=(rand()%6)+1; }
     for(j=0;j<DICE_HEIGHT;++j) {
	  if (user->high_ascii) sprintf(text,"          %s          %s\n",ansidice[die1-1][j],ansidice[die2-1][j]);
	  else sprintf(text,"          %s          %s\n",asciidice[die1-1][j],asciidice[die2-1][j]);
	  write_user(user,text);
	  }
     lasttotal=total;
     total=die1+die2;
     sprintf(text,"\n~CYTotal For This Roll is ~FM%d\n",total);
     write_user(user,text);
     if (roll<2 && lasttotal==total) {
           user->cpot=user->cpot+user->cwager;
           craps_win(user);
           write_user(user,"\n~CGDo ya' wanna roll again~FY? ");
           user->misc_op=12;
	   return;
           }
      else if (roll<2 && total==7) {
           user->cpot=user->cpot-user->cwager;
           craps_lose(user);
           write_user(user,"\n~CGDo ya' wanna roll again~FY? ");
           user->misc_op=12;
           return;
           }
     else {
        if (roll<2) write_user(user,"~CGThe dealer tosses the dice back to you and says...\"You roll again.\"\n");
	}
     /* Process Second Roll */
     if (roll>1 && (total==7 || total==11)) {
          user->cpot=user->cpot+user->cwager;
          craps_win(user);
          write_user(user,"\n~CGDo ya' wanna roll again~FY? ");
          user->misc_op=12;
	  return;
          }
     else if (roll>1 && (total==2 || total==3 || total==12)) {
          user->cpot=user->cpot-user->cwager;
          craps_lose(user);
          write_user(user,"\n~CGDo ya' wanna roll again~FY? ");
          user->misc_op=12;
          return;
          }
     else {
	  if (roll>1) write_user(user,"~FMYou didn't lose, but you didn't win either!\n");
	  }
     }
     write_user(user,"\n~CGDo ya' wanna roll again~FY? ");
     user->misc_op=12;
}

void craps_win(UR_OBJECT user)
{
write_user(user,"\n~CGYou have Won!\n");
}

void craps_lose(UR_OBJECT user)
{
write_user(user,"\n~CRYou have lost!\n");
}

void craps_getout(UR_OBJECT user)
{
write_user(user,"\n~CRThe owner of the casino comes over with a couple of his bigest\n");
write_user(user,"~CRgoons and escorts you out of the Casino.  \"YOU'VE WON TOO MUCH!\"\n");
write_user(user,"~CR\"We don't want to see your face back here for a while!!\"\n\n");
}

void craps_poor(UR_OBJECT user)
{
write_user(user,"\n~CBYou're Poor!  I'm afraid your massive gambling habbits have left you with\n");
write_user(user,"~CBan empty wallet and no sence... Er I mean cents... ;-)\n\n");
write_user(user,"~CBYour only recurse now is to come back some other time, or bug a wizard!\n\n");
}

void craps_endgame(UR_OBJECT user)
{
user->cpot=0;
user->cwager=0;
user->ignall=user->ignall_store;
sprintf(text,"~CG%s~RS~CG comes back from the casino...\n",user->recap);
write_room(user->room,text);
}

void depositbank(UR_OBJECT user,int ammount,int type)
{
int oldamt,newamt;

oldamt=0; newamt=0;
oldamt=user->bank_balance;
newamt=oldamt+ammount;
user->bank_balance=newamt;
if (!type) {
	sprintf(text,"\n~CT%s Bank Account Transaction Record\n",TALKERNAME);
	write_user(user,text);
	write_user(user,"~CB~BW+---------------------------------------------------------------+~RS\n");
	write_user(user,"~CB~BW|  Transaction            | Transaction Amt  | Account Balance  |~RS\n");
	write_user(user,"~CB~BW|=========================|==================|==================|~RS\n");
	sprintf(text,"~CB~BW|  Cash Deposit           |  $%5d.00       |  $%5d.00       |~RS\n",ammount,newamt);
	write_user(user,text);
	write_user(user,"~CB~BW+---------------------------------------------------------------+~RS\n\n");
	write_user(user,"~CMThank you for using First National...~CGHave A Nice Day!\n\n");
	}
}

void withdrawlbank(UR_OBJECT user,int ammount,int type)
{
int oldamt,newamt;

oldamt=0; newamt=0;
oldamt=user->bank_balance;
newamt=oldamt-ammount;
user->bank_balance=newamt;
if (!type) {
	sprintf(text,"\n~CT%s Bank Account Transaction Record\n",TALKERNAME);
	write_user(user,text);
	write_user(user,"~CB~BW+---------------------------------------------------------------+~RS\n");
	write_user(user,"~CB~BW|  Transaction            | Transaction Amt  | Account Balance  |~RS\n");
	write_user(user,"~CB~BW|=========================|==================|==================|~RS\n");
	sprintf(text,"~CB~BW|  Cash Withdrawl         |  $%5d.00       |  $%5d.00       |~RS\n",ammount,newamt);
	write_user(user,text);
	write_user(user,"~CB~BW+---------------------------------------------------------------+~RS\n\n");
	write_user(user,"~CMThank you for using First National...~CGHave A Nice Day!\n\n");
	}
}

void givecash(UR_OBJECT user)
{
UR_OBJECT u;
int amt,bank;
amt=0; bank=0;

if (word_count<3) {
     write_user(user,"Usage:  .givecash <user> <ammount>\n");
     return;
     }
if (!(u=get_user(word[1]))) {
     write_user(user,notloggedon);
     return;
     }
if (u==user && user->level<OWNER) {
        write_user(user,"~CRYou cannot give yourself money!  NICE TRY!\n");
	return;
	}
/*
if ((user->level>=u->level) &&  (user->level<OWNER)) {
        write_user(user,"You cannot give money to anyone the same or higher level than you!\n");
        sprintf(text,"%s ~RStried to give you some money!  What a nice person eh?\n",user->recap);
	write_user(u,text);
	sprintf(text,"%s tried to give %s some money!\n",user->name,u->name);
	write_syslog(text,1);
     return;
     }
*/
amt=atoi(word[2]);
bank=u->bank_balance;
/* if (bank>100 && user->level<OWNER) { write_user(user,"They've got more than $100 alreday!\n"); return; } */
if (amt<10) { write_user(user,"~CRGawd You're Cheap!  You have to give more than $10!\n"); return; }
if (amt>1000 && user->level<OWNER) { write_user(user,"~CRUgh! More than $1,000.00?  Wanna break us?\n"); return; }
sprintf(text,"~CG%s ~CGhas just deposited $%d.00 into your bank account.\n",user->recap,amt);
write_user(u,text);
sprintf(text,"~CGYou deposit ~CY$%d.00 ~CGinto %s~CG's account.\n",amt,u->recap);
write_user(user,text);
depositbank(u,amt,0);
}

void lendcash(UR_OBJECT user)
{
UR_OBJECT u;
int amt,bank,deposit;
amt=0; bank=0; deposit=0;

if (word_count<3) {
        write_user(user,"Usage:  .lend <user> <ammount>\n");
        return;
        }
if (!(u=get_user(word[1]))) {
        write_user(user,notloggedon);
        return;
        }
if (u==user) {
        write_user(user,"~CRLoaning Yourself Money?\n~CMHow does that work?\n");
	return;
	}
if ((user->level>=u->level) &&  user->level<OWNER) {
        write_user(user,"You cannot give money to anyone the same or higher level than you!\n");
        sprintf(text,"%s tried to give you some money!  What a nice person eh?\n",user->recap);
	write_user(u,text);
	sprintf(text,"%s tried to give %s some money!\n",user->name,u->name);
	write_syslog(text,1);
        return;
        }
amt=atoi(word[2]);
bank=user->bank_balance;
if (amt<10) {
        write_user(user,"~CRYou cannot lend any less than $10.00!\n");
	return;
	}
if (amt>100) {
        write_user(user,"~CRMore than $100?  Make'em work for their money!\n");
	return;
	}
if (amt>bank) {
	write_user(user,"You don't have that kind of money in your bank account!\n");
	return;
	}
sprintf(text,"~CG%s ~RS~CGhas just deposited $%d.00 into your bank account.\n",user->recap,amt);
write_user(u,text);
sprintf(text,"~CGYou deposit $%d.00 into %s~RS~CG's account.\n",amt,u->recap);
write_user(user,text);
withdrawlbank(user,amt,0);
depositbank(u,amt,0);
}

void check_atmospheres(void)
{
UR_OBJECT u;
int at=0;

++atmos_trigger;
if (atmos_trigger>ATMOS_FREQ) {
	for(u=user_first;u!=NULL;u=u->next) {
		if (u->login>0) continue;  /* User is at login stage */
                if (u->type==CLONE_TYPE) continue;  /* Don't show to clones */
                if (u->ignall) continue;   /* User is ignoring everything */
		if (u->ignore & ATMOS_MSGS) continue; /* User ignoring atmos */
		at=intrand(ATMOSPHERES);
		if (u->room->atmos[at][0]) {
			sprintf(text,ATMOS_STRING,u->room->atmos[at]);
			write_user(u,text);
			}
		}
	atmos_trigger=0;
	}
}

/* Generate a random number */
int intrand(int max)
{
int n;
int mask;
for(mask=1;mask<max;mask*=2);
mask-=1;
do{n=random()&mask;}
while(n>=max);
return(n);
}

void edit_atmos(UR_OBJECT user,char *inpstr)
{
RM_OBJECT rm;
int i,edat;
edat=0; i=0;

if (word_count<2) {
	write_user(user,"Usage: atmos [list/number] [<text>]\n");
        write_user(user,"e.g. atmos list      ~CM<=- ~FTList All Atmospheres For This Room.\n");
        write_user(user,"e.g. atmos 1 <text>  ~CM<=- ~FTSet atmosphere #1 to <text> for this room.\n\n");
	return;
	}
rm=user->room;
if (strstr(word[1],"li")) {
        write_user(user,"~CTAtmospheres Set For This Room\n");
        write_user(user,"~CB~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
	for(i=0;i<ATMOSPHERES;++i) {
                sprintf(text,"~CM%2d~FW: ~FG%s\n",(i+1),rm->atmos[i]);
		write_user(user,text);
		}
        write_user(user,"\n~CB-=- ~FGEnd ~FYOf ~FWAtmospheres ~FB-=-\n");
	return;
	}
if (strcasecmp(rm->owner,user->name) && user->level<ARCH) {
	write_user(user,"This is not your room, you cannot edit atmospheres here!\n");
	return;
	}
edat=atoi(word[1]);
if (edat<1 || edat>ATMOSPHERES) {
        sprintf(text,"~CRThat atmosphere doesn't exist.\n~CMYou can only set atmospheres ~CT1~FM to ~FT%d.\n",ATMOSPHERES);
	write_user(user,text);
	return;
	}
inpstr=remove_first(inpstr);
if (!inpstr[0]) return;
if (!strcasecmp(inpstr,"none")) {
     rm->atmos[edat-1][0]='\0';
     write_user(user,"~CRSpecified Atmosphere Was Deleted From This Room!\n");
     if (!save_atmospheres(rm)) write_user(user,"~CWERROR: ~RSFailed to save atmospheres!\n");
     return;
     }
sprintf(text,"~CTAtmosphere~FW: ~FM%-2.2d\n~FR~~~~~~~~~~~~~~\n%s\n\n",edat,inpstr);
write_user(user,text);
strncpy(rm->atmos[edat-1],inpstr,ATMOS_LEN);
if (!save_atmospheres(rm)) write_user(user,"~CWERROR: ~RSFailed to save atmospheres!\n");
}

int save_atmospheres(RM_OBJECT rm)
{
FILE *fp;
char filename[80],temp[ARR_SIZE+1];
int i=0;

if (rm==NULL) return 0;
sprintf(filename,"%s/%s.RA",DATAFILES,rm->name);
if (!(fp=fopen(filename,"w"))) {
	sprintf(text,"save_atmospheres(): Failed to save atmospheres for room: %s\n",rm->name);
	write_syslog(text,1);
	return 0;
	}
for(i=0;i<ATMOSPHERES;++i) {
	strncpy(temp,rm->atmos[i],ARR_SIZE);
	fprintf(fp,"%s\n",temp);
	}
fclose(fp);
return 1;
}

int load_atmospheres(RM_OBJECT rm)
{
FILE *fp;
char filename[80],temp[ARR_SIZE];
int i=0;

if (rm==NULL) return 0;
sprintf(filename,"%s/%s.RA",DATAFILES,rm->name);
if (!(fp=fopen(filename,"r"))) return 0;
for(i=0;i<ATMOSPHERES;++i) {
	fgets(temp,ARR_SIZE,fp);
	temp[strlen(temp)-1]=0;
	strncpy(rm->atmos[i],temp,ATMOS_LEN);
	}
fclose(fp);
return 1;
}

/* Email a text file to a user */
int send_email(char *mail_file,char *send_to)
{

switch(double_fork()) {
  case -1 : return 0; /* double_fork() failed */
  case  0 : sprintf(text,"Sending Email To: %s, [%s]\n",send_to,mail_file);
	    write_syslog(text,1);
	    sprintf(text,SENDMAILCMD,send_to,mail_file);
            system(text);
            _exit(1);
	    return 1;
  }
return 0;
}

/* signal trapping not working, so fork twice */
/* Borrowed from Amnuts 1.42 By Andy Collington */

int double_fork(void)
{
pid_t pid;
int status;

if (!(pid=fork())) {
  switch(fork()) {
      case  0:return 0;
      case -1:_exit(-1);
      default:_exit(0);
      }
  }
if (pid<0||waitpid(pid,&status,0)<0) return -1;
if (WIFEXITED(status))
  if(WEXITSTATUS(status)==0) return 1;
  else errno=WEXITSTATUS(status);
else errno=EINTR;
return -1;
}

/* backup all talker files */
int backup_talker(void)
{
switch(double_fork()) {
  case -1 :
        sprintf(text,"~CB[~CYM~CYoenuts~CB]~CW: backup_talker(): Failed to fork backup process...\n");
        write_level(WIZ,3,text,NULL);
	sprintf(text,"backup_talker(): Failed to fork process...\n");
	write_syslog(text,1);
	return 0; /* double_fork() failed */
  case  0 : /* Start Backup Of Files */
	sprintf(text,"Backing Up Talker Files To : %s/%s.zip\n",BACKUPDIR,BACKUPFILE);
	write_syslog(text,1);
	sprintf(text,"For Zip Progress, Read File: %s/%s.log\n",LOGFILES,BACKUPFILE);
	write_syslog(text,1);
	sprintf(text,"%s/%s.zip",BACKUPDIR,BACKUPFILE);
	unlink(text);
	sprintf(text,"%s/%s.log",LOGFILES,BACKUPFILE);
	unlink(text);
	sprintf(text,"zip -v -9 -r %s/%s.zip * > %s/%s.log",BACKUPDIR,BACKUPFILE,LOGFILES,BACKUPFILE);
        system(text);
        _exit(1);
	return 1;
	}
return 0;
}

void force_backup(UR_OBJECT user)
{
if (!backup_talker()) {
	write_user(user,"~FR@> ~FTUnable To Start Backup Process,\n~FR@> ~FTBackup_talker() returned an error...\n");
	return;
	}
else {
        write_room(NULL,"\n~CGSYSTEM: ~FMA Backup Process To Backup all talker files is being\n");
        write_room(NULL,"        ~CMstarted.  This ~FR*may*~FM lag the talker a little...\n\n");
	write_user(user,"~FGMSB~FW: ~FTStarting Backup Of Talker Files...\n");
	sprintf(text,"~FGMSB~FW: ~FTBacking Up Talker Files To: ~FM%s/%s.zip\n",BACKUPDIR,BACKUPFILE);
	write_user(user,text);
	sprintf(text,"~FGMSB~FW: ~FTFor progress info, Read File: ~FM%s/%s.log\n\n",LOGFILES,BACKUPFILE);
	write_user(user,text);
	}
}

/** This Is Expirimental, And Hasn't Been Fully Implimented Yet **/

void ticmenu(UR_OBJECT user,char *inpstr,int option)
{
switch(option) {
        case 0: write_user(user,"~CGWelcome To Moenuts Tic Tac Toe.\n");
                write_user(user,"~CGFor a list of commands, enter '~FThelp~FG'.\n");
		user->misc_op=13;
		break;
	case 10: user->misc_op=13;
	default: user->misc_op=13;
	}
}

/** Check To See If A Specific File Exists **/
int file_exists(char *fname)
{
FILE *fp;

if (!(fp=fopen(fname,"r"))) return 0;
fclose(fp);
return 1;
}

/* Return the size of a file */
int filesize(char *filename)
{
int chars=0;
FILE *fp;
if ((fp=fopen(filename,"r"))==NULL) return -1;
while(getc(fp)!=EOF) chars++;
fclose(fp);
return chars;
}

/* Expirimental */
void wait_enter(UR_OBJECT user,int ret)
{
if (!ret) {
	user->misc_op=14;
	return;
	}
user->misc_op=0;
}

void finger_host(UR_OBJECT user)
{
char filename[81], *c;

if (word_count<2) {
	write_user(user,"Usage:  finger user@hostname\n");
	return;
	}

/* Check for any illegal crap in searched for filename so they cannot list
   out the /etc/passwd file for instance. */

c=word[1];
while(*c) {
	if (*c==';' || *c=='<' || *c=='>' || *c=='/') {
                sprintf(text,"Invalid Finger Host \"%s\"...\nHack Attempt Logged...\n",word[1]);
                write_user(user,text);
                sprintf(text,"%s tried to to use %s to hack the system using .finger\n",user->name,word[1]);
		write_syslog(text,1);
		return;
		}
	c++;
	}
sprintf(text,"~CGGathering Finger Information For~FT: ~FB\"~FT%s~FB\"\n",word[1]);
write_user(user,text);
switch(double_fork()) {
  case -1 : write_user(user,"Moenuts: finger: fork failure\n");
	    write_syslog("Moenuts: finger: fork failure\n",1);
	    return;
  case  0 : sprintf(text,"%s fingered \"%s\"\n",user->name,word[1]);
	    write_syslog(text,1);
	    sprintf(filename,"%s/output.%s",TEMPFILES,user->name);
	    unlink(filename);
	    sprintf(text,"finger %s > %s",word[1],filename);
            system(text);
	    switch(more(user,user->socket,filename)) {
                case 0: write_user(user,"~CRfinger_host(): Could Not Open Temporary Finger File...\n");
		case 1: user->misc_op=2;
		}
            _exit(1);
	    break;
	    }
}

void nslookup(UR_OBJECT user)
{
char filename[81], *c;

if (word_count<2) {
	write_user(user,"\nName Server Lookup.  This utility is to lookup an IP number\n");
	write_user(user,"address given a host name, or lookup a host name given an IP!\n\n");
	if (user->level<3) write_user(user,"Usage:  nslookup <hostname>\n");
	else write_user(user,"Usage:  nslookup <hostname> [<nameserver>]\n");
	return;
	}
/* Check for any illegal crap in searched for filename so they cannot list
   out the /etc/passwd file for instance. */

c=word[1];
while(*c) {
	if (*c==';' || *c=='<' || *c=='>' || *c=='/') {
                sprintf(text,"Invalid Host Host \"%s\"...\nHack Attempt Logged...\n",word[1]);
                write_user(user,text);
                sprintf(text,"%s tried to to use %s to hack the system using nslookup (host)\n",user->name,word[1]);
		write_syslog(text,1);
		return;
		}
 	c++;
	}
/* Check for any illegal crap in searched for filename so they cannot list
   out the /etc/passwd file for instance. */

c=word[2];
while(*c) {
	if (*c==';' || *c=='<' || *c=='>' || *c=='/') {
                write_user(user,"Invalid Name Server Specified...Hack Attempt Logged...\n");
                sprintf(text,"%s tried to to use %s to hack the system using .nslookup (ns)\n",user->name,word[1]);
		write_syslog(text,1);
		return;
		}
	c++;
	}
sprintf(text,"~CGAttempting Name Server Lookup For~FT: ~FB\"~FT%s~FB\"\n",word[1]);
write_user(user,text);
switch(double_fork()) {
  case -1 : write_user(user,"Moenuts: nslookup: fork failure\n");
	    write_syslog("Moenuts: nslookup: fork failure\n",1);
	    return;
  case  0 : sprintf(text,"%s performed an nslookup on \"%s\"\n",user->name,word[1]);
	    write_syslog(text,1);
	    sprintf(filename,"%s/output.%s",TEMPFILES,user->name);
	    unlink(filename);
	    if (word_count>2 && user->level>2) {
                sprintf(text,"~CMUsing Name Server~FG: ~FT%s\n",word[2]);
		write_user(user,text);
		sprintf(text,"nslookup %s %s > %s",word[1],word[2],filename);
		}
	    else sprintf(text,"nslookup %s > %s",word[1],filename);
            system(text);
	    switch(more(user,user->socket,filename)) {
                case 0: write_user(user,"~CRnslookup(): Could Not Open Temporary File...\n");
		case 1: user->misc_op=2;
		}
            _exit(1);
	    break;
	    }
}

void calender(UR_OBJECT user)
{
char filename[81];

if (word_count<2) {
	write_user(user,"Usage: calender <year>\n");
	write_user(user,"Where: <year> is the year from 1 to 999 to create a calender for.\n");
	return;
	}
if (!isnumber(word[1])) {
        write_user(user,"~CRCalender: Year must be between ~FM1 ~FRand ~FM9999~FR!\n");
	return;
	}
if (atoi(word[1])<1 || atoi(word[1])>9999) {
        write_user(user,"~CRCalender: Year must be between ~FM1 ~FRand ~FM9999~FR!\n");
	return;
	}
sprintf(text,"~CGCreating Calender For The Year ~FT%s\n",word[1]);
write_user(user,text);
switch(double_fork()) {
  case -1 : write_user(user,"Moenuts: calender: fork failure\n");
	    write_syslog("Moenuts: calender: fork failure\n",1);
	    return;
  case  0 : sprintf(text,"%s requested calender for year \"%s\"\n",user->name,word[1]);
	    write_syslog(text,1);
	    sprintf(filename,"%s/output.%s",TEMPFILES,user->name);
	    unlink(filename);
	    sprintf(text,"cal %s > %s",word[1],filename);
            system(text);
	    viewfile(user,filename);
            _exit(1);
	    break;
	    }
}

void tpromote(UR_OBJECT user)
{
int oldlevel=0;
UR_OBJECT u;

if (word_count<2) {
	write_user(user,"Usage: tpromote <user>\n");
	return;
	}
if (!(u=get_user(word[1]))) {
	write_user(user,notloggedon);
	return;
	}
if (u==user) {
        write_user(user,"~CRTemporarily promoting yourself?  A little crazy don'cha think?\n");
	return;
	}
if (user->level<=u->level) {
        write_user(user,"~CRYou cannot temporarily promote anyone the same or higher level!\n");
	return;
	}
sprintf(text,"%s~RS~CG's Original Level is %s\n",u->recap,level_name[oldlevel]);
if ((u->level+1)!=OWNER) {
	if (!u->temp_level) u->temp_level=u->level;
        oldlevel=u->temp_level;
	u->level++;
        sprintf(text,"~CGYou have been temporarily promoted to level ~FT%s~FG!\n",level_name[u->level]);
	write_user(u,text);
        sprintf(text,"~CG%s~RS~CG starts to glow as their power increases...\n",u->recap);
	write_room(NULL,text);
	u->temp_level=oldlevel;
	}
else {
        write_user(user,"~CRA user cannot be temp promoted to the highest level!\n");
	return;
	}
}

/*********************************************************************************/
/*** Moesoft's Soft Socials v1.2 (C)1998-2004 Michael Irving (moe@moenuts.com) ***/
/*** Special Thanks To addIct and squirt For Helping Debug the second revision ***/
/*********************************************************************************/

int do_socials(UR_OBJECT user,char *inpstr)


{
UR_OBJECT u;
FILE *fp;
char *remove_first();
char filename[81], text2[ARR_SIZE+2], text3[ARR_SIZE+2];
char social[6][ARR_SIZE+2];
char line[ARR_SIZE+2],*comword=NULL;
char *name,*vname;
int i=0,s=0,nos=0,retcode=0,ug=0,vg=0;
char *gen1[]={"it","him","her"};
char *gen2[]={"its","his","her"};
char *gen3[]={"it","he","she"};

text2[0]=' ';
memset(filename,0,sizeof(filename)-1);
memset(text2,0,sizeof(text2)-1);
memset(text3,0,sizeof(text3)-1);
for (i = 0; i < 6; ++i)
{
	memset(social[i],0,sizeof(social[i])-1);
}
memset(line,0,sizeof(line)-1);

if (user->level<USER || (user->muzzled & JAILED) || (user->muzzled)) { return 0; }
if (word[0][0]=='.') comword=(word[0]+1);
else comword=word[0];
sprintf(filename,"%s/%s",DATAFILES,SOCIALFILE);
if (!(fp=fopen(filename,"r"))) {
  sprintf("do_socials(): Cannot Open Socials file: '%s'\n",filename);
  write_syslog(text,1);
  return 0;
  }
while(!feof(fp)) {
  s++;
  if (feof(fp)) { fclose(fp); return 0; }
  for(i=0;i<6;++i) {
    if (!feof(fp)) {
    fgets(line,254,fp);
      if (feof(fp) && i<5) {
	/*
	sprintf(text,"do_socials(): Socials File Is Incorrect Format Or Damaged On Line %d/%d/%d.\n",s,nos,i);
	write_syslog(text,1);
	write_user(user,"~CB[ ~CYM~CYoenuts ~CB]~CY: ~CRPremature End of Social Error, Contact Admin.\n");

	Addict says: Don't forget to fclose() man! I'm not fixing that ;)
	*/
	fclose(fp);
	return 0;
	}
    line[strlen(line)-1] = '\0';
    strcpy(social[i],line);
    if (i==5 && (!line[0]=='*')) {
	sprintf(text,"do_socials(): Socials File Is Incorrect Format Or Damaged On Line %d/%d/%d.\n",s,nos,i);
	write_syslog(text,1);
	write_user(user,"~CB[ ~CYM~CYoenuts ~CB]~CY: ~CRBroken Socials, Contact Admin.\n");
	fclose(fp);
	return 0;
	}
    if (!strcmp(line,"*")) nos++;
    }
  }
  /* Lets Figure Out The User's Gender */
  ug=0;
  if (user->prompt & 4) ug=2; /* Her */
  if (user->prompt & 8) ug=1; /* Him */
  if (!strncasecmp(comword,social[0],strlen(comword))) {
    if (word_count<2) {
        /* Let Substitute Line 1 for genders */
	sprintf(text3,social[2],user->recap);
        strrep(text3,"$UN",user->recap,ARR_SIZE);
        strrep(text3,"$UG1",gen1[ug],ARR_SIZE);
        strrep(text3,"$UG2",gen2[ug],ARR_SIZE);
        strrep(text3,"$UG3",gen3[ug],ARR_SIZE);
	strcat(text3,"\n");
	write_room(user->room,text3);
	record(user->room,text3);
	retcode=1;
	fclose(fp);
	return retcode;
	}
    if (word_count>1) {
	if (!(u=get_user(word[1]))) {
	  sprintf(text3,"%s\n",social[4]);
          /* Let Substitute Line 2 for genders */
          strrep(text3,"$UN",user->recap,ARR_SIZE);
          strrep(text3,"$UG1",gen1[ug],ARR_SIZE);
          strrep(text3,"$UG2",gen2[ug],ARR_SIZE);
          strrep(text3,"$UG3",gen3[ug],ARR_SIZE);
          strrep(text3,"$TXT",inpstr,ARR_SIZE);
	  write_room(user->room,text3);
	  record(user->room,text3);
	  retcode=1;
	  fclose(fp);
	  return retcode;
	  }
      else {
	if (u==user && !USE_SOCIALS_ON_SELF) {
          sprintf(text,"~CRYou cannot use ~FB'~FT%s~FB'~FR on yourself!\n",social[0]);
	  write_user(user,text);
	  retcode=1;
	  fclose(fp);
	  return retcode;
	  }
	vg=0;
        if (u->prompt & 4) vg=2;
        if (u->prompt & 8) vg=1;
	if (user->vis) name=user->recap; else name=invisname;
	if (u->vis) vname=u->recap; else vname=invisname;
	if (u->hidden) { write_user(user,notloggedon); return 1; }
	sprintf(text3,"%s",social[3]);
        /* Let Substitute Line 2 for genders */
        strrep(text3,"$UN",name,ARR_SIZE);
        strrep(text3,"$UG1",gen1[ug],ARR_SIZE);
        strrep(text3,"$UG2",gen2[ug],ARR_SIZE);
        strrep(text3,"$UG3",gen3[ug],ARR_SIZE);
        strrep(text3,"$VN",vname,ARR_SIZE);
        strrep(text3,"$VG1",gen1[vg],ARR_SIZE);
        strrep(text3,"$VG2",gen2[vg],ARR_SIZE);
        strrep(text3,"$VG3",gen3[vg],ARR_SIZE);
	inpstr=remove_first(inpstr);
	if (!inpstr[0]) { strcpy(text2," "); }
	else { strncpy(text2,inpstr,ARR_SIZE); }
        strrep(text3,"$TXT",text2,ARR_SIZE); /* and this is why? */
	if (strstr(social[1],"personal")) {
           sprintf(text,"~FW-> ~RS%s\n",text3);
	   write_user(u,text);
           record_tell(u,text);
           sprintf(text,"~CW<~FGSent To ~FT%s~RS~CW>\n~CW-> ~RS%s\n",u->recap,text3);
	   write_user(user,text);
	   record_tell(user,text);
	   retcode=1;
	   fclose(fp);
	   return retcode;
	   }
        else if (u->room!=user->room) {
           sprintf(text,"~CT!! ~RS%s\n",text3);
	   write_room(NULL,text);
	   record_shout(text);
	   retcode=1;
	   fclose(fp);
	   return retcode;
	   }
	else {
           sprintf(text,"%s\n",text3);
	   write_room(user->room,text);
	   record(user->room,text);
           retcode=1;
	   fclose(fp);
	   return retcode;
	   }
	}
      }
    }
  if (retcode) break;
  }
fclose(fp);
return retcode;
}

void list_socials(UR_OBJECT user)
{
FILE *fp;
char filename[80];
char social[6][255];
char line[255];
int i,s,nos,cnt;

// if (user->level<MEMBER) return;
i=0; s=0; nos=0; cnt=0;
sprintf(filename,"%s/%s",DATAFILES,SOCIALFILE);
if (!(fp=fopen(filename,"r"))) {
	sprintf(text,"list_socials(): Can't open socials file: '%s'\n",filename);
	write_syslog(text,1);
        write_user(user,"~CRSorry, the socials are missing...\n");
	return;
	}
if (user->high_ascii) write_user(user,"~CBƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
else write_user(user,"~CB----------------------------------------------------------------------------\n");
write_user(user,"~CG    Moenuts Socials Listing ~FB-=- ~FMAll Socials Start With A Period ~FB-=-\n");
if (user->high_ascii) write_user(user,"~CBƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
else write_user(user,"~CB----------------------------------------------------------------------------\n");
while(!feof(fp)) {
	s++;
	if (feof(fp)) { fclose(fp); return; }
	for(i=0;i<6;++i) {
		if (!feof(fp)) {
			fgets(line,254,fp);
			line[strlen(line)-1]=0;
			strcpy(social[i],line);
			if (!strcmp(line,"*")) nos++;
			}
		}
	if (social[0]) {
		cnt++;
		if (cnt==1) write_user(user,"  ");
		if (strstr(social[1],"personal")) sprintf(text,"~FB* ~FT%-15.15s~RS  ",social[0]);
		else if (strstr(social[1],"public")) sprintf(text,"~FB! ~FM%-15.15s~RS  ",social[0]);
		else sprintf(text,"  ~FG%-15.15s  ",social[0]);
		write_user(user,text);
		if (cnt==4) { write_user(user,"\n"); cnt=0; }
		}
	for(i=0;i<6;++i) social[i][0]='\0';
	}
if (cnt>0) write_user(user,"\n");
fclose(fp);
if (user->high_ascii) write_user(user,"~CBƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
else write_user(user,"~CB----------------------------------------------------------------------------\n");
sprintf(text,"~CM There are currently ~FG%d~FM socials available to you.  ! = Public, * = Personal\n",nos);
write_user(user,text);
write_user(user,"~CG Usage  ~FW: ~FT.~FM<~FYsocialname~FM> ~FB[~FM<~FRusername~FM>~FB] [~FM<~FRtext~FM>~FB]\n");
write_user(user,"~CG Example~FW: ~FT.~FYpoke ~FB/ ~FT.~FYpoke ~FM<~FRusername~FM> ~FB/ ~FT.~FYpoke ~FM<~FRtext~FM>\n");
if (user->high_ascii) write_user(user,"~CBƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
else write_user(user,"~CB----------------------------------------------------------------------------\n");
}

void quit_user(UR_OBJECT user)
{
if (user->confirm) {
        sprintf(text,"~CGAre you really leavin' us %s~RS~CR?  ",user->recap);
	write_user(user,text);
	if (confirm(user,0)) {	disconnect_user(user);	return; }
	}
else {
	sprintf(file,"%s",QUITSCREEN);
	show_screen(user);
	disconnect_user(user);
	}
}

/* Expirimental */
int confirm(UR_OBJECT user,int yesno)
{
if (!yesno) {
	sprintf(text,"~FB[~FYY~FB]~FMes ~FRor ~FB[~FYN~FB]~FMo~FW: ");
	write_user(user,text);
	user->misc_op=14;
	return 0;
	}
if (yesno==1) {
	user->misc_op=0;
	return 0;
	}
if (yesno==2) {
	user->misc_op=0;
	return 1;
	}
return 0;
}

/* Strip White Spaces and Chars < 33 from the begining of a string */
void whitespace(char text[])
{
if (!text[0]) return;
while(text[0]<33) {
	if (!text[0]) return;
	strcpy(&text[0],&text[1]);
	}
}

char *center(char *string,int clen)
{
static char text2[ARR_SIZE*2];
char text3[ARR_SIZE*2];
char *colour_com_strip();
int len=0,spc=0;

strcpy(text3,string);
len=strlen(colour_com_strip(text3));
if (len>clen) { strcpy(text2,text3); return text2; }
spc=(clen/2)-(len/2);
sprintf(text2,"%*.*s",spc,spc," ");
strcat(text2,text3);
return text2;
}

/*** Create a user ***/
void make_newuser(UR_OBJECT user)
{
UR_OBJECT u;
int i=0;

if (word_count<3) {
	write_user(user,"Usage: makeuser <username> <password>\n");
	return;
	}
strtolower(word[1]);
word[1][0]=toupper(word[1][0]);
for (i=0;i<strlen(word[1]);++i) {
	if (!isalpha(word[1][i])) {
		write_user(user,"~O~LFRYou can only use letters in the username!\n\n");
		return;
		}
	}
if (strlen(word[1])>USER_NAME_LEN) {
        write_user(user,"~CRThat username is too long!\n");
	return;
	}
if (strlen(word[2])>PASS_LEN) {
        sprintf(text,"~CRThe password given for %s is too long!\n",word[1]);
	write_user(user,text);
	return;
	}
if (strlen(word[2])<3) {
        sprintf(text,"~CRThe password given for %s is too short!\n",word[1]);
	write_user(user,text);
	return;
	}
if ((u=get_user(word[1]))) {
        write_user(user,"~CRYou cannot use the same name as a person online!\n");
	return;
	}
if ((u=create_user())==NULL) {
	sprintf(text,"%s: unable to create temporary user object.\n",syserror);
	write_user(user,text);
	write_syslog("ERROR: Unable to create temporary user object in make_newuser().\n",0);
	return;
	}
strcpy(u->name,word[1]);
if (!load_user_details(u)) {
	strcpy(u->pass,(char *)crypt(word[2],"NU"));
	strcpy(u->recap,u->name);
        strncpy(u->desc,DEFAULTDESC,USER_DESC_LEN);
        strcpy(u->in_phrase,"arrives in the room.");
        strcpy(u->out_phrase,"walks out of the room.");
	strcpy(u->level_alias,"Newbie");
	strcpy(u->roomname,"None");
	strcpy(u->roomtopic,"No Topic Set!");
	strcpy(u->roomgiver,"Nobody");
	strcpy(u->login_room,room_first->name);
	strcpy(u->email,"myname@myserver.com");
	strcpy(u->homepage,"www.mywebpage.com");
	strcpy(u->married,"Nobody");
	strcpy(u->birthday,"No Birthday Set!");
	strcpy(u->icq,"None");
	strcpy(u->last_site,"never.logged.on");
	strcpy(u->fakeage,"Newborn");
	strcpy(u->predesc,"None");
	u->bank_balance=0;
	u->last_login=(int)time(0);
	u->last_login_len=0;
	u->total_login=0;
	u->age=0;
	u->roombanned=0;
	u->level=0;
	u->muzzled=0;
	u->command_mode=0;
	u->prompt=prompt_def;
	u->colour=colour_def;
	u->charmode_echo=charecho_def;
        save_user_details(u,0);
	sprintf(text,"Make_newuser(): New user \"%s\" created.\n",u->name);
        write_user(user,text);
	write_syslog(text,1);
	add_userlist(u->name);
	destruct_user(u);
	destructed=0;
	strcpy(word[1],u->name);
	examine(user);
	return;
	}
else {
        sprintf(text,"%s~RS~CR already exists as a user here!\n",u->recap);
	write_user(user,text);
	destruct_user(u);
	destructed=0;
	return;
	}
}

void finduser(UR_OBJECT user)
{
FILE *fp;
int uf=0, ufx=0, colpos=0;
char filename[81], line[ARR_SIZE+1], find[USER_NAME_LEN+1], ufilename[81];
char temp[USER_NAME_LEN+1];

if (word_count<2) {
	write_user(user,"Usage     : finduser <pattern to match> [or 'all']\n");
	write_user(user,"Example   : finduser moe\n");
	write_user(user,"Would Find: All users with 'moe' in their name.\n\n");
	return;
	}
rebuild_userlist(NULL);
strncpy(find,word[1],USER_NAME_LEN);
sprintf(filename,"%s/%s",USERFILES,USERLIST);
if (!(fp=fopen(filename,"r"))) {
	write_user(user,"ERROR: Could not open userlist file!\n");
	return;
	}
sprintf(text,"~CGSearching Userlist for ~FB\"~FT%s~FB\"~FG...\n\n",find);
write_user(user,text);
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
while(!feof(fp)) {
	strncpy(temp,line,USER_NAME_LEN);
	strtolower(line);
	if (strstr(line,find) || !strncasecmp(find,"all",3)) {
		line[0]=toupper(line[0]);
		sprintf(ufilename,"%s/%s.D",USERFILES,temp);
		if (!file_exists(ufilename)) {
			ufx++;
			}
		else {
                        sprintf(text,"~CT%-*.*s  ~RS",USER_NAME_LEN,USER_NAME_LEN,line);
			if (colpos>=4) { colpos=0; strcat(text,"\n"); }
			write_user(user,text);
			uf++;
			colpos++;
			}
		}
	fgets(line,ARR_SIZE,fp);
	line[strlen(line)-1]=0;
	}
fclose(fp);
/* sprintf(text,"\n~CGUsers Found With Accounts~FW: ~FT%d~FB, ~FGWithout Accounts~FW: ~FT%d\n\n",uf,ufx); */
sprintf(text,"\n\n~CGUsers Found Matching Search Pattern~FW: ~FT%d\n\n",uf);
write_user(user,text);
/* if (ufx>0) write_level(ARCH,3,"~CRfinduser(): The userfile needs to be rebuilt!\n",NULL); */
}

int viewfile(UR_OBJECT user,char *filename)
{
FILE *fp;
char ch, fname[81],ch2[2];
strcpy(fname,filename);

if (!(fp=fopen(fname,"r"))) return 0;
while(!feof(fp)) {
	ch=getc(fp);
	sprintf(ch2,"%c",ch);
	write(user->socket,ch2,strlen(ch2));
	}
fclose(fp);
/* Reset Terminal At End Of File */
write(user->socket,"\13\033[0m",5);
return 1;
}

void news(UR_OBJECT user)
{
char filename[81];

sprintf(filename,"%s/%s.%s",SCREENFILES,NEWSFILE,fileext[user->high_ascii]);
if (user->muzzled & JAILED) {
	write_user(user,"~FT%% ~FMYou cannot use the ~FY\"~FGnews~FY\" ~FMcommand...\n");
	return;
	}
if (user->high_ascii && file_exists(filename)) viewfile(user,filename);
else {
	    switch(more(user,user->socket,filename)) {
            case 0: write_user(user,"You pick up the newspaper, and to your surprise, Its Empty!!\n");  break;
	    case 1: user->misc_op=2;
	    }
     }
}

void newswire(UR_OBJECT user)
{
char filename[81];

sprintf(filename,"%s",NEWSWIRE);
if (user->muzzled & JAILED) {
	write_user(user,"~FT%% ~FMYou cannot use the ~FY\"~FGwire~FY\" ~FMcommand...\n");
	return;
	}
    switch(more(user,user->socket,filename)) {
       case 0: write_user(user,"\n< Sorry, The News Wire Is Currently Unavailale at this time >\n");  break;
       case 1: user->misc_op=2;
       }
}

/*** Records the last users who visited the talker ***/
void record_lastlog(char *str)
{
     strncpy(lastlogbuff[lastlogline],str,LASTLOG_LEN);
     lastlogbuff[lastlogline][LASTLOG_LEN]='\n';
     lastlogbuff[lastlogline][LASTLOG_LEN+1]='\0';
     lastlogline=(lastlogline+1)%LASTLOG_LINES;
}

/*** Clear the last login log buffer ***/
void clear_lastlog(void)
{
int c;

for(c=0;c<LASTLOG_LINES;++c) lastlogbuff[c][0]='\0';
lastlogline=0;
}

void lastlogin(UR_OBJECT user,int type)
{
int i=0,line=0,cnt=0,c=0;

switch(type) {
	case 0: /* Show Login Log */
	for(i=0;i<LASTLOG_LINES;++i) if (lastlogbuff[i][0]) c++;
	for(i=0;i<LASTLOG_LINES;++i) {
	line=(lastlogline+i)%LASTLOG_LINES;
	if (lastlogbuff[line][0]) {
		cnt++;
          if (cnt==1) {
		write_user(user,"\n");
                sprintf(text,"~FB-~CB=~CT+ ~FYLast %d people to visit %s ~CT+~CB=~FB-",c,TALKERNAME);
		write_user(user,center(text,78));
                if (user->high_ascii) write_user(user,"\n~CBƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
                else write_user(user,"\n~CB------------------------------------------------------------------------------\n");
               }
          write_user(user,lastlogbuff[line]);
          }
	}
if (!cnt) {
        write_user(user,"~CMThere Are No Last Logins To View.\n");
	return;
	}
else {
     if (user->high_ascii) write_user(user,"~CBƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
     else write_user(user,"~CB------------------------------------------------------------------------------\n");
     }
	return;

	case 1: /* Add To Login Log */
        sprintf(text,"~CW[ ~FG%s ~FW]~FT: ~RS%s %s\n",long_date(2),user->recap,user->desc);
	record_lastlog(text);
	return;

	case 2:
	clear_lastlog();
        write_user(user,"~CGLast login log has been cleared...\n");
	return;

	default: /* Unknown loginlog Option */
	sprintf(text,"Lastlog(): Unknown Log Option %d!\n",type);
	write_syslog(text,1);
	return;
	}
}

int post_office(UR_OBJECT user,char *address,char *to,char *subject,char *file,int type)
{
FILE *fp1, *fp2;
char file2[81],line[ARR_SIZE+1], adstore[81], *colour_com_strip();
char subj[81];
int l=0;

sprintf(file2,"%s/mailfile.%s",TEMPFILES,user->name);
sprintf(adstore,"%s",address);
write_user(user,"\n~CTMoesoft's Post Office Gateway v0.9b (C)1998 Moesoft Developments.\n");
if (!(fp1=fopen(file,"r"))) {
        write_user(user,"~CRPost Office : Copy : could not open file for read!.\n");
	return 0;
	}
if (!(fp2=fopen(file2,"w"))) {
        write_user(user,"~CRPost Office : Copy : could not open output file.\n");
	return 0;
	}
if (type==1) {
	sprintf(subj,"Your Smail From %s",TALKERNAME);
	subject=subj;
	fprintf(fp2,"-=+ Your Smail From %s +=-\n\n",TALKERNAME);
	/* Get Rid Of Last Read Date In Smail First Line Seeing It's Smail */
	fgets(line,DNL,fp1);
	line[strlen(line)-1]=0;
	}
else fprintf(fp2,"From: %s <%s>\nSubject: %s\n",user->name,user->email,subject);
sprintf(text,"~CGSending Internet E-Mail To~FW: ~FT%s\n~CGSubject~FW: ~FT%s\n\n",to,subject);
write_user(user,text);
line[0]='\0';
fgets(line,ARR_SIZE,fp1);
while(!feof(fp1)) {
     if (feof(fp1)) break;
     line[strlen(line)-1]=0;
     fprintf(fp2,"%s\n",colour_com_strip(line));
     line[0]='\0';
     l++;
     fgets(line,ARR_SIZE,fp1);
     }
if (type==1) fprintf(fp2,"%s",email_signature);
else fprintf(fp2,"%s",gate_email_signature);
fclose(fp1);  fclose(fp2);
/* Create Mail Process */
switch(double_fork()) {
  case -1 : write_user(user,"Post Office: Error: Fork Error, Could not spawn mailer, Mail not sent!\n");
            return 0; /* double_fork() failed */
  case  0 : sprintf(text,"Sending Email: From: %s, To: %s, [%d Lines]\n",user->name,to,l);
	    write_syslog(text,1);
	    sprintf(text,SENDMAILCMD,to,file2);
            system(text);
            _exit(1);
	    return 1;
  }
return 1;
}

/*** !! ** Create Outgoing Email ** In Development ** !! ***/
void make_email(UR_OBJECT user,char *inpstr,int done_editing)
{
FILE *fp;
char *c,filename[80];
char subject[81];

if (!done_editing) {
     if (word_count<2) {
                write_user(user,"~CTMoesoft's Internet E-Mail Gateway Beta-Test v0.9b\n");
		write_user(user,"Usage: .email <address>\nExample: .email user@foo.bar.com\n");
		return;
		}
	if (strlen(word[1])>120 || strlen(word[1])<10) {
                write_user(user,"~CTMoesoft's Internet E-Mail Gateway Beta-Test v0.9\n");
                write_user(user,"~CRThe email address you specified is way too long or too short!\n");
		return;
		}
	if (!valid_email(user,word[1])) return;
	strncpy(user->to_email,word[1],120);
        sprintf(text,"~CT%s decides to write some Internet Email...\n",user->recap);
        write_room_except(user->room,text,user);
	write_user(user,"\n");
        sprintf(text,"~FG-~CB=~CM] ~FWWriting Internet E-Mail to ~FT%s ~CM[~CG=~FG-",user->to_email);
        write_user(user,center(text,78));
	write_user(user,"\n\n");
	user->misc_op=15;
	editor(user,NULL);
	return;
	}
/* Write The Editor Contents To A File */
sprintf(filename,"%s/%s.email",MAILSPOOL,user->name);
if (!(fp=fopen(filename,"w"))) {
	sprintf(text,"%s: Unable to spool E-mail to send it!\n",syserror);
	write_user(user,text);
	sprintf("ERROR: Couldn't spool email!  filename = %s!\n",filename);
	write_syslog(text,0);
	return;
	}
/* fprintf(fp,"From: %s <%s>\n",user->name,user->email); */
fprintf(fp,"To: %s\n",user->to_email);
fprintf(fp,"Subject: Email Sent By %s from %s\n",user->name,TALKERNAME);
if (valid_email(user,user->email)) fprintf(fp,"Reply-to: %s\n\n",user->email);
/* *** TEMPORARILY REMOVED ***
fprintf(fp,"Moenuts-Reply-To: %s <%s@%s>\n",user->name,user->name,SHORTNAME);
fprintf(fp,"Moenuts-Talker-Address: telnet://%s\n",TALKERADDR);
fprintf(fp,"Moenuts-Talker-Homepage: %s\n",TALKERURL);
fprintf(fp,"Moenuts-Version: %s\n",MNVERSION);
fprintf(fp,"Moenuts-Gateway: %s\n",GWVERSION);
*/
fprintf(fp,"-+- Message by %s from %s (%s) -+-\n\n",user->name,TALKERNAME,TALKERADDR);
c=user->malloc_start;
while(c!=user->malloc_end) putc(*c++,fp);
fclose(fp);
sprintf(text,"~CT%s has finnished writing their Internet Email...\n",user->recap);
write_room_except(user->room,text,user);
sprintf(subject,"Mail From %s at %s",user->name,TALKERNAME);
if (!post_office(user,"",user->to_email,subject,filename,0)) {
        write_user(user,"~CRPost Office Failed To Deliver Your Mail!, Sorry!\n");
	}
user->to_email[0]='\0';
user->misc_op=0;
word[1][0]='\0';
unlink(filename); /* Delete Mail Spool As We Don't Need It Anymore! */
}

/*** Destruct a user's personal room if the room is empty when they logoff ***/
void destruct_user_room(UR_OBJECT user)
{
UR_OBJECT u;
RM_OBJECT rm;
int cnt;

if (!(rm=get_room(user->roomname))) {
#ifdef DEBUG
  write_user(user,"~CRRoom not removed from memory because it doesn't exist!\n");
#endif
	return;
	}
/*
if (rm->access!=PERSONAL) {
#ifdef DEBUG
        write_user(user,"~CRThis room is not a personal room, so it cannot be destructed!\n");
#endif
	return;
	}
*/
cnt=0;
for(u=user_first;u!=NULL;u=u->next) if (u->room==rm) ++cnt;
if (cnt>0) {
#ifdef DEBUG
        write_user(user,"~CTYour room is not empty, so it will not be removed from memory.\n");
#endif
	return;
	}
#ifdef DEBUG
  write_user(user,"~CGYour room has been removed from memory.\n");
#endif
destruct_room(rm);
}

void update_time_bank(UR_OBJECT user)
{
int acctime=0,credit=0,rate=CREDIT_RATE;

if (user->bank_update) return;
acctime=(user->total_login/60);
credit=acctime/rate;
user->bank_balance=(user->bank_balance+credit);
user->bank_update=1;
write_user(user,"~FYUpdating Your Bank Balance From Your Accumulated Online Time...\n");
sprintf(text,"~FTTotal Credits Earned: $%d\n~FTYour Bank Balance: $%d\n",credit,user->bank_balance);
write_user(user,text);
}

/* Page a user via the ICQ Network using the pager.icq.com server          */
/* This can be adapted to other talkers, but it requires Andy Collington's */
/* double_fork() function used in Amnuts, Moenuts and RaMTITS based codes  */
/* (C)1999 Michael Irving, All Rights Reserved, (C) Moesoft Developments.  */

void icqpage(UR_OBJECT user,char *inpstr)
{
UR_OBJECT u;
char filename[81],icq_num[50],reciever[USER_NAME_LEN+2];
FILE *fp;

if (word_count<2) {
     write_user(user,"Usage: .icqpage <user> [<message>]\n");
     return;
     }
if ((u=get_user(word[1]))!=NULL) {    /* Online user */
	if (!isnumber(u->icq)) {
             write_user(user,"\n~CRSorry, user does not have a valid ICQ # Set.\n");
             write_user(user,"~CRThey must have first used .set icq <their icq #>\n");
	     return;
	     }
	sprintf(filename,"%s/icqpage.%s",TEMPFILES,user->name);
	strcpy(icq_num,u->icq);
	strcpy(reciever,u->name);
	if (!(fp=fopen(filename,"w"))) {
             write_user(user,"~CRERROR!  Cannot Create Page Message File...\n");
	     return;
	     }
	fprintf(fp,"Moesoft's ICQ Pager for Moenuts!\n");
	fprintf(fp,"You have been paged by %s from %s.\n",user->name,TALKERNAME);
	fprintf(fp,"Telnet to %s for live Chat!\n",TALKERADDR);
	if (word_count>2) {  /* Only Include Message Header If a message exists */
	     fprintf(fp,"Message:\n");
	     inpstr=remove_first(inpstr);
	     fprintf(fp,"%s\n",inpstr);
	     }
	fclose(fp);
	send_icq_page(user,filename,icq_num);
	return;
	}
if ((u=create_user())==NULL) {      /* user is offline */
	sprintf(text,"%s: unable to create temporary user object.\n",syserror);
        write_user(user,text);
        write_syslog("ERROR: Unable to create temporary user object in icqpage.\n",0);
        return;
        }
word[1][0]=toupper(word[1][0]);
strcpy(u->name,word[1]);
if (!load_user_details(u)) {
        write_user(user,nosuchuser);
        destruct_user(u);
        destructed=0;
        return;
        }
strcpy(icq_num,u->icq);
strcpy(reciever,u->name);
destruct_user(u);
destructed=0;
if (!isnumber(icq_num)) {
        write_user(user,"\n~CRSorry, user does not have a valid ICQ # Set.\n");
        write_user(user,"~CRThey must have first used .set icq <their icq #>\n");
	return;
	}
sprintf(filename,"%s/icqpage.%s",TEMPFILES,user->name);
if (!(fp=fopen(filename,"w"))) {
        write_user(user,"~CRERROR!  Cannot Create Page Message File...\n");
	return;
	}
fprintf(fp,"Moesoft's ICQ Pager for Moenuts!\n");
fprintf(fp,"You have been paged by %s from %s.\n",user->name,TALKERNAME);
fprintf(fp,"Telnet to %s for live Chat!\n",TALKERADDR);
if (word_count>2) {  /* Only Include Message Header If a message exists */
	fprintf(fp,"Message:\n");
	inpstr=remove_first(inpstr);
	fprintf(fp,"%s\n",inpstr);
	}
fclose(fp);
send_icq_page(user,filename,icq_num);
return;
}

/* Now that we've created the pager info, now it's time to send the page! */
void send_icq_page(UR_OBJECT user,char *fname,char *icq_num)
{
char send_icq_num[256];

strncpy(send_icq_num,icq_num,255);
strcat(send_icq_num,"@pager.icq.com");

switch(double_fork()) {
  case -1 : return; /* double_fork() failed */
  case  0 :
         sprintf(text,"\n~FTPageing ~FY%s ~RS~CTvia the ICQ Network!\n",icq_num);
         write_user(user,text);
         write_user(user,"~FTIf the user is online they will recieve your message in a couple minutes\n");
         write_user(user,"~FTOtherwise, they will get your message next time they log onto the ICQ Network!\n");
         sprintf(text,"Sending an ICQ Page from %s to %s\n",user->name,icq_num);
	 write_syslog(text,1);
         sprintf(text,SENDMAILCMD,send_icq_num,fname);
         system(text);
         _exit(1);
	 return;
	 }
}


/* Variable Replace Function For Profiles.
   Portions (C)1999 Curmitt (curmitt@talkernet.net)
   Written for Moenuts. */

char *profsub(char *stptr,UR_OBJECT user)
{
int hour=0,ampm=0;
char age[5],levname[31],gender[11],curtime[10],curdate[10],name[USER_RECAP_LEN+4];
char *strrep();
static char tempstr[(ARR_SIZE*3)+2];

/* Convert Age To A String */
sprintf(text,"%d",user->age);
strncpy(age,text,4);

/* Fix Name With Reset */
strcpy(name,user->recap);
strcat(name,"~RS");

/* Get Levelname Based On Gender */
strncpy(levname,level_name[user->level],30);
if (user->prompt & FEMALE) strncpy(levname,level_name_fem[user->level],30);

/* Convert Gender To Readable Form */
strncpy(gender,"It",10);
if (user->prompt & MALE) strncpy(gender,"Gentalman",10);
if (user->prompt & FEMALE) strncpy(gender,"Lady",10);

/* Time and Date Conversions */
if (thour>12) { hour=(thour-12); ampm=1; }
else { hour=thour; ampm=0; }
sprintf(curtime,"%d:%02d%s",hour,tmin,(!ampm?"am":"pm"));
sprintf(curdate,"%d/%02d/%d",tmonth,tmday,tyear);

/* Process The String And Substitute Where Necessary */

  strncpy(tempstr,stptr,ARR_SIZE*3);
  strncpy(tempstr,strrep(tempstr,"$N",name,ARR_SIZE*3),ARR_SIZE*3);
  strncpy(tempstr,strrep(tempstr,"$E",user->email,ARR_SIZE*3),ARR_SIZE*3);
  strncpy(tempstr,strrep(tempstr,"$I",user->icq,ARR_SIZE*3),ARR_SIZE*3);
  strncpy(tempstr,strrep(tempstr,"$M",user->married,ARR_SIZE*3),ARR_SIZE*3);
  strncpy(tempstr,strrep(tempstr,"$W",user->homepage,ARR_SIZE*3),ARR_SIZE*3);
  strncpy(tempstr,strrep(tempstr,"$G",gender,ARR_SIZE*3),ARR_SIZE*3);
  strncpy(tempstr,strrep(tempstr,"$L",levname,ARR_SIZE*3),ARR_SIZE*3);
  strncpy(tempstr,strrep(tempstr,"$A",user->level_alias,ARR_SIZE*3),ARR_SIZE*3);
  strncpy(tempstr,strrep(tempstr,"$T",curtime,ARR_SIZE*3),ARR_SIZE*3);
  strncpy(tempstr,strrep(tempstr,"$D",curdate,ARR_SIZE*3),ARR_SIZE*3);
  strncpy(tempstr,strrep(tempstr,"$B",user->birthday,ARR_SIZE*3),ARR_SIZE*3);
  strncpy(tempstr,strrep(tempstr,"$Z",user->desc,ARR_SIZE*3),ARR_SIZE*3);
  strncpy(tempstr,strrep(tempstr,"$S",user->site,ARR_SIZE*3),ARR_SIZE*3);
  strncpy(tempstr,strrep(tempstr,"$R",user->room->name,ARR_SIZE*3),ARR_SIZE*3);
  strncpy(tempstr,strrep(tempstr,"$P",user->predesc,ARR_SIZE*3),ARR_SIZE*3);
  strncpy(tempstr,strrep(tempstr,"$I",user->login_room,ARR_SIZE*3),ARR_SIZE*3);
  strncpy(tempstr,strrep(tempstr,"$HC",user->hang_word,ARR_SIZE*3),ARR_SIZE*3);

return tempstr;
}

/*** Make a new news file ***/
void addnews(UR_OBJECT user,char *inpstr,int done_editing)
{
FILE *fp;
char *c,filename[80];
char line[162];

sprintf(filename,"%s/%s.ascii",SCREENFILES,NEWSFILE);
if (!done_editing) {
        if (word_count<2) {
                write_user(user,"Usage:  addnews <topic>\n");
		write_user(user,"Usage:  addnews --delnews\n");
                return;
                }
	if (!strcmp(word[1],"--delnews")) {
		sprintf(filename,"%s/%s",SCREENFILES,NEWSFILE);
		unlink(filename);
                write_user(user,"~CRNewsfile Deleted.\n");
		return;
		}
        sprintf(text,"~CM%s is updating the news...\n",user->name);
	write_room(user->room,text);
	sprintf(newstopic,"%-40.40s",inpstr);
        write_user(user,"~CS\n~CT+----------------------------------------------------------------------------+\n");
        if (!file_exists(filename)) write_user(user,"~CT|                  ~FGCreating A New News File~FT                                  |\n");
        else write_user(user,"~CT|                  ~FGAdding To The New News File~FT                               |\n");
        write_user(user,"~CT+----------------------------------------------------------------------------+\n");
        sprintf(text," ~CTNews Written By~FW: ~RS%s ~RS~FGon ~RS~FT%s\n",user->name,long_date(2));
        write_user(user,text);
        sprintf(text," ~CTNews Topic     ~FW: ~CG%s\n",newstopic);
        write_user(user,text);
        write_user(user,"~CT+----------------------------------------------------------------------------+\n\n");
        user->misc_op=16;
	editor(user,NULL);
	return;
	}

if (!(fp=fopen(filename,"a"))) {
        sprintf(text,"%s: couldn't append to the news file\n",syserror);
	write_user(user,text);
        sprintf("ERROR: Couldn't open file %s to write in addnews().\n",filename);
	write_syslog(text,0);
	return;
	}
sprintf(line,"~CT+----------------------------------------------------------------------------+\n");
fputs(line,fp);
sprintf(line," ~CTNews Written By~FW: ~RS%s~RS ~FGon ~FT%s\n",user->recap,long_date(2));
fputs(line,fp);
sprintf(line," ~CTNews Topic     ~FW: ~CG%s\n",newstopic);
fputs(line,fp);
sprintf(line,"~CT+----------------------------------------------------------------------------+\n");
fputs(line,fp);
c=user->malloc_start;
while(c!=user->malloc_end) putc(*c++,fp);
sprintf(line,"~CT+----------------------------------------------------------------------------+\n");
fputs(line,fp);
fclose(fp);
sprintf(text,"%s added to the news file.\n",user->name);
write_syslog(text,1);
write_user(user,"New News Stored.\n");
}

int startup_script(UR_OBJECT user)
{
int retcode=-1,linecount=0;
FILE *fp;
char filename[81];
char line[ARR_SIZE+1], *cmdstring;

sprintf(filename,"%s/%s.SCR",USERFILES,user->name);
write_user(user,"~FTSystem~FW: ~FGProcessing Your Startup Script...\n");
if (!(fp=fopen(filename,"r"))) {
	write_user(user,"~FRNo Startup Script Found, Skipping...\n");
	retcode=0;
	return retcode;
	}

fgets(line,ARR_SIZE,fp);
while(!feof(fp)) {
	whitespace(line);
	line[strlen(line)-1]=0;
	cmdstring=line;
	clear_words();
	word_count=wordfind(cmdstring);
	linecount++;
	sprintf(text,"Script [%02d]: %s\n",linecount,cmdstring);
   	if (!strcasecmp(word[1],"if")) {
		cmdstring=remove_first(cmdstring);
		if (!strcasecmp(word[2],"newmail")) {
			cmdstring=remove_first(cmdstring);
			exec_com(user,cmdstring);
			}
		else if (!strcasecmp(word[2],"online")) {
			cmdstring=remove_first(cmdstring);
			if (user_online(word[3])) {
				word[3][0]=toupper(word[3][0]);
                                sprintf(text,"\07~CY~BMOnline Alert~RS~CT: ~RS~FG%s is Online!\n",word[3]);
				}
			}
		}
	else if (!strcasecmp(word[1],"exec")) {
		 cmdstring=remove_first(cmdstring);
		 exec_com(user,cmdstring);
		 }
	fgets(line,ARR_SIZE,fp);
	}
fclose(fp);
sprintf(text,"Script Complete, Lines Processed: %d\n",linecount);
write_user(user,text);
retcode=1;
return retcode;
}

/* Check To See If User Is Online, If so, returns a 1, else 0 */
int user_online(char *username)
{
UR_OBJECT user;
if (!(user=get_user(username))) return 0;
return 1;
}


/* --------------------- Start of Seamless Reboot functions ----------------- */
/* Command for calling .sreboot */
void sreboot_com(UR_OBJECT user)
{

if (word_count<1 && !isnumber(word[1])) {
	write_user(user,"Usage: .sreboot [<secs/-cancel>]\n");
	return;
	}
if (rs_which==1) {
        write_user(user,"~CR The reboot countdown is currently active, cancel it first.\n");
	return;
	}
if (!rs_which) {
        write_user(user,"~CR The shutdown countdown is currently active, cancel it first.\n");
	return;
	}
if (!strncasecmp(word[1],"-cancel",2)) {
	if (rs_which!=2) {
                write_user(user,"~CM The seamless reboot countdown isn't currently active.\n");
		return;
		}
	if (rs_countdown && rs_which==2 && rs_user==NULL) {
                write_user(user,"~CM Someone else has set the seamless reboot countdown.\n");
		return;
		}
        write_room(NULL,"~CB[~CYM~CYoenuts~CB]~FT: ~FB-~RS~FT Seamless reboot has been cancelled..\n");
	sprintf(text,"[SREBOOT] - Cancelled by: [%s]\n",user->name);
	write_syslog(text,1);
	rs_countdown=0;	rs_announce=0;  rs_which=-1;  rs_user=NULL;
	return;
	}
if (rs_countdown && rs_which==2) {
        write_user(user,"~CR The seamless reboot countdown is currently active, cancel it first.\n");
	return;
	}
if (word_count<2) {
	rs_countdown=0;  rs_announce=0;  rs_which=-1;  rs_user=NULL;
	}
else {
	rs_countdown=atoi(word[1]);
	rs_which=2;
	}
write_user(user,"~BP~CB[~CRWarning~CB] ~CT- ~CRThis will reboot the talker.\n");
write_user(user,"~CM            Are you sure you want to do this?~FT:~RS ");
user->misc_op=17;
no_prompt=1;
}

#define REBOOTING_DIR   "reboot"
#define USER_LIST_FILE  "reboot/_ulist"
#define TALKER_SYS_INFO "reboot/_sysinfo"
#define CHILDS_PID_FILE "reboot/_child_pid"
#define ROOM_LIST_FILE  "reboot/_rlist"

char rebooter[USER_NAME_LEN+2];
char mrname[ROOM_NAME_LEN+2];

typedef struct talker_sys_info {
	int sockets[2];
	char mr_name[ROOM_NAME_LEN+2];
	char rebooter[USER_NAME_LEN+2];
	int logins,new_users;
	time_t boot_time;
	} talker_sys_info;
/* Builds the system information. */
int build_sysinfo(UR_OBJECT user)
{
talker_sys_info tsi;
FILE *fp;

memset(&tsi,0,sizeof(talker_sys_info));
strcpy(tsi.mr_name,room_first->name);

memset(&tsi,0,sizeof(talker_sys_info));
strcpy(tsi.mr_name,room_first->name);
tsi.sockets[0]=listen_sock[0];
tsi.sockets[1]=listen_sock[1];
tsi.boot_time=boot_time;
if (user) strcpy(tsi.rebooter,user->name);
if (!(fp=fopen(TALKER_SYS_INFO,"w"))) {
	if (user) {
                sprintf(text,"~CW[~FRERROR~FW] - ~RS~FG Unable to open reboot system info file [%s]\n",strerror(errno));
		write_user(user,text);
		}
	else {
		sprintf(text,"[ERROR] - Unable to open reboot info file: [%s]\n",strerror(errno));
		write_syslog(text,0);
		}
	return -1;
	}
fwrite((void *)&tsi,sizeof(talker_sys_info),1,fp);
fclose(fp);
return 0;
}

/* Builds the logged in users room information. */
int build_room_list(UR_OBJECT user)
{
FILE *fp;
RM_OBJECT rm;

fp=fopen(ROOM_LIST_FILE,"w");
if (!fp) {
	if (user) {
                sprintf(text,"~CW[~FRERROR~FW] - ~RS~FG Unable to open reboot room list file [%s]\n",strerror(errno));
		write_user(user,text);
		}
	else {
		sprintf(text,"[ERROR] - Unable to open reboot room list file: [%s]\n",strerror(errno));
		write_syslog(text,0);
		}
	return -1;
	}
for(rm=room_first;rm!=NULL;rm=rm->next) fprintf(fp,"%s\n",rm->name);
fclose(fp);
return 0;
}

/* This will build a list of all the users and the room which they are in. */
int build_user_list(UR_OBJECT user)
{
FILE *fp;
RM_OBJECT rm;
UR_OBJECT u;
int i;

fp=fopen(USER_LIST_FILE,"w");
if (!fp) {
	if (user) {
                sprintf(text,"~CW[~FRERROR~FW] - ~RS~FG Unable to open reboot user list file [%s]\n",strerror(errno));
		write_user(user,text);
		}
	else {
		sprintf(text,"[ERROR] - Unable to open reboot user list file: [%s]\n",strerror(errno));
		write_syslog(text,0);
		}
	return -1;
	}
for(rm=room_first;rm!=NULL;rm=rm->next) {
	for (i=0;i<strlen(rm->name);++i) {
		if (rm->name[i]==' ') rm->name[i]='_';
		}
	}
for (u=user_first;u!=NULL;u=u->next) {
	if (u->login || u->type==CLONE_TYPE) continue;
	fprintf(fp,"%s %s\n",u->name,u->room->name);
	}
fclose(fp);
return 0;
}

/* Build the logged in users information */
int build_loggedin_users_info(UR_OBJECT user)
{
UR_OBJECT u;
FILE *fp;

for (u=user_first;u!=NULL;u=u->next) {
        if (u->malloc_start!=NULL) {
                write_user(u,"\n~CB[~FTEDITOR~FB]~FT: ~FRSeamless reboot requested close, I Must Exit Now, Sorry.\n");
                editor_done(u);
                }
	if (u->login) {
                write_user(u,"\n~CRTalker Reboot in progress.. Aborting your connection.\n~CMPlease try logging in again in a few seconds...\n");
		close(u->socket);
		continue;
		}
	sprintf(text,"%s/%s",REBOOTING_DIR,u->name);
	if (!(fp=fopen(text,"w"))) {
		if (user) {
                        sprintf(text,"~CW[~FRERROR~FW] - ~RS~FG Unable to open reboot user info file for '%s': [%s]\n",u->name,strerror(errno));
			write_user(user,text);
			}
		else {
			sprintf(text,"[ERROR] - Unable to open reboot user info file for '%s': [%s]\n",u->name,strerror(errno));
			write_syslog(text,0);
			}
		return -1;
		}
	fwrite((void *)u,sizeof(struct user_struct),1,fp);
	fclose(fp);
	}
return 0;
}

/* Do the reboot. */
void do_reboot(UR_OBJECT user)
{
UR_OBJECT u;
FILE *fp;
int cpid;
char *args[]={ progname,confile,NULL };

write_room(NULL,"~CB[~CYM~CYoenuts~CB]~CT: ~CGSeamless Reboot In Progress, Please Wait...\n");
/* First get rid of any clones */
for (u=user_first;u!=NULL;u=u->next) {
	if (u->login || u->type!=CLONE_TYPE) continue;
	write_user(u->owner,"~CB[~CYM~CYoenuts~CB]~CT:~CRYour Clone Must Be Destroyed.\n");
	destroy_user_clones(u);
	}
if (build_sysinfo(user)<0) {
	if (user) write_user(user,"~FG Reboot failed <build_sysinfo()>\n");
	else write_syslog("[ERROR] - Reboot failed <build_sysinfo()>\n",1);
	return;
	}
if (build_room_list(user)<0) {
	if (user) write_user(user,"~FG Reboot failed <build_room_list()>\n");
	else write_syslog("[ERROR] - Reboot failed <build_room_list()>\n",1);
	return;
	}
if (build_user_list(user)<0) {
	if (user) write_user(user,"~FG Reboot failed <build_user_list()>\n");
	else write_syslog("[ERROR] - Reboot failed <build_user_list()>\n",1);
	return;
	}
if (build_loggedin_users_info(user)<0) {
	if (user) write_user(user,"~FG Reboot failed <build_loggedin_users_info()>\n");
	else write_syslog("[ERROR] - Reboot failed <build_loggedin_users_info()>\n",1);
	return;
	}
for (u=user_first;u!=NULL;u=u->next) {
	if (u->login || u->type==CLONE_TYPE) continue;
	save_user_details(u,1);
	}
close_sockets();
cpid=fork();
switch (cpid) {
	case -1:
	  write_syslog("[ERROR] - Failed to fork() in rebooting.\n",0);
          if (user) write_user(user,"~CRFailed to fork.\n");
	  break;
	case 0 :
	  execvp(progname,args);
	  /* run the background process and die */
	  write_syslog("[ERROR] - Exiting child process.\n",0);
	  break;
	default:
	  /* parents thread.. put the childs PID to file for reboot matching */
	  fp=fopen(CHILDS_PID_FILE,"w");
	  if (fp) {  fprintf(fp,"%u",(unsigned int)cpid);  fclose(fp);  }
	  else {
		sprintf(text,"[ERROR] - Failed to open %s. Reason: [%s] Reboot failed.\n",CHILDS_PID_FILE,strerror(errno));
		write_syslog(text,0);
		}
	  exit(0);
	}
}

/* After the reboot functions.. */
void trans_to_quiet(UR_OBJECT user,char rmname[ROOM_NAME_LEN])
{
RM_OBJECT aroom;

aroom=get_room(rmname);
if (aroom) {  user->room=aroom;  return;  }
else {  user->room=room_first;  return;  }
}

/* Create a user */
UR_OBJECT create_user_template(UR_OBJECT user)
{
UR_OBJECT u;

u=(UR_OBJECT)malloc(sizeof(struct user_struct));
memcpy(u,user,sizeof(struct user_struct));
if (!user_first) {  user_first=u;  u->prev=NULL;  }
else {  user_last->next=u;  u->prev=user_last;  }
u->next=NULL;
user_last=u;
return u;
}

/* Retrieve the talkers system info. */
int retrieve_sysinfo(void)
{
talker_sys_info tsi;
FILE *fp;

memset(&tsi,0,sizeof(talker_sys_info));
fp=fopen(TALKER_SYS_INFO,"r");
if (!fp) {
	write_syslog("Failed to open reboot system info file for reading.\n",1);
	return -1;
	}
fread((void *)&tsi,sizeof(talker_sys_info),1,fp);
fclose(fp);
strcpy(mrname,tsi.mr_name);
listen_sock[0]=tsi.sockets[0];
listen_sock[1]=tsi.sockets[1];
boot_time=tsi.boot_time;
strncpy(rebooter,tsi.rebooter,USER_NAME_LEN);
return 0;
}

/* First we retrieve the rooms. */
void retrieve_rooms(void)
{
FILE *fp;
RM_OBJECT rm;
char rmname[ARR_SIZE+1];

fp=fopen(ROOM_LIST_FILE,"r");
if (!fp) {
	write_syslog("[ERROR] - fopen failed in retrieve_rooms().\n",1);
	return;
	}
/* First we go through and re-create all the rooms. */
fgets(rmname,ARR_SIZE,fp);
while (!feof(fp)) {
	rmname[strlen(rmname)-1]=0;
	if ((rm=get_room(rmname))) goto NEXTROOM;
	if (!(rm=create_room())) {
		sprintf(text,"[ERROR] - Failed to create room object for room %s in retrieve_rooms().\n",rmname);
		write_syslog(text,0);
		goto NEXTROOM;
		}
	NEXTROOM:
	fgets(rmname,ARR_SIZE,fp);
	}
fclose(fp);
}

/* Now we do the users */
void retrieve_users(void)
{
FILE *fp1,*fp2;
UR_OBJECT u;
RM_OBJECT rm;
struct user_struct spanky;
char name[USER_NAME_LEN+2],rmname[ROOM_NAME_LEN+2];
/* int i; */

fp1=fopen(USER_LIST_FILE,"r");
if (!fp1) {
	write_syslog("[ERROR] - fopen failed in retrieve_users().\n",0);
	return;
	}
fscanf(fp1,"%s %s",name,rmname);
while (!feof(fp1)) {
	sprintf(text,"%s/%s",REBOOTING_DIR,name);
	fp2=fopen(text,"r");
	if (fp2) {
                /* for (i=0;i<strlen(rmname);++i) if (rmname[i]=='_') rmname[i]=' '; */
		fread((void *)&spanky,sizeof(struct user_struct),1,fp2);
		create_user_template(&spanky);
		num_of_users++;
		u=get_user(name);
		if (u) {
                        /* Re-Create User Rooms */
                        if (ALLOW_USER_ROOMS && (strcasecmp(u->roomname,"None"))) makeroom(u);
			rm=get_room(rmname);
			if (rm) { u->room=rm; load_room_preferences(rm); }
			else u->room=room_first;
			}
		fclose(fp2);
		}
	else {
		sprintf(text,"[ERROR] - Failed to open reboot user info for '%s': [%s]\n",name,strerror(errno));
		write_syslog(text,0);
		}
	fscanf(fp1,"%s %s",name,rmname);
	}
fclose(fp1);
return;
}

/* put the spaces back in the rooms. */
void redo_rooms(void)
{
RM_OBJECT rm;
int i=0;

for(rm=room_first;rm!=NULL;rm=rm->next) {
        for (i=0;i<strlen(rm->name);i++) {
		if (rm->name[i]=='_') rm->name[i]=' ';
		}
	}
}

/* Now we recuperate from what just happened ;-) */
int possibly_reboot(void)
{
UR_OBJECT u,chk;
RM_OBJECT rm;
FILE *fp;
char r[16];

write_syslog("[Moenuts] - Checking for a reboot proof.....\n",0);
fp=fopen(CHILDS_PID_FILE,"r");
if (!fp) {
	write_syslog("[Moenuts] - Can't find child's PID. This isn't a reboot.\n",0);
	return 0;
	}
memset(r,0,16);
fgets(r,15,fp);
sprintf(text,"[Moenuts] - Reading child's PID.. boot: [%u] - child: [%u]\n",(unsigned int)getpid(),(unsigned int)atoi(r));
write_syslog(text,0);
if (getpid()!=atoi(r)) {
	write_syslog("[Moenuts] - This isn't a reboot.\n",0);
	return 0;
	}
write_syslog("[Moenuts] - [possibly_reboot()] Rebooting...\n",0);
retrieve_sysinfo();
retrieve_rooms();
/* We don't have spaces in rooms yet ;) */
/* redo_rooms(); */
checkbadrooms();
retrieve_users();
system("rm -f " REBOOTING_DIR "/*");
/* Now check to see if the users room is NULL, if so, move em to room_first */
for (chk=user_first;chk!=NULL;chk=chk->next) {
	if (chk->room) continue;
	if (chk->room==NULL) {
		rm=room_first;
		if (rm) chk->room=rm;
		else write_syslog("[ERROR] - room_first wasn't found after sreboot..\n",0);
		}
	}
if (rebooter[0]) {
	u=get_user(rebooter);
/*	if (u) write_room(NULL,"~CB[~CYM~CYoenuts~CB]~CT: ~CMSeamless reboot has completed.\n"); */
/*	else { */
		sprintf(text,"[%s] didn't survive the reboot. :~(\n",rebooter);
		write_syslog(text,0);
/*		} */
 	}
write_room(NULL,"~CB[~CYM~CYoenuts~CB]~CT: ~CMSeamless reboot has completed.\n");
return 1;
}

/* This is function goes through all sockets that are open on listen_sock[] -
 - and closes them.                                                        */
void close_sockets(void)
{
UR_OBJECT u;
int i,d=0;
for (i=3;i<(i<<12);i++) { /* Start at 3 so we don't kill stdout, etc */
	if (i==listen_sock[0] || i==listen_sock[1]) continue;
	for (u=user_first;u!=NULL;u=u->next) if (i==u->socket) break;
	if (u!=NULL) continue;
	if (close(i)==0) d++;
	}
return;
}

/* Get and return the mail address domain from a given email address */
char *emaildomain(char *emailaddr)
{
char temp[ARR_SIZE+1];
int atpos=0;

if (!emailaddr[0]) return NULL;
strncpy(temp,emailaddr,ARR_SIZE);
atpos=(int)strstr(temp,"@");
emailaddr+=(atpos+1);
return emailaddr;
}

/* Check For Bad (NULL) rooms and destruct them if they exist
   Cuz they are the spawn of Satin and therefore are evil ;) */
void checkbadrooms(void)

{
RM_OBJECT room;

for(room=room_first;room!=NULL;room=room->next) {
        /* If Room Name Is Null, it's Obviously Corrupted */
        if (!room->name[0]) destruct_room(room);
        }
return;
}

/* Clears The Topic In Specified Room(s) */

void ctopic(UR_OBJECT user)
{
RM_OBJECT room;
int i=0;

if (!word[0][0] && user->level<ARCH) {
	user->room->topic[0]='\0';
	write_user(user,"The Topic Has Been Cleared In This Room.\n");
	sprintf(text,"%s has cleared the topic in this room.\n",user->name);
	write_room_except(user->room,text,user);
	return;
	}
if (!word[0][0] && user->level<ARCH) {
	write_user(user,"Usage:: ctopic <room>/all\n");
	return;
	}
if (strcasecmp(word[1],"all")) {
	room=get_room(word[1]);
	if (room!=NULL) {
		room->topic[0]='\0';
		sprintf(text,"~CMThe topic in the %s~RS~CM room has been cleared.\n",room->recap);
		write_user(user,text);
		return;
		}
	write_user(user,nosuchroom);
	write_user(user,"Usage: ctopic [room]/[all]\n");
	return;
	}
if (!strcasecmp(word[1],"all")) {
	for(room=room_first;room!=NULL;room=room->next) {
		i++;
		room->topic[0]='\0';
		sprintf(text,"~CMThe topic in the %s~RS~CM room has been cleared.\n",room->recap);
		write_user(user,text);
		}
	sprintf(text,"Total Room Topics Cleared: %d\n",i);
	write_user(user,text);
	return;
	}
write_user(user,"Usage: ctopic [room]/[all]\n");
return;
}

void alias(UR_OBJECT user, char* inpstr)  {
  char *temp;

  if (word_count<2) {
      write_user(user,"Usage  : alias <alias>\n");
      write_user(user,"Note   : Temporarily Changes Your Name!\n");
      return;
      }
  if (strlen(inpstr)>USER_RECAP_LEN-1) {
       write_user(user,"~CRYour Alias Is Too Long!!  ~FMRemove some colors maybe?\n");
       return;
       }
    temp=colour_com_strip(inpstr);
    if (!strcasecmp(temp,user->name)) {
       /* Store Old Recap If This Is The First Time */
       strncpy(user->alias,user->recap,USER_RECAP_LEN);
       }
    strncpy(user->recap,inpstr,USER_RECAP_LEN);
    sprintf(text,"\n~CMYour alias has been changed to~FR:~RS %s\n",user->recap);
    write_user(user,text);
    return;
}

void blah(UR_OBJECT user) {
	/* do absolutely nothing */
	user->last_input=time(0);
	user->afk=0;
	user->warned=0;
	return;
	}


void rebuild_userlist(UR_OBJECT user)
{
FILE *fp;
FILE *fp2;

int uf=0, ufx=0;
char filename[81], outfile[81], line[ARR_SIZE+1], ufilename[81];
char temp[USER_NAME_LEN+1];

sprintf(filename,"%s/%s",USERFILES,USERLIST);
sprintf(outfile,"%s/%s.temp",USERFILES,USERLIST);
sprintf(text,"sort %s > %s",filename,outfile);
system(text);
unlink(filename);
rename(outfile,filename);

if (!(fp=fopen(filename,"r"))) {
	write_user(user,"ERROR: Could not open userlist file!\n");
	return;
	}
if (!(fp2=fopen(outfile,"w"))) {
	write_user(user,"ERROR: Could not open temporary output file for writing.\n");
	return;
	}
write_user(user,"~FG~ULChecking Userlist File For Errors...\n\n");
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
while(!feof(fp)) {
	strncpy(temp,line,USER_NAME_LEN);
	strtolower(line);
	line[0]=toupper(line[0]);
	sprintf(ufilename,"%s/%s.D",USERFILES,temp);
	sprintf(text,"~FTChecking for ~FY%-*s's Userfile ",USER_NAME_LEN,temp);
	write_user(user,text);
	if (!file_exists(ufilename)) {
		ufx++;
		write_user(user,"~FR - Does Not Exist, Removing Entry From Userlist! ~RS\n");
		}
	else {
		sprintf(text,"~FG - [~UL%s~RS~FG] Exists, Updating Userlist. ~RS\n",line);
		write_user(user,text);
		strcat(line,"\n");
		fprintf(fp2,"%s",line);
		uf++;
		}
	fgets(line,ARR_SIZE,fp);
	line[strlen(line)-1]=0;
	}
fclose(fp);
fclose(fp2);
write_user(user,"~FMSorting Userlist...\n");
sprintf(text,"sort %s > %s",outfile,filename);
system(text);
unlink(outfile);
sprintf(text,"~FGActive Accounts~FW: ~FT%d~FB\n~FRRemoved Entries~FW: ~FT%d\n\n",uf,ufx);
write_user(user,text);
}


void purge_users(UR_OBJECT user)
{

int offline=0;
char uname[USER_NAME_LEN+1],line[ARR_SIZE+1], filename[81];
FILE *fp;
UR_OBJECT u;

if (word_count<2) {
	write_user(user,"Usage: purge [# of days]\n\n");
	return;
	}

sprintf(filename,"%s/%s",USERFILES,USERLIST);
write_user(user,"Opening the user list and gathering the user info...\n");
if (!(fp=fopen(filename,"r"))) {
	write_user(user,"Could not open userlist...\n");
	return;
	}

fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
while(!feof(fp)) {
        strncpy(uname,line,USER_NAME_LEN);
        strtolower(line);
        line[0]=toupper(line[0]);
	strncpy(uname,line,USER_NAME_LEN);
	/* Get User Details */
        if (!(u=get_user(word[1]))) {
           if ((u=create_user())==NULL) {
                sprintf(text,"%s: unable to create temporary user object.\n",syserror);
                write_user(user,text);
                write_syslog("ERROR: Unable to create temporary user object in status().\n",0);
                return;
                }
        offline=1;
        word[1][0]=toupper(word[1][0]);
        strcpy(u->name,word[1]);
        if (!load_user_details(u)) {
                write_user(user,nosuchuser);
                destruct_user(u);
                destructed=0;
                return;
                }

	fgets(line,ARR_SIZE,fp);
	line[strlen(line)-1]=0;
	}
   }
}

/*** Brand A User ***/
void brand_user(UR_OBJECT user,char *inpstr)
{
UR_OBJECT u;
char *remove_first();

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (user->muzzled & SCUM) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (word_count<3) {
	write_user(user,"Usage: brand <user> <brand description>\n");
	write_user(user,"~FRRule ~FT: ~FYMake sure when branding another user, you both are in agreeance.\n");
	return;
	}
if (!(u=get_user(word[1]))) {
	write_user(user,"~CRBoth of you ~ULmust~RS~CR be online before you can brand someone.\n");
	return;
	}
if (!strcmp(u->name,user->name)) {
	write_user(user,"~CRBranding Yourself?  Are you that lonely?\n");
	return;
	}
write_room(NULL,"~CB[ ~CYA branding takes place! ~CB]\n");
sprintf(text,"~CMYou have been branded by %s~CM.\n",user->recap);
write_user(u,text);
sprintf(text,"~RS%s ~RSwinces as they get branded by %s ~RS...\n",u->recap,user->recap);
write_room(NULL,text);
strncpy(u->branded_by,user->name,USER_NAME_LEN);
inpstr=remove_first(inpstr);
if (strlen(inpstr)>BRAND_DESC_LEN-1) write_user(user,"~FRYour branding description is too long, it has been truncated to fit.\n~RS");
strncpy(u->brand_desc,inpstr,BRAND_DESC_LEN-1);
sprintf(text,"~CYThe Brand~CT: ~RS%s\n\n",u->brand_desc);
write_room(NULL,text);
}

/*** Un-Brand A User (WiZ) ***/
void unbrand_user(UR_OBJECT user)
{
UR_OBJECT u;

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (user->muzzled & SCUM) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (word_count<2) {
	write_user(user,"Usage: unbrand <user>\n");
	return;
	}
if (!(u=get_user(word[1]))) {
	write_user(user,"~CRThe user ~ULmust~RS~CR be online before you can unbrand them.\n");
	return;
	}
if (!strcmp(u->name,user->name)) {
	write_user(user,"~CRYou cannot un-brand yourself, ask another staff member to remove your brand.\n");
	return;
	}
write_room(NULL,"~CB[ ~CYA Brand Removal Takes Place! ~CB]\n");
sprintf(text,"~CMYour brand has been removed by %s~CM.\n",user->recap);
write_user(u,text);
sprintf(text,"~RS%s ~RShas their branding removed by %s ~RS...\n",u->recap,user->recap);
write_room(NULL,text);
strncpy(u->branded_by,"Nobody",USER_NAME_LEN);
strncpy(u->brand_desc,"None",BRAND_DESC_LEN);
}

void capture(UR_OBJECT user)
{
int i=0,line=0;
char filename[81];
FILE *fp;

/* Submit A User's Tell Buffer (USERFILES/User.TB) */
if (!strcasecmp(word[1],"-tells")) {
	sprintf(filename,"%s/%s.TB",USERFILES,user->name);
	if (!(fp=fopen(filename,"w"))) {
		write_user(user,"Failed To Open File To Log Tells To...Sorry...\n");
		return;
		}
	write_user(user,"Your tells will now be submitted for review by the staff\nAnd your tell review buffer will be cleared...\n");
	fprintf(fp,"[ CAPTURE ]: Tells Submitted By %s %s\n",user->name,long_date(0));
	fprintf(fp,"~CT%s\n",HORIZ);
	for(i=0;i<REVTELL_LINES;++i) {
		line=(user->revline+i)%REVTELL_LINES;
		if (user->revbuff[line][0]) {
			fprintf(fp,"%s",user->revbuff[line]);
	  		}
	}
	fprintf(fp,"~CT%s\n",HORIZ);
	fclose(fp);
	for(i=0;i<REVTELL_LINES;++i) user->revbuff[i][0]='\0';
	write_user(user,"Your review buffer has ben cleared.\nYour tells were submitted for review by the staff.\n\n");
	sprintf(text,"%s ~RShas just willingly submitted their tell buffer for review by the staff\nUsing the capture command.\n",user->recap);
	write_room(NULL,text);
	return;
	}
if (!strcmp(word[1],"-read")) {
	sprintf(text,"%s/%s.TB",USERFILES,user->name);
	switch(more(user,user->socket,text)) {
		case 0: write_user(user,"Could not open your tell capture.\n\n");
		case 1: user->misc_op=2;
		}
	return;
	}
write_user(user,"Usage: .capture <-tells | -review>\n\n");
write_user(user,"~BP~CR~ULWARNING~RS~FR: ~FYBy using this command, you grant the staff permission\n");
write_user(user,"~FYto read the tells currently in your tell buffer at the time you\n");
write_user(user,"~FYuse this command.  If you do not agree to this, ~FTdo not use\n~FTthis command!\n\n");
}


/*** The Who List Re-Make Of Shackles For Moenuts (modified/re-written by moe@moenuts.com) ***/
void shackles_who(UR_OBJECT user)
{
UR_OBJECT u;
RM_OBJECT rm;
int cnt,total,invis,mins,idle,logins,ugender,uplaying,rcnt;
char line[USER_NAME_LEN+USER_DESC_LEN*2];
char rname[ROOM_RECAP_LEN+1],idlestr[20],ucaste[2],ulevel[2];
char *ny2[]={ "N","Y" };
char *sex2[]={"N","M","F"};
char *center();

total=0;  invis=0;  logins=0;  ugender=0;  uplaying=0;

write_user(user,"\n~CT)-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-(\n");
sprintf(text,"~FGCurrent Users %s\n",long_date(1));
write_user(user,center(text,78));
write_user(user,"  ~CTName          Desc                   Sex/Lev : Room         : Cst/Ply/Tm/Id\n");
write_user(user,"~CT)-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-(\n\n");
for(rm=room_first;rm!=NULL;rm=rm->next) { 
for(u=user_first;u!=NULL;u=u->next) {
  if (u->type==CLONE_TYPE) continue;
  if (u->login) continue;
  if (!(u->room==rm)) continue;
  if (u->room->hidden && user->level<OWNER) continue;
  if (u->hidden && user->level<OWNER) continue;
  if (!u->vis && user->level<u->level) continue;
  mins=(int)(time(0) - u->last_login)/60;
  idle=(int)(time(0) - u->last_input)/60;
  ++total;
  if (!u->vis) {
    ++invis;
    if (u->level>user->level && !(user->level>=OWNER)) continue;
    }
  sprintf(line," %s~RS %s",u->recap,u->desc);
  if (!u->vis) line[0]='I';
  if (u->room->hidden) line[0]='&';
  if (u->hidden) line[0]='#';
  strcpy(rname,u->room->recap);
  /* Count number of colour coms to be taken account of when formatting */
  cnt=colour_com_count(line);
  rcnt=colour_com_count(rname);
  if (u->afk) strcpy(idlestr,"~FRAFK~RS");
  else if (u->malloc_start!=NULL) strcpy(idlestr,"~FTEDIT~RS");
  else if (idle>=30) strcpy(idlestr,"~FYIDLE~RS");
  else sprintf(idlestr,"%d/%d",mins,idle);
  ugender=0;
  if (u->prompt & 4) ugender=2;
  if (u->prompt & 8) ugender=1;
  strncpy(ucaste,bdsm_types[u->bdsm_type],1);
  strncpy(ulevel,level_name[u->level],1);
  sprintf(text,"%-*.*s~RS  ~CG%-1.1s~RS/%-1.1s : %-*.*s : ~CY%-1.1s~RS/~FT%-1.1s~RS/%s\n",(41+cnt*3),(44+cnt*3),line,sex2[ugender],ulevel,(12+rcnt*3),(12+rcnt*3),rname,ucaste,ny2[uplaying],idlestr);
  write_user(user,text);
  }
 }
write_user(user,"\n~CR)-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-(\n");
sprintf(text,"Total of ~CT%d~RS visible, ~CT%d~RS invisible",total-invis,invis);
write_user(user,center(text,78));
write_user(user,"\n~CR)-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-O-(\n\n");
}

/*** The Who List Re-Make Of Shackles For Moenuts (modified/re-written by moe@moenuts.com) ***/
void camelot_who(UR_OBJECT user)
{
UR_OBJECT u;
RM_OBJECT rm;
int total,invis,mins,idle,logins,ugender,uplaying;
char line[USER_NAME_LEN+USER_DESC_LEN+2];
char rname[ROOM_RECAP_LEN+1],idlestr[25],rank[USER_ALIAS_LEN+1];
char uname[USER_NAME_LEN+1], udesc[USER_DESC_LEN+1];
char *sex2[]={"-","M","F"};
char *center(),*colour_com_strip();

total=0;  invis=0;  logins=0;  ugender=0;  uplaying=0;

sprintf(text,"\n-=+ Current Countrymen on %s %s +=-\n\n",SHORTNAME,long_date(1));
write_user(user,text);
for(rm=room_first;rm!=NULL;rm=rm->next) { 
for(u=user_first;u!=NULL;u=u->next) {
  if (u->type==CLONE_TYPE) continue;
  if (u->login) continue;
  if (u->room->hidden && user->level<OWNER) continue;
  if (u->hidden && user->level<OWNER) continue;
  if (!u->vis && user->level<u->level) continue;
  if (!(u->room==rm)) continue;
  mins=(int)(time(0) - u->last_login)/60;
  idle=(int)(time(0) - u->last_input)/60;
  ++total;
  if (!u->vis) {
    ++invis;
    if (u->level>user->level && !(user->level>=OWNER)) continue;
    }
  strcpy(uname,colour_com_strip(u->recap));
  strcpy(udesc,colour_com_strip(u->desc));
  sprintf(line,"%s~RS %s",uname,udesc);
  strcpy(rname,colour_com_strip(u->room->recap));
  strncpy(rank,colour_com_strip(u->level_alias),6);
  if (u->afk) sprintf(idlestr,"%5.5d/%3.3d (afk)",mins,idle);
  else if (u->malloc_start!=NULL) sprintf(idlestr,"%5.5d/%3.3d (edit)",mins,idle);
  else sprintf(idlestr,"%5.5d/%3.3d mins.",mins,idle);
  ugender=0;
  if (u->prompt & 4) ugender=2;
  if (u->prompt & 8) ugender=1;
  if (!u->vis) strcat(idlestr,"I"); 
  if (u->room->hidden) strcat(idlestr,"&");
  if (u->hidden) strcat(idlestr,"#");
  sprintf(text,"%-42.42s~RS|%-1.1s~RS|%-14.14s~RS|%-6.6s~RS|%s \n",line,sex2[ugender],rname,rank,idlestr);
  write_user(user,text);
  }
 }
sprintf(text,"\nTotal of %d countrymen signed on\n\n",total-invis);
write_user(user,text);
}


/*** The Who List Re-Make Of Medival Times For Moenuts (Written by moe@moenuts.com) ***/
void medival_times_who(UR_OBJECT user)
{
UR_OBJECT u;
RM_OBJECT rm;
int cnt,total,invis,mins,idle,logins,rcnt;
char line[USER_NAME_LEN+USER_DESC_LEN*2];
char rname[ROOM_RECAP_LEN+1],idlestr[20],ulevel[10];
char text2[ARR_SIZE];

total=0;  invis=0;  logins=0;

sprintf(text,"Current Users %s",long_date(1));
sprintf(text2,"~CR| ~CW%-74.74s ~CR|\n",text);
write_user(user,"~CR+-==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==-+\n");
write_user(user,text2);
write_user(user,"~CR| ~CWName                                           : Room             : Tm/Id  ~CR|\n");
write_user(user,"~CR+-==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==-+\n\n");

for(rm=room_first;rm!=NULL;rm=rm->next) {
for(u=user_first;u!=NULL;u=u->next) {
  if (u->type==CLONE_TYPE) continue;
  if (u->login) continue;
  if (u->room->hidden && user->level<OWNER) continue;
  if (u->hidden && user->level<OWNER) continue;
  if (!u->vis && user->level<u->level) continue;
  if (!(u->room==rm)) continue;
  mins=(int)(time(0) - u->last_login)/60;
  idle=(int)(time(0) - u->last_input)/60;
  ++total;
  if (!u->vis) {
    ++invis;
    if (u->level>user->level && !(user->level>=OWNER)) continue;
    }
  sprintf(line," %s~RS %s",u->recap,u->desc);
  if (!u->vis) line[0]='I';
  if (u->room->hidden) line[0]='&';
  if (u->hidden) line[0]='#';
  strcpy(rname,u->room->recap);
  /* Count number of colour coms to be taken account of when formatting */
  cnt=colour_com_count(line);
  rcnt=colour_com_count(rname);
  if (u->afk) strcpy(idlestr,"~FRAFK~RS");
  else if (u->malloc_start!=NULL) strcpy(idlestr,"~FTEDIT~RS");
  else if (idle>=30) strcpy(idlestr,"~FYIDLE~RS");
  else sprintf(idlestr,"%d/%d",mins,idle);
  strncpy(ulevel,level_name[u->level],1);
  sprintf(text,"  %-*.*s ~FM%-1.1s ~FW: %-*.*s ~FW: %s\n",(44+cnt*3),(44+cnt*3),line,ulevel,(16+rcnt*3),(16+rcnt*3),rname,idlestr);
  write_user(user,text);
  }
 }
write_user(user,"\n~CR+-==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==-+\n");
sprintf(text,"Total of %d users currently logged on.",total-invis);
sprintf(text2,"~CR| ~CW%-74.74s ~CR|\n",text);
write_user(user,text2);
write_user(user,"~CR+-==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==-+\n\n");
}


/*** Dark Garden Wholist Moenuts By Katrina and Moe - Based On Shackles Who ***/
void dark_garden_who(UR_OBJECT user)
{
UR_OBJECT u;
int cnt,total,invis,mins,idle,logins,ugender,uplaying,rcnt;
char line[USER_NAME_LEN+USER_DESC_LEN*2];
char rname[ROOM_RECAP_LEN+1],idlestr[20],ucaste[2],ulevel[2];
char *ny2[]={ "N","Y" };
char *sex2[]={"N","M","F"};
char *center();
RM_OBJECT rm;

total=0;  invis=0;  logins=0;  ugender=0;  uplaying=0;

sprintf(text,"\n~CR@~FG}-'--,--'--,--'--,-{~CR@@~FG}-'-,--{~CR@ ~FY%-12.12s ~CR@~FG}-'-,--{~CR@@~FG}-'--,--'--,--'--,-{~CR@\n",SHORTNAME);
write_user(user,text);
sprintf(text,"~FGCurrent Users %s\n",long_date(1));
write_user(user,center(text,78));
write_user(user,"  ~CTName          Desc                   Sex/Lev : Room         : Cst/Ply/Tm/Id\n");
write_user(user,"~CR@~FG}-'--,--'--,--'--,-{~CR@@~FG}-'--,--'--,--{~CR@@~FG}--'--,--'--,-{~CR@@~FG}-'--,--'--,--'--,-{~CR@\n\n");
for(rm=room_first;rm!=NULL;rm=rm->next) {
for(u=user_first;u!=NULL;u=u->next) {
  if (u->type==CLONE_TYPE) continue;
  if (u->login) continue;
  if (u->room->hidden && user->level<OWNER) continue;
  if (u->hidden && user->level<OWNER) continue;
  if (!u->vis && user->level<u->level) continue;
  if (!(u->room==rm)) continue;
  mins=(int)(time(0) - u->last_login)/60;
  idle=(int)(time(0) - u->last_input)/60;
  ++total;
  if (!u->vis) {
    ++invis;
    if (u->level>user->level && !(user->level>=OWNER)) continue;
    }
  sprintf(line," %s~RS %s",u->recap,u->desc);
  if (!u->vis) line[0]='I';
  if (u->room->hidden) line[0]='&';
  if (u->hidden) line[0]='#';
  strcpy(rname,u->room->recap);
  /* Count number of colour coms to be taken account of when formatting */
  cnt=colour_com_count(line);
  rcnt=colour_com_count(rname);
  if (u->afk) strcpy(idlestr,"~FRAFK~RS");
  else if (u->malloc_start!=NULL) strcpy(idlestr,"~FTEDIT~RS");
  else if (idle>=30) strcpy(idlestr,"~FYIDLE~RS");
  else sprintf(idlestr,"%d/%d",mins,idle);
  ugender=0;
  if (u->prompt & 4) ugender=2;
  if (u->prompt & 8) ugender=1;
  strncpy(ucaste,bdsm_types[u->bdsm_type],1);
  strncpy(ulevel,level_name[u->level],1);
  sprintf(text," %-*.*s~RS  ~CG%-1.1s~RS/%-1.1s ~RS: %-*.*s ~RS: ~CY%-1.1s~RS/~FT%-1.1s~RS/%s\n",(40+cnt*3),(40+cnt*3),line,sex2[ugender],ulevel,(12+rcnt*3),(12+rcnt*3),rname,ucaste,ny2[uplaying],idlestr);
  write_user(user,text);
  }
 }
write_user(user,"\n~CR@~FG}-'--,--'--,--'--,-{~CR@@~FG}-'--,--'--,--{~CR@@~FG}--'--,--'--,-{~CR@@~FG}-'--,--'--,--'--,-{~CR@\n");
sprintf(text,"Total of ~CT%d~RS visible, ~CT%d~RS invisible",total-invis,invis);
write_user(user,center(text,78));
write_user(user,"\n~CR@~FG}-'--,--'--,--'--,-{~CR@@~FG}-'--,--'--,--{~CR@@~FG}--'--,--'--,-{~CR@@~FG}-'--,--'--,--'--,-{~CR@\n\n");
}


/* Strrep re-write by addIct */

char *strrep(char *line, char *find, char *replace, size_t max_len) {
size_t len=0, matches=0;
size_t line_len, find_len, replace_len;
char *o, *i;
if(line==NULL || find==NULL || replace==NULL) return line;
if((*find=='\000') || (*replace=='\000')) return line;
i=line; line_len=strlen(line); find_len=strlen(find); replace_len=strlen(replace);
while(*i!='\000') {
 if(!strncasecmp(i,find,find_len)) { matches++; i+=find_len; }
 else {i++;}
 }

i=line; if(replace_len > find_len) {
 len = replace_len - find_len; len *= matches; i += len; len += line_len;
 if(len > max_len) return line; memmove(i, line, line_len); }
for(o=line;*i!='\000';) {
 if(!strncasecmp(i,find,find_len)) {
 memcpy(o,replace,replace_len); o+=replace_len; i+=find_len;}
 else { *o=*i; o++; i++; }
 }
*o='\000';
return line;
}

char *namestring(char *sstring) {

char *strrep();
static char t[ARR_SIZE*3+2];
char name[USER_RECAP_LEN+5];
char name2[USER_NAME_LEN+5];

UR_OBJECT u;

strcpy(t,sstring);
for(u=user_first;u!=NULL;u=u->next) {
	strcpy(name,u->recap);
	strcat(name,"~RS");
	strcpy(name2,u->name);
	strcpy(t,strrep(sstring,u->name,name,ARR_SIZE*3));
	strtolower(name2);
	strcpy(t,strrep(sstring,name2,name,ARR_SIZE*3));
	strtoupper(name2);
	strcpy(t,strrep(sstring,name2,name,ARR_SIZE*3));
	}
return t;
}

void trivia_answer(UR_OBJECT user, char *inpstr)
{
UR_OBJECT u;
int cnt=0;

if (!system_trivia) {
	write_user(user,"There currently is no official trivia game in progress...\n");
	return;
	}
sprintf(text,"%s ~RSsubmits an answer for trivia.\n",user->recap);
write_room(user->room,text);

for(u=user_first;u!=NULL;u=u->next) {
	if (u->level==GAMES) {
		cnt++;
		sprintf(text,"\n~FTTrivia Answer From~RS: ~FG%s\n~FT> ~FG%s\n",user->recap,inpstr);
		write_user(u,text);
		}
	}
	if (cnt) {
		sprintf(text,"\n~FTTrivia Answer Submitted~RS: ~FG%s\n",inpstr);
		write_user(user,text);
		return;
		}
	else { 	write_user(user,"There is no game masters on to submit your answer to.\n");
		return;
		}
}

void trivia_question(UR_OBJECT user, char *inpstr)
{
RM_OBJECT rm;

if (!system_trivia) {
        write_user(user,"There currently is no official trivia game in progress...\n");
        return;
        }
if (!inpstr[0]) {
	write_user(user,"Usage: .question <trivia question>\n\n");
	return;
	}
rm=user->room;
sprintf(text,"\n~CB-----------------------------~CG[ ~CYThe Question Is: ~CG]~CB-----------------------------\n\n~CT>~RS %s \n\n~CB------------------------------------------------------------------------------\n",inpstr);
write_room(rm,text);
return;
}

void trivia_win(UR_OBJECT user)
{
UR_OBJECT u;
char gender[8];
int winnings=0,points=0;

if (!system_trivia) {
	write_user(user,"No Official Trivia Game In Progress, You cannot grant points at this time.\n");
	return;
	}
if (word_count<2) {
	write_user(user,"Usage: .twin <user> <points>\n");
	return;
	}
if (!(u=get_user(word[1]))) {
	write_user(user,notloggedon);
	return;
	}
if (user==u) {
	write_user(user,"You cannot give yourself points.\n");
	return;
	}

points=atoi(word[2]);

if (!points) { write_user(user,"No points awarded. (points=0)\n");
	return;
	}

strcpy(gender,"their");
if (u->prompt & 4) strcpy(gender,"her");
if (u->prompt & 8) strcpy(gender,"his");

if (word[3][0]=='B' || word[3][0]=='b') { sprintf(text,"~FG%s ~RS~FTwas awarded %d bonus points for %s answer.\n\n",u->recap,points,gender); }
else { sprintf(text,"~FG%s ~RS~FTwas awarded %d points for %s answer.\n\n",u->recap,points,gender); }

write_room(user->room,text);
winnings=(points*TRIVIA_POINT_AMOUNT);
u->bank_balance=u->bank_balance+winnings;
sprintf(text,"%s has been awarded $%d for %s answer.\n",u->recap,winnings,gender);
write_user(user,text);
sprintf(text,"You've been awarded $%d for your answer.\nYour Current Bank Balance Is $%d\n\n",winnings,u->bank_balance);
write_user(u,text);
return;
}

void trivia_start_stop(UR_OBJECT user)
{

if (system_trivia) write_user(user,"Trivia Is In Progress!\n");
else write_user(user,"There Is No trivia Game In Progress\n");

if (!strcasecmp(word[1],"start")) {
	write_room(NULL,"\n~CY!!! ~CTTRIVIA HAS STARTED IN THE TRIVIA ROOM  - GOOD LUCK TO EVERYONE ~CY!!!\n\n");
	system_trivia=1;
	return;
	}
if (!strcasecmp(word[1],"stop")) {
	write_room(NULL,"\n~CR!!! ~CMTRIVIA HAS ENDED - CONGRATULATIONS TO THE WINNERS ~CR!!!\n\n");
	system_trivia=0;
	return;
	}
write_user(user,"Usage: trivia start|stop\n\n");
return;
}

void trivia_banner(UR_OBJECT user)
{
if (system_trivia) {
	write_user(user,"\n~CB[~CY M~CYOENUTS ~CB]~CW: ~CGTrivia Is In Progress In The Trivia Room!\n");
	write_user(user,"~RS             ~CGEveryone Is Welcome To Join!  Type:  .go trivia  to join!\n\n");
	return;
	}
}

/*** See if user has key to enter personal room. ***/
int has_key(RM_OBJECT room, UR_OBJECT user)
{
FILE *fp;
char line[82],filename[80];

sprintf(filename,"%s/%s.key",DATAFILES,room->name);
if (!(fp=fopen(filename,"r"))) return 0;
fscanf(fp,"%s",line);
while(!feof(fp)) {
	if (line[0]!='#' && line[0]) {
		if (strstr(user->name,line)) {  fclose(fp);  return 1;  }
		if (strstr("ALL",line)) {  fclose(fp);  return 1;  }
		}
	fscanf(fp,"%s",line);
	}
fclose(fp);
return 0;
}

/* List who has keys for the current room */
void list_keys(UR_OBJECT user)
{

char filename[81];

if ((user->room->access & PERSONAL) && !strcasecmp(user->room->owner,user->name)) {
	write_user(user,"~CTUsers who have keys to this room:\n");
	write_user(user,"~CT---------------------------------\n\n");
	sprintf(filename,"%s/%s.key",DATAFILES,user->room->name);
	switch(more(user,user->socket,filename)) {
		case 0:
                write_user(user,"~FRNobody has a key for this room.\n");
		return;
		case 1: user->misc_op=2;
		}
	return;
	}
else if (user->level<OWNER) {
	write_user(user,"~CRThis isn't your room, so you cannot find out who has keys to it.\n");
	return;
	}
else {  write_user(user,"~CRThis is either not a personal room, or not your personal room.\n");
        write_user(user,"Go to your personal room if you wish to see who has keys to it.\n");
        return;
        }
}

/* Load Room Details */
int load_room_preferences(RM_OBJECT room)
{
FILE *fp;
char line[ARR_SIZE],filename[80],tempstr[6];
int temp1=0,temp2=0,temp3=0,temp4=0,temp5=0,temp6=0;

sprintf(filename,"%s/%s.pref",DATAFILES,room->name);
if (!file_exists(filename)) {
	if (!save_room_preferences(room,0)) write_syslog("|> Moenuts: save_room_preferences() - Update Failed!\n",1);
	}

else {
	sprintf(text,"Loading room preferences for room \"%s\"...\n",room->name);
	write_syslog(text,1);
	}
if (!(fp=fopen(filename,"r"))) return 0;

/* 30 integers */

fscanf(fp,"%d %d %d %d %d %d\n",&temp1,&temp2,&temp3,&temp4,&temp5,&temp6);
fscanf(fp,"%d %d %d %d %d %d\n",&temp1,&temp2,&temp3,&temp4,&temp5,&temp6);
fscanf(fp,"%d %d %d %d %d %d\n",&temp1,&temp2,&temp3,&temp4,&temp5,&temp6);
fscanf(fp,"%d %d %d %d %d %d\n",&temp1,&temp2,&temp3,&temp4,&temp5,&temp6);
fscanf(fp,"%d %d %d %d %d %d\n",&temp1,&temp2,&temp3,&temp4,&temp5,&temp6);

if (feof(fp)) {
	write_syslog("Error Loading Room Preferences, pre-mature End Of File Detected!\n",1);
	fclose(fp);
	return 0;
	}

/* And 30 Strings */

fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strncpy(room->recap,line,ROOM_RECAP_LEN);
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strncpy(room->owner2,line,USER_NAME_LEN);
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strncpy(tempstr,line,5);
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strncpy(tempstr,line,5);
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strncpy(tempstr,line,5);
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strncpy(tempstr,line,5);
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strncpy(tempstr,line,5);
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strncpy(tempstr,line,5);
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strncpy(tempstr,line,5);
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strncpy(tempstr,line,5);
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strncpy(tempstr,line,5);
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strncpy(tempstr,line,5);
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strncpy(tempstr,line,5);
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strncpy(tempstr,line,5);
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strncpy(tempstr,line,5);
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strncpy(tempstr,line,5);
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strncpy(tempstr,line,5);
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strncpy(tempstr,line,5);
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strncpy(tempstr,line,5);
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strncpy(tempstr,line,5);
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strncpy(tempstr,line,5);
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strncpy(tempstr,line,5);
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strncpy(tempstr,line,5);
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strncpy(tempstr,line,5);
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strncpy(tempstr,line,5);
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strncpy(tempstr,line,5);
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strncpy(tempstr,line,5);
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strncpy(tempstr,line,5);
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strncpy(tempstr,line,5);
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strncpy(tempstr,line,5);

fclose(fp);
return 1;
}


int save_room_preferences(RM_OBJECT room,int type)
{
FILE *fp;
char filename[80],tempstr[ARR_SIZE];
int temp, i=0;

sprintf(filename,"%s/%s.pref",DATAFILES,room->name);
if (!(fp=fopen(filename,"w"))) {
	sprintf(text,"save_room_preferences(): Failed to save %s.pref room preferences.\n",room->name);
	write_syslog(text,1);
	return 0;
	}

/* 30 integers */

temp=0;  strcpy(tempstr,"None");  /* Temporary Variable Initialization */

/* If type = 0, Create an empty preferences file */

if (!type) {
	for(i=0;i<5;i++) { fprintf(fp,"0 0 0 0 0 0\n"); }
	}
else {
	fprintf(fp,"%d %d %d %d %d %d\n",temp,temp,temp,temp,temp,temp);
	fprintf(fp,"%d %d %d %d %d %d\n",temp,temp,temp,temp,temp,temp);
	fprintf(fp,"%d %d %d %d %d %d\n",temp,temp,temp,temp,temp,temp);
	fprintf(fp,"%d %d %d %d %d %d\n",temp,temp,temp,temp,temp,temp);
	fprintf(fp,"%d %d %d %d %d %d\n",temp,temp,temp,temp,temp,temp);
	}

/* 30 String Spots */

if (type==0) {
	fprintf(fp,"%s\n",room->name);
	fprintf(fp,"%s\n",room->owner);
	for(i=0;i<28;i++) { fprintf(fp,"None\n"); }
	}
else {
	fprintf(fp,"%s\n",room->recap);
	fprintf(fp,"%s\n",room->owner2);
	fprintf(fp,"%s\n",tempstr);
	fprintf(fp,"%s\n",tempstr);
	fprintf(fp,"%s\n",tempstr);
	fprintf(fp,"%s\n",tempstr);
	fprintf(fp,"%s\n",tempstr);
	fprintf(fp,"%s\n",tempstr);
	fprintf(fp,"%s\n",tempstr);
	fprintf(fp,"%s\n",tempstr);
	fprintf(fp,"%s\n",tempstr);
	fprintf(fp,"%s\n",tempstr);
	fprintf(fp,"%s\n",tempstr);
	fprintf(fp,"%s\n",tempstr);
	fprintf(fp,"%s\n",tempstr);
	fprintf(fp,"%s\n",tempstr);
	fprintf(fp,"%s\n",tempstr);
	fprintf(fp,"%s\n",tempstr);
	fprintf(fp,"%s\n",tempstr);
	fprintf(fp,"%s\n",tempstr);
	fprintf(fp,"%s\n",tempstr);
	fprintf(fp,"%s\n",tempstr);
	fprintf(fp,"%s\n",tempstr);
	fprintf(fp,"%s\n",tempstr);
	fprintf(fp,"%s\n",tempstr);
	fprintf(fp,"%s\n",tempstr);
	fprintf(fp,"%s\n",tempstr);
	fprintf(fp,"%s\n",tempstr);
	fprintf(fp,"%s\n",tempstr);
	fprintf(fp,"%s\n",tempstr);
	}
fclose(fp);
return 1;
}

void room_recap(UR_OBJECT user, char *inpstr)
{
RM_OBJECT room;
char *remove_first();

  if (word_count<2) {
      write_user(user,"Usage  : roomcap <alias>\n");
      write_user(user,"Note   : Recap A Room Name!\n");
      return;
      }
  room=get_room(word[1]);
  if (!room) {
	write_user(user,nosuchroom);
	return;
	}
  inpstr=remove_first(inpstr);
  if (strlen(inpstr)>ROOM_RECAP_LEN) {
       write_user(user,"~CRYour room recap is too long!  ~FMRemove some colors maybe?\n");
       return;
       }
    strncpy(room->recap,inpstr,ROOM_RECAP_LEN);
    sprintf(text,"\n~CM%s ~CMhas recapped this room to %s\n",user->recap,room->recap);
    write_room(room,text);
    sprintf(text,"~CMYou have recapped the %s ~CMroom to %s\n~CMReloading Room Preferences...\n",room->name,room->recap);
    write_user(user,text);
    save_room_preferences(room,1);
    load_room_preferences(room);
    return;
}



void dictionary(UR_OBJECT user, char *inpstr)
{
char filename[81];
char tempstr[ARR_SIZE+1],line[ARR_SIZE+1];
char *remove_first();
char validchars[65]="1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
int c=0;
FILE *fp;

strncpy(tempstr,inpstr,ARR_SIZE);
word_count=wordfind(inpstr);

if (!inpstr[0]) {
	write_user(user,"Usage: dictionary <word>\n");
	write_user(user,"Where: <word> is the word you wish the meaning for.\n");
	return;
	}
for(c=0;c<strlen(tempstr);c++) {
	if (!rindex(validchars,tempstr[c])) {
                sprintf(text,"~CRThat's not a word, A word cannot contain special characters.  Letters Only.\n");
                write_user(user,text);
                sprintf(text,"%s tried to to use %s to hack the system in dictionary().\n",user->name,tempstr);
		write_syslog(text,1);
		return;
		}
	}

sprintf(text,"~CGGathering dictionary entry for ~FT\"%s\"\n",inpstr);
write_user(user,text);
switch(double_fork()) {
  case -1 : write_user(user,"Moenuts: dictionary lookup : fork failure\n");
	    write_syslog("Moenuts: dictionary lookup: fork failure\n",1);
	    return;
  case  0 : sprintf(text,"%s requested the meaning for the word \"%s\"\n",user->name,tempstr);
	    write_syslog(text,1);
	    sprintf(filename,"%s/output.%s",TEMPFILES,user->name);
	    unlink(filename);
	    sprintf(text,"%s \"%s\" > %s",DICTIONARY_SCRIPT,inpstr,filename);
            system(text);
	    if (!(fp=fopen(filename,"r"))) {
		write_user(user,"Could not open temporary file for reading in dictionary();\n");
		return;
		}
	    fgets(line,ARR_SIZE,fp);
	    while(!feof(fp)) {
	    	line[strlen(line)-1]=0;
		strcat(line,"\n");
        	write_user(user,line);
        	fgets(line,ARR_SIZE,fp);
        	}
 	    fclose(fp);
            _exit(1);
            break;
            }
}

/* Steal from a user when they are idling by Michael Doig */
void steal_user_money(UR_OBJECT user)
{
UR_OBJECT u;
/* RM_OBJECT rm; */
int temp,user_money_stolen,idle;

/* rm=get_room(default_idle); */
if (word_count<2) {
   write_user(user,"Usage: steal <user>\n");
   return;
   }
if (!(u=get_user(word[1]))) {
   write_user(user,notloggedon);
   return;
   }
if (u->room!=user->room) {
   sprintf(text,"%s ~RSis not in the same room as you.\n",u->recap);
   write_user(user,text);
   return;
   }
if (u->afk) {
   write_user(user,"You can't steal from someone AFK.\n");
   return;
   }
if (!user->vis) {
   write_user(user,"Don't be a cheap ass! Become visable first. Maybe they're idle cause you're invis!\n");
   return;
   }
   /*  Moenuts doesn't really have an idle room as of yet... Maybe someday :)
if (u->room==rm) {
   sprintf(text,"You can't steal from someone in the %s~RS room!\n",rm->recap);
   write_user(user,text);
   return;
   }
   */
idle=(int)(time(0) - u->last_input)/60;
if (idle<=10) {
   sprintf(text,"%s ~RShasn't been idling long enough.\n",u->recap);
   write_user(user,text);
   return;
   }
if (u->bank_balance<1) {
   write_user(user,"You can't steal from someone with no money.\n");
   return;
   }
if ((int)u->stealtime + 5*60 > (int)time(0)) {
   sprintf(text,"%s~RS has been stolen from recently!\n",u->recap);
   write_user(user,text);
   return;
   }
if ((int)u->attempttime + 3*60 > (int)time(0)) {
   sprintf(text,"%s~RS has had an attempt at their wallet recently!\n",u->recap);
   write_user(user,text);
   return;
   }
srand(time(0));
temp=rand()%75;
if (temp<45) {
/*   if (!strcasecmp(user->name,"NOFX")) {
	user_money_stolen=1;
	}
   else { */
	user_money_stolen=rand()%200;
/*	} */
   if (user_money_stolen > u->bank_balance) user_money_stolen = u->bank_balance;
      u->bank_balance-=user_money_stolen;
      user->bank_balance+=user_money_stolen;
      u->stealtime = time(0);
      sprintf(text,"%s ~RSjust stole $%d from you. Don't idle!\n",user->recap,user_money_stolen);
      write_user(u,text);
      sprintf(text,"%s ~RSnotices %s ~RSidling, and steals $%d from them.\n",user->recap,u->recap,user_money_stolen);
      write_room_except(u->room,text,u);
      return;
      }
   else {
      u->attempttime = time(0);
      sprintf(text,"You fail to steal any money from %s\n",u->recap);
      write_user(user,text);
      sprintf(text,"%s~RS just tried to steal money from you, but failed!\n",user->recap);
      write_user(u,text);
      return;
      }
}

/* Adapted from TalkerOS to Amnuts 221 by Michael Doig,
   To Moenuts by Michael Irving. */
void spin(UR_OBJECT user)
{
  UR_OBJECT u;
  RM_OBJECT rm;
  int users, choice, i;

  rm = user->room;

  users = 0;
  u = user_first;
  while (u != NULL) {
   if ((u->room == rm) && (u->vis))
     users++;
   u = u->next;
  }
  if (users<=1) {
    write_user(user,"This command only works if there are at least 2 people in the room!\n");
    return;
  }
  u = user;
  while (u == user) {
    choice = 1 + (int) (1.0 * users * rand() / (RAND_MAX + 1.0));

    i = 0;
    u = user_first;
    while (i < choice) {
     if ((u->room == rm) && (u->vis))
       i++;
     if (i < choice)
       u = u->next;
    }
  }
  sprintf(text,"~CG%s ~CGspins the bottle and the bottle points to ~CY... %s\n",user->recap,u->recap);
  write_room(user->room,text);
}

/* Adapted from TalkerOS to Amnuts221 by Michael Doig,
   To Moenuts by Michael Irving. */

void flip_coin(UR_OBJECT user)
{
  int coin;

  coin = 1 + (int) (2.0 * rand() / (RAND_MAX + 1.0));
  if (coin == 1) sprintf(text,"%s ~CYflips a coin ... the coin shows ~CTheads\n",user->recap);
  else sprintf(text,"%s ~CYflips a coin ... the coin shows ~CTtails\n",user->recap);
  write_room(user->room,text);
}

/* Adapted from TalkerOS to Amnuts221 by Michael Doig,
   To Moenuts by Michael Irving. */

void roll_dice(UR_OBJECT user)
{
  int dice,j;

  dice = 1 + (int) (6.0 * rand() / (RAND_MAX + 1.0));
  sprintf(text,"%s ~CYthrows the dice and rolls a ~CT%d\n\n",user->recap,dice);
  write_room(user->room,text);
  for(j=0;j<DICE_HEIGHT;++j) {
  if (user->high_ascii) sprintf(text,"          %s \n",ansidice[dice-1][j]);
       else sprintf(text,"          %s \n",asciidice[dice-1][j]);
       write_room(user->room,text);
       }

}

/* Award User Money Based On Time Online.  Everytime the events are processed,
   CREDIT_RATE will be added to the user's bank_balance */

void credit_user_time(void)
{

UR_OBJECT u;
int idle=0, mins=0,credits=0;

credit_timer+=heartbeat;
if (credit_timer<60) return;
credit_timer=0;

for(u=user_first;u!=NULL;u=u->next) {
	if (u->login) continue;
	if (u->type==CLONE_TYPE) continue;
	idle=0;
	idle=(int)(time(0) - u->last_input)/60;
	mins=(int)(time(0) - u->last_login)/60;
	credits=CREDIT_RATE;
	if (idle>MAX_IDLE_TIME) {
#ifdef DEBUG
	//	sprintf(text,"[DEBUG]: %s is idle for more than %d minutes. (Idle: %d), No credits received.\n",u->name,MAX_IDLE_TIME,idle);
	//	write_room(NULL,text);
#endif
		continue;
		}
	if (u->afk) {

#ifdef DEBUG
	//	sprintf(text,"[DEBUG]: %s is AFK and does not receive any credits.\n",u->name);
	//	write_room(NULL,text);
#endif
		continue;
		}
	u->bank_balance+=credits;
#ifdef DEBUG
	// sprintf(text,"[DEBUG]: %s receives %d credits, their bank balance is %d  [%d Mins]\n",u->name,credits,u->bank_balance,mins);
	// write_room(NULL,text);
#endif
	}
	return;
}

/* Fight another user */
void fight_another(UR_OBJECT user,char *inpstr)
{
char other_user[ARR_SIZE];
UR_OBJECT user2;
int mode;

user2=NULL;

if (!strlen(inpstr)) {
        write_user(user,"Fight status:\n");
if (fight.issued) {
        sprintf(text,"Aggressor:   %s\n",fight.first_user->name);
        write_user(user,text);
        sprintf(text,"Defender:    %s\n",fight.second_user->name);
        write_user(user,text);
        }
     else
        write_user(user,"   No current fight is challenged.\n");
        return;
        }
mode = 0;
if (!strcmp(inpstr,"reset")) {
        reset_chal(user, inpstr);
        return;
        }

if (!strcmp(inpstr,"1"))   mode = 2;
if (!strcmp(inpstr,"yes")) mode = 2;
if (!strcmp(inpstr,"0"))   mode = 1;
if (!strcmp(inpstr,"no"))  mode = 1;

	if (!mode) {
        sscanf(inpstr, "%s", other_user);
        if(!(user2 = get_user(other_user))) {
	write_user(user,notloggedon);  return;
        }
if (user2->level > WIZ)
if (!(user2->vis) && (user->level < OWNER))
        {
        write_user(user, notloggedon);
        return;
        }
if (user2->afk) {
        sprintf(text,"%s is afk at the moment.\n",user2->name);
        write_user(user,text);
        return;
        }
/*if (user->room==room_first) {
	write_user(user,"No fighting in this room !!!\nYou can fight in any room except this one !!\n");
	return;
        }*/
if (user->room!=user2->room) {
	write_user(user,"You cannot fight someone not in your room.\n");
        sprintf(text,"~CM%s~FG wants to fight you in room~FM %s~FW !!\n",user->name,user->room->name);
	write_user(user2,text);
	return;
        }
	mode = 3;
        }

if (fight.issued && mode == 3) {
        write_user(user, "Sorry, you must wait until the others are done.\n");
        return;
        }
if (!fight.issued && (mode == 1 || mode == 2) ) {
        write_user(user, "You are not being challenged to a fight at this time.\n");
        return;
        }
if ((mode == 1 || mode == 2) && fight.second_user != user) {
        write_user(user, "~FYYou are not the challenged user...type~CW .fight~RS~FY to see\n");
        return;
        }
if (mode == 3) {
if (user == user2) {
        write_user(user,"Which personality is it today? (Fighting yourself...tsk tsk tsk)\n");
        return;
        }
/* check for standard fight room */
issue_chal(user, user2);
return;
}
if (mode == 1) {
        sprintf(text,wimp_text[ rand() % num_wimp_text ], user->name);
        write_room(user->room,text);
        reset_chal(user, inpstr);
        return;
        }
if (mode == 2) {
        accept_chal(user);
        reset_chal(user,inpstr);
        return;
        }
}

/* get a random number based on rank */
int get_odds_value(UR_OBJECT user)
{
/* return( (rand() % odds[user->level]) + 1 ); */
return( (rand() % odds[2]) + 1 );
}

/* determine the result of a random event between two users */
int determ_rand(UR_OBJECT u1, UR_OBJECT u2)
{
int v1, v2, v3, result;
float f_fact;

v1 = get_odds_value(u1);
v2 = get_odds_value(u2);

if (v1 == v2) {  /* truely amazing, a real tie */
        return(TIE);
        }
if (v1 > v2) {
        result = 1;
        f_fact = (float)((float)v2/(float)v1);
        }
   else {
        result = 2;
        f_fact = (float)((float)v1/(float)v2);
        }
v3 = (int) (f_fact * 100.0);
if (v3 > CLOSE_NUMBER) {
        if (  rand() % 2 )
        return(TIE);
        else
        return(BOTH_LOSE);
        }
return(result);
}

/* Issue a fight challenge to another user */
void issue_chal(UR_OBJECT user,UR_OBJECT user2)
{
fight.first_user  = user;
fight.second_user = user2;
fight.issued      = 1;
fight.time        = time(0);
        sprintf(text, chal_text[ rand() % num_chal_text ],user->name,user2->name);
        write_room(user->room,text);
        write_user(user2,CHAL_LINE);
        write_user(user2,CHAL_LINE2);
        write_user(user,CHAL_ISSUED);
        }

/* Accept a fight challenge */
void accept_chal(UR_OBJECT user)
{
char *aname,*bname;
int x;
UR_OBJECT a,b;
        a=fight.first_user;
        b=fight.second_user;
        x = determ_rand(a, b);
if (x == TIE) {
        sprintf(text, tie1_text[ rand() % num_tie1_text ],a->name,b->name);
        a->fight_draw++; b->fight_draw++;
        write_room(a->room,text);
        return;
        }
if (x == BOTH_LOSE) {
        sprintf(text, tie2_text[ rand() % num_tie2_text ],a->name,b->name);
        a->fight_lose++; b->fight_lose++;
        write_room(a->room,text);
        disconnect_user(a);
        disconnect_user(b);
        sprintf(text,"~CGTsk,tsk, violence will get you no where!!\n");
        write_room(a->room,text);
        return;
        }
if (x == 1) {
        sprintf(text, wins1_text[ rand() % num_wins1_text ],a->name,b->name);
        a->fight_win++; b->fight_lose++;
        write_room(a->room,text);
	aname=a->recap; bname=b->recap;
        disconnect_user(b);
        sprintf(text,"~CGThat'll teach ~FM%s ~FGto mess with ~FM%s~FG!!\n",bname,aname);
        write_room(a->room,text);
        return;
        }
if (x == 2) {
        sprintf(text, wins2_text[ rand() % num_wins2_text ],b->name,a->name);
        b->fight_win++; a->fight_lose++;
        write_room(b->room,text);
	aname=a->recap; bname=b->recap;
        disconnect_user(a);
        sprintf(text,"~CGThat'll teach %s to mess with ~FM%s~FG!!\n",aname,bname);
        write_room(b->room,text);
        return;
        }
}

/* Reset the fight */
void reset_chal(UR_OBJECT user,char *inpstr)
{
        fight.first_user = NULL;
        fight.second_user = NULL;
        fight.issued = 0;
        fight.time = 0;
        }

/* Clears a users fight record */
void clear_fight_record(UR_OBJECT user)
{
UR_OBJECT u;

if (word_count<2) {
        write_user(user,"Usage: .clrfight <user>\n");  return;
        }
if (!(u=get_user(word[1]))) {
        write_user(user,notloggedon);  return;
        }
u->fight_win=0;
u->fight_lose=0;
u->fight_draw=0;
}

/* Russian Roulette */
void russian_roulette(UR_OBJECT user)
{
int shot,chance;
char *name;
shot=0;  chance=0;
shot=rand()%7;
shot++;

name=user->recap;
if (user->vis || !user->hidden) name=user->recap; else name=invisname;
sprintf(text,"~CT%s decides to play russian roulette and puts the gun to their head.\n",name);
write_room(user->room,text);
sprintf(text,"~CR%s pulls the trigger and........",name);
write_room(user->room,text);
chance=rand()%5;
chance++;
if (chance==shot) {
     write_room(user->room,"~CRBANG!!\n");
     disconnect_user(user);
     sprintf(text,"~CMOH MY GOD! ~CR%s ~CWblew a hole in their head and brain chunks flew everywhere!  ~CWEWW~CY!!\n",name);
     write_room(user->room,text);
     return;
     }
else {
     write_room(user->room,"~CTC~CRL~CMI~CGC~CYK~CB!~CM!~CG!\n");
     }
}

/* Lets you start paintballing another user! */
void paintball(UR_OBJECT user, char *inpstr)
{
UR_OBJECT user2;
int success,usrcol=0;
char tempname[USER_NAME_LEN+5];

if (!strlen(inpstr)) {
  write_user(user, "Usage: .paintball <user>\n");
  return;
  }
if (!(user2=get_user(word[1]))) {
	write_user(user,notloggedon);  return;
	}
if (user2->level > WIZ)
if (!(user2->vis) && (user->level < OWNER))
        {
        write_user(user, notloggedon);
        return;
        }
if (!user->vis || !user2->vis) {
       write_user(user,"Are you BLIND?!?! YOU CAN NOT SHOOT SOMETHING YOU CAN NOT SEE!!!!!.\n");
       return;
       }

if (user2->afk) {
        sprintf(text,"~CY%s~CT is AFK at the moment.\n",user2->name);
	write_user(user,text);
        }
if (user==user2) {
  write_user(user,"~CROnly crack heads are allowed to pop caps in themselves!\n");
  return;
  }

/* Can't have people starting paintball wars in the main room! */
if (user->room==room_first) {
        write_user(user,"~CWNo trying to shoot people here!!!\n");
        write_user(user,"~CWYou can shoot people in any room except this one!!\n");
	return;
        }

if (user2->room != user->room) {
        sprintf(text,"~CW%s~CM is not here to paintball with you!\n",user2->recap);
        write_user(user,text);
        return;
        }
if (user->paintballs==0) {
        write_user(user,"~CGYou are out of ammo, slap a new clip in~CW!!\n");
        sprintf(text,"~CYYou hear the click of an empty gun as you see~CG %s~CY starting to reload.\n",user->recap);
        write_room_except(user->room,text,user);
        return;
        }
success = (((unsigned short) rand()) > 30000); /* should be # between 0-65535 */
if (!success) {  /* beep here */
        sprintf(text,"~CWYou take a shot at~CM %s~CW but miss by a mile!!\n", user2->recap);
        write_user(user,text);
        sprintf(text,"~CWEagle Eye ~CG%s~CW takes a shot at~CG %s~CW, but totally misses.\n", user->recap,user2->recap);
        write_room_except(user->room,text,user);
        }
   else {
        user2->hps--;
        if (user2->hps == 0) {
                user->hps=10;      /* full health to survivor */
                user->splatters++;
                sprintf(text,"~CGYou shoot~CW %s ~CGwithout mercy and your health is ResToReD.\n", user2->recap);
                write_user(user,text);
                sprintf(text,"~CW%s ~CRS~CMP~CTL~CGA~CYT~CRT~CME~CTR~CGS ~CW%s ~CMshowing no mercy!\n",user->recap,user2->recap);
                write_room_except(user->room,text,user);
                write_user(user2,"~CGYou got splattered by paintballs!!~CR PAY BACK TIME!!!\n");
                user2->painted++;
                save_user_details(user2,1);
                disconnect_user(user2);
                }
           else {
		/* 25-32 */
		usrcol=intrand(7);
		sprintf(tempname,"~%s%s",colcom[usrcol+25],colour_com_strip(user2->recap));
		strncpy(user2->recap,tempname,USER_RECAP_LEN);
                sprintf(text,"~FGYou ~CMp~CTA~CGi~CYN~CRt ~FY%s\n",user2->recap);
                write_user(user,text);
                sprintf(text,"~FG%s~FY whistles like an idiot, then.. ~FTBANG! ~FG%s~FY gets ~CMp~CTA~CGi~CYN~CRt~CME~CTd~CW!!\n",user->recap,user2->recap);
                write_room_except(user->room,text,user);
                if (user2->hps == 2) {
                        write_user(user2,"~CWBetter hope that you don't get hit twice more!!\n");
                        sprintf(text,"~CG%s~CT is almost dead, HURRY~CW!!!\n",user2->recap);
                        write_room_except(user2->room,text,user2);
                        }
                }
        }
user->paintballs--;
return;
}

/* Lets a user reload their paintball gun */
void paintball_reload(UR_OBJECT user)
{

char usergender[4];
strcpy(usergender,"its");
if (user->prompt & 4) strcpy(usergender,"her");
if (user->prompt & 8) strcpy(usergender,"his");

if (user->room==room_first) {
        write_user(user,"~CWCan't reload in the main room, sorry!\n");
	return;
        }
if (user->paintballs > 0) {
        sprintf(text,"~CTYou have~CW %d ~CTpaintballs left before you can reload.\n",user->paintballs);
        write_user(user,text);
        }
   else {
        user->paintballs = 6;
        write_user(user,"~FWYou put 6 more paintballs in your gun\n");
        sprintf(text,"~CTYou hear~CG %s~CT reloading %s gun!!\n",user->name,usergender);
        write_room_except(user->room,text,user);
        }
return;
}

/* Ask the Magic Eight ball a question!! */
void eight_ball(UR_OBJECT user, char* inpstr)
{
int i;

if (!strlen(inpstr)) {
        write_user(user,"~CREightball ~CGsays~CW: ~CTPlease ask a question.\n");
        return;
        }

/* Checks to see if what the user asks the eight ball is in the form of a question! */

if (!strstr(inpstr," ") || !strstr(inpstr,"?")) {
        write_user(user,"~CREightball ~CGsays: ~CTTry asking a QUESTION! You know, end with a '?'\n");
        return;
        }
i = rand() % NUM_BALL_LINES;
inpstr[0]=toupper(inpstr[0]);
sprintf(text,"~CR%s ~CRasks the magic eightball~FW:~RS %s\n",user->recap,inpstr);
write_room(user->room,text);
sprintf(text,"~CTT~CGh~CYE ~CTE~CGi~CYG~CTh~CGT~CY-~CTB~CGa~CYL~CTL ~CGs~CYp~CTE~CGa~CYk~CTS ~CGa~CYn~CTD ~CGs~CYa~CTY~CGs~CW: ~RS%s\n\r",ball_text[i]);
write_room(user->room,text);
}

/*** Say user speech ***/
void mutter(UR_OBJECT user,char *inpstr)
{
RM_OBJECT rm;
UR_OBJECT u;
char *name, text2[(ARR_SIZE*2)+1];
char *remove_first();

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
if (user->muzzled & 1) {
	write_user(user,"You are muzzled, you cannot speak.\n");
	return;
	}
if (word_count<2 && user->command_mode) {
     write_user(user,"Usage: mutter <message>\n");
     return;
     }
if (word_count<2) {
        write_user(user,"Usage: .mutter <user>\n");  return;
        }
if (!(u=get_user(word[1]))) {
        write_user(user,notloggedon);  return;
        }
inpstr=remove_first(inpstr);
if (user->type==CLONE_TYPE) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_room_except(user->room,text,u);
        record_mutter(text);
	return;
	}
rm=user->room;
if (rm->access==FIXED_PUBLIC && ban_swearing && contains_swearing(inpstr)) {
	write_user(user,noswearing);
	return;
	}
if (user->muzzled & FROZEN) {
	if (user->vis) name=user->recap; else name=invisname;
        if (user->hidden) name=hiddenname;
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	sprintf(text2,"~FG[~FTFROZEN~FG]~CT: %s",text);
	record_mutter(text2);
	return;
	}
if (user->vis) name=user->recap; else name=invisname;
if (user->hidden) name=hiddenname;
if (user==u) {
        sprintf(text,"%s ~FTmutters to everyone~FW:~RS %s\n",name,inpstr);
	write_room(user->room,text);
	}
else {
	sprintf(text,"%s ~FTmutters to everyone except ~CR%s~RS: %s\n",name,u->recap,inpstr);
	write_room_except(user->room,text,u);
	}
record_mutter(text);
}

/*** Records mutters (so that they can be .rev'd by high staff) ***/
void record_mutter(char *str)
{
     strncpy(mutterrevbuff[mutrevline],str,REVIEW_LEN);
     mutterrevbuff[mutrevline][REVIEW_LEN]='\n';
     mutterrevbuff[mutrevline][REVIEW_LEN+1]='\0';
     mutrevline=(mutrevline+1)%REVIEW_LINES;
}

/*** Clear the mutter review buffer in the room ***/
void clear_mutterrevbuff(void)
{
int c;

for(c=0;c<REVIEW_LINES;++c) mutterrevbuff[c][0]='\0';
mutrevline=0;
}

void mutrevclr(UR_OBJECT user)
{
char *name;

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
        write_user(user,text);
        return;
        }
clear_mutterrevbuff();
if (user->hidden) return;
if (user->vis) name=user->recap; else name=invisname;
sprintf(text,"%s clears the Mutter Review Buffer\n",name);
write_level(GOD,3,text,NULL);
}

/*** Show recorded shouts and shout emotes ***/
void revmutter(UR_OBJECT user)
{
int i,cnt,line;
char *center();

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text);
	return;
	}
cnt=0;
for(i=0;i<REVIEW_LINES;++i) {
	line=(mutrevline+i)%REVIEW_LINES;
	if (mutterrevbuff[line][0]) {
		cnt++;
          if (cnt==1) {
		write_user(user,"\n");
          sprintf(text,"Mutters Made Recently\n");
		write_user(user,center(text,78));
                if (user->high_ascii) write_user(user,"\n~CMƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
                else write_user(user,"\n~CM------------------------------------------------------------------------------\n");
               }
          write_user(user,mutterrevbuff[line]);
          }
	}
if (!cnt) {
     write_user(user,"~CRThere is no mutters to review.\n");
	return;
	}
else {
     if (user->high_ascii) write_user(user,"~CMƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
     else write_user(user,"~CM------------------------------------------------------------------------------\n");
     }
}

void text_color(UR_OBJECT user)
{
int i=0;

if (word_count<2) {
	write_user(user,"\nUsage: textcolor # \n\nValid Colors Are:\n~FT-----------------\n");
	/* List Available Colors */
	for(i=0;i<MAX_TEXTCOLOR;i++) {
	  sprintf(text,"~CT%2d ~CW- %s%s\n",i,textcolor[i],textcolorname[i]);
	  write_user(user,text);
	  }
	  write_user(user,"\n");
	  return;
	}
if (atoi(word[1])<0 || atoi(word[1])>MAX_TEXTCOLOR-1) {
	write_user(user,"~CRThat's an invalid color number!\n\n");
	for(i=0;i<MAX_TEXTCOLOR;i++) { sprintf(text,"~CT%2d ~CW- %s%s\n",i,textcolor[i],textcolorname[i]); write_user(user,text); }
	write_user(user,"\n");
	return;
	}
user->textcolor=atoi(word[1]);
sprintf(text,"~CTYou set your text color to~CW: ~CG[~CY%d~CG] ~CM- %s\n\n",user->textcolor,textcolorname[user->textcolor]);
write_user(user,text);
return;
}
