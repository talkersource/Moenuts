/*###########################################################################
#                                                                           #
#      _.-._ Moenuts v1.61e (C)1997, 2001 Michael Irving, AKA Moe _.-._     #
#                   (C)1997 - 2001 Moesoft Developments                     #
#               Based on NUTS v3.3.3 (C)1996 Neil Robertson                 #
#                                                                           #
#############################################################################
#                                                                           #
#  I'd like to thank the following people for giving me a hand in the       #
#  creation of this code.  Reddawg, Thanks for the base code and the        #
#  help you've given me.  Andy, Thanks for adding the Samesite and          #
#  a couple other commands.  Squirt, thanks for supplying me with code      #
#  snipets that got lost, like the game modules, and Nerfingdummy for       #
#  orinally helping me add Poker and Tic Tac Toe, and for answering         #
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
#include "moenuts161e.h"                /* Main Header File For Moenuts    */
#include "prompts.h"                    /* Moenuts Prompt File             */
#include "games.c"                      /* Include Game Library            */
#include "protos.h"                     /* ANSI C Protoypes For Moenuts    */
#include "afk.h"                        /* AFK Message Library             */
#include "hermanquotes.h"               /* Quote Of The Day Library        */
#include "moesofthtml.c"                /* Moesoft's Auto-HTML Functions   */
#include "figlet.c"                     /* Add Figlets To Moenuts ;)       */
#include "figlet.protos.h"              /* Figlet prototypes               */
/***************************************************************************/
/*                Please Do Not Change The Following                       */
/***************************************************************************/
#define VERSION "3.3.3"                 /* NUTS Base Version               */
#define MNVERSION "1.61e"               /* Moenuts Version                 */
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

printf("\033[0m\n%s\n Moenuts version %s Telnet Chat Server Is Now Booting...\n",HORIZ,MNVERSION);
printf(" Based on NUTS %s (C)1996 by Neil Robertson.\n%s\n",VERSION,HORIZ);
init_globals();
sprintf(text,">> Moenuts v%s Server Booting...\n",MNVERSION);
write_syslog(text,0);
set_date_time();
init_signals();
load_and_parse_config();
if (!possibly_reboot()) init_sockets();
load_hist();
printf(">> Checking Message Boards...\n");
check_messages(NULL,1);
printf(">> Counting Current Userbase...\n");
count_users();

/* Run in background automatically. */
#ifndef DEBUG
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
sprintf(text,">> Moenuts %s successfully booted with PID %d on %s \n\n",MNVERSION,pid,long_date(1));
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
		/* If remote user or clone ignore */
		if (user->type!=USER_TYPE) {  user=next;  continue; }
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
                    sprintf(text,"\07~OL-> ~FMYou Have New Tells!  ~RStype ~FM.revtell ~RSto view them.\n");
				write_user(user,text);
				user->chkrev=0;
				}  
               sprintf(text,"%s comes back from being AFK.\n",user->name);
			write_syslog(text,1);
			if (user->vis) {
				sprintf(afktext,bafk_msg[rand()%MAX_BAFK],user->recap);
				sprintf(text,BAFKSTRING,afktext);
				write_room(user->room,text);
				}
			if (user->afk==2) {
				user->afk=0;
				prompt(user);
				user=next;
				continue;
				}
			user->afk=0;
			}
		if (!word_count) {
               if (misc_ops(user,inpstr))  {  user=next;  continue;  }
               if (user->command_mode) prompt(user);
               user=next;  continue;                        
			}
		if (misc_ops(user,inpstr))  {  user=next;  continue;  }
		com_num=-1;
		whitespace(inpstr);
          if (user->command_mode || strchr(".,;/!<>-'#[]@*",inpstr[0])) 
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
     sprintf(text,"~FB-~OL=~FT] ~FMConnection From~FW: ~FY%s ~FT[~FB=~RS~FB-\n",site);
     write_level(ARCH,3,text,NULL);
     }
if (site_banned(site)) {
        write_sock(accept_sock,"\n\rLogins were banned from your site or domain thanks to others!\n\n\r");
	close(accept_sock);
	sprintf(text,"~OL~FRAttempted login from banned site:~RS~OL %s ~FRwas disconected\n",site);
        write_level(WIZ,3,text,NULL);
	sprintf(text,"Attempted login from banned site %s.\n",site);
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
printf(">> Parsing config file \"%s\"...\n",filename);
if (!(fp=fopen(filename,"r"))) {
        perror(">> NUTS: Can't open config file");  boot_exit(1);
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
                             fprintf(stderr,">> NUTS: Unknown section header on line %d.\n",config_line);
                             fclose(fp);  boot_exit(1);
                             }
		}
	switch(section_in) {
		case 1: parse_init_section();  got_init=1;  break;
		case 2: parse_rooms_section(); got_rooms=1; break;
		default:
               fprintf(stderr,">> NUTS: Section header expected on line %d.\n",config_line);
			boot_exit(1);
		}
	fgets(line,81,fp);
	}
fclose(fp);

/* See if required sections were present (SITES is optional) and if
   required parameters were set. */
if (!got_init) {
     fprintf(stderr,">> NUTS: INIT section missing from config file.\n");
	boot_exit(1);
	}
if (!got_rooms) {
     fprintf(stderr,">> NUTS: ROOMS section missing from config file.\n");
	boot_exit(1);
	}
if (!port[0]) {
     fprintf(stderr,">> NUTS: Main port number not set in config file.\n");
	boot_exit(1);
	}
if (!port[1]) {
     fprintf(stderr,">> NUTS: Wiz port number not set in config file.\n");
	boot_exit(1);
	}
if (port[0]==port[1]) {
     fprintf(stderr,">> NUTS: Port numbers must be unique.\n");
	boot_exit(1);
	}
if (room_first==NULL) {
     fprintf(stderr,">> NUTS: No rooms configured in config file.\n");
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
                fprintf(stderr,">> NUTS: Room %s has undefined link label '%s'.\n",rm1->name,rm1->link_label[i]);
			boot_exit(1);
			}
		}
        if (!(load_atmospheres(rm1))) atmos_assign(rm1);
	}
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
          fprintf(stderr,">> NUTS: Unexpected INIT section header on line %d.\n",config_line);
		boot_exit(1);
		}
	return;
	}
op=0;
while(strcmp(options[op],wrd[0])) {
	if (options[op][0]=='*') {
          fprintf(stderr,">> NUTS: Unknown INIT option on line %d.\n",config_line);
		boot_exit(1);
		}
	++op;
	}
if (!wrd[1][0]) {
     fprintf(stderr,">> NUTS: Required parameter missing on line %d.\n",config_line);
	boot_exit(1);
	}
if (wrd[2][0] && wrd[2][0]!='#') {
     fprintf(stderr,">> NUTS: Unexpected word following init parameter on line %d.\n",config_line);
	boot_exit(1);
	}
val=atoi(wrd[1]);
switch(op) {
	case 0: /* main port */
	case 1:
        /* case 2:  ? Netlink Port ? */
	if ((port[op]=val)<1 || val>65535) {
                fprintf(stderr,">> NUTS: Illegal port number on line %d.\n",config_line);
		boot_exit(1);
		}
	return;

	case 3:  
	if ((system_logging=onoff_check(wrd[1]))==-1) {
          fprintf(stderr,">> NUTS: System_logging must be ON or OFF on line %d.\n",config_line);
		boot_exit(1);
		}
	return;

	case 4:
	if ((minlogin_level=get_level(wrd[1]))==-1) {
		if (strcmp(wrd[1],"NONE")) {
               fprintf(stderr,">> NUTS: Unknown level specifier for minlogin_level on line %d.\n",config_line);
			boot_exit(1);	
			}
		minlogin_level=-1;
		}
	return;

	case 5:  /* message lifetime */
	if ((mesg_life=val)<1) {
          fprintf(stderr,">> NUTS: Illegal message lifetime on line %d.\n",config_line);
		boot_exit(1);
		}
	return;

	case 6: /* wizport_level */
	if ((wizport_level=get_level(wrd[1]))==-1) {
          fprintf(stderr,">> NUTS: Unknown level specifier for wizport_level on line %d.\n",config_line);
		boot_exit(1);	
		}
	return;

	case 7: /* prompt defaults */
	if ((prompt_def=onoff_check(wrd[1]))==-1) {
          fprintf(stderr,">> NUTS: Prompt_def must be ON or OFF on line %d.\n",config_line);
		boot_exit(1);
		}
	return;

	case 8: /* gatecrash level */
	if ((gatecrash_level=get_level(wrd[1]))==-1) {
          fprintf(stderr,">> NUTS: Unknown level specifier for gatecrash_level on line %d.\n",config_line);
		boot_exit(1);	
		}
	return;

	case 9:
	if (val<1) {
          fprintf(stderr,">> NUTS: Number too low for min_private_users on line %d.\n",config_line);
		boot_exit(1);
		}
	min_private_users=val;
	return;

	case 10:
	if ((ignore_mp_level=get_level(wrd[1]))==-1) {
          fprintf(stderr,">> NUTS: Unknown level specifier for ignore_mp_level on line %d.\n",config_line);
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
          fprintf(stderr,">> NUTS: Invalid message check time on line %d.\n",config_line);
		boot_exit(1);
		}
	sscanf(wrd[1],"%d:%d",&mesg_check_hour,&mesg_check_min);
	if (mesg_check_hour>23 || mesg_check_min>59) {
          fprintf(stderr,">> NUTS: Invalid message check time on line %d.\n",config_line);
		boot_exit(1);	
		}
	return;

        case 12:
	if ((max_users=val)<1) {
          fprintf(stderr,">> NUTS: Invalid value for max_users on line %d.\n",config_line);
		boot_exit(1);
		}
	return;

        case 13:
	if ((heartbeat=val)<1) {
          fprintf(stderr,">> NUTS: Invalid value for heartbeat on line %d.\n",config_line);
		boot_exit(1);
		}
	return;

        case 14:
	if ((login_idle_time=val)<10) {
          fprintf(stderr,">> NUTS: Invalid value for login_idle_time on line %d.\n",config_line);
		boot_exit(1);
		}
	return;

        case 15:
	if ((user_idle_time=val)<10) {
          fprintf(stderr,">> NUTS: Invalid value for user_idle_time on line %d.\n",config_line);
		boot_exit(1);
		}
	return;

        case 16: 
	if ((password_echo=yn_check(wrd[1]))==-1) {
          fprintf(stderr,">> NUTS: Password_echo must be YES or NO on line %d.\n",config_line);
		boot_exit(1);
		}
	return;

        case 17: 
	if ((ignore_sigterm=yn_check(wrd[1]))==-1) {
          fprintf(stderr,">> NUTS: Ignore_sigterm must be YES or NO on line %d.\n",config_line);
		boot_exit(1);
		}
	return;

        case 18:
	if ((max_clones=val)<0) {
          fprintf(stderr,">> NUTS: Invalid value for max_clones on line %d.\n",config_line);
		boot_exit(1);
		}
	return;

        case 19:
	if ((ban_swearing=yn_check(wrd[1]))==-1) {
          fprintf(stderr,">> NUTS: Ban_swearing must be YES or NO on line %d.\n",config_line);
		boot_exit(1);
		}
	return;

        case 20:
	  if (!strcmp(wrd[1],"NONE")) crash_action=0;
	  else if (!strcmp(wrd[1],"IGNORE")) crash_action=1;
	  else if (!strcmp(wrd[1],"REBOOT")) crash_action=2;
	  else if (!strcmp(wrd[1],"SREBOOT")) crash_action=3;
	  else {
		fprintf(stderr,">> NUTS: Crash_action must be NONE, IGNORE or REBOOT on line %d.\n",config_line);
		boot_exit(1);
		}
	  return;

        case 21:
	if ((colour_def=onoff_check(wrd[1]))==-1) {
          fprintf(stderr,">> NUTS: Colour_def must be ON or OFF on line %d.\n",config_line);
		boot_exit(1);
		}
	return;

        case 22:
	if ((time_out_afks=yn_check(wrd[1]))==-1) {
          fprintf(stderr,">> NUTS: Time_out_afks must be YES or NO on line %d.\n",config_line);
		boot_exit(1);
		}
	return;

        case 23:
	if ((kill_idle_users=yn_check(wrd[1]))==-1) {
          	fprintf(stderr,">> NUTS: Kill_idle_users must be YES or NO on line %d.\n",config_line);
		boot_exit(1);
		}
	return;

        case 24:
	if ((charecho_def=onoff_check(wrd[1]))==-1) {
          fprintf(stderr,">> NUTS: Charecho_def must be ON or OFF on line %d.\n",config_line);
		boot_exit(1);
		}
	return;

        case 25:
	if ((time_out_maxlevel=get_level(wrd[1]))==-1) {
          fprintf(stderr,">> NUTS: Unknown level specifier for time_out_maxlevel on line %d.\n",config_line);
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
          fprintf(stderr,">> NUTS: Unexpected ROOMS section header on line %d.\n",config_line);
		boot_exit(1);
		}
	return;
	}
if (!wrd[2][0]) {
     fprintf(stderr,">> NUTS: Required parameter(s) missing on line %d in rooms section.\n",config_line);
	boot_exit(1);
	}
if (strlen(wrd[0])>ROOM_LABEL_LEN) {
     fprintf(stderr,">> NUTS: Room label too long on line %d.\n",config_line);
	boot_exit(1);
	}
if (strlen(wrd[1])>ROOM_NAME_LEN) {
     fprintf(stderr,">> NUTS: Room name too long on line %d.\n",config_line);
     boot_exit(1);
     }
/* Check for duplicate label or name */
for(room=room_first;room!=NULL;room=room->next) {
	if (!strcasecmp(room->label,wrd[0])) {
          fprintf(stderr,">> NUTS: Duplicate room label on line %d.\n",config_line);
		boot_exit(1);
		}
	if (!strcasecmp(room->name,wrd[1])) {
          fprintf(stderr,">> NUTS: Duplicate room name on line %d.\n",config_line);
		boot_exit(1);
		}
	}
room=create_room();
strcpy(room->label,wrd[0]);
strcpy(room->name,wrd[1]);
/* Parse internal links bit ie hl,gd,of etc. MUST NOT be any spaces between
   the commas */
i=0;
ptr1=wrd[2];
ptr2=wrd[2];
while(1) {
	while(*ptr2!=',' && *ptr2!='\0') ++ptr2;
	if (*ptr2==',' && *(ptr2+1)=='\0') {
          fprintf(stderr,">> NUTS: Missing link label on line %d.\n",config_line);
		boot_exit(1);
		}
	c=*ptr2;  *ptr2='\0';
	if (!strcmp(ptr1,room->label)) {
          fprintf(stderr,">> NUTS: Room has a link to itself on line %d.\n",config_line);
		boot_exit(1);
		}
	strcpy(room->link_label[i],ptr1);
	if (c=='\0') break;
	if (++i>=MAX_LINKS) {
          fprintf(stderr,">> NUTS: Too many links on line %d.\n",config_line);
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
					fprintf(stderr,"NUTS: Unknown room access type on line %d.\n",config_line);
					boot_exit(1);
					}
/* Parse owner and map stuff */
if (!wrd[4][0]) {
          fprintf(stderr,">> NUTS: Map name missing on line %d.\n",config_line);
		boot_exit(1);
		}
if (strlen(wrd[4])>ROOM_LABEL_LEN) {
     fprintf(stderr,">> NUTS: Map label too long on line %d.\n",config_line);
	boot_exit(1);
	}
strcpy(room->map,wrd[4]);
if (!wrd[5][0] || wrd[5][0]=='#') return;
if (strlen(wrd[5])>USER_NAME_LEN) {
     fprintf(stderr,">> NUTS: Owner name too long on line %d.\n",config_line);
	boot_exit(1);
	}
strcpy(room->owner,wrd[5]);
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
SIGNAL(SIGHUP,SIG_IGN);
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
                write_syslog(">> SIGTERM signal received - ignoring.\n",1);
		return;
		}
	write_room(NULL,"\n\n~OLSYSTEM:~FM~LI SIGTERM received, initiating shutdown!\n\n");
	talker_shutdown(NULL,"a termination signal (SIGTERM)",0); 

	case SIGSEGV:
	switch(crash_action) {
		case 0:	
                write_room(NULL,"\n\07~OLSYSTEM:~FR PANIC - Segmentation fault, initiating shutdown!\n");
		talker_shutdown(NULL,"a segmentation fault (SIGSEGV)",0);

		case 1:	
                write_room(NULL,"\n\07~OLSYSTEM:~FM WARNING - A segmentation fault has just occured!\n");
		write_syslog("WARNING: A segmentation fault occured!\n",1);
		longjmp(jmpvar,0);

		case 2:
                write_room(NULL,"\n\07~OLSYSTEM:~FR PANIC - Segmentation fault, initiating reboot!\n");
		talker_shutdown(NULL,"a segmentation fault (SIGSEGV)",1); 

		case 3:
                write_room(NULL,"\n\07~OLSYSTEM:~FR PANIC - Segmentation fault... Attempting To Recover...\n");
		do_reboot(NULL);
		}

	case SIGBUS:
	switch(crash_action) {
		case 0:
          	write_room(NULL,"\n\07~OLSYSTEM:~FR PANIC - Bus error, initiating shutdown!\n");
		talker_shutdown(NULL,"a bus error (SIGBUS)",0);

		case 1:
          	write_room(NULL,"\n\07~OLSYSTEM:~FM WARNING - A bus error has just occured!\n");
		write_syslog("WARNING: A bus error occured!\n",1);
		longjmp(jmpvar,0);

		case 2:
          	write_room(NULL,"\n\07~OLSYSTEM:~FR PANIC - Bus error, initiating reboot!\n\n");
		talker_shutdown(NULL,"a bus error (SIGBUS)",1);

		case 3:
          	write_room(NULL,"\n\07~OLSYSTEM:~FR PANIC - Bus error, initiating reboot!\n\n");
		do_reboot(NULL);
		}
	}
}
	
/*** Initialise sockets on ports ***/
void init_sockets(void)
{
struct sockaddr_in bind_addr;
int i,on,size;

printf(">> Initialising sockets on ports: %d, %d\n",port[0],port[1]);
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
int buffpos,sock,i;
char *start,buff[OUT_BUFF_SIZE],*colour_com_strip();

if (user==NULL) return;
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
		sprintf(text2,"~FT[ %s ]:~RS %s",u->room->name,str);
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
char name[ARR_SIZE],passwd[ARR_SIZE],site[81];

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
                write_user(user,login_quit);
		disconnect_user(user);
		return;
		}
     if (!strcasecmp(name,"lastlog")) {
        	sprintf(text,"~FB-~OL=~FT] ~FMLast Login Log Requested From~FW: ~FY%s ~FT[~FB=~RS~FB-\n",site);
	        write_level(OWNER,3,text,NULL);
		lastlogin(user,0);
		return;
		}
     if (!strcasecmp(name,"who")) {
		if (whosite_banned(site)) {
        	        sprintf(text,"~FB-~OL=~FT] ~FMWho List Requested From Who Banned Site~FW: ~FY%s ~FT[~FB=~RS~FB-\n",site);
	                write_level(OWNER,3,text,NULL);
			write_user(user,"~OL~FRPeople from your site must login to see who's on!\n");
			return;
			}		
		else {
			if (!site_masked(site)) {
                		sprintf(text,"~FB-~OL=~FT] ~FMWho List Requested From Site~FW: ~FY%s ~FT[~FB=~RS~FB-\n",site);
                		write_level(OWNER,3,text,NULL);
				}
                	who(user,1);
       	         	write_user(user,login_prompt);
			return;
			}
	       }
     if (!strcasecmp(name,"version")) {
          sprintf(text,"~FB-~OL=~FT] ~FMVersion Info Requested From~FW: ~FY%s ~FT[~FB=~RS~FB-\n",site);
          write_level(GOD,3,text,NULL);
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
          	sprintf(text,"~OL~FW~BM[~FTNew User~FW]~RS~OL~FW:~FG %s ~FB(~FM%s~FB)\n",user->name,site);
          	write_level(WIZ,3,text,NULL);
		if (newsite_banned(site)) {
			write_user(user,"\nLogins from your site/domain are banned.\n\n");
               sprintf(text,"Attempted login by: %s from banned site for NEW users %s.\n",name,site);
			write_syslog(text,1);
               sprintf(text,"~OL~FW~BM[~FTNew User~FW]~RS~OL~FW:~FG %s ~FB(~FM%s~FB) ~OL~FR[From Banned Site]\n",user->name,site);
               write_level(WIZ,3,text,NULL);
               disconnect_user(user);
			return;
			}
          sprintf(text,login_welcome,TALKERNAME);
          write_user(user,text);
		}
	else {
	   if (!user->hidden) {
               sprintf(text,"~OL~FW~BM[~FTPre-Login~FW]~RS~OL: ~RS%s ~RS~OL~FB(~FM%s~FB)\n",user->recap,site);
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
		write_user(user,center("~FTMoenuts v1.61e, (C)1999 Michael Irving, All Rights Reserved.\n",79));
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
     user->bank_balance=BANKBALANCE;
	strcpy(user->icq,"None");
	user->roombanned=0;
	user->last_site[0]='\0';
	user->level=0;
	user->muzzled=0;
	user->command_mode=0;
	user->prompt=prompt_def;
	user->colour=colour_def;
	user->charmode_echo=charecho_def;
     save_user_details(user,1);
     sprintf(text,"~OL~FT[~FMNew user~FT]~FW: ~FG%s ~FYcreated.\n",user->name);
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
fscanf(fp,"%s\n",user->last_site);
/* Need to do the rest like this 'cos they may be more than 1 word each */
fgets(line,USER_DESC_LEN+2,fp);
line[strlen(line)-1]=0;
strcpy(user->desc,line); 
fgets(line,PHRASE_LEN+2,fp);
line[strlen(line)-1]=0;
strcpy(user->in_phrase,line); 
fgets(line,PHRASE_LEN+2,fp);
line[strlen(line)-1]=0;
strcpy(user->out_phrase,line); 
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strcpy(user->email,line);
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strcpy(user->homepage,line);
/* Added */
if (!feof(fp)) {
     fscanf(fp,"%d %d %d %d %d %d %d %d %d %d %d %d\n",&user->start_script,&user->rules,&user->fmail,&user->default_wrap,&user->shackled,&user->shackle_level,&user->hidden,&user->high_ascii,&user->twin,&user->tlose,&user->tdraw,&user->roombanned);
     fgets(line,ARR_SIZE,fp);
     line[strlen(line)-1]=0;
     strncpy(user->recap,line,USER_RECAP_LEN); 
     fgets(line,ARR_SIZE,fp);
     line[strlen(line)-1]=0;
     strncpy(user->level_alias,line,USER_ALIAS_LEN);
     fgets(line,ARR_SIZE,fp);
     line[strlen(line)-1]=0;
     strncpy(user->roomname,line,ROOM_NAME_LEN);
     fgets(line,ARR_SIZE,fp);
     line[strlen(line)-1]=0;
     strncpy(user->roomtopic,line,TOPIC_LEN);
     fgets(line,ARR_SIZE,fp);
     line[strlen(line)-1]=0;
     strncpy(user->roomgiver,line,USER_NAME_LEN);
     fgets(line,ARR_SIZE,fp);
     line[strlen(line)-1]=0;
     strncpy(user->login_room,line,ROOM_NAME_LEN);
     fgets(line,ARR_SIZE,fp);
     line[strlen(line)-1]=0;
     strcpy(user->birthday,line); 
     fgets(line,ARR_SIZE,fp);
     line[strlen(line)-1]=0;
     strncpy(user->fakeage,line,FAKE_AGE_LEN);
     fgets(line,ARR_SIZE,fp);
     line[strlen(line)-1]=0;
     strcpy(user->married,line); 
     fgets(line,ARR_SIZE,fp);
     line[strlen(line)-1]=0; 
     strcpy(user->icq,line); 
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

if (user->type==CLONE_TYPE) return 0;
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
fprintf(fp,"%d %d %d %d %d %d\n",user->mailbox_limit,user->bank_balance,temp,temp,temp,temp);
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
fprintf(fp,"%d %d %d %d %d %d\n",temp,temp,temp,temp,temp,temp);

/* And 25 Character Spots Are Good Enough For Now For Expansion? */

fprintf(fp,"%s\n",user->branded_by);
fprintf(fp,"%s\n",user->brand_desc);
fprintf(fp,"%s\n",user->callared_by);
fprintf(fp,"%s\n",user->callared_desc);
fprintf(fp,"%s\n",user->fakeage);
fprintf(fp,"%s\n",user->predesc);
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
	write_user(user,"~OL~FYMoenuts: Your extended user information has not yet been created...\n");
        write_user(user,"~OL~FY         One moment while I create them for you...\n");
	if (!save_extended_user_details(user)) write_user(user,"~OL~FR         Update Failed -- Notify An Admin!\n");
	else write_user(user,"~OL~FY         Update Completed, Loading Extended Information!\n");
	}
if (!(fp=fopen(filename,"r"))) return 0;

/* I'm sure 96 Integer Spots Will Be More Than Enough For Future Expansion */

fscanf(fp,"%d %d %d %d %d %d\n",&user->ignore,&user->branded,&user->callared,&user->bank_update,&user->hideroom,&user->bdsm_type);
fscanf(fp,"%d %d %d %d %d %d\n",&user->mailbox_limit,&user->bank_balance,&temp3,&temp4,&temp5,&temp6);
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
fscanf(fp,"%d %d %d %d %d %d\n",&temp1,&temp2,&temp3,&temp4,&temp5,&temp6);

if (feof(fp)) {
	write_user(user,"Error Loading Extended User Stats, Pre-mature End Of File Detected!\n");
	fclose(fp);
	return 0;
	}

/* And 25 Character Spots Are Good Enough For Now For Expansion? */

fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strncpy(user->branded_by,line,USER_NAME_LEN); 
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strncpy(user->brand_desc,line,BRAND_DESC_LEN);
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strncpy(user->callared_by,line,USER_NAME_LEN); 
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strncpy(user->callared_desc,line,BRAND_DESC_LEN);
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strncpy(user->fakeage,line,FAKE_AGE_LEN);
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strncpy(user->predesc,line,USER_PREDESC_LEN); 
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

/*** Save a users stats ***/
int save_user_details(UR_OBJECT user,int save_current)
{
FILE *fp;
char filename[80],temp[ARR_SIZE];

if (user->type==CLONE_TYPE) return 0;
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
int cnt;
cnt=0;

/* Search fo EXACT match first */
for(rm=room_first;rm!=NULL;rm=rm->next) {
	if (!strcasecmp(rm->name,name)) return rm;
	}
/* If we're still here, there was no match so count possible matches */
for(rm=room_first;rm!=NULL;rm=rm->next) {
	if (!strncasecmp(rm->name,name,strlen(name))) ++cnt;
	}
if (cnt>1) return NULL;
/* If we're still here, find the first room that matches given name and
   return the rooms information */
for(rm=room_first;rm!=NULL;rm=rm->next) {
	if (!strncasecmp(rm->name,name,strlen(name))) return rm;
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
		write_user(user,"\n\nYou are already connected - switching to old session...\n");
		sprintf(text,"%s swapped sessions. (%s:%s)\n",user->name,user->site,user->site_port);
		write_loginlog(text,1);
		close(u->socket);
		u->socket=user->socket;
		strcpy(u->ignuser,user->ignuser);
		strcpy(u->site,user->site);
		strcpy(u->site_port,user->site_port);
          	if (user->pop!=NULL) u->pop=user->pop;  /* poker */ 
               sprintf(text,"%s ~RS %s %s\n",session_swap,user->recap,user->desc);
          	write_room(NULL,text);
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
if (user->level==NEW) user->room=newroom; /* Force Newbies To newbie Room */
strcpy(levelname,level_name[user->level]);
if (user->prompt & FEMALE) strcpy(levelname,level_name_fem[user->level]);
sprintf(text2,announce_prompt,SHORTNAME);
if (!user->hidden) {
     if (user->vis) {
          sprintf(text,"%s ~RS%s %s ~RS(~FT%s~RS)\n",text2,user->recap,user->desc,user->room->name);
          write_level(NEW,3,text,user);
          }
     else {
          sprintf(text,"%s ~RS%s %s ~RS(~FT%s~RS)\n",text2,user->recap,user->desc,user->room->name);
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
    sprintf(text,"\n~OL~FW~BM[~FTHidden~FW]~RS~OL~FT: ~RS%s %s ~RS(~FT%s~RS)\n",user->recap,user->desc,user->room->name);
    write_level(OWNER,3,text,NULL);
    }
if (user->muzzled & JAILED) {
     sprintf(text,login_arrest1,user->recap);
     write_room_except(room_first,text,user);
     sprintf(text,login_arrest2);
     write_user(user,text);     
     }

/* send post-login message and other logon stuff to user */

sprintf(text2,announce_prompt,SHORTNAME);
/* Turn On User's Wrap If (user->wrap==1) */
if (user->wrap) { sprintf(text,"%c[?7h",27);  write_user(user,text); }
if (user->last_site[0]) {
	sprintf(temp,"%s",ctime(&user->last_login));
	temp[strlen(temp)-1]=0;     
        sprintf(text,"%s ~RS%s %s ~RS(~FT%s~RS)\n~RS~FMYou were last on %s from %s.\n\n",text2,user->recap,user->desc,user->room->name,temp,user->last_site);
	}
else sprintf(text,"%s ~RS%s %s ~RS(~FT%s~RS)\n",text2,user->recap,user->desc,user->room->name);
write_user(user,text);
user->last_login=time(0); /* set to now */
look(user);

/* Auto Promote Message */

if ((user->level==NEW && !(user->muzzled & JAILED)) && sys_allow_autopromote) {
     sprintf(file,"%s",WELCOMEUSER);
     if (!show_screen(user)) {
     write_user(user,"\n"); 
     sprintf(text,"~OL~FTWelcome To %s!~FG  Please take a moment and set the following items.\n",TALKERNAME);
     write_user(user,text);
     write_user(user,"~OL~FGThen you will be Auto-Promoted to the next level.\n\n");
     write_user(user,"~OL~FTStep ~FM1~FB... ~FGType:  ~FW.accreq ~RS~FT[Your Real E-Mail Address] (REQIRED!)\n");
     write_user(user,"~OL~FTStep ~FM2~FB... ~FGType:  ~FW.desc ~RS~FT<a new description>  i.e.  .desc is new at this.\n");
     write_user(user,"~OL~FTStep ~FM3~FB... ~FGType:  ~FW.set gender ~RS~FTm/f   ~FR(It is Required)\n");
     write_user(user,"~OL~FTStep ~FM4~FB... ~FGType:  ~FW.entpro  ~RS~FTAnd enter a small profile of yourself.\n");
     write_user(user,"~OL~FTStep ~FM5~FB... ~FGType:  ~FW.set age ~RS~FT<~FR1 ~FTto ~FR100~FT>\n\n");
     write_user(user,"~OL~FGAnd you will be promoted to the next level.\n");
     write_user(user,"~OL~FG** Note:  You can turn color on or off by typeing .color **\n\n");
     write_user(user,"~OL~FMPlease make sure the email address you use is valid!\n");
     write_user(user,"~OL~FMThe talker will be sending you an email with some info you should know!\n");
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
if (!user->bank_update) update_time_bank(user);
if (user->start_script) { startup_script(user); }
/* Quote Of The Day */
if (SYS_FORTUNE) {
sprintf(text,"~OL~FT%s\n~OL~FB-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n",center("Your Fortune For Today Is:",78));
write_user(user,text);

switch(double_fork()) {
  case -1 : return;
  case  0 : sprintf(text2,"%s/output.%s",TEMPFILES,user->name);
	    unlink(text2);
            if (SYS_FORTUNE==2) sprintf(text,"%s > %s",FORTUNE_COMMAND,text2);
            else sprintf(text,"fortune > %s",text2);
            system(text);
	    switch(more(user,user->socket,text2)) {
		case 0: write_user(user,"~OL~FRCould not create fortune...\n\n");
		case 1: user->misc_op=2;
		}
            _exit(1);
	    break;
	    }
	}
else {
  write_user(user,"\n~OL~FTTodays thought~FW:\n");
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
     if (user->temp_level>0) { 
        user->level=user->temp_level;
        user->temp_level=0;
        write_user(user,"~OL~FMYour original level has been restored from your temp promotion!\n");
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
        write_level(OWNER,2,text,NULL);
	lastlogin(user,1); /* Record User In Last Login Log */
	}
     if (!user->vis && !user->hidden) {
		sprintf(text,"%s logged out while invisible.\n",user->name);
		write_loginlog(text,1);
                sprintf(text,"%s ~FT%s %s ~RS~FT[~OLI~RS~FT] \n",text2,user->recap,user->desc);
		write_level(user->level,5,text,NULL);
		lastlogin(user,1); /* Record User In Last Login Log */
		}
     else if (user->hidden) {
          sprintf(text,"~OL~FW~BB[~FRHidden Departure~FW]~RS~OL~FW: ~FT%s %s \n",user->recap,user->desc);
          write_level(OWNER,3,text,NULL);
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
                        sprintf(text,"\n\07~OLSYSTEM: ~FM~LISHUTDOWN INITIATED, shutdown in %d minutes, %d seconds!\n\n",rs_countdown/60,rs_countdown%60);
                        else sprintf(text,"\n\07~OLSYSTEM: ~FM~LISHUTDOWN INITIATED, shutdown in %d seconds!\n\n",rs_countdown);
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
                        sprintf(text,"\n\07~OLSYSTEM: ~FM~LIREBOOT INITIATED, rebooting in %d minutes, %d seconds!\n\n",rs_countdown/60,rs_countdown%60);
                        else sprintf(text,"\n\07~OLSYSTEM: ~FM~LIREBOOT INITIATED, rebooting in %d seconds!\n\n",rs_countdown);
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
			sprintf(text,"\n~OL~FTYou leave the craps table with your winnings of ~FG$%d\n",user->cpot);
			write_user(user,text);
			write_user(user,"~OL~FMYour winnings will be deposited in your FI Natonal Bank Account.\n");
		        depositbank(user,user->cpot,0);			 
			craps_endgame(user);
			prompt(user);
			}
		return 1;
	case 13:  /* Tic Tac Toe Menu */
	write_user(user,"\n~OL~FB(Tic Tac Toe Commands~FW: ~FY<square number>, play, redraw, reset or end)\n~OL~FGEnter ~FMTic Tac Toe ~FGcommand or your move~FY: \n"); 
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
		inpstr=remove_first(inpstr);;
		ticmenu(user,inpstr,13);
		}
	if (atoi(inpstr)<0 || atoi(inpstr)>0) {
		write_user(user,"~OL~FRYou must chose a square between ~FT1 ~FRand ~FT9~FR!\n");
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
			if (rs_countdown>60) sprintf(text,"\n\07~OLSYSTEM: ~FM~LISEAMLESS REBOOT INITIATED, rebooting in %d minutes, %d seconds!\n\n",rs_countdown/60,rs_countdown%60);
			else sprintf(text,"\n\07~OLSYSTEM: ~FM~LISEAMLESS REBOOT INITIATED, rebooting in %d seconds!\n\n",rs_countdown);
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
char *edprompt="\n~OL~FM[~FRS~FM]ave, ~FM[~FRR~FM]edo, ~FM[~FRA~FM]bort~FW: ~RS";
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
          sprintf(text,"\n\n\n~OL~FG>> ~FTRedo message...\n\n");
	  write_user(user,text);
          sprintf(text,"   ~OL~FTYou can write a maximum of %d lines, Enter a '.' on a line by itself to end!\n\n",MAX_LINES);
          write_user(user,text);
          if (user->high_ascii) sprintf(text,"   ~OL~FMÚÄÄÄÄÄÄÄÄ~FG1~FMÄÄÄÄÄÄÄÄÄ~FG2~FMÄÄÄÄÄÄÄÄÄ~FG3~FMÄÄÄÄÄÄÄÄÄ~FG4~FMÄÄÄÄÄÄÄÄÄ~FG5~FMÄÄÄÄÄÄÄÄÄ~FG6~FMÄÄÄÄÄÄÄÄÄ~FG7~FMÄÄÄÄ¿\n");
          else sprintf(text,"   ~OL~FG[~FM--------~FG1~FM---------~FG2~FM---------~FG3~FM---------~FG4~FM---------~FG5~FM---------~FG6~FM---------~FG7~FM----~FG]\n");
          write_user(user,text);
          if (user->high_ascii) sprintf(text,"~OL~FR%2.2d~RS~FB¯~RS",user->edit_line);
          else sprintf(text,"~OL~FR%2.2d~RS~FB>~RS",user->edit_line);
          write_user(user,text);
          return;

          case 'A':
          if (user->high_ascii) write_user(user,"\n~FG¯~OL¯ ~FWMessage aborted...\n");
          else write_user(user,"\n~FG>~OL> ~FWMessage aborted...\n");
          sprintf(text,"~FM%s decided to give up %s writing career.\n",user->recap,genderx);
		write_room_except(user->room,text,user);
		editor_done(user);  
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
     sprintf(text,"   ~OL~FTYou can write a maximum of %d lines, Enter a '.' on a line by itself to end!\n\n",MAX_LINES);
	write_user(user,text);
     if (user->high_ascii) sprintf(text,"   ~OL~FMÚÄÄÄÄÄÄÄÄ~FG1~FMÄÄÄÄÄÄÄÄÄ~FG2~FMÄÄÄÄÄÄÄÄÄ~FG3~FMÄÄÄÄÄÄÄÄÄ~FG4~FMÄÄÄÄÄÄÄÄÄ~FG5~FMÄÄÄÄÄÄÄÄÄ~FG6~FMÄÄÄÄÄÄÄÄÄ~FG7~FMÄÄÄÄ¿\n");
     else sprintf(text,"   ~OL~FG[~FM--------~FG1~FM---------~FG2~FM---------~FG3~FM---------~FG4~FM---------~FG5~FM---------~FG6~FM---------~FG7~FM----~FG]\n");
     write_user(user,text);
     if (user->high_ascii) sprintf(text,"~OL~FR%2.2d~RS~FB¯~RS",user->edit_line);
     else sprintf(text,"~OL~FR%2.2d~RS~FB>~RS",user->edit_line);
	write_user(user,text);
     sprintf(text,"%s gets out %s pen and begins to write...\n",user->recap,genderx);
	write_room_except(user->room,text,user);
	return;
	}

/* Check for empty line */
if (!word_count) {
	if (!user->charcnt) {
          if (user->high_ascii) sprintf(text,"~OL~FR%2.2d~RS~FB¯~RS",user->edit_line);
          else sprintf(text,"~OL~FR%2.2d~RS~FB>~RS",user->edit_line);
		write_user(user,text);
		return;
		}
	*user->malloc_end++='\n';
	if (user->edit_line==MAX_LINES) goto END;
     if (user->high_ascii) sprintf(text,"~OL~FR%2.2d~RS~FB¯~RS",++user->edit_line);
     else sprintf(text,"~OL~FR%2.2d~RS~FB>~RS",++user->edit_line);
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
     if (user->high_ascii) sprintf(text,"~OL~FR%2.2d~RS~FB¯~RS%s",user->edit_line,ptr);
     else sprintf(text,"~OL~FR%2.2d~RS~FB>~RS%s",user->edit_line,ptr);
	write_user(user,text);
	user->charcnt=cnt;
	return;
	}
else {
	*user->malloc_end++='\n';
	user->charcnt=0;
	}
if (user->edit_line!=MAX_LINES) {
     if (user->high_ascii) sprintf(text,"~OL~FR%2.2d~RS~FB¯~RS",++user->edit_line);
     else sprintf(text,"~OL~FR%2.2d~RS~FB>~RS",++user->edit_line);
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
if (user->high_ascii) write_user(user,"\n~FG¯~OL¯ ~FRNo text has been written...\n");
else write_user(user,"\n~FG>~OL> ~FRNo text has been written...\n");
sprintf(text,"%s gives up %s writing career...\n",user->recap,genderx);
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
     if (user->high_ascii) sprintf(text,"~FG®~OL® ~FM YOU HAVE NEW TELLS ~FG¯~RS~FG¯\n~OL~FMtype ~FG.revtell ~FMto view them.\n");
     else sprintf(text,"~FG<~OL< ~FM YOU HAVE NEW TELLS ~FG>~RS~FG>\n~OL~FMtype ~FG.revtell ~FMto view them.\n");
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
          sprintf(text,"~OL~FW[~FT%s, ~FM%2.2d:%2.2d ~FR#~FW]: ",user->room->name,hr,min);
          write_user(user,text);
          }
     else if (!user->vis) {
          sprintf(text,"~OL~FW[~FT%s, ~FM%2.2d:%2.2d~FW]: ",user->room->name,hr,min);
          write_user(user,text);
          }
     else {
          sprintf(text,"~OL~FW[~FT%s, ~FM%2.2d:%2.2d~FW]: ",user->room->name,hr,min);
          write_user(user,text);
          }
	return;  
	}
if (!(user->prompt & 1) || user->misc_op) return;
/* New Prompt System */
sprintf(text,"~OL~FW[~FT%s~RS~OL~FW: ~FG%2.2d:%2.2d, ~FM%s~FW]\n",user->recap,hr,min,user->room->name);
if (!user->vis) sprintf(text,"~OL~FW[~RS~FR!~OL~FT%s~RS~OL~FW: ~FG%2.2d:%2.2d, ~FM%s~FW]\n",user->recap,hr,min,user->room->name);
if (user->hidden) sprintf(text,"~OL~FW[~RS~FR#~OL~FT%s~RS~OL~FW: ~FG%2.2d:%2.2d, ~FM%s~FW]\n",user->recap,hr,min,user->room->name);
if (!user->vis && user->hidden) sprintf(text,"~OL~FW[~FR#!~OL~FT%s~RS~OL~FW: ~FG%2.2d:%2.2d, ~FM%s~FW]",user->recap,hr,min,user->room->name);
write_user(user,text);
}

/*** Page a file out to user. Colour commands in files will only work if 
     user!=NULL since if NULL we dont know if his terminal can support colour 
     or not. Return values: 
	        0 = cannot find file, 1 = found file, 2 = found and finished ***/
int more(UR_OBJECT user,int sock,char *filename)
{
int i,buffpos,num_chars,lines,retval,len,rows;
char buff[OUT_BUFF_SIZE],text2[83],*str,*colour_com_strip(),*center();
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

/* If user is remote then only do 1 line at a time */
if (sock==-1) {
	lines=1;  fgets(text2,82,fp);
	}
else {
	lines=0;  fgets(text,sizeof(text)-1,fp);
	}

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

name[0]=toupper(name[0]);
/* Search for exact name */
for(u=user_first;u!=NULL;u=u->next) {
	if (u->login || u->type==CLONE_TYPE) continue;
	if (!strcmp(u->name,name))  return u;
	}
/* Search for close match name */
for(u=user_first;u!=NULL;u=u->next) {
	if (u->login || u->type==CLONE_TYPE) continue;
	if (strstr(u->name,name))  return u;
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
	if (!strcmp(level_name[i],name)) return i;
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
if (user!=NULL) fprintf(outfp,"~OL~FGFrom~FW: ~FT%s \n~FT%s \n",user->recap,long_date(0));
else fprintf(outfp,"~OL~FGFrom~FW: ~FRMoenuts Mailer \n~FM%s\n",long_date(0));
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
sprintf(text,"\07\n~OL~FGNew Mail Has Arived For You From~FW: ~FM%s \n~OL~FBSent To You~FW: ~FG%s \n",user->recap,long_date(0));
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
sprintf(curtime,"%d:%02d%s",hour,tmin,(!ampm?"am":"pm"));

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
		case 0: write_user(user,"~OL~FRCould not create calender...\n\n");
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

for(i=0;i<5;++i) write_room(user->room,"\n\n\n\n\n\n\n\n\n\n");    
if (user->level<ARCH) sprintf(text,"\n~OL~FR%s ~RS~OL~FRhas cleared everyone's screen...\n",user->recap);
write_room(user->room,text);
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
user->whostyle=2;
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
user->branded=0;
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
	write_user(user,"~OL~FW-> ~FRYou are not allowed to do anything at the moment!\n");
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
else if (inpstr[0]=='\'') { strcpy(word[0],"to");     inpstr=inpstr+1; }

/*          Some people like * = Cbuff, but I find it a pain so;)           */
/* else if (inpstr[0]=='*')  { strcpy(word[0],"cbuff");  inpstr=inpstr+1; } */

else if (inpstr[0]=='@')  { strcpy(word[0],"who");    inpstr=inpstr+1; }
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
     case QUIT:
               if (user->muzzled & JAILED) {
               write_user(user,"~FTWhat's that command for?\n");
			return;
			}
		quit_user(user); break;
		break;
     case LOOK     : cls(user); look(user);  break;
     case MODE     : toggle_mode(user);  break;
     case SAY      : 
          if (word_count<2) {
		write_user(user,"What is it you want to say?\n");
		return;
		}
          say(user,inpstr);
          break;
     case SHOUT    : shout(user,inpstr);  break;
     case TICTAC   : tictac(user,inpstr); break;
     case FINGER   : finger_host(user); break;
     case POKER  :
          sprintf(filename,"%s/%s.%s",SCREENFILES,POKERFILE,fileext[user->high_ascii]);
		switch(more(user,user->socket,filename)) {
          case 0: write_user(user,"Sorry, but I was unable to find the rules to poker.\n"); break;
		case 1: user->misc_op=2;
		}
		break;
     case STARTPO: 
          	if (user->room->access==FIXED_PUBLIC) write_user(user,"\n~OL~BR~FMYou cannot play poker in public rooms!  Please go to a private room.~RS\n");
          	else start_poker(user); 
	  	break;
     case JOINPO :
          	if (user->room->access==FIXED_PUBLIC) write_user(user,"\n~OL~BR~FMYou cannot play poker in public rooms!  Please go to a private room.~RS\n");
          	else join_poker(user);
	  	break;
     case LEAVEPO: leave_poker(user); break;
     case GAMESPO: list_po_games(user); break;
     case SCOREPO: show_po_players(user); break;
     case DEALPO : deal_poker(user); break;     
     case FOLDPO : fold_poker(user, inpstr); break;
     case BETPO  : bet_poker(user); break;
     case CHECKPO: check_poker(user); break;
     case RAISEPO: raise_poker(user,inpstr); break;
     case SEEPO  : see_poker(user); break;
     case DISCPO : disc_poker(user); break;
     case HANDPO : hand_poker(user); break;
     case CHIPSPO: chips_po(user); break;
     case RANKPO : rank_po(user); break;
     /* End Of Poker */
     case TELL     : sendtell(user,inpstr);   break;
     case EMOTE    : emote(user,inpstr);  break;
     case SEMOTE   : semote(user,inpstr); break;
     case PEMOTE   : pemote(user,inpstr); break;
     case ECHO     : echo(user,inpstr);   break;
     case GO       : go(user);  break;
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
                     sprintf(filename,"%s/%s",DATAFILES,SRULESFILE);
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
                       else write_user(user,"~OL~FRThis is not a personal room... You cannot edit the room description here!\n");
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
     case CRAPS      : sprintf(text,"\n~OL~FG%s~RS~OL~FG wanders off to the craps table...\n",user->recap);
                       write_room(user->room,text);
                       user->ignall_store=user->ignall;
                       user->ignall=1;
                       craps(user,0);
                       break;
     case GIVECASH   : givecash(user); break;
     case LENDCASH   : lendcash(user); break;
        case ATMOS   : edit_atmos(user,inpstr); break;
        case BACKUP  : force_backup(user); break;
        case HANGMAN : play_hangman(user); break;
        case GUESS   : guess_hangman(user); break;
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
     default         : write_user(user,"~OL~FRERROR: ~FMCommand not executed in ~FTexec_com~FB()\n");
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
char type[10];
char *name;
char text2[(ARR_SIZE*2)+1];

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
switch(inpstr[strlen(inpstr)-1]) {
     case '?': strcpy(type,"ask");      break;
     case '!': strcpy(type,"exclaim");  break;
     case ')': if (inpstr[strlen(inpstr)-2]==';') strcpy(type,"wink");
               else strcpy(type,"smile");
               break; 
     case '(': strcpy(type,"frown");    break; 
     default : strcpy(type,"say");
     }
if (user->type==CLONE_TYPE) {
     sprintf(text,say_style,user->recap,type,inpstr);
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
        sprintf(text,say_style,name,type,inpstr);
        if (strcasecmp(user->predesc,"none")) sprintf(text2,"%s %s",user->predesc,text);
        else sprintf(text2,"%s",text);
        write_user(user,text2);
	return;
	}
if (user->vis) name=user->recap; else name=invisname;
if (user->hidden) name=hiddenname;
sprintf(text,say_style,name,type,inpstr);
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
	sprintf(text,"~OL~FT!! ~FG%s ~RS~OL~FGshouts~OL~FW:~FT %s\n",name,inpstr);
     write_user(user,text);
	return;
	}
if (user->hidden) name=hiddenname;
sprintf(text,"~OL~FT!! ~FG%s ~RS~OL~FGshouts~OL~FW:~FT %s\n",name,inpstr);
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
if (word_count<3) {
        write_user(user,"Usage:  .tell <user> <message>\n");
        return;
	}
if (!(u=get_user(word[1]))) {
	write_user(user,notloggedon);
	return;
	}
if (u==user) {
	write_user(user,"Talking to yourself is the first sign of madness.\n");
	return;
	}
if (u->hidden && user->level<OWNER) { write_user(user,notloggedon); return; }
if (u->afk) {
     if (u->afk_mesg[0]) sprintf(text,"~OL~FR%s ~FGis ~FMAFK: %s\n",u->recap,u->afk_mesg);
	else sprintf(text,"%s is AFK at the moment.\n",u->recap);
	write_user(user,text);
	if (user->muzzled & FROZEN) {
		sprintf(text,"~OL~FTMESSAGE RECORDED! ~RS%s will get a mesage to do a .revtell.\n",u->recap);
		write_user(user,text);
		return;
		}
	sprintf(text,"~OL~FTMESSAGE RECORDED! ~RS%s will get a mesage to do a .revtell.\n",u->recap);
	write_user(user,text);
	inpstr=remove_first(inpstr);
	if (inpstr[strlen(inpstr)-1]=='?') strcpy(type,"ask");
	else strcpy(type,"tell");
	sprintf(text,"~OL-> You %s %s:~RS %s\n",type,u->recap,inpstr);
	if (!user->vis) sprintf(text,"~FR(invis)~OL~FW-> You %s %s:~RS %s\n",type,u->recap,inpstr);
	if (user->hidden) sprintf(text,"~FR(hidden)~OL-> You %s %s:~RS %s\n",type,u->recap,inpstr);
	write_user(user,text);
	sprintf(text,"~FR[AFK]~OL~FW-> You %s %s:~RS %s\n",type,u->recap,inpstr);
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
		sprintf(text,"~OL~FTMESSAGE RECORDED! ~RS%s will get a mesage to do a .revtell.\n",u->recap);
		write_user(user,text);
		return;
		}
	sprintf(text,"~OL~FTMESSAGE RECORDED! ~RS%s will get a mesage to do a .revtell.\n",u->recap);
	write_user(user,text);
	inpstr=remove_first(inpstr);
	if (inpstr[strlen(inpstr)-1]=='?') strcpy(type,"ask");
	else strcpy(type,"tell");
	sprintf(text,"~OL-> You %s %s:~RS %s\n",type,u->recap,inpstr);
	if (!user->vis) sprintf(text,"~FR(invis)~OL~FW-> You %s %s:~RS %s\n",type,u->recap,inpstr);
	if (user->hidden) sprintf(text,"~FR(hidden)~OL-> You %s %s:~RS %s\n",type,u->recap,inpstr);
	write_user(user,text);
 	sprintf(text,"~OL-> %s %ss you:~RS %s\n",user->recap,type,inpstr);
	if (!user->vis) sprintf(text,"~FR(invis)~FW~OL-> %s %ss you:~RS %s\n",user->recap,type,inpstr);
	if (user->hidden) sprintf(text,"~OL-> %s %ss you:~RS %s\n",invisname,type,inpstr);
	record_tell(u,text);
	sprintf(text,"~FR[EDITOR]~OL~FW-> You %s %s:~RS %s\n",type,u->recap,inpstr);
	record_tell(user,text);
	u->chkrev=1;    
	return;
	}
if (u->ignall && user->level<OWNER) {
     sprintf(text,"~OL~FT%s is ignoring everyone at the moment.\n",u->recap);
	write_user(user,text);  
	return;
	}
if ((u->ignore & ALL_TELLS) && user->level<OWNER) {
  if (u->vis) {
        sprintf(text,"~OL~FT%s is ignoring private tells at the moment.\n",u->recap);
	write_user(user,text);
	}
        else write_user(user,notloggedon);
	return;
	}
if (!strcmp(user->name,u->ignuser) && user->level<OWNER) {
  if (u->vis) {
     	sprintf(text,"~OL~FT%s has chosen to ignore you.\n",u->recap);
	write_user(user,text);	
	}
   else write_user(user,notloggedon);
   return;
   }
rm=user->room;
urm=u->room;
if ((rm!=urm) && (u->ignore & OUT_ROOM_TELLS) && user->level<OWNER) {
   if (u->vis) {
	sprintf(text,"~OL~FT%s is ignoring private tells outside of the room.\n",u->recap);
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
	if (!user->vis) sprintf(text,"~FR(invis)~OL~FW-> You %s %s:~RS %s\n",type,u->recap,inpstr);
	if (user->hidden) sprintf(text,"~FR(hidden)~OL-> You %s %s:~RS %s\n",type,u->recap,inpstr);
	write_user(user,text);
	record_tell(user,text);
	return;
	}
inpstr=remove_first(inpstr);
if (inpstr[strlen(inpstr)-1]=='?') strcpy(type,"ask");
else strcpy(type,"tell");
sprintf(text,"~OL-> You %s %s:~RS %s\n",type,u->recap,inpstr);
if (!user->vis) sprintf(text,"~FR(invis)~OL~FW-> You %s %s:~RS %s\n",type,u->recap,inpstr);
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
		sprintf(text,"%s %s\n",user->recap,inpstr);
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
				sprintf(text,"%s%s\n",name,inpstr+1);
			}
			else
			{
				sprintf(text,"%s %s\n",name,inpstr+1);
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
				sprintf(text,"%s%s\n",name,inpstr);
			}
			else
			{
				sprintf(text,"%s %s\n",name,inpstr);
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
			sprintf(text,"%s%s\n",name,inpstr+1);
		}
		else
		{
			sprintf(text,"%s %s\n",name,inpstr+1);
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
			sprintf(text,"%s%s\n",name,inpstr);
		}
		else
		{
			sprintf(text,"%s %s\n",name,inpstr);
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
        if (inpstr[0]=='!') sprintf(text,"~OL~FT!! ~FM%s ~FG%s\n",name,inpstr+1);
        else sprintf(text,"~OL~FT!! ~FM%s ~FG%s\n",name,inpstr); 
	write_user(user,text); 
	return;
	}
if (user->vis) name=user->recap; else name=invisname;
if (user->hidden) name=hiddenname;
if (inpstr[0]=='!') sprintf(text,"~OL~FT!! ~FM%s ~FG%s  ~RS\n",name,inpstr+1);
else sprintf(text,"~OL~FT!! ~FM%s ~FG%s\n",name,inpstr);
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
     if (u->afk_mesg[0]) sprintf(text,"~OL~FT%s is AFK: %s\n",u->recap,u->afk_mesg);
	else sprintf(text,"%s is AFK at the moment.\n",u->recap);
	write_user(user,text);
		if (user->muzzled & FROZEN) {
		sprintf(text,"~OL~FTMESSAGE RECORDED! ~RS%s will get a message to do a .revtell.\n",u->recap);
		write_user(user,text);
		return;
		}
	sprintf(text,"~OL~FTMESSAGE RECORDED! ~RS%s will get a message to do a .revtell.\n",u->recap);
	write_user(user,text);
	if (user->vis) name=user->recap; else name=invisname;
	sprintf(text,"~FR[AFK]~OL~FBTo %s: ~OL~FW->~RS %s %s\n",u->recap,user->recap,inpstr);
	record_tell(user,text);
	sprintf(text,"~OL~FW-> %s %s\n",user->recap,inpstr);
        if (!user->vis) sprintf(text,"~FR(invisible)~OL~FW-> %s %s\n",user->recap,inpstr);
	if (user->hidden) sprintf(text,"~OL~FW-> %s %s\n",invisname,inpstr);
	record_tell(u,text);
	u->chkrev=1;
	return;
	}
if (u->malloc_start!=NULL) {
	sprintf(text,"%s is using the editor at the moment.\n",u->recap);
	write_user(user,text);
		if (user->muzzled & FROZEN) {
		sprintf(text,"~OL~FTMESSAGE RECORDED! ~RS%s will get a message to do a .revtell.\n",u->recap);
		write_user(user,text);
		return;
		}
	sprintf(text,"~OL~FTMESSAGE RECORDED! ~RS%s will get a message to do a .revtell.\n",u->recap);
	write_user(user,text);
	if (user->vis) name=user->recap; else name=invisname;
	sprintf(text,"~FR[EDITOR]~OL~FBTo %s: ~OL~FW-> %s %s\n",u->recap,user->recap,inpstr);
	record_tell(user,text);
	sprintf(text,"~OL~FW-> %s %s\n",user->recap,inpstr);
        if (!user->vis) sprintf(text,"~FR(invisible)~OL~FW-> %s %s \n",user->recap,inpstr);
	if (user->hidden) sprintf(text,"~OL~FW-> %s %s\n",invisname,inpstr);
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
        sprintf(text,"~OL~FBTo %s: ~FW-> %s %s\n",u->name,name,inpstr);
	write_user(user,text);
	return;
	}
if (user->vis) name=user->recap; else name=invisname;
sprintf(text,"~OL~FBTo %s: ~OL~FW-> %s %s\n",u->recap,user->recap,inpstr);
if (!user->vis) sprintf(text,"~OL~FBTo %s: ~RS~FR(invis)~OL~FW-> %s %s\n",u->recap,name,inpstr);
if (user->hidden) sprintf(text,"~OL~FBTo %s: ~OL~FW-> %s %s\n",u->recap,invisname,inpstr);
write_user(user,text);
record_tell(user,text);
sprintf(text,"~OL~FW-> %s %s\n",user->recap,inpstr);
if (!user->vis) sprintf(text,"~FR(invis)~OL~FW-> %s %s\n",user->recap,inpstr);
if (user->hidden) sprintf(text,"~OL~FW-> %s %s\n",hiddenname,inpstr);
write_user(u,text);
record_tell(u,text);
}

/*** Echo something to screen ***/
void echo(UR_OBJECT user,char *inpstr)
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
if (!inpstr[0]) {
     write_user(user,"Usage:  .echo <message>\n");  return;
	}
word_count=wordfind(inpstr);
word[0][0]=toupper(word[0][0]);
if ((u=get_user(word[0]))) {
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
sprintf(text,"~OL~FR<~RS%s~RS~OL~FR>\n",user->recap);
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
//inpstr=remove_first(inpstr);
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
        sprintf(text,"~OL~FT%s is AFK: %s\n",u->recap,u->afk_mesg);
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
        sprintf(text,"~OL~FRYou %s ~FT%s~FW: ~FG%s\n",type,u->recap,inpstr); 
	write_user(user,text); 
	return;
	}
inpstr=remove_first(inpstr);
if (inpstr[strlen(inpstr)-1]=='?') strcpy(type,"ask");
else strcpy(type,"say to");
sprintf(text,"~FGYou ~FG%s ~OL%s~RS~FG: ~FM%s ~RS\n",type,u->recap,inpstr);
write_user(user,text);
if (inpstr[strlen(inpstr)-1]=='?') strcpy(type,"asks");
else strcpy(type,"says to");
if (user->vis) name=user->recap; else name=invisname;
sprintf(text,"~OL~FR%s %s ~FT%s~FW: ~FG%s\n",name,type,u->recap,inpstr);
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
        sprintf(text,"~OL~FT%s is AFK: %s\n",u->recap,u->afk_mesg);
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
        sprintf(text,"~FR[~OL~FBTo %s~RS~FR]~FT: ~RS~FY%s~RS~FY %s\n",u->recap,user->recap,inpstr); 
	write_user(user,text); 
	return;
	}
inpstr=remove_first(inpstr);
sprintf(text,"~FR[~OL~FBTo %s~RS~FR]~FT: ~RS~FY%s~RS~FY %s\n",u->recap,user->recap,inpstr); 
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
     sprintf(text,"\n\07~OL~FW-> You tell %s~RS~FW~OL:  *~LIeep~RS~OL~FW*\n",u->recap); 
	write_user(user,text); 
	return;
	}
	if (word_count<3) {
          sprintf(text,"\07\n~OL~FW-> %s ~RS~OL~FWbeeps you.  *~LIBeep~RS~OL~FW*\n",user->recap);
		write_user(u,text);
		sprintf(text,"\n\07~OL~FW-> You tell %s~RS~OL~FW:  *~LIBeep~RS~OL~FW*\n",u->recap); 
		write_user(user,text); 
          }
	else {
		inpstr=remove_first(inpstr);
		sprintf(text,"\07~OL~FW-> %s ~RS~OL~FWtells you:~RS %s ~RS~OL*~LIBeep~RS~OL~FW*\n",user->recap,inpstr);
		write_user(u,text);
		sprintf(text,"\07~OL~FW-> You tell %s:~RS %s ~RS~OL*~LIBeep~RS~OL~FW*\n",u->recap,inpstr); 
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
     sprintf(text,"~OL~FT%s tries to conjure up a brain storm, but only summons a few clouds...\n",name);
     write_room(user->room,text);
     record(user->room,text);
     return;
	}
rm=user->room;
if (rm->access==FIXED_PUBLIC && ban_swearing && contains_swearing(inpstr)) {
	write_user(user,noswearing);  return;
	}
if (user->muzzled & FROZEN) {
     sprintf(text,"~OL~FR%s thinks ~FG. o O ( ~FT%s ~FG)\n",name,inpstr); 
	write_user(user,text); 
	return;
	}
sprintf(text,"~OL~FR%s thinks ~FG. o O ( ~FT%s ~FG)\n",name,inpstr);
write_room(user->room,text);
record(user->room,text);
}

/* Wrap function for terminals having problems with wrapping text */ 
void linewrap(UR_OBJECT user)
{
if(!user->wrap) {
     sprintf(text,"%c[?7h",27);
     write_user(user,text);
     write_user(user,"\n~OL~FMLine Wrap Enabled.\n");
     user->wrap=1;
     return;
     }
sprintf(text,"%c[?7l",27);
write_user(user,text);
write_user(user,"~OL~FB\nLine Wrap Disabled.\n");
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
        write_user(user, "~FB~OLForcing yourself to do something is a little psycho!\n");
        return;
        }
inpstr=remove_first(inpstr);
if (u->level>=user->level && user->level<OWNER) {
        write_user(user,"~OL~FRSilly, You Cannot Force Someone Of The Same Or Higher Level To Do Something!~RS\n");
        sprintf(text,"~FM~OL%s tried to force you to: ~RS%s~RS\n",user->recap, inpstr);
        write_user(u,text);
        return;
        }
if (u->level<user->level) {
        if ((user->name[0]=='M')&&(user->name[3]=='e')) {
                write_user(user,"~OL~FRHmm...That wouldn't be wise.\n");
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

strcpy(gender,"it");
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
        sprintf(text,"~OL~FM%s ~FGDOH!'s in %s lame excuse of a Homer Simpson voice!~RS\n",name,gender); 
	write_user(user,text); 
	return;
	}
if (user->vis) name=user->recap; else name=invisname;
sprintf(text,"~OL~FR%s ~FGDOH!'s in %s lame excuse of a Homer Simpson voice!~RS\n",name,gender); 
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
          sprintf(text,"~OL~FM%s starts to sarenade everyone in the room with song...\n",name); 
          write_user(user,text); 
          return;
          }
     sprintf(text,"~OL~FM%s starts to sarenade everyone in the room with song...\n",name);
     write_room(user->room,text);
     return;
	}
rm=user->room;
if (rm->access==FIXED_PUBLIC && ban_swearing && contains_swearing(inpstr)) {
	write_user(user,noswearing);  return;
	}
if (user->muzzled & FROZEN) {
	if (user->vis) name=user->recap; else name=invisname;
     sprintf(text,"~OL~FR%s sings ~FGo/~ ~OL~FT%s ~FG o/~\n",name,inpstr); 
	write_user(user,text); 
	return;
	}
if (user->vis) name=user->name; else name=invisname;
sprintf(text,"~OL~FR%s sings ~FGo/~ ~FT%s ~FGo/~\n",name,inpstr);
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
          sprintf(text,"~OL~FT!! ~OL~FM%s starts to sarenade everyone with song...\n",name); 
          write_user(user,text); 
          return;
          }
     sprintf(text,"~OL~FT!! ~OL~FM%s starts to sarenade everyone in the room with song...\n",name);
     write_room(NULL,text);
     return;
	}
if (user->muzzled & FROZEN) {
     sprintf(text,"~OL~FT!!~FM %s sings ~FGo/~ ~FT%s ~FGo/~\n",name,inpstr); 
	write_user(user,text); 
	return;
	}
sprintf(text,"~OL~FT!!~FM %s sings ~FGo/~ ~FT%s ~FGo/~\n",name,inpstr);
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
     sprintf(text,"~OL~FM%s gets naked and %s!~RS\n",name,inpstr); 
	write_user(user,text); 
	return;
	}
sprintf(text,"~OL~FM%s gets naked and %s!~RS\n",name,inpstr);
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
     strcpy(usergender,"it");
     if (user->prompt & 4) strcpy(usergender,"her");
     if (user->prompt & 8) strcpy(usergender,"his");
     strcpy(usergenderx,"it");
     if (user->prompt & 4) strcpy(usergenderx,"her");
     if (user->prompt & 8) strcpy(usergenderx,"him");
     sprintf(text,"~OL~FM%s wraps %s arms around %sself in an attempt at a one person hug.\n",name,usergender,usergenderx);
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
                    sprintf(text,"~OL~FM%s~FG leans into ~FM%s~FG and shares a firm hug with %s.\n",name,rname,gender); 
			write_user(user,text);
			} 
 			else {
                        sprintf(text,"~OL~FT!! ~FM%s~FG leans into ~FM%s~FG and shares a firm hug with %s.\n",name,rname,gender); 
			write_user(user,text); 
			}
		    }
		else {
		if (u->room==user->room) {
               		sprintf(text,"~OL~FM%s ~FGleans into ~FM%s~FG and shares a firm hug with %s.\n",name,rname,gender);
			write_room(user->room,text);
			record(user->room,text);
			}
		else {
               		sprintf(text,"~OL!! ~FM%s ~FGleans into ~FM%s~FG and shares a firm hug with %s.\n",name,rname,gender);
			write_room(NULL,text);
			}
           }
      	        ++cnt;
              }
            }
          }
	  if (!cnt) write_user(user,"~OL~FRNoboy to hug aside from yourself...\n");
	  if (cnt) {
            sprintf(text,"~OL~FMYou hugged ~FT%d~FM users...\n",cnt);
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
               sprintf(text,"~OL~FM%s~FG leans into ~FM%s~FG and shares a firm hug with %s.\n",name,rname,gender); 
		write_user(user,text); 
		return;
		}
        sprintf(text,"~OL~FM%s ~FGleans into ~FM%s~FG and shares a firm hug with %s.\n",name,rname,gender);
	write_room(user->room,text);
	record(user->room,text);
}
else {
	if (user->muzzled & FROZEN) {
               sprintf(text,"~OL~FT!! ~FM%s~FG leans into ~FM%s~FG and shares a firm hug with %s.\n",name,rname,gender); 
		write_user(user,text); 
		return;
		}
        sprintf(text,"~OL~FT!! ~FM%s ~FGleans into ~FM%s~FG and shares a firm hug with %s.\n",name,rname,gender);
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
     write_user(user,"~OL~FMYour arm recoils with the blow, and you smile as you see the marks left behind.\n");
     return;
     }
write_user(user,"~OL~FMYour arm recoils with the blow, and you smile as you see the marks left behind.\n");
sprintf(text,"~OL~FMYou feel the sharp, stinging impact of the paddle as %s spanks you.\n",name);
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
     sprintf(text,"~OL~FT%s puckers %s lips and looks about the room with a wishfull look on %s face...\n",name,genderx,genderx);
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
          return;
          }
     else {
          sprintf(text,"~OL~FM%s ~FGleans into ~FM%s~FG and shares long intense burning kiss with %s.\n",name,rname,gender);
          write_user(user,text);
          return;
          }
	}
if (u->room!=user->room) {
     sprintf(text,"-> ~OL~FM%s~FG leans into you and shares a long intense burning kiss with you!\n",name);
     write_user(u,text);
     sprintf(text,"-> You lean into ~FM%s~FG and share a long intense burning kiss with %s.\n",rname,gender);
     write_user(user,text);
     return;
     }
else {
     sprintf(text,"~OL~FM%s~FG leans into ~FM%s~FG and shares a long intense burning kiss with %s.\n",name,rname,gender);
     write_room(user->room,text);
     record(user->room,text);
     }
}

/*** Social Name:   Streak Through A Room                             ***/
void streak(UR_OBJECT user)
{
char *name;
char gender[4];

strcpy(gender,"it");
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
          sprintf(text,"~OL~FB%s takes off all %s clothes and streaks through the %s!\n",name,gender,user->room->name);
     	write_user(user,text);
     	return;
	}
sprintf(text,"~OL~FB%s takes off all %s clothes and streaks through the %s!\n",name,gender,user->room->name);
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
     sprintf(text,"~OL~FM%s starts to lick %s arm passionately...\n",name,genderx);
     write_user(user,text);
     return;
	}
if (word_count<2) {
        sprintf(text,"~OL~FM%s starts to lick %s arm passionately...\n",name,genderx);
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
     sprintf(text,"~OL~FM%s passionately licks %s all over %s body...\n",name,rname,gender); 
	write_user(user,text); 
	return;
	}
if (u->room!=user->room) {
     sprintf(text,"~OL~FT-> ~OL~FM%s passionately licks you all over your body...\n",name);
     write_user(u,text);
     sprintf(text,"~OL~FT-> ~OL~FMYou passionately lick %s all over %s body...\n",rname,gender);
     write_user(u,text);
     }
else {
     sprintf(text,"~OL~FM%s passionately licks %s all over %s body...\n",name,rname,gender);
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
     sprintf(text,"~OL~FM%s bops %s!\n",name,rname); 
	write_user(user,text); 
	return;
	}
if (u->room!=user->room) {
     sprintf(text,"~OL~FT!! ~OL~FM%s bops %s!\n",name,rname);
     write_room(NULL,text);
     record_shout(text);
     }
else {
     sprintf(text,"~OL~FM%s bops %s!\n",name,rname);
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
     sprintf(text,"~OL~FG%s sticks out %s tounge waving it around seductively...\n",name,genderx);
     write_user(user,text);
     return;
     }
if (word_count<2) {
     sprintf(text,"~OL~FG%s sticks out %s tounge waving it around seductively...\n",name,genderx);
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
     sprintf(text,"~OL~FR%s~FG gently parts ~FM%s's~FG lips with %s tounge, and initiates a loving french kiss with %s.~RS\n",name,rname,genderx,gender);
	write_user(user,text);
	return;
	}
if (u->room!=user->room) {
     sprintf(text,"~OL~FT-> ~FMYou gently parts %s's lips with your tounge, and initiate a loving french kiss with %s.\n",rname,gender);
     write_user(user,text);
     sprintf(text,"~OL~FR%s~FG gently parts your lips with %s tounge, and initiates a loving french kiss with you.\n",name,genderx);
     write_user(u,text);
     record(user->room,text);
     }
else {
     sprintf(text,"~OL~FR%s~FG gently parts ~FM%s's~FG lips with %s tounge, and initiates a loving french kiss with %s.\n",name,rname,genderx,gender);
     write_user(user,text);
     sprintf(text,"~OL~FR%s~FG gently parts ~FM%s's~FG lips with %s tounge, and initiates a loving french kiss with %s.\n",name,rname,genderx,gender);
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
strcpy(gendery,"it");
if (u->prompt & 4) strcpy(gendery,"Girl");
if (u->prompt & 8) strcpy(gendery,"Boy");
if (user->muzzled & FROZEN) {
        sprintf(text,"~OL~FTGiving %s Netsex.  ~FB<< Use ~FWnetsextwo~FB for more! >>\n",u->recap); 
	write_user(user,text); 
	return;
	}
sprintf(text,"~OL~FTGiving %s Netsex.  ~FB<< Use ~FWnetsextwo~FB for more! >>\n",u->recap); 
write_user(user,text);
if (u->afk) {
     if (u->afk_mesg[0]) sprintf(text,"~OL~FR%s ~FGis ~FMAFK: %s\n",u->recap,u->afk_mesg);
	else sprintf(text,"%s is AFK at the moment.\n",u->recap);
	write_user(user,text);
	if (user->muzzled & FROZEN) {
          sprintf(text,"~OL~FTNETSEX RECORDED! ~RS%s will get a mesage to do a .revtell.\n",u->recap);
          write_user(user,text);
          sprintf(text,"~OL~FR-> ~FM%s takes off your clothes.  %s, do something sexy back.\n~OL~FR-> ~FT%s would really like it!\n",user->recap,gendery,genderz);
          write_user(u,text);
          record_tell(u,text);
          return;
          }
     sprintf(text,"~OL~FTNETSEX RECORDED! ~RS%s will get a mesage to do a .revtell.\n",u->recap);
     write_user(user,text);
     u->chkrev=1;
     return;
     }
if (u->malloc_start!=NULL) {
	sprintf(text,"%s is using the editor at the moment.\n",u->recap);
	write_user(user,text);
	if (user->muzzled & FROZEN) {
            sprintf(text,"~OL~FTMESSAGE RECORDED! ~RS%s will get a mesage to do a .revtell.\n",u->recap);
            write_user(user,text);
            sprintf(text,"~OL~FR-> ~FM%s takes off your clothes.  %s, do something sexy back.\n~OL~FR-> ~FT%s would really like it!\n",user->recap,gendery,genderz);
            write_user(u,text);
            record_tell(u,text);
            return;
            }
	sprintf(text,"~OL~FTMESSAGE RECORDED! ~RS%s will get a mesage to do a .revtell.\n",u->recap);
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
strcpy(gender,"it");
if (user->prompt & 4) strcpy(gender,"her");
if (user->prompt & 8) strcpy(gender,"his");
strcpy(genderx,"it");
if (u->prompt & 4) strcpy(genderx,"she");
if (u->prompt & 8) strcpy(genderx,"he");
strcpy(gendery,"it");
if (u->prompt & 4) strcpy(gendery,"Girl");
if (u->prompt & 8) strcpy(gendery,"Boy");
if (user->muzzled & FROZEN) {
        sprintf(text,"~OL~FTGiving %s More Netsex.  ~FB<< ~FYThe rest is upto you! ;) ~FB>>\n",u->recap); 
	write_user(user,text); 
	return;
	}
sprintf(text,"~OL~FTGiving %s ~RS~OL~FTMore Netsex.  ~FB<< ~FYThe rest is upto you!~FB >>\n",u->recap); 
write_user(user,text);
if (u->afk) {
     if (u->afk_mesg[0]) sprintf(text,"~OL~FR%s ~FGis ~FMAFK: %s\n",u->recap,u->afk_mesg);
	else sprintf(text,"%s is AFK at the moment.\n",u->recap);
	write_user(user,text);
	if (user->muzzled & FROZEN) {
          sprintf(text,"~OL~FTNETSEX RECORDED! ~RS%s will get a mesage to do a .revtell.\n",u->recap);
		write_user(user,text);
          sprintf(text,"~OL~FR-> ~FM%s moves %s hands about your body and pushes U on the bed.\n",user->recap,gender);
          write_user(u,text);
          record_tell(u,text);
          sprintf(text,"~OL~FR-> ~FM%s jumps on the bed. . .~FWTRUST ME\n",user->recap);
          write_user(u,text);
          record_tell(u,text);
	  return;
	  }
	sprintf(text,"~OL~FTNETSEX RECORDED! ~RS%s will get a mesage to do a .revtell.\n",u->recap);
	write_user(user,text);
     	u->chkrev=1;
	return;
	}
if (u->malloc_start!=NULL) {
	sprintf(text,"%s is using the editor at the moment.\n",u->recap);
	write_user(user,text);
	if (user->muzzled & FROZEN) {
		sprintf(text,"~OL~FTMESSAGE RECORDED! ~RS%s will get a mesage to do a .revtell.\n",u->recap);
		write_user(user,text);
          sprintf(text,"~OL~FR-> ~FM%s moves %s hands about your body and pushes U on the bed.\n",user->recap,gender);
          write_user(u,text);
          record_tell(u,text);
          sprintf(text,"~OL~FR-> ~FM%s jumps on the bed. . .~FWTRUST ME\n",user->recap);
          write_user(u,text);
          record_tell(u,text);
		return;
		}
	sprintf(text,"~OL~FTMESSAGE RECORDED! ~RS%s will get a mesage to do a .revtell.\n",u->recap);
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

int valid_email(UR_OBJECT user,char *address)
{
int e=0;

write_user(user,"\n~OL~FYChecking Email Address...\n");
if (!address) {
	write_user(user,"~OL~FRWARNING~FW: Nothing To Check!\n");
	return 0;
	}
if (!strstr(address,"@")) {
	write_user(user,"~OL~FRWARNING~FW: ~FTNo Host Name In Email Address! (user@myhost.com)\n");
	e++;
	}
if (!strstr(address,".")) {
	write_user(user,"~OL~FRWARNING~FW: ~FTHostname Incomplete In Email Address!\n");
	e++;
	}
if (address[0]=='@') {
	write_user(user,"~OL~FRWARNING~FW: ~FMNo username in email address!\n");
	e++;
	}
if (strstr(address,"root@")) {
	write_user(user,"~OL~FRWARNING~FW: ~FTRoot Email Accounts Not Permited!\n");
	e++;
	}
if (strstr(address,"localhost")) {
	write_user(user,"~OL~FRWARNING~FW: ~FY\"~FGlocalhost~FY\" ~FTDomains Not Permitted!\n");
	e++;
	}
if (contains_swearing(address)) {
	write_user(user,"~OL~FRWARNING~FW ~FTEmail address contains swearing!\n");
	return -1;
	}
if (!e) {
	write_user(user,"~OL~FGEmail address passed the email check!\n");
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
        write_user(user,"  ~OL~FBÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿\n");
        write_user(user,"  ~OL~FB³    ~RS~FR_.-'~'-._( ~OL~FMUser's Settings Menu ~RS~FR)_.-'~'-._    ~OL~FB³\n");
        write_user(user,"  ~OL~FBÃÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´\n");
        write_user(user,"  ~OL~FB³ ~FTItem:     ~OL~FB³ ~FGItem Description:                    ~OL~FB³\n");
        write_user(user,"  ~OL~FBÃÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´\n");
        write_user(user,"  ~OL~FB³ ~FTage       ~OL~FB³ ~FGSet Your Age                         ~OL~FB³\n");
        write_user(user,"  ~OL~FB³ ~FTansi      ~OL~FB³ ~FGSet High Ascii on/off/test           ~OL~FB³\n");
        write_user(user,"  ~OL~FB³ ~FTbirth     ~OL~FB³ ~FGSet Your Birthdate                   ~OL~FB³\n");
        write_user(user,"  ~OL~FB³ ~FTemail     ~OL~FB³ ~FGSet Your Email Address               ~OL~FB³\n");
        write_user(user,"  ~OL~FB³ ~FTforward   ~OL~FB³ ~FGSet Email Forwarding On/Off          ~OL~FB³\n");
        write_user(user,"  ~OL~FB³ ~FTgender    ~OL~FB³ ~FGSet Your Gender to m/f               ~OL~FB³\n");
        write_user(user,"  ~OL~FB³ ~FThomepage  ~OL~FB³ ~FGSet Your Homepage URL                ~OL~FB³\n");
        write_user(user,"  ~OL~FB³ ~FTicq       ~OL~FB³ ~FGSet Your ICQ Number                  ~OL~FB³\n");
        write_user(user,"  ~OL~FB³ ~FTlastroom  ~OL~FB³ ~FGSet Last Room Warping on/off         ~OL~FB³\n");
        write_user(user,"  ~OL~FB³ ~FTmarried   ~OL~FB³ ~FGSet Marrige Status                   ~OL~FB³\n");
        write_user(user,"  ~OL~FB³ ~FTrecap     ~OL~FB³ ~FGSet Name Recapitalization w/ colors! ~OL~FB³\n");
/*      write_user(user,"  ~OL~FB³ ~FTstartup   ~OL~FB³ ~FGSet Startup Script On/Off   (*)      ~OL~FB³\n"); */
        write_user(user,"  ~OL~FB³ ~FTwho       ~OL~FB³ ~FGSet your who list style              ~OL~FB³\n");
        write_user(user,"  ~OL~FBÃÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´\n");
        write_user(user,"  ~OL~FB³ ~FYUsage:úúúúúúset <item> <value>                   ~OL~FB³\n");
        write_user(user,"  ~OL~FB³ ~FYExample:úúúúset email name@server.com            ~OL~FB³\n");
        write_user(user,"  ~OL~FBÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ\n");
        }
   else {
        write_user(user,"  ~OL~FB+--------------------------------------------------+\n");
        write_user(user,"  ~OL~FB|    ~RS~FR_.-'~'-._( ~OL~FMUser's Settings Menu ~RS~FR)_.-'~'-._    ~OL~FB|\n");
        write_user(user,"  ~OL~FB|--------------------------------------------------|\n");
        write_user(user,"  ~OL~FB| ~FTItem:     ~OL~FB| ~FGItem Description:                    ~OL~FB|\n");
        write_user(user,"  ~OL~FB|-----------+--------------------------------------|\n");
        write_user(user,"  ~OL~FB| ~FTage       ~OL~FB| ~FGSet Your Age                         ~OL~FB|\n");
        write_user(user,"  ~OL~FB| ~FTansi      ~OL~FB| ~FGSet High Ascii on/off/test           ~OL~FB|\n");
        write_user(user,"  ~OL~FB| ~FTbirth     ~OL~FB| ~FGSet Your Birthdate                   ~OL~FB|\n");
/*      write_user(user,"  ~OL~FB| ~FTconfirm   ~OL~FB| ~FGSet Confirmation on/off              ~OL~FB|\n"); */
        write_user(user,"  ~OL~FB| ~FTemail     ~OL~FB| ~FGSet Your Email Address               ~OL~FB|\n");
        write_user(user,"  ~OL~FB| ~FTfakeage   ~OL~FB| ~FGSet Your Fake Age Phrase             ~OL~FB|\n");
        write_user(user,"  ~OL~FB| ~FTforward   ~OL~FB| ~FGSet Email Forwarding On/Off          ~OL~FB|\n");
        write_user(user,"  ~OL~FB| ~FTgender    ~OL~FB| ~FGSet Your Gender To m/f               ~OL~FB|\n");
        write_user(user,"  ~OL~FB| ~FThideroom  ~OL~FB| ~FGSet Your Personal Room Hidden        ~OL~FB|\n");
        write_user(user,"  ~OL~FB| ~FThomepage  ~OL~FB| ~FGSet Your Homepage URL                ~OL~FB|\n");
        write_user(user,"  ~OL~FB| ~FTicq       ~OL~FB| ~FGSet Your ICQ Number                  ~OL~FB|\n");
        write_user(user,"  ~OL~FB| ~FTlastroom  ~OL~FB| ~FGSet Last Room Warping on/off         ~OL~FB|\n");
        write_user(user,"  ~OL~FB| ~FTmarried   ~OL~FB| ~FGSet Marrige Status                   ~OL~FB|\n");
        write_user(user,"  ~OL~FB| ~FTrecap     ~OL~FB| ~FGSet Name Recapitalization w/ colors! ~OL~FB|\n");
/*      write_user(user,"  ~OL~FB| ~FTstartup   ~OL~FB| ~FGSet Startup Script On/Off   (*)      ~OL~FB|\n"); */
        write_user(user,"  ~OL~FB| ~FTwho       ~OL~FB| ~FGSet your who list style              ~OL~FB|\n");
        write_user(user,"  ~OL~FB|--------------------------------------------------|\n");
        write_user(user,"  ~OL~FB| ~FYUsage:......~FGset <item> <value>                   ~OL~FB|\n");
        write_user(user,"  ~OL~FB| ~FYExample:....~FGset email name@server.com            ~OL~FB|\n");
        write_user(user,"  ~OL~FB+--------------------------------------------------+\n");
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
		sprintf(text,"\n~OL~FRThe email address you set, ~FB'~FT%s~FB' ~FRis not a valid email address!\n",user->email);
		write_user(user,text);
		write_user(user,"~OL~FMUse .set email <your address> and set a valid address!\n");
		write_user(user,"~OL~FMThen try turning forwarding on again!");
		user->fmail=0;
		return;
		}
	else {
		sprintf(text,"~OL~FGYour smail will be forwarded to~FW: ~FT%s\n\n",user->email);
		user->fmail=1;
		return;
		}
     }
     if (!strcmp(word[2],"off")) {
	write_user(user,"~OL~FMSmail Forwarding Turned Off...\n\n");
	user->fmail=0;
	return;
	}
     write_user(user,"Usage: .set forward on/off\n");
     return;
     }
if (!strcasecmp(word[1],"startup")) {
     write_user(user,"~OL~FRStartup Scripts Aren't Available Yet.\n");
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
		case 0: write_user(user,"~OL~FBCurrent Mode~FT: ~FGStandard ASCII Character Set.\n");  break;
	  	case 1: write_user(user,"~OL~FBCurrent Mode~FT: ~FGHigh Ascii/ANSI Character Set.\n"); break;
	  	case 2: write_user(user,"~OL~FBCurrent Mode~FT: ~FGHigh Ascii/ANSI Linux Console.\n"); break;
	  	default: write_user(user,"~OL~FBCurrent Mode~FT: ~FGStandard ASCII Character Set.\n");  break;
	  	}
          write_user(user,"\n~OL~FGYou have turned high ASCII~FT/~FGANSI screens ~FMON~FT!\n\n");
          write_user(user,"~FGIf you see oddball characters, you may need to load a font which supports\n");
          write_user(user,"~FGhigh-ascii, i.e. Terminal (Windows Font).  or an IBM PC Character Set.\n\n");
          write_user(user,"~FGLinux users can try typeing .set ansi on -linux\n");
          write_user(user,"~FGbut you MUST be using the local linux console (the actual linux box).\n\n");
          return;
          }
     if (!strncasecmp(word[2],"off",2)) {
          user->high_ascii=0;
	  switch(user->high_ascii) {
		case 0: write_user(user,"~OL~FBCurrent Mode~FT: ~FGStandard ASCII Character Set.\n");  break;
	  	case 1: write_user(user,"~OL~FBCurrent Mode~FT: ~FGHigh Ascii/ANSI Character Set.\n"); break;
	  	case 2: write_user(user,"~OL~FBCurrent Mode~FT: ~FGHigh Ascii/ANSI Linux Console.\n"); break;
	  	default: write_user(user,"~OL~FBCurrent Mode~FT: ~FGStandard ASCII Character Set.\n");  break;
	  	}
          write_user(user,"\n~OL~FGYou have turned high-ascii screens ~FMOFF!\n\n");
          write_user(user,"Every terminal should support this, as it uses standard characters\n");
          write_user(user,"to draw pictures.  Use this if you have problems with the ansi option.\n");
          return;
         }
     if (!strncasecmp(word[2],"test",4)) {
	  switch(user->high_ascii) {
		case 0: write_user(user,"~OL~FBCurrent Mode~FT: ~FGStandard ASCII Character Set.\n");  break;
	  	case 1: write_user(user,"~OL~FBCurrent Mode~FT: ~FGHigh Ascii/ANSI Character Set.\n"); break;
	  	case 2: write_user(user,"~OL~FBCurrent Mode~FT: ~FGHigh Ascii/ANSI Linux Console.\n"); break;
	  	default: write_user(user,"~OL~FBCurrent Mode~FT: ~FGStandard ASCII Character Set.\n");  break;
	  	}
          write_user(user,"\n   If this test is sucessful, you should see 3 boxes, 2 latters and\n");
          write_user(user,"   some fancy brackets around the boxes etc.  If not, you will see a\n");
          write_user(user,"   bunch of letters not making any sence:-)  If this is the case, try\n");
          write_user(user,"   changing the font on your telnet client to an IBM PC compatable\n");
          write_user(user,"   character set.  I.e. the \"Terminal\" font in windows.\n\n");
          write_user(user,"   ~FR°±²Ûß ~OL~FYÚ¿Ö·É»Ì¹Ã´ ~RS~FRßÛ²±°ßßßßßßßßßßßßßßßßßßßßßßßßß°±²Ûß ~OL~FYÚ¿Ö·É»Ì¹Ã´~RS~FR ßÛ²±° \n");
          write_user(user,"   ~FR°±²İ  ~OL~FY³³ººººººÃ´ ~RS~FR Ş²±° ~FTMoenuts High Ascii Test ~RS~FR°±²İ  ~OL~FY³³ººººººÃ´ ~RS~FR Ş²±° \n");
          write_user(user,"   ~FR°±²ÛÜ ~OL~FYÀÙÓ½È¼Ì¹Ã´ ~RS~FRÜÛ²±°ÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜ°±²ÛÜ ~OL~FYÀÙÓ½È¼Ì¹Ã´ ~RS~FRÜÛ²±° \n");
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
                write_user(user,"\n~OL~FRYour age ~FWMUST~FR be greater than ~FT1~FR...\n");
		return;
		}
        if (val>1000) {
                write_user(user,"\n~OL~FRYour age ~FWMUST ~FRbe less than ~FT1000~FR...\n");
		return;
		}
        user->age=val;
        sprintf(text,"\n~OL~FMYour age has been set to:~RS %d\n",user->age);
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
          sprintf(text,"~OL~FRThat style not available Must be 1 - %d.\n",MAX_WHOS);
          write_user(user,text);
          return;
		}
     if (val>MAX_WHOS) {
          sprintf(text,"~OL~FRThat style not available Must be 1 - %d.\n",MAX_WHOS);
          write_user(user,text);
	  who(user,0);
          return;
		}
        user->whostyle=val;
        sprintf(text,"\n~OL~FMYour who style set to:~RS %s\n",who_list_style[user->whostyle]);
	write_user(user,text);
	return;
	}
if (!strcasecmp(word[1],"email")) {
	if (word_count<3) {
                write_user(user,"\nUsage: set email <email address>\n");
		return;
		}
        inpstr=remove_first(inpstr);
        strcpy(user->email,inpstr);
	if (!valid_email(user,user->email)) {
		write_user(user,"~OL~FRE-Mail forwarding turned off...\n");
		user->fmail=0;
		}
        sprintf(text,"\n~OL~FMYou set your email address to~FR:~RS %s\n",user->email);
	write_user(user,text);
	return;
	}
if (!strcasecmp(word[1],"homepage") || !strcasecmp(word[1],"webpage")) {
	if (word_count<3) {
                write_user(user,"\nUsage: set homepage <homepage url>\n");
		return;
		}
        inpstr=remove_first(inpstr);
        strcpy(user->homepage,inpstr);
        sprintf(text,"\n~OL~FMYou set your homepage to~FR:~RS %s\n",user->homepage);
	write_user(user,text);
	return;
	}
if (!strcasecmp(word[1],"icq")) {
	if (word_count<3) {
                write_user(user,"\nUsage: set icq <your ICQ Number>\n");
		return;
		}
        inpstr=remove_first(inpstr);
        strcpy(user->icq,inpstr);
        sprintf(text,"\n~OL~FMYou set your ICQ Number to~FR:~RS %s\n",user->icq);
	write_user(user,text);
	return;
	}
if (!strncasecmp(word[1],"birth",4)) {
	if (word_count<3) {
                write_user(user,"\nUsage: set birthday <MM/DD/YY>\n");
		return;
		}
        inpstr=remove_first(inpstr);
        strcpy(user->birthday,inpstr);
        sprintf(text,"\n~OL~FMYou set your birthday to~FR:~RS %s\n",user->birthday);
	write_user(user,text);
	return;
	}
if (!strcasecmp(word[1],"married")) {
	if (word_count<3) {
                write_user(user,"\nUsage: set married <User's name you're married to>\n");
		return;
		}
        inpstr=remove_first(inpstr);
        strcpy(user->married,inpstr);
        sprintf(text,"\n~OL~FMYou declair you're married to~FR:~RS %s\n",user->married);
	write_user(user,text);
	return;
	}
if (!strcasecmp(word[1],"recap")) {
	if (word_count<3) {
          write_user(user,"Usage  : set recap <recapped name>\n");
		write_user(user,"Note   : Recaped Names can have colors in them!\n"); 
		write_user(user,"Example: set recap ^~FRm^~OLO^~FWe\n");
		write_user(user,"Result : ~FRm~OLO~FWe ~RS~OL~FMsays~FW: ~RSWhateva!\n\n");
		return;
		}
          inpstr=remove_first(inpstr);
	if (strlen(inpstr)>USER_RECAP_LEN-1) {
		write_user(user,"~OL~FRYour Recap Is Too Long!!  ~FMRemove some colors maybe?\n");
		return;
		}
          temp=colour_com_strip(inpstr);
          if (!strcasecmp(temp,user->name)) {
		strncpy(user->recap,inpstr,USER_RECAP_LEN);
          sprintf(text,"\n~OL~FMYou recap your name to~FR:~RS %s\n",user->recap);
		write_user(user,text);
		return;
		}
	sprintf(text,"\n~OL~FRYou recap does not match your name!\n");
	write_user(user,text);
	return;
	}
if (!strcasecmp(word[1],"predesc")) {
	if (word_count<3) {
          write_user(user,"Usage  : set predesc <recapped name>\n");
		write_user(user,"Example: set predesc ^~FRS^~OLi^~FWr\n");
		return;
		}
          inpstr=remove_first(inpstr);
	if (strlen(inpstr)>USER_PREDESC_LEN-1) {
		write_user(user,"~OL~FRYour Pre-Description Is Too Long!!  ~FMRemove some colors maybe?\n");
		return;
		}
	strncpy(user->predesc,inpstr,USER_PREDESC_LEN);
        sprintf(text,"\n~OL~FMYou set your Pre-Description to~FR:~RS %s\n",user->predesc);
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
		write_user(user,"~OL~FRYour gender is already set to Female!\n~OL~FMTalk to an Admin if you need it changed!\n");
		return;
		}
		if (user->prompt & MALE) {
		write_user(user,"~OL~FRYour gender is already set to Male!\n~OL~FMTalk to an Admin if you need it changed!\n");
		return;
		}
		write_user(user,"Your gender has been set to Male\n");
		user->prompt+=MALE;
		autopromote(user);
		return;
		}
	if (!strncasecmp(word[2],"f",1)) {
		if (user->prompt & MALE) {
		write_user(user,"~OL~FRYour gender is already set to Male!\n~OL~FMTalk to an Admin if you need it changed!\n");
		return;
		}
		if (user->prompt & FEMALE) {
		write_user(user,"~OL~FRYour gender is already set to Female!\n~OL~FMTalk to an Admin if you need it changed!\n");
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
          write_user(user,"\n~OL~FGYou will be returned to the room you logged out from\n");
          write_user(user,"~OL~FGIf it is available on your next login, unless you're under arrest\n~OL~FGFor any reason.\n");
          return;
          }
     if (!strcasecmp(word[2],"off")) {
          user->last_room=0;
          write_user(user,"\n~OL~FGYou will be sent to the main room when you login.\n");
          write_user(user,"~OL~FGUnless you're under arrest for any reason.\n");
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
          write_user(user,"\n~OL~FGYou will be asked about certain actions, i.e. Quitting!\n\n");
          return;
          }
     if (!strncasecmp(word[2],"off",2)) {
          user->confirm=0;
          write_user(user,"\n~OL~FGYou will not be asked about certain actions, i.e. Quitting!\n\n");
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
	sprintf(text,"\n~OL~FTYour fake age has been set to: ~RS%s\n",user->fakeage);
	write_user(user,text);
	return;
	}
if (!strcasecmp(word[1],"hideroom")) {
	if (strcasecmp(user->roomname,"none")) {
	   switch(user->hideroom) {
	   case 0: write_user(user,"~FTYour room will now be hidden...\n");
		user->hideroom=1;
		if (!(room=get_room(user->roomname))) {
			write_user(user,"~OL~FRRoom Has Not Been Created Yet!\n");
			}
		else {
			if (!strcasecmp(room->owner,user->name)) {
				sprintf(text,"~FMSetting Room ~FT[~FG%s~FT] ~FMTo Hidden...\n",room->name);
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
			write_user(user,"~OL~FRRoom Has Not Been Created Yet!\n");
			}
		else {
			if (!strcasecmp(room->owner,user->name)) {
				sprintf(text,"~FMSetting Room ~FT[~FG%s~FT] ~FMTo Visible...\n",room->name);
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
	write_user(user,"~OL~FRYou don't have a room to hide!\n");
	return;
	}
   }

/* Set User's BDSM Role */
if (!strcasecmp(word[1],"role")) {
	if (!word[2][0] || !isnumber(word[2]) || !strcasecmp(word[2],"list")) {
		write_user(user,"\n~BM~FY ~ULThe Current Roles Are As Follows:~RS~BM ~RS\n\n");
		for(tint=0;tint<MAX_BDSM_TYPES;tint++) {
			sprintf(text,"~OL~FG%2d~RS~FG... ~FY%-20s ~FW: ~FT%s\n",(tint+1),bdsm_types[tint],bdsm_type_desc[tint]);
			write_user(user,text);
			}
		write_user(user,"\n~OL~FMUsage: .set role [role number]\n");
		}
	bdsmt=atoi(word[2]);
	if (!bdsmt) { write_user(user,"Invalid role!  Type: .set role list to see valid roles!\n"); return; }
	if (bdsmt>MAX_BDSM_TYPES) { write_user(user,"Try again!  Type: .set role list to see valid roles!\n"); return; }
	bdsmt--;
	user->bdsm_type=bdsmt;
	sprintf(text,"~FTYou have set your role to~FM: ~OL~FY%s\n",bdsm_types[user->bdsm_type]);
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
     sprintf(text,"~OL~FT!! ~FY%s ~OL~FYyells ~FT--> ~FG%s\n",name,inpstr);
	write_user(user,text);
	return;
	}
sprintf(text,"~OL~FT!! ~FY%s ~OL~FYyells ~FT--> ~FG%s\n",name,inpstr);
write_room(NULL,text);
}

/** Show Available Ranks **/

void show_ranks(UR_OBJECT user)
{
sprintf(file,"%s",RANKSFILE);
if (!(show_screen(user))) {
     write_user(user,"~FG>~OL> ~FRRanks Not Available At This Time!\n");
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
       write_user(user,"~OL~FRYou can use ~FB'~FT.edit~FB' ~FRto create a room description...\n");
	}
else {
	fgets(line,512,fp);
	while(!feof(fp)) {
                write_user(user,line);
		line[0]=0;
		fgets(line,512,fp);
		}
	fclose(fp);
	}
if (rm->access & PRIVATE) sprintf(text,"\n~OL~FBYou are in the ~FR%s~FB, in the ~FT%s ~FBarea which is ",rm->name,rm->map);
else if (rm->access & PERSONAL) sprintf(text,"\n~FBYou are in ~FT%s~FB's room, \"~FT%s~FB\" in the ~FT%s ~FBarea\n~FBThis room is ",rm->owner,rm->name,rm->map);
else sprintf(text,"\n~OL~FBYou are in the ~FG%s~FB, in the ~FT%s~FB area which is ",rm->name,rm->map);
switch(rm->access) {
     case PUBLIC        : strcat(text,"~FGPublic");  break;
     case PRIVATE       : strcat(text,"~FRPrivate");  break;
     case PERSONAL      : strcat(text,"~FTPersonal");  break;
     case FIXED_PUBLIC  : strcat(text,"~FRfixed ~FGPublic");  break;
     case FIXED_PRIVATE : strcat(text,"~FRfixed ~FRPrivate");  break;
     case FIXED_PERSONAL: strcat(text,"~FRfixed ~FTPersonal");  break;
     }
strcat(text,".\n");
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
     write_user(user,"~FTExits~FW:  ");
     	for(i=0;i<MAX_LINKS;++i) {
     	   if (rm->link[i]==NULL) break;
     	   if (rm->link[i]->access & PERSONAL && user->level<=WIZ) continue;
     	   if (rm->link[i]->access & PRIVATE) sprintf(text,"~FR%-20s  ",rm->link[i]->name);
     	   else if (rm->link[i]->access & PERSONAL) sprintf(text,"~FT%-20s  ",rm->link[i]->name);
     	   else sprintf(text,"~FG%-20s  ",rm->link[i]->name);
     	   ++cnt;
	   if (cnt==3) { strcat(text,"\n        ~OL"); cnt=0; }
     	   write_user(user,text);
 	  }
       }
users=0;
for(u=user_first;u!=NULL;u=u->next) {
     if (u->room!=rm || u==user || (!u->vis && u->level>user->level)) continue;
     if (u->hidden && user->level<OWNER) continue;
     if (!users++) write_user(user,people_here_prompt);
     if (u->afk) ptr=afk; else ptr=null;
     if (!u->vis) sprintf(text,"    ~OL~FR! ~RS%s %s~RS  ~BR%s\n",u->recap,u->desc,ptr);
	else sprintf(text,"      ~RS%s %s~RS  ~FR%s\n",u->recap,u->desc,ptr);
	write_user(user,text);
	}
if (!users) write_user(user,no_people_here_prompt);
write_user(user,"\n");
}

/*** Move to another room ***/
void go(UR_OBJECT user)
{
RM_OBJECT rm;
int i;

if (user->muzzled & JAILED) {
	sprintf(text,"~FT%% ~FMYou cannot use the ~FY\"~FG%s~FY\" ~FMcommand...\n",command[com_num]);
	write_user(user,text); 
	return;
	}
if (user->shackled) { write_user(user,"~OL~FRYou are shackled to this room, you cannot leave!\n");  return; }
if (word_count<2) {
	write_user(user,"Go where?\n");
	return;
	}
if ((rm=get_room(word[1]))==NULL) {
	write_user(user,nosuchroom);
        sprintf(text,room_not_found,word[1]);
	write_user(user,text);
 	return;
	}
if (!strcasecmp(rm->name,"Members_Only") && user->level<MEMBER) {
	sprintf(text,"~OL~FRYou must be a member to enter that room!\n");
	write_user(user,text);
	return;
	}
if (rm==user->room) {
        sprintf(text,already_in_room,rm->name);
 	write_user(user,text);
	return;
	}
/* See if link from current room */
for(i=0;i<MAX_LINKS;++i) {
	if (user->room->link[i]==rm) {
		if (rm->access & PERSONAL && (!(user->invite_room==rm))) {
			if (strcmp(user->name,rm->owner)) {
                    if (user->level<WIZ) sprintf(text,"~OL~FRThe ~RS~OL%s~FR is not adjoined to here.\n",rm->name);
                    else sprintf(text,"~OL~FRThe ~RS~OL%s~FR room is a ~FTpersonal~FR room and you are not the owner.\n",rm->name);
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
if (rm->access & PERSONAL && (user->invite_room==rm)) {
     if (user->level<WIZ) move_user(user,rm,2);
	else move_user(user,rm,1);
	return;
	}
if (user->level<WIZ) {
	sprintf(text,"~OL~FRThe ~FT%s~FR is not adjoined to here.\n",rm->name);
	write_user(user,text);  
	return;
	}
if (rm->access & PERSONAL && user->level<OWNER) {
	if (strcmp(user->name,rm->owner)) {
		sprintf(text,"~OL~FRThe ~FT%s~FR room is a ~FMpersonal~FR room and you are not the owner.\n",rm->name);
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
	write_user(user,"That room is currently private, you cannot enter.\n");  
	return;
	}
/* Reset invite room if in it */
if (user->invite_room==rm) user->invite_room=NULL;
if (user->roombanned && (!strcasecmp(rm->owner,user->name)) && !teleport==-1) {
	sprintf(text,"~OL~FRYou're banned from the %s room, you cannot be moved there!",rm->name);
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
        	sprintf(text,"~FM~OL%s appears from out of nowhere...\n",user->recap);
		write_room(rm,text);
        	sprintf(text,"\n~FT~OL%s disapears into thin air...\n\n",user->recap);
		write_room_except(old_room,text,user);
		}
	goto SKIP;
	}
if (teleport==3) {
	if (user->vis && !user->hidden) {
        sprintf(text,"\n~OL~FM%s %s.~RS\n",user->recap,user->in_phrase);
	write_room(rm,text);
	}
	goto SKIP;
	}

OWNERMOVE:

if (teleport==2) {
        write_user(user,move_prompt_user);
        sprintf(text,move_new_room_prompt,user->recap);
	write_room(rm,text);
        sprintf(text,move_old_room_prompt,user->recap);
        write_room_except(old_room,text,user);
	goto SKIP;
	}
sprintf(text,"\n%s %s.\n",user->recap,user->in_phrase);
write_room(rm,text);
sprintf(text,"\n~OL~FM%s %s to the %s.~RS\n",user->recap,user->out_phrase,rm->name);
write_room_except(user->room,text,user);

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
	write_user(user,"~OL~FRHey... Why not just 'go' there yourself?\n");
	return;
	}
if (u->shackled && u->shackle_level>user->level) {
	write_user(user,"~OL~FRThat user is shackled and you are not powerful enough to move them.\n");
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
	write_user(user,"~OL~FRTrying to move someone of equal or higher level is a little crazy!\n");
	return;
	}
if (rm==u->room) {
	sprintf(text,"~OL%s~FG is already in the ~RS~OL%s.\n",u->recap,rm->name);
	write_user(user,text);
	return;
	};
if (!has_room_access(user,rm) && user->level<OWNER) {
	sprintf(text,"~OL~FRThe %s is currently private, ~RS~OL%s ~FRcannot be moved there.\n",rm->name,u->recap);
	write_user(user,text);  
	return;
	}
if ((rm->access & PERSONAL) && user->level<OWNER) {
	if (!(!strcmp(user->name,rm->owner)) || user->level<OWNER) {
		sprintf(text,"~OL~FRThe ~RS~OL%s ~FRis currently ~FTpersonal, ~RS~OL%s ~FRcannot be moved there.\n",rm->name,u->recap);
		write_user(user,text);  
		return;
		}
	}
if (((rm->access & PERSONAL) && (!strcasecmp(u->name,rm->owner)) && user->roombanned) && user->level==OWNER) {
	if (user->vis) name=user->recap; else name=invisname;
     if (user->hidden) name=hiddenname;
     sprintf(text,illegal_move_prompt,name);
	write_room(user->room,text);
	sprintf(text,"%s illegally moved %s from room %s to room %s\n",user->name,u->name,u->room->name,rm->name);
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
sprintf(text,"%s moved %s from room %s to room %s\n",user->name,u->name,u->room->name,rm->name);
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
if (user->hidden) name="~OL~FR(~FM?~FR)~RS";
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
		sprintf(text,"~OL~FRThe ~RS~OL%s~FR room is a ~FTpersonal~FR room and you are not the owner.\n",rm->name);
		write_user(user,text);
		return;
		}
	}
if (rm->access==PUBLIC) {
	write_user(user,"~OL~FRRoom is public... You cannot uninvite people from here...\n");
	return;
	}
if (!strcasecmp(word[1],"all")) {
	cnt=0;
	write_user(user,"~OL~FRUn-inviting all users invited into this room...\n");
	for(u=user_first;u!=NULL;u=u->next) {
        	if (u->invite_room==rm) {
                	u->invite_room=NULL;
                	sprintf(text,"~OLUninvite: ~FB%s's invite has been removed...\n",u->recap);
                	write_user(user,text);
                	sprintf(text,"~OL~FT>>>> ~FRYour invite to the ~FT%s~FR room has been taken away...\n",rm->name);
                	write_user(u,text);
			++cnt;
               		}
		}
	if (!cnt) write_user(user,"~OL~FWUninvite: ~FRThere was nobody to uninvite!\n");
	else {
		sprintf(text,"~OL~FWUninvite:~FG %d users were uninvited from the room.\n",cnt);
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
	sprintf(text,"~OLUninvite: ~FB%s's invite has been removed...\n",u->recap);
	write_user(user,text);
	sprintf(text,"~OL~FT>>>> ~FRYour invite to the ~FT%s~FR room has been taken away...\n",rm->name);
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
	write_user(user,"~OL~FRYou're currently shackled, so no use knocking!\n");
	return;
	}
if (word_count<2) {
	write_user(user,"~OL~FMKnock on which room's door?\n");
	return;
	}
if ((rm=get_room(word[1]))==NULL) {
	write_user(user,nosuchroom);
	return;
	}
if (rm==user->room) {
	sprintf(text,"~FGYou're already in the ~FT%s~FG room silly!\n",rm->name);
	write_user(user,text);
	return;
	}
if (user->muzzled & FROZEN) {
	sprintf(text,user_knock_prompt,rm->name);
	write_user(user,text); 
	return;
	}
if (rm->access & PERSONAL) {
	sprintf(text,user_knock_prompt,rm->name);
	write_user(user,text);
	sprintf(text,room_knock_prompt,user->recap);
	write_room(rm,text);
	return;
	}	
for(i=0;i<MAX_LINKS;++i) if (user->room->link[i]==rm) goto GOT_IT;
sprintf(text,"~OL~FRThe ~FM%s~FR is not adjoined to here.\n",rm->name);
write_user(user,text);
return;

GOT_IT:
if (!(rm->access & PRIVATE)) {
	sprintf(text,"~OL~FGThe ~RS~OL%s~FG is currently public, try .go, or .join a user there!\n",rm->name);
	write_user(user,text);
	return;
	}
sprintf(text,user_knock_prompt,rm->name);
write_user(user,text);
sprintf(text,user_room_knock_prompt,user->recap,rm->name);
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
	write_user(user,"~OL~FRWho do you wish to invite?\n");
	return;
	}
rm=user->room;
if (!(rm->access & PERSONAL)) {
	if (!(rm->access & PRIVATE)) {
		write_user(user,"~OL~FRThis room is public anyways!~RS\n~OL~FTJust ask them to join you!\n");
		return;
		}
	}
if (!(u=get_user(word[1]))) {
	write_user(user,notloggedon);  return;
	}
if (u==user) {
	write_user(user,"~OL~FRDoes mental insanity run in your family?\n");
	return;
	}
if (u->shackled) {
	write_user(user,"~OL~FRThat user is currently shackled, and cannot leave their current room.\n");
	return;
	}
if (u->room==rm) {
	sprintf(text,"~OL%s ~FGis already here!\n",u->recap);
	write_user(user,text);
	return;
	}
if (u->invite_room==rm) {
	sprintf(text,"~OL%s ~FRhas already been invited into here.\n",u->recap);
	write_user(user,text);
	return;
	}
if (rm->access & PERSONAL) {
	if (strcmp(user->name,rm->owner)) {
		sprintf(text,"~OL~FRThe ~RS~OL%s~FR room is a ~FTpersonal~FR room and you are not the owner.\n",rm->name);
		write_user(user,text);
		return;
		}
	}
sprintf(text,"~FGYou invite ~OL%s ~RS~FGin.\n",u->recap);
write_user(user,text);
if (user->vis) name=user->recap; else name=invisname;
if (!user->hidden) sprintf(text,"~OL~FM%s ~FGhas invited you into the~RS~OL %s.\n",name,rm->name);
else sprintf(text,"~OL~FMAn unseen entity has invited you into ~FG%s\n",rm->name); 
write_user(u,text);
if (user->vis) name=user->recap; else name=invisname;
sprintf(text,"~OL~FM%s ~RS~OL~FBhas invited %s ~RS~OL~FBto join you.\n",name,u->recap);
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
sprintf(text,"~FG~OL%s ~RS~FGhas set the topic to: ~RS%s  ~RS\n",name,inpstr);
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
sprintf(text,"~OL~FM~BG>> Rooms Available <<");
write_user(user,center(text,78));
write_user(user,"\n\n~FTRoom name            : Access  Users  Mesgs  Map   Topic\n\n");
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
        sprintf(text,"%-20s : %9s~RS    %3.3d    %3.3d  %s  %-27.27s\n",rm->name,access,cnt,rm->mesg_cnt,rm->map,colour_com_strip(rm->topic));
        write_user(user,text);
        }
sprintf(text,"\n~OL~FGThere are ~FT%d~FG rooms currently available.\n",rooms);
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
	sprintf(text,"~OL~FGYou are already in the ~RS~OL%s!\n",rm->name);
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
if (user->shackled) { write_user(user,"~OL~FRYou are shackled to this room, you cannot leave!\n");  return; }
rm=u->room;
if (rm==user->room) {
	sprintf(text,"~OL~FGYou are already in the ~FT%s!\n",rm->name);
	write_user(user,text);
	return;
	}
if (rm->access & PERSONAL && (!(user->invite_room==rm))) {
	sprintf(text,"%s ~RS~OL~FRis in a personal room!\n",u->name);
	write_user(user,text);
     sprintf(text,user_join_request);
	write_user(user,text);
        sprintf(text,join_request,user->recap);
	write_room(u->room,text);
	return;
	}
if (rm->access & PRIVATE && (!(user->invite_room==rm))) {
	sprintf(text,"~OL~FRThe ~FT%s ~FRis currently private.\n",rm->name);
	write_user(user,text);
     sprintf(text,user_join_request);
	write_user(user,text);
     sprintf(text,join_request,user->recap);
	write_room(u->room,text);
	return;
  	}
if (!strcasecmp(rm->name,jail_room) && user->level<WIZ) {
	sprintf(text,"~OL~FRThat room is the ~RS~OLJAIL ~FRroom ~RS~OL%s ~FRcould arrest you!\n",u->recap);
	write_user(user,text);
     sprintf(text,join_request,user->recap);
	write_room(u->room,text);
	return;
	}
sprintf(text,"~FM~OL%s ~RS~FM%s to join ~OL%s ~RS~FMin the ~OL%s.\n",user->recap,user->out_phrase,u->recap,rm->name);
write_room_except(user->room,text,user);
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
	write_user(user,"~OL~FGYou are now ignoring everyone.\n");
	sprintf(text,"~OL~FR%s is now ignoring everyone.\n",user->recap);
	write_room_except(user->room,text,user);
	user->ignall=1;
	return;
	}
write_user(user,"~OL~FGYou will now hear everyone again.\n");
sprintf(text,"~OL~FG%s is listening again.\n",user->recap);
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
		write_user(user,"~OL~FGYou are now listening to everything.\n");  
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
	write_user(user,"~OL~FGYou are now listening to all tells and private emotes.\n");  
	user->ignore-=ALL_TELLS;
	return;
	}
if (strstr(word[1],"tells")) {
	write_user(user,"~OL~FRYou were already listening to all tells and private emotes.\n");  
	return;
	}
if (strstr(word[1],"other") && (user->ignore & OUT_ROOM_TELLS)) {
	write_user(user,"~OL~FRYou are listening to tells and private emotes from other rooms.\n");  
	user->ignore-=OUT_ROOM_TELLS;
	return;
	}
if (strstr(word[1],"other")) {
	write_user(user,"~OL~FGYou were already listening to tells and private emotes from other rooms.\n");  
	return;
	}
if (strstr(word[1],"shout") && (user->ignore & SHOUT_MSGS)) {
	write_user(user,"~OL~FGYou are listening to shouts.\n");  
	user->ignore-=SHOUT_MSGS;
	return;
	}
if (strstr(word[1],"shout")) {
	write_user(user,"~OL~FRYou were already listening to shouts.\n");  
	return;
	}
if (strstr(word[1],"log") && (user->ignore & LOGON_MSGS)) {
	write_user(user,"~OL~FGYou are now listening to login mesages.\n");  
	user->ignore-=LOGON_MSGS;
	return;
	}
if (strstr(word[1],"log")) {
	write_user(user,"~OL~FRYou were already listening to login mesages.\n");  
	return;
	}
if (strstr(word[1],"beep") && (user->ignore & BEEP_MSGS)) {
	write_user(user,"~OL~FGYou are listening to beep mesages.\n");  
	user->ignore-=BEEP_MSGS;
	return;
	}
if (strstr(word[1],"beep")) {
	write_user(user,"~OL~FRYou were already listening to beep mesages.\n");  
	return;
	}
if (strstr(word[1],"bcast") && (user->ignore & BCAST_MSGS)) {
	write_user(user,"~OL~FGYou are listening to broadcast mesages.\n");  
	user->ignore-=BCAST_MSGS;
	return;
	}
if (strstr(word[1],"bcast")) {
	write_user(user,"~OL~FRYou were already listening to brodcast mesages.\n");  
	return;
	}
if (strstr(word[1],"picture") && (user->ignore & ROOM_PICTURE)) {
	write_user(user,"~OL~FGYou are listening to pictures.\n");  
	user->ignore-=ROOM_PICTURE;
	return;
	}
if (strstr(word[1],"picture")) {
	write_user(user,"~OL~FRYou were already listening to pictures.\n");  
	return;
	}
if (strstr(word[1],"most") && (user->ignore==MOST_MSGS)) {
	write_user(user,"~OL~FGYou are ignoring almost everything now.\n");  
	user->ignore=ZERO;
	return;
	}
if (strstr(word[1],"most")) {
	write_user(user,"~OL~FRYou were already listening to all most everything except tells and private emotes from your room.\n");  
	return;
	}
if (strstr(word[1],"at") && (user->ignore & ATMOS_MSGS)) {
	write_user(user,"~OL~FGYou are now listening to atmospheres.\n");  
	user->ignore-=ATMOS_MSGS;
	return;
	}
if (strstr(word[1],"at")) {
	write_user(user,"~OL~FRYou were already listening to atmospheres mesages.\n");  
	return;
	}
if (strstr(word[1],"wiz") && (user->ignore & WIZARD_MSGS)) {
	write_user(user,"~OL~FGYou are listening to wiztells and wemotes.\n");  
	user->ignore-=WIZARD_MSGS;
	return;
	}
if (strstr(word[1],"wiz")) {
	write_user(user,"~OL~FGYou were already listening to wiztells and wemotes.\n");  
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
sprintf(text,"~OL~FGYou are now listening tells & pemotes from: ~RS%s\n",u->recap);
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
	write_user(user,"~OL~FRYou are already ignoring all tells and private emotes.\n");  
	return;
	}
if (strstr(word[1],"tells")) {
	write_user(user,"~OL~FGYou are now ignoring all tells and private emotes.\n");  
	user->ignore+=ALL_TELLS;
	return;
	}
if (strstr(word[1],"other") && (user->ignore & OUT_ROOM_TELLS)) {
	write_user(user,"~OL~FRYou are already ignoring tells and private emotes from other rooms.\n");  
	return;
	}
if (strstr(word[1],"other")) {
	write_user(user,"~OL~FGYou are ignoring tells and private emotes from other rooms.\n");  
	user->ignore+=OUT_ROOM_TELLS;
	return;
	}
if (strstr(word[1],"shout") && (user->ignore & SHOUT_MSGS)) {
	write_user(user,"~OL~FRYou are already ignoring shouts.\n");  
	return;
	}
if (strstr(word[1],"shout")) {
	write_user(user,"~OL~FGYou are ignoing shouts.\n");  
	user->ignore+=SHOUT_MSGS;
	return;
	}
if (strstr(word[1],"log") && (user->ignore & LOGON_MSGS)) {
	write_user(user,"~OL~FRYou are already ignoring login mesages.\n");  
	return;
	}
if (strstr(word[1],"log")) {
	write_user(user,"~OL~FGYou are ignoring login mesages.\n");  
	user->ignore+=LOGON_MSGS;
	return;
	}
if (strstr(word[1],"beep") && (user->ignore & BEEP_MSGS)) {
	write_user(user,"~OL~FRYou are already ignoring beep mesages.\n");  
	return;
	}
if (strstr(word[1],"beep")) {
	write_user(user,"~OL~FGYou are ignoring beep mesages.\n");  
	user->ignore+=BEEP_MSGS;
	return;
	}
if (strstr(word[1],"bcast") && (user->ignore & BCAST_MSGS)) {
	write_user(user,"~OL~FRYou are already ignoring broadcast mesages.\n");  
	return;
	}
if (strstr(word[1],"bcast")) {
	write_user(user,"~OL~FGYou are ignoring brodcast mesages.\n");  
	user->ignore+=BCAST_MSGS;
	return;
	}
if (strstr(word[1],"picture") && (user->ignore & ROOM_PICTURE)) {
	write_user(user,"~OL~FRYou are already ignoring pictures.\n");  
	return;
	}
if (strstr(word[1],"picture")) {
	write_user(user,"~OL~FGYou are ignoring pictures.\n");  
	user->ignore+=ROOM_PICTURE;
	return;
	}
if (strstr(word[1],"most") && (user->ignore==MOST_MSGS)) {
	write_user(user,"~OL~FRYou are ignoring everything except tells and private emotes from your room.\n");  
	return;
	}
if (strstr(word[1],"most")) {
	write_user(user,"~OL~FGYou are ignoring everything except tells and private emotes from your room.\n");  
	user->ignore=MOST_MSGS;
	return;
	}
if (strstr(word[1],"wiz") && (user->ignore & WIZARD_MSGS)) {
	write_user(user,"~OL~FRYou are already wiztells and wemotes.\n");  
	return;
	}
if (strstr(word[1],"wiz")) {
	write_user(user,"~OL~FGYou are ignoring wiztells and wemotes.\n");  
	user->ignore+=WIZARD_MSGS;
	return;
	}
if (strstr(word[1],"at") && (user->ignore & ATMOS_MSGS)) {
	write_user(user,"~OL~FRYou are already ignoring atmospheres.\n");  
	return;
	}
if (strstr(word[1],"at")) {
	write_user(user,"~OL~FGYou are ignoring atmospheres.\n");  
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
sprintf(text,"~OL~FGYou are now ignoring tells & pemotes from: ~RS%s\n",u->recap);
write_user(user,text);
strcpy(user->ignuser,u->name);
}

/************************** MISCELLANIOUS COMMANDS ************************/

/*** Switch between command (Ew-too) and speech (nuts) mode ***/
void toggle_mode(UR_OBJECT user)
{
if (user->command_mode) {
     write_user(user,"~OL~FYYou are now in ~FGSpeech~FY mode.\n~OL~FYUse ~FM.mode~FY to switch to ~FREw-Too~FM mode.\n");
	user->command_mode=0;
     return;
	}
write_user(user,"~OL~FTYou are now in ~FREw-Too ~FTmode.\n~OL~FTUse ~FMmode~FT to switch back!\n");
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
     write_user(user,"~OL~FYTime Prompt Is Now Off.  Type .prompt to toggle prompt on/off.\n");
	user->prompt--;  return;
	}
write_user(user,"~OL~FYTime Prompt Is Now On.  Type .prompt to toggle prompt on/off\n");
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
     write_user(user,"~OL~FGAnsi Color has been turned on!\n");
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
if (user->high_ascii) write_user(user,"~FTÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
else write_user(user,"~FT------------------------------------------------------------------------------\n");
for(u=user_first;u!=NULL;u=u->next) {
	if (u->type==CLONE_TYPE) continue;
        if (!u->vis || u->hidden) continue; 
        if (u->level==NEW || u->level==USER) continue;
	if (++cnt==1) {
         	sprintf(text,"\n~FM-~OL=~FR[ ~RSStaff Present at %s %s ~OL~FR]~FM=~RS~FM-\n\n",TALKERNAME,long_date(1));
 	 	write_user(user,text);
	}
        sprintf(text," ~OL~FMName: ~FT%-20.20s ~FMRank: ~FT%-20.20s ~FMLevel: %s\n",colour_com_strip(u->recap),u->level_alias,level_name[u->level]);
	write_user(user,text);
	}
if (!cnt) {
      write_user(user,"There are no staff members currently logged on.\n");
      }
else {
	if (user->high_ascii) write_user(user,"~FRÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
	else write_user(user,"~FR------------------------------------------------------------------------------\n");
     	sprintf(text,"~OL~FGThere are ~FM%d~FG staff members online!\n",cnt);
	write_user(user,text);
	}
if (user->high_ascii) write_user(user,"~FTÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
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
                    		write_user(user,"~OLYou are now AFK with the session locked, enter your password to unlock it.\n");
				if (inpstr[0]) {
					strcpy(user->afk_mesg,inpstr);
					}
				user->afk=2;			
				return;
				}
          	write_user(user,"~OLYou are now AFK with the session locked, enter your password to unlock it.\n");
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
                   sprintf(text,"~OL~FW-~RS~FM %s ~RS~OL~FG%s ~RS~FW- (AFK)\n",user->recap,user->afk_mesg);
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
if (user->vis) {
	if (user->afk_mesg[0]) 
        	sprintf(text,"~OL~FW-~RS~FM %s ~RS~OL~FG%s ~RS~FW- (AFK)\n",user->recap,user->afk_mesg);
                else {
			sprintf(afktext,afk_msg[rand()%MAX_AFK],user->recap);
			sprintf(text,AFKSTRING,afktext);
			}
	write_room(user->room,text);
	sprintf(text,"%s went AFK\n",user->recap);
	write_syslog(text,1);
	}
}


/*** A newbie is requesting an account. Get his email address off him so we
     can validate who he is before we promote him and let him loose as a 
     proper user. ***/
void account_request(UR_OBJECT user,char *inpstr)
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
	write_user(user,"Usage: accreq <an email address we can contact you on + any relevent info>\n");
	return;
	}
/* Could check validity of email address I guess but its a waste of time.
   If they give a duff address they don't get an account, simple. ***/
if (valid_email(user,inpstr)) {
	sprintf(text,"ACCOUNT REQUEST from %s: %s.\n",user->name,inpstr);
	write_syslog(text,1);
	sprintf(text,"~OL~FMAccount Request from ~FT%s: %s\n",user->name,inpstr);
	write_level(WIZ,1,text,NULL);
	write_user(user,"~OL~FTIf the email address you gave is valid, you should receive\n");
	write_user(user,"~OL~FTan email from us shortly welcoming you to our talker!\n");
	user->accreq=1;
	send_email(inpstr,WELCOMEMAIL);	
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
	write_user(user,"~OL~FRYou are not powerfull enough to clear the review buffer here.\n");
	return;
	}
if (user->level>=ARCH) {
	clear_revbuff(rm); 
	sprintf(text,out_of_room_cbuff,rm->name);
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
                sprintf(text,review_header,rm->name);
                write_user(user,center(text,78));
                if (user->high_ascii) write_user(user,"\n~OL~FMÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
                else write_user(user,"~OL~FM------------------------------------------------------------------------------\n");
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
     if (user->high_ascii) write_user(user,"~OL~FMÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
     else write_user(user,"~OL~FM------------------------------------------------------------------------------\n");
     if (grepmode) {
	     sprintf(text,"~OL~FYTotal Lines That Contains ~FT\"%s\"~FB:~FG %d ~FYLines\n",word[2],matches);
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
                    if (user->high_ascii) write_user(user,"\n~OL~FMÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
                    else write_user(user,"\n~OL~FM------------------------------------------------------------------------------\n");
                    }
	  	write_user(user,user->revbuff[line]); 
	  	}
	}
if (!cnt) write_user(user,no_tell_review_prompt);
else {
     if (user->high_ascii) write_user(user,"~OL~FMÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
     else write_user(user,"~OL~FM------------------------------------------------------------------------------\n");
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
	        if (user->high_ascii) write_user(user,"\n~OL~FMÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
                else write_user(user,"\n~OL~FM------------------------------------------------------------------------------\n");
                }
           write_user(user,wizrevbuff[line]);
	   }
	}
if (!cnt) write_user(user,no_wiz_review_prompt);
else {
     if (user->high_ascii) write_user(user,"~OL~FMÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
     else write_user(user,"~OL~FM------------------------------------------------------------------------------\n");
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
                if (user->high_ascii) write_user(user,"\n~OL~FMÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
                else write_user(user,"\n~OL~FM------------------------------------------------------------------------------\n");
               }
          write_user(user,shoutrevbuff[line]);
          }
	}
if (!cnt) {
     write_user(user,no_shout_review_prompt);
	return;
	}
else {
     if (user->high_ascii) write_user(user,"~OL~FMÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
     else write_user(user,"~OL~FM------------------------------------------------------------------------------\n");
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
	sprintf(text,"~OL~FTYou are known as~FW: ~RS%s %s\n",user->recap,user->desc);
	write_user(user,text);
	return;
	}
if (strstr(word[1],"(CLONE)")) {
	write_user(user,"You cannot have that description.\n");
	return;
	}
if (strlen(inpstr)>USER_DESC_LEN-1) write_user(user,"~FRYour description is too long, it has been truncated to fit.\n~RS");
strncpy(user->desc,inpstr,USER_DESC_LEN-1);
sprintf(text,"~OL~FTYou will be known as~FW: ~RS%s %s\n",user->recap,user->desc);
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
write_user(user,"~OL~FGYour Profile Has Been Stored.\n");
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
     sprintf(text,"~OL~FW-> ~FM%s~RS~OL~FM's gender has been set to Male.\n",u->recap);
     write_user(user,text);
     sprintf(text,"~OL~FW-> ~FM%s~RS~OL~FM has set your gender to Male.\n",user->recap);
     write_user(u,text);
     return;
     } 
if (!strncasecmp(word[2],"f",1)) {
     if (u->prompt & FEMALE) { write_user(user,"User's Gender Is Already Set To Female!\n"); return; }
     if (u->prompt & MALE) u->prompt-=MALE;
     u->prompt+=FEMALE;
     sprintf(text,"~OL~FW-> ~FM%s~RS~OL~FM's gender has been set to Female.\n",u->recap);
     write_user(user,text);
     sprintf(text,"~OL~FW-> ~FM%s~RS~OL~FM has set your gender to Female.\n",user->recap);
     write_user(u,text);
     return;
     }
if (!strncasecmp(word[2],"n",1)) {
     if (u->prompt & MALE) u->prompt-=MALE;
     else if (u->prompt & FEMALE) u->prompt-=FEMALE;
     u->prompt+=NEUTER;
     sprintf(text,"~OL~FW-> ~FM%s~RS~OL~FM's gender has been set to Neuter.\n",u->recap);
     write_user(user,text);
     sprintf(text,"~OL~FW-> ~FM%s~RS~OL~FM has set your gender to Neuter.\n",user->recap);
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
if (user->high_ascii) write_user(user,"\n~FTÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ~FM[ ~OL~FGCurrent User Rooms ~RS~FM]~FTÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ\n\n");
else write_user(user,"\n~FT---------------------------~FM[ ~OL~FGCurrent User Rooms ~RS~FM]~FT-----------------------------\n\n");
if (user->high_ascii) sprintf(text,"~FRÄÄÄ~FM[ ~OL~FG%-20s ~RS~FM]~FRÄÄÄÄ·\n",room_first->name);
else sprintf(text,"~FR---~FM[ ~OL~FG%-20s ~RS~FM]~FR----+\n",room_first->name);
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
     if (user->high_ascii) write_user(user,"                               ~FRº\n");
     else write_user(user,"                               ~FR|\n");
     if (cnt==total) {
          if (user->high_ascii) sprintf(text,"                               ~FRÓ~FBÄÄÄ~FT¯ ~OL~FR%s\n",rm->name);
          else sprintf(text,"                               ~FR+~FB---~FT=~FG]~FM> ~OL~FR%s\n",rm->name);
          }
     else {
          if (user->high_ascii) sprintf(text,"                               ~FRÇ~FBÄÄÄ~FT¯ ~OL~FR%s\n",rm->name);
          else sprintf(text,"                               ~FR|~FB---~FT=~FG]~FM> ~OL~FR%s\n",rm->name);
          }
	write_user(user,text);
	}
sprintf(text,"\n~OL~FGPublic Rooms~FT: ~FM%-3d  ~FGPrivate Rooms~FW: ~FM%-3d  ~FGPersonal Rooms~FW: ~FM%-3d  ~FGTotal~FW: ~FM%d\n",pub,priv,per,cnt);
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
        write_user(user,"~OL~FMWhen you enter a room, others will see:\n");
        sprintf(text,"%s %s",user->name,user->in_phrase);
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
sprintf(text,"%s %s",user->name,user->out_phrase);
write_user(user,text);
}

/*** Show who is on ***/
void who(UR_OBJECT user,int style)
{
int req=0;

if (strstr(word[1],"help")) {
        write_user(user,"Usage: who [<#> or <name>]\n");
        write_user(user,"Note : If no arguments specified, your default who style is used.\n\n");               
        write_user(user,"~OL~FGThe Current Who List Stles Are:\n");
        write_user(user,"~FT~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
        write_user(user,"~OL~FT1~FY... ~FMShort Who (Show Only Usernames)\n");
        write_user(user,"~OL~FT2~FY... ~FMHouse Of Pleasure And Ecstacy Who List Style\n");
        write_user(user,"~OL~FT3~FY... ~FMStairway To Heaven Who List Style\n");
        write_user(user,"~OL~FT4~FY... ~FMMoenuts Byroom Who List Style\n\n");
        write_user(user,"~OL~FT5~FY... ~FMDragon's Cove Who List style\n\n");
        write_user(user,"~OL~FT6~FY... ~FMNcohafmuta Who List Style\n");
        return;
        }
req=atoi(word[1]);
if (req>0 && req<7) style=req;
if (!req && !strncasecmp(word[1],"short",5)) style=1;
else if (!req && !strncasecmp(word[1],"hope",4))  style=2;
else if (!req && !strncasecmp(word[1],"stair",5)) style=3;
else if (!req && !strncasecmp(word[1],"byr",3)) style=4;
else if (!req && !strncasecmp(word[1],"room",4)) style=4;
else if (!req && !strncasecmp(word[1],"drago",5)) style=5;
else if (!req && !strncasecmp(word[1],"dcove",5)) style=5;
else if (!req && !strncasecmp(word[1],"ncoh",4))  style=6;

/* We put this here so that arrested/scum ALWAYS get the SHORT WHO list! */
if (user->muzzled & JAILED || user->muzzled & SCUM) style=1;

switch(style) {
     case 1: shortwho(user); break;
     case 2: hopewho(user);  break;
     case 3: stairwho(user); break;
     case 4: byroom(user);   break;
     case 5: dcovewho(user); break;
     case 6: ncohwho(user);  break;
     default:
	write_user(user,"~OL~FGThe Current Who List Stles Are:\n");
	write_user(user,"~FT~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
        write_user(user,"~OL~FT1~FY... ~FMShort Who (Show Only Usernames)\n");
        write_user(user,"~OL~FT2~FY... ~FMHouse Of Pleasure And Ecstacy Who List Style\n");
        write_user(user,"~OL~FT3~FY... ~FMStairway To Heaven Who List Style\n");
        write_user(user,"~OL~FT4~FY... ~FMMoenuts Byroom Who List Style\n");
        write_user(user,"~OL~FT5~FY... ~FMDragon's Cove Who List style\n\n");
        write_user(user,"~OL~FT6~FY... ~FMNcohafmuta Who List Style\n");
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
     u=user;
     sprintf(text,user_ustat_header,TALKERNAME);
     write_user(user,center(text,78));
     if (user->high_ascii) write_user(user,"\n~OL~FMÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
     else write_user(user,"\n~OL~FM------------------------------------------------------------------------------\n");
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
     	if (user->high_ascii) write_user(user,"\n~OL~FMÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
     	else write_user(user,"\n~OL~FM------------------------------------------------------------------------------\n");
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
sprintf(text,"~OL~FBLevel         ~FM: ~FG%-22.22s  ~FBRank          ~FM: ~FG%s\n",levelname,u->level_alias);
write_user(user,text);
if (!offline) { 
	sprintf(text,"~OL~FBWaiting tells ~FM: ~FG%-22.22s",waittell);
	write_user(user,text);
     if (u->ignall) sprintf(text,"  ~OL~FBIgnoring      ~FM: ~FGEverything\n");
     else sprintf(text,"  ~OL~FBIgnoring      ~FM: ~FG%s\n",ignoringstuff);
	write_user(user,text);
	}
sprintf(text,"~OL~FBGender        ~FM: ~FG%-22.22s  ~FBUser Is Online~FM: ~FG%s\n",gendertype,noyes2[hs]);
write_user(user,text);
sprintf(text,"~OL~FBVisible       ~FM: ~FG%-22.22s  ~FBStatus        ~FM: ~FG%s\n",noyes2[u->vis],isbad);
write_user(user,text);
sprintf(text,"~OL~FBUnread mail   ~FM: ~FG%-22.22s  ~FBCharacter Echo~FM: ~FG%s\n",noyes2[has_unread_mail(u)],offon[u->charmode_echo]);
write_user(user,text);
sprintf(text,"~OL~FBAnsi Colour   ~FM: ~FG%-22.22s  ~FBAnsi Emulation~FM: ~FG%s\n",offon[u->colour],offon[u->high_ascii]);
write_user(user,text);
if (!offline) sprintf(text,"~OL~FBLogin Room    ~FM: ~FG%-22.22s  ~FBInvited To    ~FM: ~FG%s\n",u->login_room,ir);
else sprintf(text,"~OL~FBLogin Room    ~FM: ~FG%-22.22s  ~FBLog Into Last ~FM: ~FG%s\n",u->login_room,offon[u->last_room]);
write_user(user,text);
sprintf(text,"~OL~FBDescription   ~FM: ~RS%s\n",u->desc);
write_user(user,text);
sprintf(text,"~OL~FBIn phrase     ~FM: ~RS%s\n",u->in_phrase);
write_user(user,text);
sprintf(text,"~OL~FBOut phrase    ~FM: ~RS%s\n",u->out_phrase);
write_user(user,text);
if (strcasecmp(u->roomname,"none")) {
     	if (user->high_ascii) write_user(user,"~OL~FMÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
     	else write_user(user,"~OL~FM------------------------------------------------------------------------------\n");
	sprintf(text,"~OL~FBRoom Name     ~FM: ~FG%s",u->roomname);
	write_user(user,text);
	if (u->roombanned) write_user(user,"     ~OL~FR-=] ~FMBANNED~FR [=-\n");
	else write_user(user,"\n");
	sprintf(text,"~OL~FBRoom Topic    ~FM: ~RS%s\n",u->roomtopic);
	write_user(user,text);
	if (user->level>WIZ) {
		sprintf(text,"~OL~FBRoom Given By ~FM: ~FG%s\n",u->roomgiver);
		write_user(user,text);
		}
     	if (user->high_ascii) write_user(user,"~OL~FMÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
     	else write_user(user,"~OL~FM------------------------------------------------------------------------------\n");
	}
days=u->total_login/86400;
hours=(u->total_login%86400)/3600;
mins=(u->total_login%3600)/60;
sprintf(text,"~OL~FBTotal login   ~FM: ~FY%d ~FTdays, ~FY%d ~FThours, ~FY%d ~FTminutes\n",days,hours,mins);
write_user(user,text);
if (user->high_ascii) write_user(user,"~OL~FMÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
else write_user(user,"~OL~FM------------------------------------------------------------------------------\n");
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
if (user->high_ascii) write_user(user,"~OL~FMÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
else write_user(user,"~OL~FM------------------------------------------------------------------------------\n");
sprintf(text,"~OL~FYMoenuts version %s - Telnet Chat Server System Status",MNVERSION);
write_user(user,center(text,78));
if (user->high_ascii) write_user(user,"\n~OL~FMÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
else write_user(user,"\n~OL~FM------------------------------------------------------------------------------\n");
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
if (user->high_ascii) write_user(user,"~OL~FMÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
else write_user(user,"~OL~FM------------------------------------------------------------------------------\n");
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
if (user->high_ascii) write_user(user,"~OL~FMÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
else write_user(user,"~OL~FM------------------------------------------------------------------------------\n");
}

/*** Examine a user ***/
void examine(UR_OBJECT user)
{
UR_OBJECT u,u2;
FILE *fp;
char filename[80],line[129],ir[30],levelname[USER_ALIAS_LEN+2],levelname2[50],gendertype[16],genderx[4];
char *profsub();
int new_mail,days,hours,mins,timelen,days2,hours2,mins2,idle;

if (user->muzzled & JAILED) {
        write_user(user,"~FB%% ~FMYou are in jail, you cannot use the ~FB\"~FGexamine~FB\" command...\n");  
	return;
	}
if (word_count<2) strcpy(word[1],user->name);
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
	u2=NULL;
	}
else u2=u;
sprintf(text,examine_style,u->recap,u->desc);
write_user(user,text);
if (user->high_ascii) write_user(user,"~OL~FMÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
else write_user(user,"~OL~FM------------------------------------------------------------------------------\n");
sprintf(filename,"%s/%s.P",USERFILES,u->name);
if (!(fp=fopen(filename,"r"))) write_user(user,no_profile_prompt);
else {
	fgets(line,128,fp);
	while(!feof(fp)) {
		line[strlen(line)-1]=0;
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
if (user->high_ascii) write_user(user,"~OL~FMÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
else write_user(user,"~OL~FM------------------------------------------------------------------------------\n");
if (u2==NULL) {
	sprintf(text,"~OL~FGLevel Alias ~FW: ~FT%-30.30s  ~OL~FGLevel Name  ~FW: ~FT%s\n",levelname,levelname2);
	write_user(user,text);
        sprintf(text,"~OL~FGRole        ~FW: ~FT%s\n",bdsm_types[u->bdsm_type]);
	write_user(user,text);
	sprintf(text,"~OL~FGLast Login  ~FW: %s",ctime((time_t *)&(u->last_login)));
	write_user(user,text);
     	sprintf(text,"~OL~FGWhich was   ~FW: %d days, %d hours, %d minutes ago\n",days2,hours2,mins2);
	write_user(user,text);
          sprintf(text,"~OL~FGWas on for  ~FW: %d hours, %d minutes\n~OL~FGTotal login ~FW:~FM %d days, %d hours, %d minutes\n",u->last_login_len/3600,(u->last_login_len%3600)/60,days,hours,mins);
	write_user(user,text);
     	sprintf(text,"~OL~FGGender      ~FW: %s\n",gendertype);
	write_user(user,text);
     	if (user->level>=WIZ) {
        	sprintf(text,"~OL~FGLast site   ~FW: %s\n",u->last_site);
	  	write_user(user,text);
	  	}
     	strcpy(genderx,"its");
     	if (u->prompt & 4) strcpy(genderx,"her");
     	if (u->prompt & 8) strcpy(genderx,"his");
     	if (new_mail>u->read_mail) {
           sprintf(text,"~OL~FGMail Status ~FW: ~FM%s hasn't read %s new mail yet!\n",u->recap,genderx);
	   write_user(user,text);
	   }
     sprintf(text,"~OL~FGEmail Addy  ~FW: ~FT%s\n",u->email);
     write_user(user,text);
     sprintf(text,"~OL~FGWebpage URL ~FW: ~FT%s\n",u->homepage);
     write_user(user,text);
     sprintf(text,"~OL~FGAge         ~FW: ~FT%s\n",u->fakeage);
     write_user(user,text);
     sprintf(text,"~OL~FGBirth Date  ~FW: ~FT%s\n",u->birthday);
     write_user(user,text);
     if (strcasecmp(u->married,"nobody")) {
          sprintf(text,"~OL~FGMarried To  ~FW: ~FT%s\n",u->married);
          write_user(user,text);
          }
     sprintf(text,"~OL~FGICQ Number  ~FW: ~FT%s\n",u->icq);
     write_user(user,text);
     sprintf(text,"~OL~FGBank Balance~FW: $%d\n",u->bank_balance);
     write_user(user,text);
     sprintf(text,"~OL~FGTic Tac Toe ~FW: ~FMWins~FW:~FT %-5d ~FRLoses~FW:~FT %-5d ~FMDraws~FW:~FT %d\n",u->twin,u->tlose,u->tdraw);
     write_user(user,text);
     if (user->high_ascii) write_user(user,"~OL~FMÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
     else write_user(user,"~OL~FM------------------------------------------------------------------------------\n");
     write_user(user," ~FGFor more information, use: ~OL.ustat <username>\n");
     if (user->high_ascii) write_user(user,"~OL~FMÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
     else write_user(user,"~OL~FM------------------------------------------------------------------------------\n");
     destruct_user(u);
     destructed=0;
     return;
     }
     idle=(int)(time(0) - u->last_input)/60;
     sprintf(text,"~OL~FGLevel Alias  ~FW: ~FT%-30.30s  ~OL~FGLevel Name  ~FW: ~FT%s\n",levelname,levelname2);
     write_user(user,text);
     sprintf(text,"~OL~FGRole         ~FW: ~FT%s\n",bdsm_types[u->bdsm_type]);
     write_user(user,text);
     sprintf(text,"~OL~FGIgnoring all ~FW:~FT %s\n",noyes2[u->ignall]);
     write_user(user,text);
     sprintf(text,"~OL~FGOn since     ~FW:~FM %s~OL~FGOn for       ~FW:~FT %d hours, %d minutes\n",ctime((time_t *)&u->last_login),hours2,mins2);
     write_user(user,text);
     if (u->afk) {
	     sprintf(text,"~OL~FGIdle for     ~FW:~FM %d minutes ~BR(AFK)\n",idle);
	     write_user(user,text);
	     if (u->afk_mesg[0]) {
          	sprintf(text,"~OL~FGAFK message  ~FW: ~FT%s\n",u->afk_mesg);
		write_user(user,text);
		}
	     }
     else {
             sprintf(text,"~OL~FGIdle for     ~FW: ~FM%d minutes\n",idle);
	     write_user(user,text); 
	     }
    sprintf(text,"~OL~FGTotal login  ~FW: ~FM%d days, %d hours, %d minutes\n",days,hours,mins);
    write_user(user,text);
    sprintf(text,"~OL~FGGender       ~FW: %s\n",gendertype);
    write_user(user,text);
    if (user->level>=WIZ) {
        sprintf(text,"~OL~FMSite         ~FW: ~FT%s:%s\n",u->site,u->site_port);
        write_user(user,text);
        }
    if (new_mail>u->read_mail) {
        sprintf(text,"~OL~FGMail Status  ~FW: ~FM%s has unread mail.\n",u->recap);
        write_user(user,text);
        }
     sprintf(text,"~OL~FGEmail Addy   ~FW: ~FT%s\n",u->email);
     write_user(user,text);
     sprintf(text,"~OL~FGWebpage URL  ~FW: ~FT%s\n",u->homepage);
     write_user(user,text);
     sprintf(text,"~OL~FGAge          ~FW: ~FT%s\n",u->fakeage);
     write_user(user,text);
     sprintf(text,"~OL~FGBirth Date   ~FW: ~FT%s\n",u->birthday);
     write_user(user,text);
     if (strcasecmp(u->married,"nobody")) {
          sprintf(text,"~OL~FGMarried To   ~FW: ~FT%s\n",u->married);
          write_user(user,text);
          }
     sprintf(text,"~OL~FGICQ Number   ~FW: ~FT%s\n",u->icq);
     write_user(user,text);
     sprintf(text,"~OL~FGBank Balance ~FW: ~FT$~FM%d\n",u->bank_balance);
     write_user(user,text);
     sprintf(text,"~OL~FGTic Tac Toe  ~FW: ~FMWins~FW:~FT %-5d ~FRLoses~FW:~FT %-5d ~FMDraws~FW:~FT %d\n",u->twin,u->tlose,u->tdraw);
     write_user(user,text);
     if (user->high_ascii) write_user(user,"~OL~FMÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
     else write_user(user,"~OL~FM------------------------------------------------------------------------------\n");
     write_user(user," ~FGFor more information, use: ~OL.ustat <username>\n");
     if (user->high_ascii) write_user(user,"~OL~FMÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
     else write_user(user,"~OL~FM------------------------------------------------------------------------------\n");
}

/*** Do the help ***/
void help(UR_OBJECT user)
{
int ret,len,i;
char filename[80];
char *c,*comword=NULL;

if (user->muzzled & SCUM) {
     if (user->high_ascii) write_user(user,"~OL~FMÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
     else write_user(user,"~OL~FM------------------------------------------------------------------------------\n");
     write_user(user,"~FG                  Commands available for level ~OL~FTCRIM\n");
     if (user->high_ascii) write_user(user,"~OL~FMÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
     else write_user(user,"~OL~FM------------------------------------------------------------------------------\n");
     write_user(user,"~FM  [ ~OL~FTScum~RS~FM ]\n");
     write_user(user,"~RS  quit      look      who        help       say        mode       rules\n");
     write_user(user,"~RS  ranks     time\n");
     if (user->high_ascii) write_user(user,"~OL~FMÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
     else write_user(user,"~OL~FM------------------------------------------------------------------------------\n");
     write_user(user,"~FG For help on a command, type ~OL.help <command>~RS~FG.\n");
     if (user->high_ascii) write_user(user,"~OL~FMÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
     else write_user(user,"~OL~FM------------------------------------------------------------------------------\n");
     return;
     }
if (user->muzzled & JAILED) {
     if (user->high_ascii) write_user(user,"~OL~FMÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
     else write_user(user,"~OL~FM------------------------------------------------------------------------------\n");
     write_user(user,"~FG                  Commands available for level ~OL~FTJAILED\n");
     if (user->high_ascii) write_user(user,"~OL~FMÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
     else write_user(user,"~OL~FM------------------------------------------------------------------------------\n");
     write_user(user,"~FM  [ ~OL~FTJailed~RS~FM ]\n");
     write_user(user,"~RS  look      who        help       say        mode       rules       ranks\n");
     write_user(user,"~RS  time\n");
     if (user->high_ascii) write_user(user,"~OL~FMÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
     else write_user(user,"~OL~FM------------------------------------------------------------------------------\n");
     write_user(user,"~FG For help on a command, type ~OL.help <command>~RS~FG.\n");
     if (user->high_ascii) write_user(user,"~OL~FMÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
     else write_user(user,"~OL~FM------------------------------------------------------------------------------\n");
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
        sprintf(text,"~OL~FB%% ~FRHelp: There is no help on the topic ~FB\"~FY%s~FB\"~FR.\n",word[1]);
	write_user(user,text);
	return;
	}
if (com_level[com_num]>user->level) {
        sprintf(text,"~OL~FB%% ~FRYou do not have access to view the help to the %s command!\n",command[com_num]);
	write_user(user,text);
	return;
	}
sprintf(filename,"%s/%s",HELPFILES,word[1]);
if (!(ret=more(user,user->socket,filename))) {
        sprintf(text,"~OL~FRSorry, could not find the help file ~FB\"~FY%s~FB\".\n",word[1]);
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
if (user->high_ascii) write_user(user,"~OL~FMÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
else write_user(user,"~OL~FM------------------------------------------------------------------------------\n");
sprintf(text,help_header,level_name[user->level]);
write_user(user,center(text,78));
if (user->high_ascii) write_user(user,"\n~OL~FMÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
else write_user(user,"\n~OL~FM------------------------------------------------------------------------------\n");
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
if (user->high_ascii) write_user(user,"~OL~FMÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
else write_user(user,"~OL~FM------------------------------------------------------------------------------\n");
sprintf(text,help_footer1,com,cmds);
write_user(user,text);
write_user(user,help_footer2);
if (user->high_ascii) write_user(user,"~OL~FMÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
else write_user(user,"~OL~FM------------------------------------------------------------------------------\n");
}

/*** Show info on the NUTS code ***/
void help_nuts(UR_OBJECT user)
{
if (user->high_ascii) write_user(user,"~OL~FMÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
else write_user(user,"~OL~FM------------------------------------------------------------------------------\n");
sprintf(text,"~BR~OL~FMMoeNUTS version %s (C)1997,1998 Michael Irving (Moe)~RS\n",MNVERSION);
write_user(user,text);
write_user(user,"~FG   Based on NUTS v3.3.3 (C)1996 Neil Robertson, Portions (C)1997 Reddawg\n"); 
if (user->high_ascii) write_user(user,"~OL~FMÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
else write_user(user,"~OL~FM------------------------------------------------------------------------------\n");
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
if (user->high_ascii) write_user(user,"~OL~FMÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
else write_user(user,"~OL~FM------------------------------------------------------------------------------\n");
write_user(user,"Moenuts is customized NUTS 3.3.3.  It was customized by Michael Irving.  I would\n");
write_user(user,"like to send out SPECIAL thanks to Reddawg and Nerfingdummy Andy, Squirt and Arny for\n");
write_user(user,"helping me with many additions to this code which by myself wouldn't be\n");
write_user(user,"possible and or help with recovering pieces of code I had lost.\n");
if (user->high_ascii) write_user(user,"~OL~FMÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
else write_user(user,"~OL~FM------------------------------------------------------------------------------\n");
write_user(user,"~FG  This version by Moe, Portions by Reddawg, Andy Collington and Rob Melhuish.  Thanks guys!\n");
if (user->high_ascii) write_user(user,"~OL~FMÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
else write_user(user,"~OL~FM------------------------------------------------------------------------------\n");
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
sprintf(text,message_board_header,rm->name);
write_user(user,center(text,78));
write_user(user,"\n\n");
sprintf(filename,"%s/%s.B",DATAFILES,rm->name);
if (!(ret=more(user,user->socket,filename))) {
     sprintf(text,read_no_messages,rm->name);
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
sprintf(text,"PT: %d %d\r~FG~OLFrom   ~FW: ~FT%s        ~RS\n~OL~FGWrote  ~FW:~FT %s\n",(int)(time(0)),newmsg,name,long_date(0));
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
     sprintf(text,"%s wiped the message board in the %s room.\n",user->recap,rm->name);
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
		sprintf(text,"%s wiped all messages from the board in the %s.\n",user->name,rm->name);
		write_syslog(text,1);
		rm->mesg_cnt=0;
		return;
		}
	if (cnt==num) {
		unlink("tempfile"); /* cos it'll be empty anyway */
          write_user(user,wipe_user_all_deleted);
		user->room->mesg_cnt=0;
          sprintf(text,"%s wiped all messages from the board in the %s.\n",user->name,rm->name);
		}
	else {
		rename("tempfile",infile);
		check_messages(user,1);
          sprintf(text,wipe_deleted_top,num);
		write_user(user,text);
		user->room->mesg_cnt-=num;
          sprintf(text,"%s wiped %d messages from the board in the %s.\n",user->name,num,rm->name);
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
		sprintf(text,"%s wiped all messages from the board in the %s.\n",user->name,rm->name);
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
		sprintf(text,"%s wiped all messages from the board in the %s.\n",user->name,rm->name);
		}
	else {
		rename("tempfile",infile);
		check_messages(user,1);
          sprintf(text,wipe_deleted_bottom,total);
		write_user(user,text);
		user->room->mesg_cnt-=total;
		sprintf(text,"%s wiped %d messages from the board in the %s.\n",user->name,total,rm->name);
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
		sprintf(text,"%s wiped all messages from the board in the %s.\n",user->name,rm->name);
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
     sprintf(text,"%s wiped one message from the board in the %s.\n",user->name,rm->name);
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
		sprintf(text,"%s wiped all messages from the board in the %s.\n",user->name,rm->name);
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
     sprintf(text,"%s wiped %d messages from the board in the %s.\n",user->name,total,rm->name);
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
		sprintf(text,"%s wiped all messages from the board in the %s.\n",user->name,rm->name);
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
	sprintf(text,"%s wiped %d message from the board in the %s.\n",user->name,total,rm->name);
	write_syslog(text,1);
     sprintf(text,wipe_deleted_some,name,total);
	write_room_except(rm,text,user);
	return;
	}
write_user(user,"~OL~FRUnknown error in wipe\n");
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
                         sprintf(text,"~BM*** %s ***\n\n",rm->name);
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
     write_user(user,"~OL~FT~BMÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿~RS\n");
     write_user(user,"~OL~FT~BM³                            Your Smail Box                                 ~FT³~RS\n");
     write_user(user,"~OL~FT~BMÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ~RS\n");
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
		write_user(user,"~OL~FRAll messages deleted.\n");
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
		write_user(user,"~OL~FRAll messages deleted.\n");
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
write_user(user,"~OL~FRUnknown error in wipe\n");
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
     write_user(user,"~OL~FT~BMÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿~RS\n");
     write_user(user,"~OL~FT~BM³                   You have mail from the following people                 ~FT³~RS\n");
     write_user(user,"~OL~FT~BMÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ~RS\n");
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
if (user->high_ascii) write_user(user,"\n~OL~FBÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
else write_user(user,"\n~FB-------------------------------------------------------------------------------\n");
sprintf(text,mailbox_total,cnt);
write_user(user,text);
if (user->high_ascii) write_user(user,"~OL~FBÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n\n");
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
	write_user(user,"~OL~FRThat room is ~FTPERSONAL ~FRand you have no invite so you cannot create a clone there.\n");  
	return;
	}
/* Count clones and see if user already has a copy there , no point having 
   2 in the same room */
cnt=0;
for(u=user_first;u!=NULL;u=u->next) {
	if (u->type==CLONE_TYPE && u->owner==user) {
		if (u->room==rm) {
			sprintf(text,"You already have a clone in the %s.\n",rm->name);
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
	sprintf(text,clone_prompt,rm->name);
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
               sprintf(text,"\n~OLSYSTEM: ~FM%s has destroyed your clone in the %s.\n",user->name,rm->name);
			write_user(u2,text);
			}
		destructed=0;
		return;
		}
	}
if (u2==user) sprintf(text,"You do not have a clone in the %s.\n",rm->name);
else sprintf(text,"%s does not have a clone the %s.\n",u2->recap,rm->name);
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
          write_user(user,"~OL~FT~BMÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿~RS\n");
          write_user(user,"~OL~FT~BM³                     Rooms You Currently Have Clones In                     ~FT³~RS\n");
          write_user(user,"~OL~FT~BMÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ~RS\n");
          }
     else {
          write_user(user,"\n~FM-------------------------------------------------------------------------------\n");
          write_user(user,"~FG                       Rooms You Currently Have Clones In\n");
          write_user(user,"~FM-------------------------------------------------------------------------------\n\n");
          }
     }
     sprintf(text,"Room     : %s\n",u->room->name);
	write_user(user,text);
	}
if (!cnt) write_user(user,"You have no clones.\n");
else {
if (user->high_ascii) write_user(user,"~OL~FMÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n\n");
else write_user(user,"~FM-------------------------------------------------------------------------------\n\n");
sprintf(text,"~FRYou have a total of %d clones.\n",cnt);
write_user(user,text);
if (user->high_ascii) write_user(user,"~OL~FMÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n\n");
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
          if (user->high_ascii) write_user(user,"~OL~FMÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
          else write_user(user,"~OL~FM-------------------------------------------------------------------------------\n");
          sprintf(text," ~OL~FTCurrent clones %s \n",long_date(1));
          write_user(user,text);
          if (user->high_ascii) write_user(user,"~OL~FMÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
          else write_user(user,"~OL~FM-------------------------------------------------------------------------------\n");
          }
     sprintf(text,all_clone_style,u->name,u->room);
	write_user(user,text);
	}
     if (user->high_ascii) write_user(user,"~OL~FMÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
     else write_user(user,"~OL~FM-------------------------------------------------------------------------------\n");
     if (!cnt) write_user(user," ~FRThere are no clones on the system.\n");
else {
     sprintf(text," ~FGTotal of %d clones.\n",cnt);
	write_user(user,text);
     }
if (user->high_ascii) write_user(user,"~OL~FMÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n\n");
else write_user(user,"~OL~FM-------------------------------------------------------------------------------\n\n");
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
write_user(user,"~OL~FRYou do not have a clone in that room.\n");
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
     write_user(user,"~OL~FRTrying to commit suicide this way is a sign of madness!\n");
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
sprintf(text,"~OL~FW[~FMWiZ~FW]: ~FM%s's~RS~OL~FR connection was terminated by ~FM%s~RS~FY!!!\n",victim->recap,name);
write_level(WIZ,3,text,NULL);
disconnect_user(victim);
}

/*** Promote a user ***/
void promote(UR_OBJECT user)
{
UR_OBJECT u;
RM_OBJECT rm;
char text2[80],*name;

if (user->temp_level>0) {
     write_user(user,"~OL~FRYou are not permitted to use this command!\n");
     return;
     }
if (word_count<2) {
	write_user(user,"Usage: promote <user>\n");
     return;
	}
/* See if user is on atm */
if ((u=get_user(word[1]))!=NULL) {
     if (u->level==NEW && u->muzzled & SCUM) {
          sprintf(text,"~FG~OLYou promote %s to level: ~RS~OLNEW!~RS.\n",u->recap);
          write_user(user,text);
          if (user->vis) name=user->recap; else name=invisname; 
          sprintf(text,"~FG~OL%s has promotes you to level: ~RS~OLNEW!\n",name);
		write_user(u,text);
          sprintf(text,"%s PROMOTED %s to level NEW.\n",user->name,u->recap);
		write_syslog(text,1);
          sprintf(text,"%s PROMOTED %s to level NEW.\n",user->name,u->recap);
		write_arrestrecord(u,text,1);
		u->muzzled-=SCUM;
		return;
		}
     if (u==user) {
                write_user(user,"Trying to get a higher promotion this way is useless...\n");
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
  them for your own if you add more levels.

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
	sprintf(text,"~FG~OLYou promote %s to level: ~RS~OLNEW!~RS.\n",u->recap);
	write_user(user,text);
	if (user->vis) name=user->recap; else name=invisname;	
	sprintf(text2,"~FG~OL%s has promoted you to level: ~RS~OLNEW!\n",name);
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
	write_user(user,"You cannot promote a user to a level higher than your own.\n");
	destruct_user(u);
	destructed=0;
	return;
	}

/*** See Note Above ***

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
sprintf(text,"~OL~FGYou promote %s to level: ~RS~OL%s.\n",u->recap,level_name[u->level]);
write_user(user,text);
sprintf(text2,"~FG~OLYou have been promoted to level: ~RS~OL%s.\n",level_name[u->level]);
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
     write_user(user,"~OL~FRYou are not permitted to use this command!\n");
     return;
     }
     
if (word_count<2) {
	write_user(user,"Usage: demote <user>\n");
     return;
	}
/* See if user is on atm */
if ((u=get_user(word[1]))!=NULL) {
     if (u->level==NEW && u->muzzled & SCUM) {
		write_user(user,"You can not demote a user to level lower than~OL~FR CRIM~RS.\n");
		return;
		}
     if (u->level==NEW) {
          sprintf(text,"~FM~OLYou demote %s to level: ~RS~OLCRIM~RS.\n",u->recap);
		write_user(user,text);
		if (user->vis) name=user->recap; else name=invisname;	
          sprintf(text,"~FM~OL%s has demoted you to level: ~RS~OLPond Scum!\n",name);
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
		write_user(user,"You can not demote a user to level lower than~OL~FR CRIM~RS.\n");
		destruct_user(u);
		destructed=0;
		return;
		}
     if (u->level==NEW) {
          sprintf(text,"~FM~OLYou demote %s to level: ~RS~OLCRIM~RS.\n",u->name);
		write_user(user,text);
		if (user->vis) name=user->recap; else name=invisname;	
                sprintf(text2,"~FM~OL%s has demoted you to level: ~RS~OLCRIM!\n",name);
		send_mail(user,word[1],text2);
		sprintf(text,"%s DEMOTED %s to level CRIM.\n",user->name,u->name);
		write_syslog(text,1);
		sprintf(text,"%s DEMOTED %s to level CRIM.\n",user->name,word[1]);
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
sprintf(text,"~OL~FRYou demote %s to level: ~RS~OL%s.\n",u->recap,level_name[u->level]);
write_user(user,text);
sprintf(text2,"~FM~OLYou have been demoted to level: ~RS~OL%s.\n",level_name[u->level]);
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
          write_user(user,"~OL~FT~BMÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿~RS\n");
          write_user(user,"~OL~FT~BM³                  Listing Of Banned Sites And Domains                      ~FT³~RS\n");
          write_user(user,"~OL~FT~BMÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ~RS\n");
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
          write_user(user,"~OL~FT~BMÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿~RS\n");
          write_user(user,"~OL~FT~BM³                    Listing Of Banned Users From This Talker                ~FT³~RS\n");
          write_user(user,"~OL~FT~BMÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ~RS\n");
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
          write_user(user,"~OL~FT~BMÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿~RS\n");
          write_user(user,"~OL~FT~BM³                       Listing Of Banned Swear Words                         ~FT³~RS\n");
          write_user(user,"~OL~FT~BMÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ~RS\n");
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
          write_user(user,"~OL~FT~BMÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿~RS\n");
          write_user(user,"~OL~FT~BM³        Listing of Banned Sites And Domains For New Users                   ~FT³~RS\n");
          write_user(user,"~OL~FT~BMÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ~RS\n");
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
     write_user(user,"   i.e. .ban site 198.123.241.  ~OL(omit last number)\n\n");
     write_user(user,"Usage:  .ban new <site to be banned>\n");
     write_user(user,"   i.e. .ban new .theirsite.com\n");
     write_user(user,"   i.e. .ban new 198.123.241.   ~OL(omit last number)\n\n");
     write_user(user,"Usage:  .ban user <username>\n\n");
     return;
     }
if (!strcmp(word[1],"site")) {  ban_site(user);  return;  }
if (!strcmp(word[1],"user")) {  ban_user(user);  return;  }
if (!strcmp(word[1],"new")) {  ban_new(user);  return;  }

/* Neither site/user/new was specified, show usage! */
write_user(user,"Usage:  .ban site <site to be banned>\n");
write_user(user,"   i.e. .ban site .theirsite.com\n");
write_user(user,"   i.e. .ban site 198.123.241.     ~OL(omit last number)\n\n");
write_user(user,"Usage:  .ban new <site to be banned>\n");
write_user(user,"   i.e. .ban new .theirsite.com\n");
write_user(user,"   i.e. .ban new 198.123.241.      ~OL(omit last number)\n\n");
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
if (strstr(word[2],"npiec")) {
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
	write_user(u,"\n\07~FR~OL~LIYou have been banned from here!\n\n");
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
if (!strcmp(word[1],"site")) {  unban_site(user);  return;  }
if (!strcmp(word[1],"user")) {  unban_user(user);  return;  }
if (!strcmp(word[1],"new")) {  unban_new(user);  return;  }
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
char filename[80], text2[ARR_SIZE], *colour_com_strip(), *center();

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
     sprintf(text2,"Viewing %s's Record",colour_com_strip(u->recap));
     if (user->high_ascii) {
          write_user(user,"~OL~FT~BMÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿~RS\n");
          sprintf(text,"~OL~FT~BM³~FG%-78.78s~FT³~RS\n",center(text2,78));
          write_user(user,text);
          write_user(user,"~OL~FT~BMÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ~RS\n");
          }
     else {
          write_user(user,"~FM.-----------------------------------------------------------------------------.\n");
          sprintf(text,"~OL~FT~BM|~FG%-78.78s~FT|~RS\n",center(text2,78));
          write_user(user,text);
          write_user(user,"~FM`-----------------------------------------------------------------------------'\n");
          }
     sprintf(filename,"%s/%s.A",USERFILES,u->name);
	switch(more(user,user->socket,filename)) {
		case 0:
		sprintf(text,"~OL%s ~FRdoes not have a record to view.\n",u->name);
		write_user(user,text);
		return;
		case 1: user->misc_op=2;
		}
     if (user->high_ascii) {
          write_user(user,"~OL~FT~BMÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿~RS\n");
          write_user(user,"~OL~FT~BM³                            End Of User's Record                            ~FT³~RS\n");
          write_user(user,"~OL~FT~BMÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ~RS\n");
          }
     else {
          write_user(user,"~FM-------------------------------------------------------------------------------\n");
          write_user(user,"~FG End of User's Record \n");
          write_user(user,"~FM-------------------------------------------------------------------------------\n\n");
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
	write_user(user,"You cannot view the record a user of equal or higher level than yourself.\n");
	destruct_user(u);
	destructed=0;
	return;
	}
     sprintf(text2,"Viewing %s's Record",colour_com_strip(u->recap));
     if (user->high_ascii) {
          write_user(user,"~OL~FT~BMÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿~RS\n");
          sprintf(text,"~OL~FT~BM³~FG%-78.78s~FT³~RS\n",center(text2,78));
          write_user(user,text);
          write_user(user,"~OL~FT~BMÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ~RS\n");
          }
     else {
          write_user(user,"~FM.-----------------------------------------------------------------------------.\n");
          sprintf(text,"~OL~FT~BM|~FG%-78.78s~FT|~RS\n",center(text2,78));
          write_user(user,text);
          write_user(user,"~FM`-----------------------------------------------------------------------------'\n");
          }
     sprintf(filename,"%s/%s.A",USERFILES,u->name);
     switch(more(user,user->socket,filename)) {
	case 0:
          sprintf(text,"~OL%s ~FRdoes not have a record to view.\n",u->name);
          write_user(user,text);
          destruct_user(u);
          destructed=0;
          if (user->high_ascii) {
               write_user(user,"~OL~FT~BMÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿~RS\n");
               write_user(user,"~OL~FT~BM³                          There is no record to view!                       ~FT³~RS\n");
               write_user(user,"~OL~FT~BMÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ~RS\n");
               }
          else {
               write_user(user,"~FM-------------------------------------------------------------------------------\n");
               write_user(user,"~FG There is no record to view!\n");
               write_user(user,"~FM-------------------------------------------------------------------------------\n\n");
               }
          return;
     case 1: user->misc_op=2;
     if (user->high_ascii) {
          write_user(user,"~OL~FT~BMÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿~RS\n");
          write_user(user,"~OL~FT~BM³                            End Of User's Record                            ~FT³~RS\n");
          write_user(user,"~OL~FT~BMÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ~RS\n");
          }
     else {
          write_user(user,"~FM-------------------------------------------------------------------------------\n");
          write_user(user,"~FG End of User's Record \n");
          write_user(user,"~FM-------------------------------------------------------------------------------\n\n");
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
	sprintf(text,"~OLYou wiztell to level %s:~RS %s\n",level_name[lev],inpstr);
	write_user(user,text);
        sprintf(text,"~OL%s wiztells to level %s:~RS %s\n",user->recap,level_name[lev],inpstr);
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
char *name;

if (user->muzzled) {
	write_user(user,"You are a muzzled Wizard, shame on you!\n");  return;
	}
if (word_count<2 && inpstr[1]<33) {
	write_user(user,"Wizemote what?\n");  return;
	}
name=user->recap;
sprintf(text,wizemote_style,name,inpstr);
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
		write_user(u,"\n                                    ~OL~FY,,, \n");
		write_user(u,"                                   ~OL~FM(~FBo o~FM) \n");
		write_user(u,"~FG-------------------------------~OL~FMoOO~RS~FG--~FT(~OL~FR_~RS~FT)~RS~FG--~OL~FMOOo~RS~FG----------------------------------\n");
		write_user(u,center(inpstr,78));
		write_user(u,"\n~FG------------------------------------------------------------------------------\n\n");
		}
	   else {
		write_user(u,"\n                                    ~OL~FY,,, \n");
		write_user(u,"                                   ~OL~FM(~FBo o~FM) \n");
		write_user(u,"~FG-------------------------------~OL~FMoOO~RS~FG--~FT(~OL~FR_~RS~FT)~RS~FG--~OL~FMOOo~RS~FG----------------------------------\n");
		write_user(u,center(inpstr,78));
		write_user(u,"\n~FG------------------------------------------------------------------------------\n\n");
		}
	    }
	}
else {
	write_level(OWNER,4,"\07\n",NULL);
     if (!user->vis || user->hidden) sprintf(text,"~FB-~FM=~OL[ ~FTBroadcast Message~FM ]~RS~FM=~FB-\n");
     else sprintf(text,"~FB-~FM=~OL[ ~FTBroadcast Message From~FG %s~RS~OL~FM ]~RS~FM=~FB-\n",user->recap);
	write_level(OWNER,4,center(text,78),NULL);
     write_level(OWNER,4,"~OL~FM------------------------------------------------------------------------------\n",NULL);
	write_level(OWNER,4,center(inpstr,78),NULL);
     write_level(OWNER,4,"\n~OL~FM------------------------------------------------------------------------------\n\n",NULL);
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
     strcpy(u->desc,"~OL~FGhas been forgiven...");
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
strcpy(u->desc,"~OL~FRis beign punished...");
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
     else write_room(u->room,"~OL~FRACK!  The Jail Musta Got Blown Up!  The Convict Will Have To Remain Here!\n~OL~FRHowever, they will have limited command access...\n");
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
     sprintf(text,"~FM~OLYou freeze: ~RS~OL%s.\n",u->name);
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
sprintf(text,"~FM~OLYou have frozen: ~RS~OL%s.\n",u->name);
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
	sprintf(text,"~FG~OLYou have released %s from being frozen.\n",u->name);
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
sprintf(text,"~FG~OLYou unfreeze %s.\n",u->name);
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
	sprintf(text,"%s's password has been changed.\n",u->name);
	write_user(user,text);
	if (user->vis) name=user->name; else name=invisname;
        sprintf(text,"~FM~OLYour password has been changed by %s!\n",name);
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
		sprintf(text,"The Value %d is to ~OL~FRLow~RS for Max users. Range: 10-99.\n",val);
		write_user(user,text);
		return;
		}
	if (val>100) {
		sprintf(text,"The Value %d is to ~OL~FRHigh~RS for Max users. Range: 10-99.\n",val);
		write_user(user,text);
		return;
		}
	max_users=val;
        sprintf(text,"~OL~FMYou changed Max users from:~RS %d ~OL~FMto~RS %d.\n",temp,max_users);
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
		sprintf(text,"The Value %d is to ~OL~FRLow~RS for Max clones. Range: 1-6.\n",val);
		write_user(user,text);
		return;
		}
	if (val>6) {
		sprintf(text,"The Value %d is to ~OL~FRHigh~RS for Max clones. Range: 1-6.\n",val);
		write_user(user,text);
		return;
		}
	max_clones=val;
        sprintf(text,"~OL~FMYou changed Max clones from:~RS %d ~OL~FMto~RS %d.\n",temp,max_clones);
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
		sprintf(text,"The Value %d is to ~OL~FRLow~RS for User Idle Time Out. Range: 600-7200.\n",val);
		write_user(user,text);
		return;
		}
	if (val>7200) {
		sprintf(text,"The Value %d is to ~OL~FRHigh~RS for User Idle Time Out. Range: 600-7200.\n",val);
		write_user(user,text);
		return;
		}
	user_idle_time=val;
        sprintf(text,"~OL~FMYou changed User Idle Time Out from:~RS %d sec. ~OL~FMto~RS %d sec.\n",temp,user_idle_time);
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
		sprintf(text,"The Value %d is to ~OL~FRLow~RS for Message Life Time. Range: 1-15.\n",val);
		write_user(user,text);
		return;
		}
	if (val>15) {
		sprintf(text,"The Value %d is to ~OL~FRHigh~RS for Message Life Time. Range: 1-15.\n",val);
		write_user(user,text);
		return;
		}
	mesg_life=val;
        sprintf(text,"~OL~FMYou changed Message Life Time from:~RS %d days ~OL~FMto~RS %d days.\n",temp,mesg_life);
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
		sprintf(text,"Time Out Afks is set to ~OL~FRYES~RS Usage: change afks <yes/no>.\n");
		write_user(user,text);
		return;
		}
	if (!strcasecmp(word[2],"no") && (time_out_afks==0)) {
		sprintf(text,"Time Out Afks is set to ~OL~FRNO~RS Usage: change afks <yes/no>.\n");
		write_user(user,text);
		return;
		}
	if (!strcasecmp(word[2],"yes") && (time_out_afks==0)) {
		time_out_afks=1;
        	sprintf(text,"~OL~FMYou changed Time Out Afks from: ~RSNO ~OL~FMto ~RSYES.\n"); 
		write_user(user,text);
		sprintf(text,"%s changed Time Out Afks from: NO to YES.\n",user->name);
		write_syslog(text,1);
		return;
		}
	if (!strcasecmp(word[2],"no") && (time_out_afks!=0)) {
		time_out_afks=0;
	        sprintf(text,"~OL~FMYou Time Out Afks from: ~RSYES ~OL~FMto ~RSNO.\n"); 
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
		write_user(user,"~OL~FYSuicideing Is Now Allowed\n");
		sys_allow_suicide=1;
		sprintf(text,"%s Turned Suicideing ON.\n",user->name);
		write_syslog(text,1);
		return;
		}
	if (!strcasecmp(word[2],"no") && sys_allow_suicide) {
		write_user(user,"~OL~FGSuicideing Is No Longer Allowed\n");
		sys_allow_suicide=0;
		sprintf(text,"%s Turned Suicideing OFF.\n",user->name);
		write_syslog(text,1);
		return;
		}
	if (!strcasecmp(word[2],"yes") && sys_allow_suicide) {
		write_user(user,"~OL~FRSuicideing is already allowed.  Use .change suicide no to disallow!\n");
		return;
		}
	if (!strcasecmp(word[2],"no") && !sys_allow_suicide) {
		write_user(user,"~OL~FRSuicideing is already disabled.  Use .change suicide yes to allow it!\n");
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
		write_user(user,"~OL~FYAutopromote Has Been Turned On.\n");
		sys_allow_autopromote=1;
		sprintf(text,"%s Turned Auto-Promote ON.\n",user->name);
		write_syslog(text,1);
		return;
		}
	if (!strcasecmp(word[2],"off") && sys_allow_autopromote) {
		write_user(user,"~OL~FGAutopromote Has Been Turned Off.\n");
		sys_allow_autopromote=0;
		sprintf(text,"%s Turned Auto-Promote OFF.\n",user->name);
		write_syslog(text,1);
		return;
		}
	if (!strcasecmp(word[2],"on") && sys_allow_autopromote) {
		write_user(user,"~OL~FRAutopromote Is Already ON.  Use .change autopromote off to disable it!\n");
		return;
		}
	if (!strcasecmp(word[2],"off") && !sys_allow_autopromote) {
		write_user(user,"~OL~FRAutopromote Is Already OFF.  Use .change autopromote on to enable it!\n");
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
		write_user(user,"~OL~FYUse Quote Of The Day.\n");
		SYS_FORTUNE=0;
		sprintf(text,"%s Switched To Quote Of The Day.\n",user->name);
		write_syslog(text,1);
		return;
		}
	if (!strcasecmp(word[2],"1")) {
		write_user(user,"~OL~FGUse System Fortune.\n");
		SYS_FORTUNE=1;
		sprintf(text,"%s Switched To Fortunes.\n",user->name);
		write_syslog(text,1);
		return;
		}
	if (!strcasecmp(word[2],"2")) {
		write_user(user,"~OL~FGUse System Fortune (Offensive Mode).\n");
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
     write_user(user,"~OL~FREvent Logging Cannot Be Turned Off At This Time!\n");
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
sprintf(text,"Minlogin level set to: ~OL%s.\n",levstr);
write_user(user,text);
if (user->vis) name=user->name; else name=invisname;
sprintf(text,"%s has set the minlogin level to: ~OL%s.\n",name,levstr);
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
          write_user(u,"\n~FM~OLYour level is now below the minlogin level, disconnecting you...\n");
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
	sprintf(text,"Access for room %s is now ~FRFIXED.\n",rm->name);
	write_user(user,text);
	if (user->room==rm) {
		sprintf(text,"%s has ~FRFIXED~RS access for this room.\n",name);
		write_room_except(rm,text,user);
		}
	else {
		sprintf(text,"This room's access has been ~FRFIXED.\n");
		write_room(rm,text);
		}
	sprintf(text,"%s FIXED access to room %s.\n",user->name,rm->name);
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
sprintf(text,"Access for room %s is now ~FGUNFIXED.\n",rm->name);
write_user(user,text);
if (user->room==rm) {
	sprintf(text,"%s has ~FGUNFIXED~RS access for this room.\n",name);
	write_room_except(rm,text,user);
	}
else {
	sprintf(text,"This room's access has been ~FGUNFIXED.\n");
	write_room(rm,text);
	}
sprintf(text,"%s UNFIXED access to room %s.\n",user->name,rm->name);
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
          write_user(user,"~OL~FT~BMÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿~RS\n");
          write_user(user,"~OL~FT~BM³                           Viewing The Login Log                           ~FT³~RS\n");
          write_user(user,"~OL~FT~BMÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ~RS\n");
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
          write_user(user,"~OL~FT~BMÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿~RS\n");
          write_user(user,"~OL~FT~BM³                           Viewing The Login Log                           ~FT³~RS\n");
          write_user(user,"~OL~FT~BMÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ~RS\n");
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
               write_user(user,"~OL~FT~BMÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿~RS\n");
               sprintf(text,"~OL~FT~BM³                    Viewing Last %-3d lines of the login log.               ~FT³~RS\n",lines);
               write_user(user,text);
               write_user(user,"~OL~FT~BMÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ~RS\n");
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
if (user->high_ascii) write_user(user,"~OL~FMÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
else write_user(user,"~OL~FM------------------------------------------------------------------------------\n");
fclose(fp);
sprintf(text,"%s: Line count error.\n",syserror);
write_user(user,text);
write_syslog("ERROR: Line count error in viewloginlog().\n",0);
if (user->high_ascii) write_user(user,"~OL~FMÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n\n");
else write_user(user,"~OL~FM------------------------------------------------------------------------------\n\n");
}

/*** View the system log ***/
void viewsyslog(UR_OBJECT user)
{
FILE *fp;
char c,*emp="~FRThe system log is empty.\n";
int lines,cnt,cnt2;

if (word_count==1) {
          if (user->high_ascii) {
               write_user(user,"~OL~FT~BMÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿~RS\n");
               write_user(user,"~OL~FT~BM³                    Viewing The System Log                                 ~FT³~RS\n");
               write_user(user,"~OL~FT~BMÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ~RS\n");
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
       if (user->high_ascii) write_user(user,"~OL~FMÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n\n");
       else write_user(user,"~OL~FM------------------------------------------------------------------------------\n\n");
       return;
       }
if (cnt==lines) {
          if (user->high_ascii) {
               write_user(user,"~OL~FT~BMÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿~RS\n");
               write_user(user,"~OL~FT~BM³                    Viewing The System Log                                ~FT³~RS\n");
               write_user(user,"~OL~FT~BMÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ~RS\n");
               }
          else {
               write_user(user,"~FM------------------------------------------------------------------------------\n");
               write_user(user,"~FG                      Viewing the System Log\n");
               write_user(user,"~FM------------------------------------------------------------------------------\n");
               }
	fclose(fp);  more(user,user->socket,SYSLOG);
     if (user->high_ascii) write_user(user,"~OL~FMÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n\n");
     else write_user(user,"~OL~FM------------------------------------------------------------------------------\n\n");
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
               write_user(user,"~OL~FT~BMÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿~RS\n");
               sprintf(text,"~OL~FT~BM³                    Viewing Last %-3d lines of the login log.               ~FT³~RS\n",lines);
               write_user(user,text);
               write_user(user,"~OL~FT~BMÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ~RS\n");
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
if (user->high_ascii) write_user(user,"~OL~FMÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n\n");
else write_user(user,"~OL~FM------------------------------------------------------------------------------\n\n");
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
     write_user(user,"~OL~FRSuicide Has Been Disabled!\n");
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
write_user(user,"\n\07~FR~OL*** WARNING - This will delete your account! ***\n\nAre you sure about this (y/n)? ");
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
	write_user(user,"\n~FR~OLYour Account Has Been Deleted\n");
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
sprintf(text,"\07~FR~OL~LIUser \"%s\" deleted, Arrest Record Kept For Review!\n",word[1]);
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
	write_room(NULL,"~OLSYSTEM:~RS~FG Shutdown cancelled.\n");
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
write_user(user,"\n\07~FR~OL*** WARNING - This will shutdown the talker! ***\n\nAre you sure about this (y/n)? ");
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
	write_room(NULL,"~OLSYSTEM:~RS~FG Reboot cancelled.\n");
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
write_user(user,"\n\07~FM~OL~LI*** WARNING - This will reboot the talker! ***\n\nAre you sure about this (y/n)? ");
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
if (save_hist()) write_room(NULL,"~OL~FGPoker history has been saved...\n");
else write_room(NULL,"~OL~FRERROR: ~FMCould not save poker history...\n");
if (reboot) {
     write_room(NULL,"\07\n~OLSYSTEM: ~FMRebooting now!!\n\n");
     sprintf(text,"*** REBOOT initiated by %s ***\n",ptr);
     }
else {
     write_room(NULL,"\07\n~OLSYSTEM:~FM Shutting down now!!\n\n");
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
	sprintf(text,"\n!! REBOOT FAILED !! %s: %s\n\n",long_date(1),sys_errlist[errno]);
	write_syslog(text,0);
	exit(12);
	}
/* sprintf(text,"Shutdown Completed %s!\n",long_date(1)); */
sprintf(text,"Moenuts Chat Server Shutdown Complete!\n");
write_syslog(text,1);
exit(0);
}

/**************************** EVENT FUNCTIONS **************************/

void do_events(int sig)
{
int x;
set_date_time();
check_reboot_shutdown();
check_idle_and_timeout();
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
	sprintf(text,"~OLSYSTEM: %s in %d minutes, %d seconds.\n",w[rs_which],rs_countdown/60,rs_countdown%60);
	write_room(NULL,text);
	rs_announce=time(0);
	}
if (rs_countdown<60 && secs>=10) {
	sprintf(text,"~OLSYSTEM: %s in %d seconds.\n",w[rs_which],rs_countdown);
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
	write_room(NULL,"~OLSYSTEM: ~FGRoutine Backup Is Taking Place...");
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
	write_room(NULL,"  ~RS~FB-~OL=~FT] ~FYDone ~FT[~FB=~RS~FB-\n");
	break;

	case 1:
	printf(">> %d board files checked, %d out of date messages found.\n",board_cnt,old_cnt);
	break;

	case 2:
	sprintf(text,"%d board files checked, %d had an incorrect message count.\n",board_cnt,bad_cnt);
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
char *clr[]={"~OL~FM","~OL~FG","~OL~FM","~OL~FY","~OL~FR"};

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
    write_user(user,"\n~OL~FRNOTE:~RS Partial site strings can be given, but NO wildcards.\n");
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
	  sprintf(text,"\n~OL~FT-=+=- ~FYUsers logged in with the same site as ~FG%s~FT -=+=-\n\n",u->name);
	  write_user(user,text);
          }
	sprintf(text,"    %s %s\n",u_loop->name,u_loop->desc);
	if (!u_loop->vis) text[3]='*';
	write_user(user,text);
        }
      }

    if (!found) {
      sprintf(text,"No users currently logged on have that same site as %s.\n",u->name);
      write_user(user,text);
      }
    else {
      sprintf(text,"\nOut of ~FM~OL%d~RS users, ~FM~OL%d~RS had the site as ~FG~OL%s~RS ~FG(%s)\n\n",cnt,same,u->name,u->site);
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
    write_user(user,"Sorry, you are unable to use the ~OLall~RS option at this time.\n");
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
	sprintf(text,"\n~OL~FT-=+=- ~FYAll users from the same site as ~FG%s~FT -=+=-\n\n",u->name);
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
    if (!on) sprintf(text,"\nOut of ~FM~OL%d~RS users, ~FM~OL%d~RS had the site as ~FG~OL%s~RS ~FG(%s)\n\n",cnt,same,u->name,u->last_site);
    else sprintf(text,"\nOut of ~FM~OL%d~RS users, ~FM~OL%d~RS had the site as ~FG~OL%s~RS ~FG(%s)\n\n",cnt,same,u->name,u->site);
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
	sprintf(text,"\n~OL~FT-=+=- ~FYUsers logged in from the same site as ~FG%s~FT -=+=-\n\n",user->samesite_check_store);
	write_user(user,text);
        }
      sprintf(text,"    %s %s\n",u->name,u->desc);
      if (!u->vis) text[3]='*';
      write_user(user,text);
      }
    if (!found) {
      sprintf(text,"There are no users currently logged in with the same site as %s.\n",user->samesite_check_store);
      write_user(user,text);
      }
    else {
      sprintf(text,"\nOut of ~FM~OL%d~RS users, ~FM~OL%d~RS had the site as ~FG~OL%s\n\n",cnt,same,user->samesite_check_store);
      write_user(user,text);
      }
    return;
    }
  /* check all the users.. */
  /* open userlist to check against all users */
  sprintf(filename,"%s/%s",USERFILES,USERLIST);
  if (!(fpi=fopen(filename,"r"))) {
    write_syslog("ERROR: Unable to open userlist in samesite() - stage 2/all.\n",0);
    write_user(user,"Sorry, you are unable to use the ~OLall~RS option at this time.\n");
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
	sprintf(text,"\n~OL~FT-=+=- ~FYAll users that have the site ~FG%s~FT -=+=-\n\n",user->samesite_check_store);
	write_user(user,text);
        }
      sprintf(text,"    %s %s ~RS~FG(%s)\n",u_loop->name,u_loop->desc,u_loop->last_site);
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
    sprintf(text,"No users have the same site as %s.\n",user->samesite_check_store);
    write_user(user,text);
    }
  else {
    if (!on) sprintf(text,"\nOut of ~FM~OL%d~RS users, ~FM~OL%d~RS had the site as ~FG~OL%s~RS\n\n",cnt,same,user->samesite_check_store);
    else sprintf(text,"\nOut of ~FM~OL%d~RS users, ~FM~OL%d~RS had the site as ~FG~OL%s~RS\n\n",cnt,same,user->samesite_check_store);
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
        write_user(user,"~OL~FW>>>>~RS Silly POTATO HEAD\n");  
	return;
	}
if (u->afk) {
     if (u->afk_mesg[0]) {
      sprintf(text,"~OL~FR%s ~FGis ~FMAFK: %s\n",u->recap,u->afk_mesg);
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
        sprintf(text,"~OL~FT%s is ignoring everyone at the moment.\n",u->recap);
	write_user(user,text);  
	return;
	}
if ((u->ignore & ROOM_PICTURE) && user->level<GOD) {
        sprintf(text,"~OL~FT%s is ignoring pictures at the moment.\n",u->recap);
	write_user(user,text);
	return;
	}
if (!strcmp(user->name,u->ignuser) && user->level<GOD) {
        sprintf(text,"~OL~FT%s is ignoring you at the moment.\n",u->recap);
	write_user(user,text);	
	return;
	}
rm=user->room;
urm=u->room;
if ((rm!=urm) && (u->ignore & OUT_ROOM_TELLS) && user->level<GOD) {
        sprintf(text,"~OL~FT%s is ignoring pictures outside the room.\n",u->recap);
	write_user(user,text);	
	return;
	}
if (user->muzzled & FROZEN) {
	inpstr=remove_first(inpstr);
        sprintf(text,"\n~OL~BM~FM[ ~FTYou Send ~FW%s~FG: %s ~FM]~RS\n",u->recap,inpstr);
	write_user(user,text);	
	return;
	}
inpstr=remove_first(inpstr);
/* Check for any illegal crap in searched for filename so they cannot list 
   out the /etc/passwd file for instance. */
c=word[1];
while(*c) {
	if (*c=='.' || *c++=='/') {
		write_user(user,"~OL~FM** ~FRInvalid Filename ~FM**\n");
		sprintf(text,"%s tried to to use %s to hack the system in Pictell.\n",user->name,word[1]);
		write_syslog(text,1);
		return;
		}
	}
sprintf(filename,"%s/%s",PICFILES,inpstr);
if (!(fp=fopen(filename,"r"))) {
     sprintf(text,"~OLPictell~FB:~FT %s ~FMcould not be found...\n",inpstr);
     write_user(user,text);
     return;
     }
else {
     sprintf(text,"\n~OL~BM~FM[ ~FTYou Send ~FW%s~FG: %s ~FM]~RS\n",u->recap,inpstr);
     write_user(user,text);
     if (user->vis) name=user->recap; else name=invisname;
     sprintf(text,"\n~OL~BM~FM[ ~FT%s sends you~FG: %s ~FM]~RS\n\n",name,inpstr);
     write_user(u,text);

     /* show the file */

     fgets(line,255,fp);
	while(!feof(fp)) {
          write_user(u,line);
		fgets(line,255,fp);
		}
	fclose(fp);
	}
write_user(user,"~OL~FGPicture Has Been sent...\n");
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
		write_user(user,"~OL~FM** ~FRInvalid Filename~FM **\n");
		sprintf(text,"%s tried to to use %s to hack the system in Room Picture.\n",user->name,word[1]);
		write_syslog(text,1);
		return;
		}
	}
if (user->muzzled & FROZEN) {
	inpstr=remove_first(inpstr);
     sprintf(text,"\n~OL~BM~FM[ ~FTYou Send The Room~FW:~FG %s ~FM]~RS\n",inpstr);
	write_user(user,text);	
	return;
	}
     sprintf(filename,"%s/%s",PICFILES,inpstr);
     if (!(fp=fopen(filename,"r"))) {
          sprintf(text,"~OLRoompic~FB:~FT %s ~FMcould not be found...\n",inpstr);
          write_user(user,text);
          piclist(user);
          return;
          }
     else {
          sprintf(text,"\n~OL~BM~FM[ ~FTYou Send The Room~FW:~FG %s ~FM]~RS\n",inpstr);
          write_user(user,text);
          if (user->vis) name=user->name; else name=invisname;
          sprintf(text,"\n~OL~BM~FM[ ~FT%s sends everyone in the room~FG: %s ~FM]~RS\n\n",name,inpstr);
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
        case 0: write_user(user,"I was unable to find a pictures list.  Sorry...\n"); return;
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
		write_user(user,"~OL~FM** ~FRInvalid Picture Name~FM **\n");
		sprintf(text,"%s tried to to use %s to hack the system in View Pic.\n",user->name,word[1]);
		write_syslog(text,1);
		return;
		}
	}
     sprintf(filename,"%s/%s",PICFILES,inpstr);
     if (!(fp=fopen(filename,"r"))) {
          sprintf(text,"~OLViewpic~FB:~FT %s ~FMcould not be found...\n",inpstr);
          write_user(user,text);
          piclist(user);
          return;
          }
     else {
          sprintf(text,"\n~OL~BM~FM[ ~FTYou View The Following Picture~FW:~FG %s ~FM]~RS\n",inpstr);
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
if (!(load_atmospheres(room))) for(i=0;i<ATMOSPHERES;++i) room->atmos[i][0]='\0';
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
		sprintf(text,"~OL~FMGive %s ~RS~OL~FM a room called what?!?\n",u->recap);
		write_user(user,text);
        	write_user(user,"Usage: giveroom <user> <roomname>\n");
		return;
        	}
     if (!strcasecmp(word[2],"none")) {
		sprintf(text,"~OL~FW-> ~FRYou have taken ~FM%s~RS~OL~FR's room away.\n",u->recap);
		write_user(user,text);
		write_user(u,"~OL~FW-> ~FRYour room has been taken away!\n");
		if ((rm=get_room(u->roomname))) destruct_room(rm);
		strcpy(u->roomname,"None");
		strcpy(u->roomgiver,user->name); /* Set To Taker's Name */	
		}
	else {
		sprintf(text,"~OL~FW-> ~FGYou have given ~FM%s~RS~OL~FG a room called ~FT%s.\n",u->recap,word[2]);
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
     sprintf(text,"~OL~FMSorry %s~RS~OL~FM...  ~RS~FR-=[ ~OLEviction Notice ~RS~FR]=-\n~OL~FRYour room, ~FM%s~FR, has been taken away for reasons unknown...  ~FBPay your rent?\n",u->recap,u->roomname);
	send_mail(user,word[1],text);
	sprintf(text,"~OL~FW-> ~FRYou have taken ~FM%s~RS~OL~FR's room away.\n",u->recap);
	write_user(user,text);
	}
else {
	sprintf(text,"~OL~FYHey %s!!!  ~FG-=[ Congratulations!!! ]=-\n~OL~FGYou have been given a personal room called ~FT%s.\n",u->recap,u->roomname);
	send_mail(user,word[1],text);
	sprintf(text,"~OL~FW-> ~FGYou have given ~FM%s~RS~OL~FG a room called ~FT%s.\n",u->recap,u->roomname);
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
        write_user(user,"~OL~FWMoeNUTS: ~FR? Huh ~LI?~RS~OL~FR  The main room disapeared!!!\n");
        return;
        }
if (u==user) {
	sprintf(text,"~OL~FT!! %s~RS~OL~FG kicks themself out of their own room!\n~OL~FT!! ~FBWOW!!  ~FMSuch Tallent They Have! :-)\n",user->recap);
	write_room(NULL,text);
	move_user(user,rm,2);
        return;
        }
if (u->room!=user->room) {
        sprintf(text,"~OL~FRYou look around and realize %s~RS~OL~FR isn't in this room with you.\n",u->recap);
        write_user(user,text);
        return;
        }
move_user(u,rm,2);
sprintf(text,"~OL~FT!! ~FY%s yells: ~FR\"AND STAY OUT!!!\" ~FYto ~FM%s\n",user->recap,u->recap);
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
if (strcasecmp(u->roomname,"None")) send_mail(user,word[1],"~FM~OLYou have been banned from your personal room.\n");
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
sprintf(text,"You have un-banned %s from their room.  (~FT%s~RS)\n",u->name,u->roomname);
write_user(user,text);
strcpy(u->site,u->last_site);
save_user_details(u,0);
if (strcasecmp(u->roomname,"None")) send_mail(user,word[1],"~FM~OLYour room ban has been lifted.\n");
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
	write_user(user,"~OL~FGYou are already in your room!\n");
	return;
	}
sprintf(text,"~OL~FM%s %s to the %s\n",user->recap,user->out_phrase,rm->name);
write_room_except(user->room,text,user);
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
	write_user(user,"~OL~FRYou will be no longer hidden from others.\n");  
	user->hidden=0;
	}
else {
	user->hidden=1;  
	write_user(user,"~OL~FGYou will now be hidden from others.\n");
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
	sprintf(text,"~OL~FT%s has already been shackled by a %s!\n",u->recap,level_name[u->shackle_level]);
	write_user(user,text);
	return;
	}
if (u==user) {
     write_user(user,"Trying to shackle yourself is universally stupid!\n");
     return;
     }
if (user->level<=u->level) {
	write_user(user,"~OL~FRYou cannot shackle someone of equal or greater level than youself\n");
	sprintf(text,"~OL~FR%s thought about shackleing you to this room.\n",user->recap);
	write_user(u,text);
	sprintf(text,"%s attempted to SHACKLE %s.\n",user->name,u->name);
	write_syslog(text,1);
	return;
	}
if (user->muzzled & FROZEN) {
     sprintf(text,"~OL~FRYou shackle %s so they cannot leave.\n",rname);
     write_user(user,text);
     return;
	}
sprintf(text,"~OL~FRYou shackle %s so they cannot leave.\n",rname);
write_user(user,text);
sprintf(text,"~OL~FR%s orders the guards to shakle you to the room so you cannot leave.\n",name);
write_user(u,text);
sprintf(text,"~OL~FR%s orders the guards to shackle %s so they cannot leave this room.\n",name,rname);
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
	sprintf(text,"~OL~FT%s is not currently shackled.\n",u->recap);
	write_user(user,text);
	return;
	}
if (u==user) {
     write_user(user,"~FRYou try to remove your own shackles, but to no avail!\n");
     return;
     }
if (user->level<=u->level) {
	write_user(user,"~OL~FRYou cannot un-shackle someone of equal or greater level than youself\n");
	sprintf(text,"~OL~FR%s wanted to un-shackle you.\n",user->name);
	write_user(u,text);
	sprintf(text,"%s attempted to remove %s's shackles.\n",user->name,u->name);
	write_syslog(text,1);
	return;
	}
if (user->muzzled & FROZEN) {
     sprintf(text,"~OL~FRYou release %s from their shackles.\n",rname);
     write_user(user,text);
     return;
     }
sprintf(text,"~OL~FRYou release %s from their shackles.\n",rname);
write_user(user,text);
sprintf(text,"~OL~FR%s orders the guards to release you from your shackles.\n",name);
write_user(u,text);
sprintf(text,"~OL~FR%s orders the guards to release %s from their shackles.\n",name,rname);
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
	write_user(user,"~OL~FRYou cannot make someone of equal or greater level than youself invisible.\n");
	sprintf(text,"~OL~FR%s thought about making you disapear.\n",user->name);
	write_user(u,text);
	sprintf(text,"%s attempted to make %s invisible.\n",user->name,u->name);
	write_syslog(text,1);
	return;
	}
if (!u->vis) {
     write_user(user,"~OL~FTThat user is already invisible.\n");
     return;
     }
if (user->muzzled & FROZEN) {
     sprintf(text,"~OL~FTYou say some fancy schmancy words and make %s disappear.\n",rname);
     write_user(user,text);
     return;
     }
sprintf(text,"~OL~FTYou say some fancy schmancy words and make %s disappear.\n",rname);
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
     write_user(user,"~OL~FTThat user is already visible.\n");
     return;
     }
if (user->level<=u->level) {
	write_user(user,"~OL~FRYou cannot make someone of equal or greater level than youself visible\n");
	sprintf(text,"~OL~FR%s thought about making you visible.\n",user->name);
	write_user(u,text);
	sprintf(text,"%s attempted to make %s visible.\n",user->name,u->name);
	write_syslog(text,1);
	return;
	}
if (user->muzzled & FROZEN) {
     sprintf(text,"~OL~FTYou mumble some fancy schmancy words and make %s re-appear.\n",rname);
     write_user(user,text);
     return;
	}
sprintf(text,"~OL~FTYou mumble some fancy schmancy words and make %s re-appear.\n",rname);
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

     if (user->high_ascii) write_user(user,"\n~OL~FBÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
     else write_user(user,"~OL~FB----------------------------------------------------------------------------\n");     
     write_user(user,"~OL~FT        Moenuts (C)1997 - 1999, Michael Irving, All Rights Reserved.\n");
     if (user->high_ascii) write_user(user,"~OL~FBÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
     else write_user(user,"~OL~FB----------------------------------------------------------------------------\n");     
     sprintf(text,"~OL~FMNUTS version      ~FT: ~FGv%s\n",VERSION);
     write_user(user,text);
     sprintf(text,"~OL~FMMoenuts version   ~FT: ~FGv%s compiled at %s on %s\n",MNVERSION,COMPILE_TIME,COMPILE_DATE);
     write_user(user,text);
     sz=(strlen(COMPILE_FULL)-8);
     sprintf(text,"~OL~FMMoenuts host info ~FT: ~FG%*.*s\n",sz,sz,COMPILE_FULL);
     write_user(user,text);
     if (user->level==OWNER) {
          sprintf(text,"~OL~FMExecutable/Config ~FT: ~FG%s %s\n",progname,confile);
          write_user(user,text);
          }
     sprintf(text,"~OL~FMNumber of Users   ~FT: ~FG%d\n",user_count);
     write_user(user,text);
     sprintf(text,"~OL~FMGame  Library     ~FT: ~FG%s\n",GAMELIBID);
     write_user(user,text);
     sprintf(text,"~OL~FMQuote Library     ~FT: ~FG%s\n",QUOTELIBID);
     write_user(user,text);
     sprintf(text,"~OL~FMAFK   Library     ~FT: ~FG%s\n",AFKLIBID);
     write_user(user,text);
     sprintf(text,"~OL~FMAtmosphere Library~FT: ~FG%s\n",ATMOSLIBID);
     write_user(user,text);
     sprintf(text,"~OL~FMPrompts Library   ~FT: ~FG%s\n",PROMPT_VERSION);
     write_user(user,text);
     if (user->high_ascii) write_user(user,"~OL~FBÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
     else write_user(user,"~OL~FB----------------------------------------------------------------------------\n");
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
        if (!user->accreq) write_user(user,"Use: .accreq <your valid email address>\n"); 
        if (!strcmp(user->desc,DEFAULTDESC)) write_user(user,"Use: .desc <a short description which goes beside your name>\n");
        if (user->age==0) write_user(user,"Use: .set age <1 - 99> to set your age.\n");
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
          sprintf(text,"\n~OL~FG%s has been auto-promoted to level: ~FW%s!",user->name,level_name[user->level]);
/*
          write_room(NULL,text);
          write_room(NULL,"\n                                       ~FM,,, \n");
          write_room(NULL,"                                      ~OL~FM(~FBo o~FM) \n");
          write_room(NULL,"----------------------------------~FMoOO~RS--~FT(~OL~FR_~RS~FT)~RS--~FMOOo~RS-------------------------------\n");
          sprintf(text,autopromote_style,user->name,TALKERNAME); 
          write_room(NULL,center(text,80));
          write_room(NULL,"\n------------------------------------------------------------------------------\n\n");
*/
	  move_user(user,room_first,0); /* Move Newbie To The First (Main) Room */
  	  return;
          }
	  else {
		write_user(user,"~OL~FYYou still need to set the following items:\n");
		write_user(user,"~FY~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
                if (!user->accreq) write_user(user,"Use: .accreq <your real email address>\n"); 
                if (!strcmp(user->desc,DEFAULTDESC)) write_user(user,"Use: .desc <a short description> which goes beside your name\n");
                if (user->age==0) write_user(user,"Use: .set age <1 - 99> to set your age.\n");
                if (!genderset) write_user(user,"Use: .set gender m/f  to set your gender.\n");
		}
     }
}

/* Who List Styles */
void hopewho(UR_OBJECT user)
{
UR_OBJECT u;
UR_OBJECT cu=NULL;

int cnt,total,invis,mins,idle,logins,hidden;
char line[USER_NAME_LEN+USER_DESC_LEN*2];
char rname[ROOM_NAME_LEN+1],levelname[20];
char gender[10],text2[ARR_SIZE+1];

     cnt=0;  total=0;  invis=0;  logins=0;  hidden=0;
     write_user(user,"\n\n");
     sprintf(text2,"People At %s %s",TALKERNAME,long_date(1));
     if (user->high_ascii) {
        write_user(user,"~FB-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-\n");
           sprintf(text,"~FB-=ğ=-~OL~FT%-67.67s~RS~FB-=ğ=-\n",center(text2,66));
        write_user(user,text);
        write_user(user,"~FB-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-\n");
        write_user(user,"~FBº    ~OL~FTRoom Name:   ~RS~FBº~OL~FYG~RS~FBº ~OL~FMRank    ~RS~FBº~OL~FGTime~FW:~FRIdle~RS~FBº ~FTName and Description\n");
        write_user(user,"~FBÇÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ×Ä×ÄÄÄÄÄÄÄÄÄ×ÄÄÄÄÄÄÄÄÄ×ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
        }
     else {
        write_user(user,"~FB_.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._\n");
           sprintf(text,"~FB_.-[ ~OL~FT%-67.67s ~RS~FB]-._\n",center(text2,66));
        write_user(user,text);
        write_user(user,"~FB_.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._\n");
        write_user(user,"~FB|    ~OL~FTRoom Name:   ~RS~FB|~OL~FYG~RS~FB| ~OL~FMRank    ~RS~FB|~OL~FGTime~FW:~FRIdle~RS~FB| ~FTName and Description\n");
        write_user(user,"~FB`.-._.-._.-._.-._.+._.+._.-._.-._.-._.-._.-.+.-._.-._.-._.-._.-._.-._.-._.-._\n");
        }
     for(u=user_first;u!=NULL;u=u->next) {
          if (u->login) {logins++; continue; }
          if (u->room->hidden && user->level<OWNER) continue;
          if (u->type==CLONE_TYPE) cu=get_user(u->name);
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
          strcpy(gender,"~OL~FGN");
          if (u->prompt & FEMALE) strcpy(gender,"~OL~FMF");
          if (u->prompt & MALE)   strcpy(gender,"~OL~FTM");
          if (u->type==CLONE_TYPE) {
               strcpy(gender,"Neuter");
               if (cu->prompt & FEMALE) strcpy(gender,"~OL~FMF");
               if (cu->prompt & MALE)   strcpy(gender,"~OL~FTM");
               }
          ++total;
          if (u->login) logins++;
          if (u->hidden && user->level<OWNER) { total--; hidden++; continue; }
          if (!u->vis) {
               --total; ++invis;
               if (u->level>user->level) continue;
               }
          strcpy(rname,u->room->name);
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
          if (user->high_ascii) sprintf(text,"~RS~FBº ~OL~FT%-15.15s ~RS~FBº~OL~FY%s~RS~FBº ~OL~FM%-7.7s ~RS~FBº~OL~FG%4d~FW:~FR%-4d~RS~FBº~RS%-*.*s\n",rname,gender,levelname,mins,idle,35+cnt*3,35+cnt*3,line);
          else sprintf(text,"~OL~FT  %-15.15s ~RS~FB|~OL~FY%s~RS~FB| ~OL~FM%-7.7s ~RS~FB|~OL~FG%4d~FW:~FR%-4d~RS~FB|~RS%-*.*s\n",rname,gender,levelname,mins,idle,35+cnt*3,35+cnt*3,line);
          write_user(user,text);
          continue;
          }
     if (user->high_ascii) write_user(user,"~FB-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-\n");
     else write_user(user,"~FB_.-._.-._.-._.-._.-._.+._.-._.-._;-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._\n");
     if (user->level>WIZ) {
         sprintf(text,"  ~FRThere are %d invisible and %d people at the login stage.\n",invis,logins);
         write_user(user,text);
         }
    sprintf(text,"  ~OL~FBThere %s currently %d %s in the %s.\n",(total==1?"is":"are"),total,(total==1?"person":"people"),SHORTNAME);
    write_user(user,text);
    if (user->high_ascii) write_user(user,"~FB-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-\n\n");
    else write_user(user,"~FB_.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._\n\n");
}

void stairwho(UR_OBJECT user)
{
UR_OBJECT u;
UR_OBJECT cu=NULL;

int cnt,total,invis,mins,idle,logins,hidden;
char line[USER_NAME_LEN+USER_DESC_LEN*2];
char rname[ROOM_NAME_LEN+1],levelname[20];
char status[6], gender[7],*center();

     total=0;  invis=0;  logins=0;  hidden=0;/* Init Vars to 0 */
     write_user(user,"\n");
     sprintf(text,"~FMPeople roaming %s %s",TALKERNAME,long_date(1));
     write_user(user,center(text,75));
     write_user(user,"\n\n");
     if (user->high_ascii) {
          write_user(user,"~OL~FBÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÂÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÂÄÄÄÄÂÄ¿\n");
          write_user(user,"~OL~FB³   ~FTName And Description                  ~FB³~FTm/f~FB³~FTLevel~FB³~FTRoom      ~FB³~FTMins~FB³~FTIdle~FB³~FTS~FB³\n");
          write_user(user,"~OL~FBÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÁÂÄÂÁÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÅÄÄÄÄÅÄ´\n");
          }
     else {
          write_user(user,"~OL~FB+--------------------------------------------------------------------------+\n");
          write_user(user,"~OL~FB|   ~FTName and Description                  ~FB|~FTm/f~FB|~FTLevel~FB|~FTRoom      ~FB|~FTMins~FB|~FTIdle~FB|~FTS~FB|\n");
          write_user(user,"~OL~FB+------------------------------------------+-+------+----------+----+----+-+\n");
          }
     for(u=user_first;u!=NULL;u=u->next) {
          if (u->login) {logins++; continue; }
          if (u->room->hidden && user->level<OWNER) continue;
          if (u->type==CLONE_TYPE) cu=get_user(u->name);
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
          strcpy(rname,u->room->name);
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
          if (user->high_ascii) sprintf(text,"~OL~FB\263~RS %-*.*s ~RS~OL~FB\263~FM%-1.1s~FB\263~FR%-6.6s~FB\263~FT%-10.10s~FB\263~FY%4.4d~FB\263~FR%4.4d~FB\263~FG%-1.1s~FB\263\n",40+cnt*3,40+cnt*3,line,gender,levelname,rname,mins,idle,status);
          else sprintf(text,"~OL~FB|~RS %-*.*s ~RS~OL~FB|~FM%-1.1s~FB|~FR%-6.6s~FB|~FT%-10.10s~FB|~FY%4.4d~FB|~FR%4.4d~FB|~FG%-1.1s~FB|\n",40+cnt*3,40+cnt*3,line,gender,levelname,rname,mins,idle,status);
          write_user(user,text);
          continue;
          }
     if (user->high_ascii) write_user(user,"~OL~FBÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÁÄÁÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÁÄÄÄÄÁÄÙ\n");
     else write_user(user,"~OL~FB+------------------------------------------+-+------+----------+----+----+-+\n");
     if (user->level>WIZ) {
         sprintf(text,"  ~FRThere are %d invisible users and %d logins.\n",invis,logins);
         write_user(user,text);
         } 
    sprintf(text,"  ~FGThere are %d people roaming around.\n",total);
    write_user(user,text);
    if (user->high_ascii) write_user(user,"~OL~FBÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
    else write_user(user,"~OL~FB----------------------------------------------------------------------------\n");
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
if (user->high_ascii) write_user(user,"~FRÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
else write_user(user,"~FR------------------------------------------------------------------------------\n");
write_user(user," ~OL~FTName            ~FW:  ~FMLevel ~FBLine ~FYVisi ~FRIdle ~FGMins ~FTPort ~FBConnected From\n");
if (user->high_ascii) write_user(user,"~FTÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
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
   if (user->high_ascii) write_user(user,"~FTÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
   else write_user(user,"~FT------------------------------------------------------------------------------\n");
   sprintf(text," ~OL~FTVisible: ~FY%-4d ~FTInvisible: ~FY%-4d ~FTLogins: ~FY%d\n",total,invis,logins);
   write_user(user,text);
   if (user->high_ascii) write_user(user,"~FRÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
   else write_user(user,"~FR------------------------------------------------------------------------------\n");
}

void shortwho(UR_OBJECT user)
{
int ret,cnt,invis,logins,hidden;
char line[USER_NAME_LEN+USER_DESC_LEN*2];
UR_OBJECT u;

cnt=0; ret=0; invis=0; logins=0; hidden=0;
write_user(user,"\n");
sprintf(text,"~FM-~OL=~FR[ ~RSPeople in the %s %s ~OL~FR]~FM=~RS~FM-",TALKERNAME,long_date(1));
write_user(user,center(text,80));
if (user->high_ascii) write_user(user,"\n~OL~FMÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
else write_user(user,"\n~OL~FM------------------------------------------------------------------------------\n");
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
     if (u->room->hidden) sprintf(line,"   ~OL~FY&~OL~FR%-15.15s",colour_com_strip(u->recap));
     else sprintf(line,"    ~OL~FT%-15.15s",colour_com_strip(u->recap));
     if (!u->vis) line[2]='!';
     if (u->hidden) line[1]='#';
     write_user(user,line);
     ret++;
     if (ret==4) { ret=0; write_user(user,"\n"); }
     }        
if (ret>0 && ret<4) write_user(user,"\n");
if (user->high_ascii) write_user(user,"~OL~FMÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
else write_user(user,"~OL~FM------------------------------------------------------------------------------\n");
if (user->level>=WIZ) {
     sprintf(text,"   ~OL~FBThere are ~FM%d ~FBinvisible and ~FM%d ~FBlogins.\n",invis,logins);
     write_user(user,text);
     }
if (user->level==OWNER) { 
     sprintf(text,"   ~OL~FRThere are ~FM%d ~FRhidden users.\n",hidden);
     write_user(user,text);
     }
sprintf(text,"   ~OL~FGThere are ~FM%d~FG users online!\n",cnt);
write_user(user,text);
if (user->high_ascii) write_user(user,"~OL~FMÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
else write_user(user,"~OL~FM------------------------------------------------------------------------------\n");
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
        write_user(user,"~FB-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-\n");
           sprintf(text,"~FB-=ğ=-~OL~FT%-67.67s~RS~FB-=ğ=-\n",center(text2,66));
        write_user(user,text);
        write_user(user,"~FB-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-\n");
        }
     else {
        write_user(user,"~FB_.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._\n");
           sprintf(text,"~FB_.-[ ~OL~FT%-67.67s ~RS~FB]-._\n",center(text2,66));
        write_user(user,text);
        write_user(user,"~FB_.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._\n");
        }
for(rm=room_first;rm!=NULL;rm=rm->next) {
     if (rm->hidden && user->level<OWNER) continue;
     if (inroom(rm)) {  /* Check To See If Room Is Empty */
	sprintf(text,"~FB-~OL=~FT+  ~FG%-67.67s  ~FT+~FB=~RS~FB-\n",center(rm->name,66));
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
          strcpy(gender,"~OL~FGN");
          if (u->prompt & FEMALE) strcpy(gender,"~OL~FMF");
          if (u->prompt & MALE)   strcpy(gender,"~OL~FTM");
          if (u->type==CLONE_TYPE) {
               strcpy(gender,"Neuter");
               if (cu->prompt & FEMALE) strcpy(gender,"~OL~FMF");
               if (cu->prompt & MALE)   strcpy(gender,"~OL~FTM");
               }
          ++total;
          if (u->login) logins++;
          if (u->hidden && user->level<OWNER) { total--; hidden++; continue; }
          if (!u->vis) {
               --total; ++invis;
               if (u->level>user->level) continue;
               }
          strcpy(rname,u->room->name);
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
          if (user->high_ascii) sprintf(text,"~FB-~OL=~FT+ ~FY%s ~RS~FR: ~OL~FM%-12.12s ~RS~FR:~OL~FG%5d~FW:~FR%-5d~RS~FR:~RS%-*.*s ~RS~OL~FT+~FB=~RS~FB-\n",gender,levelname,mins,idle,39+cnt*3,39+cnt*3,line);
          else sprintf(text,"~FB-~OL=~FT+ ~FY%s ~RS~FR: ~OL~FM%-12.12s ~RS~FR:~OL~FG%5d~FW:~FR%-5d~RS~FR:~RS%-*.*s ~RS~OL~FT+~FB=~RS~FB-\n",gender,levelname,mins,idle,39+cnt*3,39+cnt*3,line);
	  write_user(user,text);
          continue;
          }
	}
    }

/*** Display Footer ***/

     if (user->high_ascii) write_user(user,"~FB-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-\n");
     else write_user(user,"~FB_.-._.-._.-._.-._.-._.+._.-._.-._;-._.-._.-._.-._.-._.-._.-._.-._.-._.-._.-._\n");
     if (user->level>WIZ) {
         sprintf(text,"  ~FRThere are %d invisible and %d people at the login stage.\n",invis,logins);
         write_user(user,text);
         }
    sprintf(text,"  ~OL~FBThere %s currently %d %s in the %s.\n",(total==1?"is":"are"),total,(total==1?"person":"people"),SHORTNAME);
    write_user(user,text);
    if (user->high_ascii) write_user(user,"~FB-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-=ğ=-\n\n");
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
sprintf(text,"~OL~FW-> ~FM%s has set your rank to ~FT\"%s\" \n",name,u->level_alias);
write_user(u,text);
sprintf(text,"~OL~FMYou set %s's rank to %s\n",u->name,u->level_alias);
write_user(user,text);
}

void execall(UR_OBJECT user,char *inpstr)
{
UR_OBJECT u;
char temp[ARR_SIZE], cmd[ARR_SIZE];
int cnt;
cnt=0;

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
strncpy(temp,inpstr,ARR_SIZE);
for(u=user_first;u!=NULL;u=u->next) {
	if (u->type==CLONE_TYPE) continue;
	if (u->afk) continue;
	if (u->hidden && user->level<OWNER) continue;
	if (u==user) continue;
	cnt++;
	inpstr[0]='\0';
	sprintf(inpstr,"%s %s %s",cmd,u->name,temp);
	clear_words();
	word_count=wordfind(inpstr);
	exec_com(user,inpstr);
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
    if (user->cpot>20000) {
          craps_getout(user);
	  depositbank(user,user->cpot,0);
	  user->cpot=0;
          user->cwager=0;
	  craps_endgame(user);
          return;
          }
     i=0; die1=0; die2=0; roll=0;
     sprintf(text,"\n~OL~FYMinimum Wager ~FB= ~FM$%-6d  ~FRMaximum Wager ~FB= ~FM$%-6d\n",CRAPS_MINWAGER,bank);
     write_user(user,text);
     sprintf(text,"~FGYou have ~OL~FT$~FM%d ~RS~FGin your pot.\n~FMHow much do you wish to wager? ",bank);
     write_user(user,text);
     user->misc_op=11;
     return;
     }
for(i=1;i<3;++i) {
     roll=i;  die1=0;  die2=0;  j=0;
     if (roll<2) write_user(user,"\n~OL~FMYou pick up the dice, shake them around a couple times then blow\n~OL~FMon them for luck, then throw them down the craps table...\n\n");
     else write_user(user,"\n~OL~FMYou pick the dice up again for your second roll...\n\n");
     while(die1<1) { die1=(rand()%6)+1; }
     while(die2<1) { die2=(rand()%6)+1; }
     for(j=0;j<DICE_HEIGHT;++j) {
	  if (user->high_ascii) sprintf(text,"          %s          %s\n",ansidice[die1-1][j],ansidice[die2-1][j]);
	  else sprintf(text,"          %s          %s\n",asciidice[die1-1][j],asciidice[die2-1][j]);
	  write_user(user,text);
	  }
     lasttotal=total;
     total=die1+die2;
     sprintf(text,"\n~OL~FYTotal For This Roll is ~FM%d\n",total);
     write_user(user,text);
     if (roll<2 && lasttotal==total) {
           user->cpot=user->cpot+user->cwager;
           craps_win(user);
           write_user(user,"\n~OL~FGDo ya' wanna roll again~FY? ");
           user->misc_op=12;
	   return;
           }
      else if (roll<2 && total==7) {
           user->cpot=user->cpot-user->cwager;
           craps_lose(user);
           write_user(user,"\n~OL~FGDo ya' wanna roll again~FY? ");
           user->misc_op=12;
           return;
           }
     else {
	if (roll<2) write_user(user,"~OL~FGThe dealer tosses the dice back to you and says...\"You roll again.\"\n");
	}
     /* Process Second Roll */
     if (roll>1 && (total==7 || total==11)) {
          user->cpot=user->cpot+user->cwager;
          craps_win(user);
          write_user(user,"\n~OL~FGDo ya' wanna roll again~FY? ");
          user->misc_op=12;
	  return;
          }
     else if (roll>1 && (total==2 || total==3 || total==12)) {
          user->cpot=user->cpot-user->cwager;
          craps_lose(user);
	  write_user(user,"\n~OL~FGDo ya' wanna roll again~FY? ");
          user->misc_op=12;
          return;
          }
     else {
	  if (roll>1) write_user(user,"~FMYou didn't lose, but you didn't win either!\n");
	  }
     }
     write_user(user,"\n~OL~FGDo ya' wanna roll again~FY? ");
     user->misc_op=12;
}

void craps_win(UR_OBJECT user)
{
write_user(user,"\n~OL~FGYou have Won!\n");
}

void craps_lose(UR_OBJECT user)
{
write_user(user,"\n~OL~FRYou have lost!\n");
}

void craps_getout(UR_OBJECT user)
{
write_user(user,"\n~OL~FRThe owner of the casino comes over with a couple of his bigest\n");
write_user(user,"~OL~FRgoons and escorts you out of the Casino.  \"YOU'VE WON TOO MUCH!\"\n");
write_user(user,"~OL~FR\"We don't want to see your face back here for a while!!\"\n\n");
}

void craps_poor(UR_OBJECT user)
{
write_user(user,"\n~OL~FBYou're Poor!  I'm afraid your massive gambling habbits have left you with\n");
write_user(user,"~OL~FBan empty wallet and no sence... Er I mean cents... ;-)\n\n");
write_user(user,"~OL~FBYour only recurse now is to come back some other time, or bug a wizard!\n\n");
}

void craps_endgame(UR_OBJECT user)
{
user->cpot=0;
user->cwager=0;
user->ignall=user->ignall_store;
sprintf(text,"~OL~FG%s~RS~OL~FG comes back from the casino...\n",user->recap);
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
	write_user(user,"\n~OL~FYFirst National Bank Account Transaction Record\n");
	write_user(user,"~BW~FK+---------------------------------------------------------------+~RS\n");
	write_user(user,"~BW~FK|  Transaction            | Transaction Amt  | Account Balance  |~RS\n");
	write_user(user,"~BW~FK|=========================|==================|==================|~RS\n");
	sprintf(text,"~BW~FK|  Cash Deposit           |  $%5d.00       |  $%5d.00       |~RS\n",ammount,newamt);  
	write_user(user,text);
	write_user(user,"~BW~FK+---------------------------------------------------------------+~RS\n\n");
	write_user(user,"~OL~FMThank you for using First National...~FGHave A Nice Day!\n\n");
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
	write_user(user,"\n~OL~FYFirst National Bank Account Transaction Record\n");
	write_user(user,"~BW~FK+---------------------------------------------------------------+~RS\n");
	write_user(user,"~BW~FK|  Transaction            | Transaction Amt  | Account Balance  |~RS\n");
	write_user(user,"~BW~FK|=========================|==================|==================|~RS\n");
	sprintf(text,"~BW~FK|  Cash Withdrawl         |  $%5d.00       |  $%5d.00       |~RS\n",ammount,newamt);  
	write_user(user,text);
	write_user(user,"~BW~FK+---------------------------------------------------------------+~RS\n\n");
	write_user(user,"~OL~FMThank you for using First National...~FGHave A Nice Day!\n\n");
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
	write_user(user,"~OL~FRYou cannot give yourself money!  NICE TRY!\n");
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
bank=u->bank_balance;
if (bank>100 && user->level<OWNER) { write_user(user,"They've got more than $100 alreday!\n"); return; }
if (amt<10) { write_user(user,"~OL~FRGawd You're Cheap!  You have to give more than $10!\n"); return; }
if (amt>100 && user->level<OWNER) { write_user(user,"~OL~FRUgh! More than $100?  Wanna break us?\n"); return; }
sprintf(text,"~OL~FG%s ~RS~OL~FGhas just deposited $%d.00 into your bank account.\n",user->recap,amt);
write_user(u,text);
sprintf(text,"~OL~FGYou deposit $%d.00 into %s~RS~OL~FG's account.\n",amt,u->recap);
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
	write_user(user,"~OL~FRLoaning Yourself Money?\n~OL~FMHow does that work?\n");
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
	write_user(user,"~OL~FRYou cannot lend any less than $10.00!\n");
	return;
	}
if (amt>100) {
	write_user(user,"~OL~FRMore than $100?  Make'em work for their money!\n");
	return;
	}
if (amt>bank) {
	write_user(user,"You don't have that kind of money in your bank account!\n");
	return;
	}
sprintf(text,"~OL~FG%s ~RS~OL~FGhas just deposited $%d.00 into your bank account.\n",user->recap,amt);
write_user(u,text);
sprintf(text,"~OL~FGYou deposit $%d.00 into %s~RS~OL~FG's account.\n",amt,u->recap);
write_user(u,text);
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
	write_user(user,"e.g. atmos list      ~OL~FM<=- ~FTList All Atmospheres For This Room.\n");
	write_user(user,"e.g. atmos 1 <text>  ~OL~FM<=- ~FTSet atmosphere #1 to <text> for this room.\n\n");
	return;
	}
rm=user->room;
if (strstr(word[1],"li")) {
	write_user(user,"~OL~FTAtmospheres Set For This Room\n");
	write_user(user,"~OL~FB~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
	for(i=0;i<ATMOSPHERES;++i) {
		sprintf(text,"~OL~FM%2d~FW: ~FG%s\n",(i+1),rm->atmos[i]);
		write_user(user,text);
		}
	write_user(user,"\n~OL~FB-=- ~FGEnd ~FYOf ~FWAtmospheres ~FB-=-\n");
	return;
	}
if (strcasecmp(rm->owner,user->name) && user->level<ARCH) {
	write_user(user,"This is not your room, you cannot edit atmospheres here!\n");
	return;
	}
edat=atoi(word[1]);
if (edat<1 || edat>ATMOSPHERES) {
	sprintf(text,"~OL~FRThat atmosphere doesn't exist.\n~OL~FMYou can only set atmospheres ~OL~FT1~FM to ~FT%d.\n",ATMOSPHERES);
	write_user(user,text);
	return;
	}
inpstr=remove_first(inpstr);
if (!inpstr[0]) return;
if (!strcasecmp(inpstr,"none")) {
     rm->atmos[edat-1][0]='\0';
     write_user(user,"~OL~FRSpecified Atmosphere Was Deleted From This Room!\n");
     if (!save_atmospheres(rm)) write_user(user,"~OL~FWERROR: ~RSFailed to save atmospheres!\n");
     return;
     }
sprintf(text,"~OL~FTAtmosphere~FW: ~FM%-2.2d\n~FR~~~~~~~~~~~~~~\n%s\n\n",edat,inpstr);
write_user(user,text);
strncpy(rm->atmos[edat-1],inpstr,ATMOS_LEN);
if (!save_atmospheres(rm)) write_user(user,"~OL~FWERROR: ~RSFailed to save atmospheres!\n");
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
	    sprintf(text,"/usr/sbin/sendmail %s < %s",send_to,mail_file);
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
	sprintf(text,"~OLSYSTEM: backup_talker(): Failed to fork backup process...\n");
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
	write_room(NULL,"\n~OL~FGSYSTEM: ~FMA Backup Process To Backup all talker files is being\n");
	write_room(NULL,"        ~OL~FMstarted.  This ~FR*may*~FM lag the talker a little...\n\n");
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
	case 0: write_user(user,"~OL~FGWelcome To Moenuts Tic Tac Toe.\n"); 
		write_user(user,"~OL~FGFor a list of commands, enter '~FThelp~FG'.\n");
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
sprintf(text,"~OL~FGGathering Finger Information For~FT: ~FB\"~FT%s~FB\"\n",word[1]);
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
		case 0: write_user(user,"~OL~FRfinger_host(): Could Not Open Temporary Finger File...\n");
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
sprintf(text,"~OL~FGAttempting Name Server Lookup For~FT: ~FB\"~FT%s~FB\"\n",word[1]);
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
      	        sprintf(text,"~OL~FMUsing Name Server~FG: ~FT%s\n",word[2]);
		write_user(user,text);
		sprintf(text,"nslookup %s %s > %s",word[1],word[2],filename);
		}
	    else sprintf(text,"nslookup %s > %s",word[1],filename);
            system(text);
	    switch(more(user,user->socket,filename)) {
		case 0: write_user(user,"~OL~FRnslookup(): Could Not Open Temporary File...\n");
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
	write_user(user,"~OL~FRCalender: Year must be between ~FM1 ~FRand ~FM9999~FR!\n");
	return;
	}
if (atoi(word[1])<1 || atoi(word[1])>9999) {
	write_user(user,"~OL~FRCalender: Year must be between ~FM1 ~FRand ~FM9999~FR!\n");
	return;
	}
sprintf(text,"~OL~FGCreating Calender For The Year ~FT%s\n",word[1]);
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
	write_user(user,"~OL~FRTemporarily promoting yourself?  A little crazy don'cha think?\n");
	return;
	}
if (user->level<=u->level) {
	write_user(user,"~OL~FRYou cannot temporarily promote anyone the same or higher level!\n");
	return;
	}
sprintf(text,"%s~RS~OL~FG's Original Level is %s\n",u->recap,level_name[oldlevel]);
if ((u->level+1)!=OWNER) {
	if (!u->temp_level) u->temp_level=u->level;
        oldlevel=u->temp_level;
	u->level++;
	sprintf(text,"~OL~FGYou have been temporarily promoted to level ~FT%s~FG!\n",level_name[u->level]);
	write_user(u,text);
	sprintf(text,"~OL~FG%s~RS~OL~FG starts to glow as their power increases...\n",u->recap);
	write_room(NULL,text);
	u->temp_level=oldlevel;
	}
else {
	write_user(user,"~OL~FRA user cannot be temp promoted to the highest level!\n");
	return;
	}
}

/***************************************************************************/
/*** Moesoft's Soft Socials, (C)1998 Michael Irving, All Rights Reserved ***/
/***************************************************************************/

int do_socials(UR_OBJECT user,char *inpstr)
{
UR_OBJECT u;
FILE *fp;
char filename[81], text2[ARR_SIZE];
char social[6][ARR_SIZE];
char line[ARR_SIZE],*comword=NULL;
int i,s,nos,retcode=0;

if (user->level<USER || (user->muzzled & JAILED) || (user->muzzled)) { return 0; }

i=0; s=0; nos=0;
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
    line[strlen(line)-1]=0;
    strcpy(social[i],line);
    if (!strcmp(line,"*")) nos++;
    }
  }
  if (!strncasecmp(comword,social[0],strlen(comword))) {
    if (word_count<2) {
	sprintf(text,social[2],user->recap);
	strcat(text,"\n");
	write_room(user->room,text);
	record(user->room,text);
	retcode=1;
	fclose(fp);
	return retcode;
	}
    if (word_count>1) {
	if (!(u=get_user(word[1]))) {
	  sprintf(text,social[4],user->recap,inpstr);
	  strcat(text,"\n");
	  write_room(user->room,text);
	  record(user->room,text);
	  retcode=1;
	  fclose(fp);
	  return retcode;
	  }
     else {
	if (u==user) {
	  sprintf(text,"~OL~FRYou cannot use ~FB'~FT%s~FB'~FR on yourself!\n",social[0]);
	  write_user(user,text);
	  retcode=1;
	  fclose(fp);
	  return retcode;
	  }
	sprintf(text2,social[3],user->recap,u->recap);
	if (strstr(social[1],"personal")) {
           sprintf(text,"~OL~FM(~FBsocial~FM)~FW-> ~RS%s\n",text2);
	   write_user(u,text);
           record_tell(u,text);
           sprintf(text,"~OL~FW<~FGSent To ~FT%s~RS~OL~FW>\n~FM(~FBsocial~FM)~OL~FW-> ~RS%s\n",u->recap,text2);
	   write_user(user,text);
	   record_tell(user,text);
	   retcode=1;
	   fclose(fp);
	   return retcode;
	   }
        else if (u->room!=user->room) {
	   sprintf(text,"~OL~FT!! ~RS%s\n",text2);
	   write_room(NULL,text);
	   record_shout(text);
	   retcode=1;
	   fclose(fp);
	   return retcode;
	   }
	else {
           sprintf(text,"%s\n",text2);
	   write_room(user->room,text);
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

if (user->level<MEMBER) return;
i=0; s=0; nos=0; cnt=0;
sprintf(filename,"%s/%s",DATAFILES,SOCIALFILE);
if (!(fp=fopen(filename,"r"))) {
	sprintf(text,"list_socials(): Can't open socials file: '%s'\n",filename);
	write_syslog(text,1);
	write_user(user,"~OL~FRSorry, the socials are missing...\n");
	return;
	}
if (user->high_ascii) write_user(user,"~OL~FBÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
else write_user(user,"~OL~FB----------------------------------------------------------------------------\n");
write_user(user,"~OL~FG    Moenuts Socials Listing ~FB-=- ~FMAll Socials Start With A Period ~FB-=-\n");
if (user->high_ascii) write_user(user,"~OL~FBÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
else write_user(user,"~OL~FB----------------------------------------------------------------------------\n");
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
if (user->high_ascii) write_user(user,"~OL~FBÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
else write_user(user,"~OL~FB----------------------------------------------------------------------------\n");
sprintf(text,"~OL~FM There are currently ~FG%d~FM socials available to you.  ! = Public, * = Personal\n",nos);
write_user(user,text);
write_user(user,"~OL~FG Usage  ~FW: ~FT.~FM<~FYsocialname~FM> ~FB[~FM<~FRusername~FM>~FB] [~FM<~FRtext~FM>~FB]\n");
write_user(user,"~OL~FG Example~FW: ~FT.~FYpoke ~FB/ ~FT.~FYpoke ~FM<~FRusername~FM> ~FB/ ~FT.~FYpoke ~FM<~FRtext~FM>\n");
if (user->high_ascii) write_user(user,"~OL~FBÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
else write_user(user,"~OL~FB----------------------------------------------------------------------------\n");
}

void quit_user(UR_OBJECT user)
{
if (user->confirm) {
	sprintf(text,"~OL~FGAre you really leavin' us %s~RS~OL~FR?  ",user->recap);
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
	write_user(user,"~OL~FRThat username is too long!\n");
	return;
	}
if (strlen(word[2])>PASS_LEN) {
	sprintf(text,"~OL~FRThe password given for %s is too long!\n",word[1]);
	write_user(user,text);
	return;
	}
if (strlen(word[2])<3) {
	sprintf(text,"~OL~FRThe password given for %s is too short!\n",word[1]);
	write_user(user,text);
	return;
	}
if ((u=get_user(word[1]))) {
	write_user(user,"~OL~FRYou cannot use the same name as a person online!\n");
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
	sprintf(text,"%s~RS~OL~FR already exists as a user here!\n",u->recap);
	write_user(user,text);
	destruct_user(u);
	destructed=0;
	return;
	}
}

void finduser(UR_OBJECT user)
{
FILE *fp;
int uf=0, ufx=0;
char filename[81], line[ARR_SIZE+1], find[USER_NAME_LEN+1], ufilename[81];
char temp[USER_NAME_LEN+1];

if (word_count<2) {
	write_user(user,"Usage     : finduser <pattern to match> [or 'all']\n");
	write_user(user,"Example   : finduser moe\n");
	write_user(user,"Would Find: All users with 'moe' in their name.\n\n");
	return;
	}
strncpy(find,word[1],USER_NAME_LEN);
sprintf(filename,"%s/%s",USERFILES,USERLIST);
if (!(fp=fopen(filename,"r"))) {
	write_user(user,"ERROR: Could not open userlist file!\n");
	return;
	}
sprintf(text,"~OL~FGSearching Userlist for ~FB\"~FT%s~FB\"~FG...\n\n",find);
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
			sprintf(text,"~OL~FMUsername ~FB'~FT%-*.*s~FB' ~FMfound in userfile~FB,~FG and account exists!\n",USER_NAME_LEN,USER_NAME_LEN,center(line,USER_NAME_LEN));
			write_user(user,text);
			uf++;
			}
		}
	fgets(line,ARR_SIZE,fp);
	line[strlen(line)-1]=0;
	}
fclose(fp);
/* sprintf(text,"\n~OL~FGUsers Found With Accounts~FW: ~FT%d~FB, ~FGWithout Accounts~FW: ~FT%d\n\n",uf,ufx); */
sprintf(text,"\n~OL~FGUsers Found Matching Search Pattern~FW: ~FT%d\n\n",uf);
write_user(user,text);
/* if (ufx>0) write_level(ARCH,3,"~OL~FRfinduser(): The userfile needs to be rebuilt!\n",NULL); */
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
          	sprintf(text,"~FB-~OL=~FT+ ~FYLast %d people to visit %s ~FT+~FB=~RS~FB-",c,TALKERNAME);
		write_user(user,center(text,78));
                if (user->high_ascii) write_user(user,"\n~OL~FBÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
                else write_user(user,"\n~OL~FB------------------------------------------------------------------------------\n");
               }
          write_user(user,lastlogbuff[line]);
          }
	}
if (!cnt) {
	write_user(user,"~OL~FMThere Are No Last Logins To View.\n");
	return;
	}
else {
     if (user->high_ascii) write_user(user,"~OL~FBÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\n");
     else write_user(user,"~OL~FB------------------------------------------------------------------------------\n");
     }
	return;

	case 1: /* Add To Login Log */
	sprintf(text,"~OL~FW[ ~FG%s ~FW]~FT: ~RS%s %s\n",long_date(2),user->recap,user->desc);
	record_lastlog(text);
	return;

	case 2:
	clear_lastlog();
	write_user(user,"~OL~FGLast login log has been cleared...\n");
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
write_user(user,"\n~OL~FTMoesoft's Post Office Gateway v0.9b (C)1998 Moesoft Developments.\n");
if (!(fp1=fopen(file,"r"))) {
	write_user(user,"~OL~FRPost Office : Copy : could not open file for read!.\n");
	return 0;
	}
if (!(fp2=fopen(file2,"w"))) {
	write_user(user,"~OL~FRPost Office : Copy : could not open output file.\n");
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
sprintf(text,"~OL~FGSending Internet E-Mail To~FW: ~FT%s\n~OL~FGSubject~FW: ~FT%s\n\n",to,subject);
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
	    sprintf(text,"/usr/sbin/sendmail %s < %s",to,file2);
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
          write_user(user,"~OL~FTMoesoft's Internet E-Mail Gateway Beta-Test v0.9b\n");
		write_user(user,"Usage: .email <address>\nExample: .email user@foo.bar.com\n");
		return;
		}
	if (strlen(word[1])>120 || strlen(word[1])<10) {
		write_user(user,"~OL~FTMoesoft's Internet E-Mail Gateway Beta-Test v0.9\n");
		write_user(user,"~OL~FRThe email address you specified is way too long or too short!\n");
		return;
		}
	if (!valid_email(user,word[1])) return;
	strncpy(user->to_email,word[1],120);
     sprintf(text,"~OL~FT%s decides to write some Internet Email...\n",user->recap);
     write_room_except(user->room,text,user);
	write_user(user,"\n");
     sprintf(text,"~FG-~OL=~FM] ~FWWriting Internet E-Mail to ~FT%s ~FM[~FG=~RS~FG-",user->to_email);
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
fprintf(fp,"Moenuts-Reply-To: %s <%s@%s>\n",user->name,user->name,SHORTNAME);
fprintf(fp,"Moenuts-Talker-Address: telnet://%s\n",TALKERADDR);
fprintf(fp,"Moenuts-Talker-Homepage: %s\n",TALKERURL);
fprintf(fp,"Moenuts-Version: %s\n",MNVERSION);
fprintf(fp,"Moenuts-Gateway: %s\n",GWVERSION);
fprintf(fp,"-+- Message by %s from %s (%s) -+-\n\n",user->name,TALKERNAME,TALKERADDR);
c=user->malloc_start;
while(c!=user->malloc_end) putc(*c++,fp);
fclose(fp);
sprintf(text,"~OL~FT%s has finnished writing their Internet Email...\n",user->recap);
write_room_except(user->room,text,user);
sprintf(subject,"Mail From %s at %s",user->name,TALKERNAME);
if (!post_office(user,"",user->to_email,subject,filename,0)) {
	write_user(user,"~OL~FRPost Office Failed To Deliver Your Mail!, Sorry!\n");
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
	write_user(user,"~OL~FRRoom not removed from memory because it doesn't exist!\n");
	return;
	}
/* 
if (rm->access!=PERSONAL) {
	write_user(user,"~OL~FRThis room is not a personal room, so it cannot be destructed!\n");
	return;
	}
*/
cnt=0;
for(u=user_first;u!=NULL;u=u->next) if (u->room==rm) ++cnt;
if (cnt>0) {
	write_user(user,"~OL~FTYour room is not empty, so it will not be removed from memory.\n");
	return;
	}
write_user(user,"~OL~FGYour room has been removed from memory.\n");
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
sprintf(text,"~FTTotal Credits Earned: $%d.\n~FTYour Bank Balance: $%d\n",credit,user->bank_balance);
write_user(user,text);
}

 
/* Page a user via the ICQ Network using the pager.mirabilis.com server    */
/* This can be adapted to other talkers, but it requires Andy Collington's */
/* double_fork() function using in Amnuts, Moenuts and RaMTITS based codes */
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
	     write_user(user,"\n~OL~FRSorry, user does not have a valid ICQ # Set.\n");
	     write_user(user,"~OL~FRThey must have first used .set icq <their icq #>\n");
	     return;
	     }
	sprintf(filename,"%s/icqpage.%s",TEMPFILES,user->name);        
	strcpy(icq_num,u->icq);
	strcpy(reciever,u->name);
	if (!(fp=fopen(filename,"w"))) {
	     write_user(user,"~OL~FRERROR!  Cannot Create Page Message File...\n");
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
	write_user(user,"\n~OL~FRSorry, user does not have a valid ICQ # Set.\n");
	write_user(user,"~OL~FRThey must have first used .set icq <their icq #>\n");
	return;
	}
sprintf(filename,"%s/icqpage.%s",TEMPFILES,user->name);        
if (!(fp=fopen(filename,"w"))) {
	write_user(user,"~OL~FRERROR!  Cannot Create Page Message File...\n");
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
switch(double_fork()) {
  case -1 : return; /* double_fork() failed */
  case  0 :
         sprintf(text,"\n~FTPageing ~FY%s ~RS~OL~FTvia the ICQ Network!\n",icq_num);
         write_user(user,text);
         write_user(user,"~FTIf the user is online they will recieve your message in a couple minutes\n");
         write_user(user,"~FTOtherwise, they will get your message next time they log onto the ICQ Network!\n");
         sprintf(text,"Sending an ICQ Page from %s to %s\n",user->name,icq_num);
	 write_syslog(text,1);
         sprintf(text,"/usr/sbin/sendmail -s \"ICQ Page From %s\" %s@pager.mirabilis.com < %s",user->name,icq_num,fname);
         system(text);
         _exit(1);
	 return;
	 }
}


/* Variable Replace Function For Profiles.  Portions (C)1999 Curmitt,
   curmitt@curmitt.talkernet.net, Written for Moesoft Dvp. */
char *profsub(char *stptr,UR_OBJECT user)
{
char *ptr=NULL;
int hour=0,ampm=0;
char age[5],levname[31],gender[11],curtime[10],curdate[10];
sprintf(text,"%d",user->age);
strncpy(age,text,4);

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

while ( (ptr=(char *)strtok(stptr, " \n\r")) != NULL) {
  /* Variable Definitions / Assign Values */
  if (!strcasecmp(ptr,"$N")) strcat(tempstr,colour_com_strip(user->recap));
  else if (!strcasecmp(ptr, "$E")) strcat(tempstr,user->email);
  else if (!strcasecmp(ptr, "$I")) strcat(tempstr,user->icq);
  else if (!strcasecmp(ptr, "$M")) strcat(tempstr,user->married);
  else if (!strcasecmp(ptr, "$W")) strcat(tempstr,user->homepage);
  else if (!strcasecmp(ptr, "$G")) strcat(tempstr,gender);
  else if (!strcasecmp(ptr, "$L")) strcat(tempstr,levname);
  else if (!strcasecmp(ptr, "$A")) strcat(tempstr,user->level_alias);
  else if (!strcasecmp(ptr, "$T")) strcat(tempstr,curtime);
  else if (!strcasecmp(ptr, "$D")) strcat(tempstr,curdate);
  else if (!strcasecmp(ptr, "$B")) strcat(tempstr,user->birthday);
  else if (!strcasecmp(ptr, "$Z")) strcat(tempstr,colour_com_strip(user->desc));
  else if (!strcasecmp(ptr, "$S")) strcat(tempstr,user->site);
  else if (!strcasecmp(ptr, "$R")) strcat(tempstr,user->room->name);
  else if (!strcasecmp(ptr, "$P")) strcat(tempstr,colour_com_strip(user->predesc));
  else if (!strcasecmp(ptr, "$I")) strcat(tempstr,user->login_room);
  else if (!strcasecmp(ptr, "$HANGCHEAT")) strcat(tempstr,user->hang_word);
  else {
	/* Prevent Overflows by some weenie trying to crash the talker
           by using EVERY code multiple times */
  	if (strlen(tempstr)+strlen(ptr)>=(ARR_SIZE*2)) { strcat(tempstr,"\n"); return tempstr; }
  	strcat(tempstr,ptr);
  	}
   strcat(tempstr," ");
   stptr=NULL;
   }
strcat(tempstr,"\n\0");
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
		write_user(user,"~OL~FRNewsfile Deleted.\n");
		return;
		}
	sprintf(text,"~OL~FM%s is updating the news...\n",user->name);
	write_room(user->room,text); 
	sprintf(newstopic,"%-40.40s",inpstr);
        write_user(user,"~CS\n~OL~FT+----------------------------------------------------------------------------+\n");
	if (!file_exists(filename)) write_user(user,"~OL~FT|                  ~FGCreating A New News File~FT                                  |\n");
	else write_user(user,"~OL~FT|                  ~FGAdding To The New News File~FT                               |\n");
        write_user(user,"~OL~FT+----------------------------------------------------------------------------+\n");
        sprintf(text," ~OL~FTNews Written By~FW: ~RS%s ~RS~FGon ~RS~FT%s\n",user->name,long_date(2));
        write_user(user,text);
        sprintf(text," ~OL~FTNews Topic     ~FW: ~OL~FG%s\n",newstopic);
        write_user(user,text);
        write_user(user,"~OL~FT+----------------------------------------------------------------------------+\n\n");
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
sprintf(line,"~OL~FT+----------------------------------------------------------------------------+\n");
fputs(line,fp);
sprintf(line," ~OL~FTNews Written By~FW: ~RS%s~RS ~FGon ~FT%s\n",user->recap,long_date(2));
fputs(line,fp);
sprintf(line," ~OL~FTNews Topic     ~FW: ~OL~FG%s\n",newstopic);
fputs(line,fp);
sprintf(line,"~OL~FT+----------------------------------------------------------------------------+\n");
fputs(line,fp);
c=user->malloc_start;
while(c!=user->malloc_end) putc(*c++,fp);
sprintf(line,"~OL~FT+----------------------------------------------------------------------------+\n");
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
				sprintf(text,"\07~OL~FY~BMOnline Alert~RS~OL~FT: ~RS~FG%s is Online!\n",word[3]);
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
	write_user(user,"~OL~FR The reboot countdown is currently active, cancel it first.\n");
	return;
	}
if (!rs_which) {
	write_user(user,"~OL~FR The shutdown countdown is currently active, cancel it first.\n");
	return;
	}
if (!strncasecmp(word[1],"-cancel",2)) {
	if (rs_which!=2) {
		write_user(user,"~OL~FM The seamless reboot countdown isn't currently active.\n");
		return;
		}
	if (rs_countdown && rs_which==2 && rs_user==NULL) {
		write_user(user,"~OL~FM Someone else has set the seamless reboot countdown.\n");
		return;
		}
	write_room(NULL,"~OL~FB~FRMoenuts~FT: ~FB-~RS~FT Seamless reboot has been cancelled..\n");
	sprintf(text,"[SREBOOT] - Cancelled by: [%s]\n",user->name);
	write_syslog(text,1);
	rs_countdown=0;	rs_announce=0;  rs_which=-1;  rs_user=NULL;
	return;
	}
if (rs_countdown && rs_which==2) {
	write_user(user,"~OL~FR The seamless reboot countdown is currently active, cancel it first.\n");
	return;
	}
if (word_count<2) {
	rs_countdown=0;  rs_announce=0;  rs_which=-1;  rs_user=NULL;
	}
else {
	rs_countdown=atoi(word[1]);
	rs_which=2;
	}
write_user(user,"\07~OL~FB[~FRWarning~FB] - ~FRThis will reboot the talker.\n");
write_user(user,"~OL~FMAre you sure you want to do this?~FT:~RS ");
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
		sprintf(text,"~OL~FW[~FRERROR~FW] - ~RS~FG Unable to open reboot system info file [%s]\n",strerror(errno));
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
		sprintf(text,"~OL~FW[~FRERROR~FW] - ~RS~FG Unable to open reboot room list file [%s]\n",strerror(errno));
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
		sprintf(text,"~OL~FW[~FRERROR~FW] - ~RS~FG Unable to open reboot user list file [%s]\n",strerror(errno));
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
                write_user(u,"\n~OL~FB[~FTEDITOR~FB]~FT: ~FRSeamless reboot requested close, I Must Exit Now, Sorry.\n");
                editor_done(u);
                }
	if (u->login) {
		write_user(u,"\n~OL~FRTalker Reboot in progress.. Aborting your connection.\n~OL~FMPlease try logging in again in a few seconds...\n");
		close(u->socket);
		continue;
		}
	sprintf(text,"%s/%s",REBOOTING_DIR,u->name);
	if (!(fp=fopen(text,"w"))) {
		if (user) {
			sprintf(text,"~OL~FW[~FRERROR~FW] - ~RS~FG Unable to open reboot user info file for '%s': [%s]\n",u->name,strerror(errno));
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

/* First get rid of any clones */
for (u=user_first;u!=NULL;u=u->next) {
	if (u->login || u->type!=CLONE_TYPE) continue;
	write_user(u->owner,"~OL~FRSeamless reboot in progress, Your Clone Must Be Destroyed.\n");
	destroy_user_clones(u);
	}
write_room(NULL,"~OL~FB[~FYMoenuts~FB]~FT: ~RS~FGSeemless Reboot In Progress, Please Wait...\n");
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
	  if (user) write_user(user,"~OL~FRFailed to fork.\n");
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
			if (rm) u->room=rm;
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
/*	if (u) write_room(NULL,"~OL~FB[~FYMoenuts~FB]~FT: ~RS~FMSeamless reboot has completed.\n"); */
/*	else { */
		sprintf(text,"[%s] didn't survive the reboot. :~(\n",rebooter);
		write_syslog(text,0);
/*		} */
 	}
write_room(NULL,"~OL~FB[~FYMoenuts~FB]~FT: ~RS~FMSeamless reboot has completed.\n");
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
		sprintf(text,"The topic in the %s room has been cleared.\n",room->name);
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
		sprintf(text,"The topic in the %s room has been cleared.\n",room->name);
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
       write_user(user,"~OL~FRYour Alias Is Too Long!!  ~FMRemove some colors maybe?\n");
       return;
       }
    temp=colour_com_strip(inpstr);
    if (!strcasecmp(temp,user->name)) {
       /* Store Old Recap If This Is The First Time */
       strncpy(user->alias,user->recap,USER_RECAP_LEN);
       }
    strncpy(user->recap,inpstr,USER_RECAP_LEN);
    sprintf(text,"\n~OL~FMYour alias has been changed to~FR:~RS %s\n",user->recap);
    write_user(user,text);
    return;
}

void blah(UR_OBJECT user) {
	/* do absolutely nothing */	
	return;
	}
