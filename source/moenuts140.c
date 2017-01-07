/*****************************************************************************
    NUTS version 3.3.3 (Triple Three :) - Copyright (C) Neil Robertson 1996
                     Last update: 18th November 1996
                         Last Mod 19 July 1997

     Moenuts v1.40 A4  (Special Thanks To Reddawg And Nerfingdummy!!) 
       ** This Version Released For Fun, And Is Not Supported **
   This version was used on the original Stairway To Heaven when it
                         was put up in 1997.

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
#include <time.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <unistd.h>
#include <setjmp.h>
#include <errno.h>

#include "moenuts140.h"

#define VERSION "3.3.3 Moenuts 1.40 A4"

/*** This function calls all the setup routines and also contains the
	main program loop ***/

main(argc,argv)
int argc;
char *argv[];
{
fd_set readmask; 
int i,len; 
char inpstr[ARR_SIZE];
char *remove_first();
UR_OBJECT user,next;
NL_OBJECT nl;

strcpy(progname,argv[0]);
if (argc<2) strcpy(confile,CONFIGFILE);
else strcpy(confile,argv[1]);

/* Startup */

printf("\n..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
printf(" Moenuts v1.40 A4 Telnet Chat Server Is Now Booting...\n");
printf(" Which Is NUTS version %s\n",VERSION);
printf("..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
init_globals();
write_syslog("\n>> Moenuts v1.40 A4 Server Booting...\n",0);
set_date_time();
init_signals();
load_and_parse_config();
init_sockets();
if (auto_connect) init_connections();
else printf(">> Skipping connect stage.\n");
printf(">> Checking boards...\n");
check_messages(NULL,1);
count_users();

/* Run in background automatically. */
switch(fork()) {
	case -1: boot_exit(11);  /* fork failure */
	case  0: break; /* child continues */
	default: sleep(1); exit(0);  /* parent dies */
	}
reset_alarm();
printf("..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
printf(" Moenuts 1.40 A4 Successfully Booted with Process ID %d\n",getpid());
printf("..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
sprintf(text,">> Moenuts successfully booted with PID %d on %s \n\n",getpid(),long_date(1));
write_syslog(text,0);

/**** Main program loop. *****/
setjmp(jmpvar); /* jump to here if we crash and crash_action = IGNORE */
while(1) {
	/* set up mask then wait */
	setup_readmask(&readmask);
	if (select(FD_SETSIZE,&readmask,0,0,0)==-1) continue;

	/* check for connection to listen sockets */
	for(i=0;i<3;++i) {
		if (FD_ISSET(listen_sock[i],&readmask)) 
			accept_connection(listen_sock[i],i);
		}

	/* Cycle through client-server connections to other talkers */
	for(nl=nl_first;nl!=NULL;nl=nl->next) {
		no_prompt=0;
		if (nl->type==UNCONNECTED || !FD_ISSET(nl->socket,&readmask)) 
			continue;
		/* See if remote site has disconnected */
		if (!(len=read(nl->socket,inpstr,sizeof(inpstr)-3))) {
			if (nl->stage==UP)
                    sprintf(text,">> NETLINK: Remote disconnect by %s.\n",nl->service);
               else sprintf(text,">> NETLINK: Remote disconnect by site %s.\n",nl->site);
			write_syslog(text,1);
			sprintf(text,"~OLSYSTEM:~RS Lost link to %s in the %s.\n",nl->service,nl->connect_room->name);
			write_room(NULL,text);
			shutdown_netlink(nl);
			continue;
			}
		inpstr[len]='\0'; 
		exec_netcom(nl,inpstr);
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
			login(user,inpstr);  user=next;  continue;  
			}

		/* If a dot on its own then execute last inpstr unless its a misc
		   op or the user is on a remote site */
		if (!user->misc_op) {
			if (!strcmp(inpstr,".") && user->inpstr_old[0]) {
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
               else write_user(user,"~FY-~OL=~FY[ ~FBYou Come Back To The Gang ~FY]~FY=~RS~FY-\n");  
			user->afk_mesg[0]='\0';
			if (user->chkrev) {
                    sprintf(text,"\07~OL-> ~FYYou Have New Tells!  ~RStype ~FY.revtell to view them.\n",user->name);
				write_user(user,text);
				user->chkrev=0;
				}  
               sprintf(text,"%s comes back from being AFK.\n",user->name);
			write_syslog(text,1);
			if (user->vis) {
                    sprintf(text,"~FY-~OL=~FY[ ~FB%s Comes Back To The Gang ~FY]~FY=~RS~FY-\n",user->name);
				write_room_except(user->room,text,user);
				}
			if (user->afk==2) {
				user->afk=0;  prompt(user);  user=next;  continue;
				}
			user->afk=0;
			}
		if (!word_count) {
			if (misc_ops(user,inpstr))  {  user=next;  continue;  }
			if (user->room==NULL) {
				sprintf(text,"ACT %s NL\n",user->name);
				write_sock(user->netlink->socket,text);
				}
			if (user->command_mode) prompt(user);
               user=next;  continue;                        
			}
		if (misc_ops(user,inpstr))  {  user=next;  continue;  }
		com_num=-1;
		if (user->command_mode || strchr(".;/!<>-='#[]",inpstr[0])) 
			exec_com(user,inpstr);
		else say(user,inpstr);
		if (!destructed) {
			if (user->room!=NULL)  prompt(user); 
			else {
				switch(com_num) {
					case -1  : /* Unknown command */
					case HOME:
					case QUIT:
					case MODE:
					case PROMPT: 
					case SUICIDE:
					case REBOOT:
					case SHUTDOWN: prompt(user);
					}
				}
			}
		user=next;
		}
	} /* end while */
}


/************ MAIN LOOP FUNCTIONS ************/

/*** Set up readmask for select ***/
setup_readmask(mask)
fd_set *mask;
{
UR_OBJECT user;
NL_OBJECT nl;
int i;

FD_ZERO(mask);
for(i=0;i<3;++i) FD_SET(listen_sock[i],mask);
/* Do users */
for (user=user_first;user!=NULL;user=user->next) 
	if (user->type==USER_TYPE) FD_SET(user->socket,mask);

/* Do client-server stuff */
for(nl=nl_first;nl!=NULL;nl=nl->next) 
	if (nl->type!=UNCONNECTED) FD_SET(nl->socket,mask);
}


/*** Accept incoming connections on listen sockets ***/
accept_connection(lsock,num)
int lsock,num;
{
UR_OBJECT user,create_user();
NL_OBJECT create_netlink();
char *get_ip_address(),*get_ip_number(),site[80];
struct sockaddr_in acc_addr;
int accept_sock,size;
int pick;

size=sizeof(struct sockaddr_in);
accept_sock=accept(lsock,(struct sockaddr *)&acc_addr,&size);
if (num==2) {
	accept_server_connection(accept_sock,acc_addr);  return;
	}
strcpy(site,get_ip_address(acc_addr));
if (site_banned(site)) {
     write_sock(accept_sock,"\n\rLogins were banned from your site or domain thanks to others!\n\n\r");
	close(accept_sock);
	sprintf(text,"~OL~FRAttempted login from banned site:~RS~OL %s ~FRwas disconected\n",site);
     write_level(WIZ,3,text,NULL);
	sprintf(text,"Attempted login from banned site %s.\n",site);
	write_syslog(text,1);
	return;
	}
pick=rand()%5;
sprintf(text,"%s.%d",MOTD1,pick);
more(NULL,accept_sock,text); /* send pre-login message */
if (num_of_users+num_of_logins>=max_users && !num) {
	write_sock(accept_sock,"\n\rSorry, the talker is full at the moment.\n\n\r");
	close(accept_sock);  
	return;
	}
if ((user=create_user())==NULL) {
	sprintf(text,"\n\r%s: unable to create session.\n\n\r",syserror);
	write_sock(accept_sock,text);
	close(accept_sock);  
	return;
	}
user->socket=accept_sock;
user->login=3;
user->last_input=time(0);
if (!num) user->port=port[0]; 
else {
	user->port=port[1];
     write_user(user,"<*> Staff Login <*>\n\n");
	}
strcpy(user->site,site);
strcpy(site,get_ip_number(acc_addr));
strcpy(user->site_port,site);
echo_on(user);
write_user(user,"May I have your name please? ");
num_of_logins++;
}


/*** Get net address of accepted connection ***/
char *get_ip_address(acc_addr)
struct sockaddr_in acc_addr;
{
static char site[80];
struct hostent *host;

strcpy(site,(char *)inet_ntoa(acc_addr.sin_addr)); /* get number addr */
if ((host=gethostbyaddr((char *)&acc_addr.sin_addr,4,AF_INET))!=NULL)
	strcpy(site,host->h_name); /* copy name addr. */
strtolower(site);
return site;
}


/*** Get net address of accepted connection ***/
char *get_ip_number(acc_addr)
struct sockaddr_in acc_addr;
{
static char site[80];
struct hostent *host;

strcpy(site,(char *)inet_ntoa(acc_addr.sin_addr)); /* get number addr */
return site;
}


/*** See if users site is banned ***/
site_banned(site)
char *site;
{
FILE *fp;
char line[82],filename[80];

sprintf(filename,"%s/%s",DATAFILES,SITEBAN);
if (!(fp=fopen(filename,"r"))) return 0;
fscanf(fp,"%s",line);
while(!feof(fp)) {
	if (strstr(site,line)) {  fclose(fp);  return 1;  }
	fscanf(fp,"%s",line);
	}
fclose(fp);
return 0;
}


/*** See if users site is banned ***/
newsite_banned(site)
char *site;
{
FILE *fp;
char line[82],filename[80];
int cnt;

sprintf(filename,"%s/%s",DATAFILES,NEWSITEBAN);
if (!(fp=fopen(filename,"r"))) return 0;
fscanf(fp,"%s",line);
while(!feof(fp)) {
	if (strstr(site,line)) {  fclose(fp);  return 1;  }
	fscanf(fp,"%s",line);
	}
fclose(fp);
return 0;
}



/*** See if user is banned ***/
user_banned(name)
char *name;
{
FILE *fp;
char line[82],filename[80];

sprintf(filename,"%s/%s",DATAFILES,USERBAN);
if (!(fp=fopen(filename,"r"))) return 0;
fscanf(fp,"%s",line);
while(!feof(fp)) {
	if (!strcmp(line,name)) {  fclose(fp);  return 1;  }
	fscanf(fp,"%s",line);
	}
fclose(fp);
return 0;
}


/*** Attempt to get '\n' terminated line of input from a character
     mode client else store data read so far in user buffer. ***/
get_charclient_line(user,inpstr,len)
UR_OBJECT user;
char *inpstr;
int len;
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
if (user->charmode_echo
    && ((user->login!=2 && user->login!=1) || password_echo)) 
	write(user->socket,inpstr,len);
return 0;
}


/*** Put string terminate char. at first char < 32 ***/
terminate(str)
char *str;
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
wordfind(inpstr)
char *inpstr;
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
clear_words()
{
int w;
for(w=0;w<MAX_WORDS;++w) word[w][0]='\0';
word_count=0;
}


/************ PARSE CONFIG FILE **************/

load_and_parse_config()
{
FILE *fp;
char line[81]; /* Should be long enough */
char c,filename[80];
int i,section_in,got_init,got_rooms;
RM_OBJECT rm1,rm2;
NL_OBJECT nl;

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
			else if (!strcmp(wrd[0],"SITES:")) section_in=3; 
				else {
                         fprintf(stderr,">> NUTS: Unknown section header on line %d.\n",config_line);
					fclose(fp);  boot_exit(1);
					}
		}
	switch(section_in) {
		case 1: parse_init_section();  got_init=1;  break;
		case 2: parse_rooms_section(); got_rooms=1; break;
		case 3: parse_sites_section(); break;
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
if (!verification[0]) {
     fprintf(stderr,">> NUTS: Verification not set in config file.\n");
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
if (!port[2]) {
     fprintf(stderr,">> NUTS: Link port number not set in config file.\n");
	boot_exit(1);
	}
if (port[0]==port[1] || port[1]==port[2] || port[0]==port[2]) {
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
	}

/* Check external links */
for(rm1=room_first;rm1!=NULL;rm1=rm1->next) {
	for(nl=nl_first;nl!=NULL;nl=nl->next) {
		if (!strcmp(nl->service,rm1->name)) {
               fprintf(stderr,">> NUTS: Service name %s is also the name of a room.\n",nl->service);
			boot_exit(1);
			}
		if (rm1->netlink_name[0] 
		    && !strcmp(rm1->netlink_name,nl->service)) {
			rm1->netlink=nl;  break;
			}
		}
	if (rm1->netlink_name[0] && rm1->netlink==NULL) {
          fprintf(stderr,">> NUTS: Service name %s not defined for room %s.\n",rm1->netlink_name,rm1->name);
		boot_exit(1);
		}
	}
printf(">> Skipping Room Description Checking...\n");
/* Load room descriptions 
for(rm1=room_first;rm1!=NULL;rm1=rm1->next) {
	sprintf(filename,"%s/%s.R",DATAFILES,rm1->name);
	if (!(fp=fopen(filename,"r"))) {
          fprintf(stderr,">> NOTE: Can't open description file for room %s.\n",rm1->name);
          sprintf(text,">> NOTE: Couldn't open description file for room %s.\n",rm1->name);
		write_syslog(text,0);
		continue;
		}
	i=0;
	c=getc(fp);
	while(!feof(fp)) {
		if (i==ROOM_DESC_LEN) {
               fprintf(stderr,">> ERROR: Description too long for room %s.\n",rm1->name);
               sprintf(text,">> ERROR: Description too long for room %s.\n",rm1->name);
			write_syslog(text,0);
			break;
			}
		rm1->desc[i]=c;  
		c=getc(fp);  ++i;
		}
	rm1->desc[i]='\0';
	fclose(fp);
	} */
}



/*** Parse init section ***/
parse_init_section()
{
static int in_section=0;
int op,val;
char *options[]={ 
"mainport","wizport","linkport","system_logging","minlogin_level","mesg_life",
"wizport_level","prompt_def","gatecrash_level","min_private","ignore_mp_level",
"rem_user_maxlevel","rem_user_deflevel","verification","mesg_check_time",
"max_users","heartbeat","login_idle_time","user_idle_time","password_echo",
"ignore_sigterm","auto_connect","max_clones","ban_swearing","crash_action",
"colour_def","time_out_afks","allow_caps_in_name","charecho_def",
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
	case 2:
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

	case 11: 
	/* Max level a remote user can remotely log in if he doesn't have a local
        account. ie if level set to WIZ a OWNER can only be a WIZ if logging in 
        from another server unless he has a local account of level OWNER */
	if ((rem_user_maxlevel=get_level(wrd[1]))==-1) {
          fprintf(stderr,">> NUTS: Unknown level specifier for rem_user_maxlevel on line %d.\n",config_line);
		boot_exit(1);	
		}
	return;

	case 12:
	/* Default level of remote user who does not have an account on site and
	   connection is from a server of version 3.3.0 or lower. */
	if ((rem_user_deflevel=get_level(wrd[1]))==-1) {
          fprintf(stderr,">> NUTS: Unknown level specifier for rem_user_deflevel on line %d.\n",config_line);
		boot_exit(1);	
		}
	return;

	case 13:
	if (strlen(wrd[1])>VERIFY_LEN) {
          fprintf(stderr,">> NUTS: Verification too long on line %d.\n",config_line);
		boot_exit(1);	
		}
	strcpy(verification,wrd[1]);
	return;

	case 14: /* mesg_check_time */
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

	case 15:
	if ((max_users=val)<1) {
          fprintf(stderr,">> NUTS: Invalid value for max_users on line %d.\n",config_line);
		boot_exit(1);
		}
	return;

	case 16:
	if ((heartbeat=val)<1) {
          fprintf(stderr,">> NUTS: Invalid value for heartbeat on line %d.\n",config_line);
		boot_exit(1);
		}
	return;

	case 17:
	if ((login_idle_time=val)<10) {
          fprintf(stderr,">> NUTS: Invalid value for login_idle_time on line %d.\n",config_line);
		boot_exit(1);
		}
	return;

	case 18:
	if ((user_idle_time=val)<10) {
          fprintf(stderr,">> NUTS: Invalid value for user_idle_time on line %d.\n",config_line);
		boot_exit(1);
		}
	return;

	case 19: 
	if ((password_echo=yn_check(wrd[1]))==-1) {
          fprintf(stderr,">> NUTS: Password_echo must be YES or NO on line %d.\n",config_line);
		boot_exit(1);
		}
	return;

	case 20: 
	if ((ignore_sigterm=yn_check(wrd[1]))==-1) {
          fprintf(stderr,">> NUTS: Ignore_sigterm must be YES or NO on line %d.\n",config_line);
		boot_exit(1);
		}
	return;

	case 21:
	if ((auto_connect=yn_check(wrd[1]))==-1) {
          fprintf(stderr,">> NUTS: Auto_connect must be YES or NO on line %d.\n",config_line);
		boot_exit(1);
		}
	return;

	case 22:
	if ((max_clones=val)<0) {
          fprintf(stderr,">> NUTS: Invalid value for max_clones on line %d.\n",config_line);
		boot_exit(1);
		}
	return;

	case 23:
	if ((ban_swearing=yn_check(wrd[1]))==-1) {
          fprintf(stderr,">> NUTS: Ban_swearing must be YES or NO on line %d.\n",config_line);
		boot_exit(1);
		}
	return;

	case 24:
	if (!strcmp(wrd[1],"NONE")) crash_action=0;
	else if (!strcmp(wrd[1],"IGNORE")) crash_action=1;
		else if (!strcmp(wrd[1],"REBOOT")) crash_action=2;
			else {
                    fprintf(stderr,">> NUTS: Crash_action must be NONE, IGNORE or REBOOT on line %d.\n",config_line);
				boot_exit(1);
				}
	return;

	case 25:
	if ((colour_def=onoff_check(wrd[1]))==-1) {
          fprintf(stderr,">> NUTS: Colour_def must be ON or OFF on line %d.\n",config_line);
		boot_exit(1);
		}
	return;

	case 26:
	if ((time_out_afks=yn_check(wrd[1]))==-1) {
          fprintf(stderr,">> NUTS: Time_out_afks must be YES or NO on line %d.\n",config_line);
		boot_exit(1);
		}
	return;

	case 27:
	if ((allow_caps_in_name=yn_check(wrd[1]))==-1) {
          fprintf(stderr,">> NUTS: Allow_caps_in_name must be YES or NO on line %d.\n",config_line);
		boot_exit(1);
		}
	return;

	case 28:
	if ((charecho_def=onoff_check(wrd[1]))==-1) {
          fprintf(stderr,">> NUTS: Charecho_def must be ON or OFF on line %d.\n",config_line);
		boot_exit(1);
		}
	return;

	case 29:
	if ((time_out_maxlevel=get_level(wrd[1]))==-1) {
          fprintf(stderr,">> NUTS: Unknown level specifier for time_out_maxlevel on line %d.\n",config_line);
		boot_exit(1);	
		}
	return;
	}
}



/*** Parse rooms section ***/
parse_rooms_section()
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
	if (!strcmp(room->label,wrd[0])) {
          fprintf(stderr,">> NUTS: Duplicate room label on line %d.\n",config_line);
		boot_exit(1);
		}
	if (!strcmp(room->name,wrd[1])) {
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
		else if (!strcmp(wrd[3],"PER")) room->access=PERSONAL;
			else if (!strcmp(wrd[3],"HIDE")) room->access=FIXED_PERSONAL;
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
/* Parse external link stuff */
if (!wrd[6][0] || wrd[6][0]=='#') return;
if (!strcmp(wrd[6],"ACCEPT")) {  
	if (wrd[7][0] && wrd[7][0]!='#') {
          fprintf(stderr,">> NUTS: Unexpected word following ACCEPT keyword on line %d.\n",config_line);
		boot_exit(1);
		}
	room->inlink=1;  
	return;
	}
if (!strcmp(wrd[6],"CONNECT")) {
	if (!wrd[7][0]) {
          fprintf(stderr,">> NUTS: External link name missing on line %d.\n",config_line);
		boot_exit(1);
		}
	if (wrd[8][0] && wrd[8][0]!='#') {
          fprintf(stderr,">> NUTS: Unexpected word following external link name on line %d.\n",config_line);
		boot_exit(1);
		}
	strcpy(room->netlink_name,wrd[7]);
	return;
	}
fprintf(stderr,">> NUTS: Unknown connection option on line %d.\n",config_line);
boot_exit(1);
}



/*** Parse sites section ***/
parse_sites_section()
{
NL_OBJECT nl;
static int in_section=0;

if (!strcmp(wrd[0],"SITES:")) { 
	if (++in_section>1) {
          fprintf(stderr,">> NUTS: Unexpected SITES section header on line %d.\n",config_line);
		boot_exit(1);
		}
	return;
	}
if (!wrd[3][0]) {
     fprintf(stderr,">> NUTS: Required parameter(s) missing on line %d in sites section.\n",config_line);
	boot_exit(1);
	}
if (strlen(wrd[0])>SERV_NAME_LEN) {
     fprintf(stderr,">> NUTS: Link name length too long on line %d.\n",config_line);
	boot_exit(1);
	}
if (strlen(wrd[3])>VERIFY_LEN) {
     fprintf(stderr,">> NUTS: Verification too long on line %d.\n",config_line);
	boot_exit(1);
	}
if ((nl=create_netlink())==NULL) {
     fprintf(stderr,">> NUTS: Memory allocation failure creating netlink on line %d.\n",config_line);
	boot_exit(1);
	}
if (!wrd[4][0] || wrd[4][0]=='#' || !strcmp(wrd[4],"ALL")) nl->allow=ALL;
else if (!strcmp(wrd[4],"IN")) nl->allow=IN;
	else if (!strcmp(wrd[4],"OUT")) nl->allow=OUT;
		else {
               fprintf(stderr,">> NUTS: Unknown netlink access type on line %d.\n",config_line);
			boot_exit(1);
			}
if ((nl->port=atoi(wrd[2]))<1 || nl->port>65535) {
     fprintf(stderr,">> NUTS: Illegal port number on line %d.\n",config_line);
	boot_exit(1);
	}
strcpy(nl->service,wrd[0]);
strtolower(wrd[1]);
strcpy(nl->site,wrd[1]);
strcpy(nl->verification,wrd[3]);
}


yn_check(wd)
char *wd;
{
if (!strcmp(wd,"YES")) return 1;
if (!strcmp(wd,"NO")) return 0;
return -1;
}


onoff_check(wd)
char *wd;
{
if (!strcmp(wd,"ON")) return 1;
if (!strcmp(wd,"OFF")) return 0;
return -1;
}


/************ INITIALISATION FUNCTIONS *************/

/*** Initialise globals ***/
init_globals()
{
verification[0]='\0';
port[0]=0;
port[1]=0;
port[2]=0;
auto_connect=1;
max_users=50;
max_clones=1;
ban_swearing=0;
heartbeat=2;
keepalive_interval=60; /* DO NOT TOUCH!!! */
net_idle_time=300; /* Must be > than the above */
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
allow_caps_in_name=1;
rs_countdown=0;
rs_announce=0;
rs_which=-1;
rs_user=NULL;
gatecrash_level=WIZ; /* minimum user level which can enter private rooms */
min_private_users=2; /* minimum num. of users in room before can set to priv */
ignore_mp_level=CODER; /* User level which can ignore the above var. */
rem_user_maxlevel=REG;
rem_user_deflevel=USER;
user_first=NULL;
user_last=NULL;
room_first=NULL;
room_last=NULL; /* This variable isn't used yet */
nl_first=NULL;
nl_last=NULL;
clear_words();
time(&boot_time);
user_count=0;
}


/*** Initialise the signal traps etc ***/
init_signals()
{
void sig_handler();

signal(SIGTERM,sig_handler);
signal(SIGSEGV,sig_handler);
signal(SIGBUS,sig_handler);
signal(SIGILL,SIG_IGN);
signal(SIGTRAP,SIG_IGN);
signal(SIGIOT,SIG_IGN);
signal(SIGTSTP,SIG_IGN);
signal(SIGCONT,SIG_IGN);
signal(SIGHUP,SIG_IGN);
signal(SIGINT,SIG_IGN);
signal(SIGQUIT,SIG_IGN);
signal(SIGABRT,SIG_IGN);
signal(SIGFPE,SIG_IGN);
signal(SIGPIPE,SIG_IGN);
signal(SIGTTIN,SIG_IGN);
signal(SIGTTOU,SIG_IGN);
}


/*** Talker signal handler function. Can either shutdown , ignore or reboot
	if a unix error occurs though if we ignore it we're living on borrowed
	time as usually it will crash completely after a while anyway. ***/
void sig_handler(sig)
int sig;
{
force_listen=1;
switch(sig) {
	case SIGTERM:
	if (ignore_sigterm) {
          write_syslog(">> SIGTERM signal received - ignoring.\n",1);
		return;
		}
     write_room(NULL,"\n\n~OLSYSTEM:~FY~LI SIGTERM received, initiating shutdown!\n\n");
	talker_shutdown(NULL,"a termination signal (SIGTERM)",0); 

	case SIGSEGV:
	switch(crash_action) {
		case 0:	
          write_room(NULL,"\n\n\07~OLSYSTEM:~FY~LI PANIC - Segmentation fault, initiating shutdown!\n\n");
		talker_shutdown(NULL,"a segmentation fault (SIGSEGV)",0); 

		case 1:	
          write_room(NULL,"\n\n\07~OLSYSTEM:~FY~LI WARNING - A segmentation fault has just occured!\n\n");
		write_syslog("WARNING: A segmentation fault occured!\n",1);
		longjmp(jmpvar,0);

		case 2:
          write_room(NULL,"\n\n\07~OLSYSTEM:~FY~LI PANIC - Segmentation fault, initiating reboot!\n\n");
		talker_shutdown(NULL,"a segmentation fault (SIGSEGV)",1); 
		}

	case SIGBUS:
	switch(crash_action) {
		case 0:
          write_room(NULL,"\n\n\07~OLSYSTEM:~FY~LI PANIC - Bus error, initiating shutdown!\n\n");
		talker_shutdown(NULL,"a bus error (SIGBUS)",0);

		case 1:
          write_room(NULL,"\n\n\07~OLSYSTEM:~FY~LI WARNING - A bus error has just occured!\n\n");
		write_syslog("WARNING: A bus error occured!\n",1);
		longjmp(jmpvar,0);

		case 2:
          write_room(NULL,"\n\n\07~OLSYSTEM:~FY~LI PANIC - Bus error, initiating reboot!\n\n");
		talker_shutdown(NULL,"a bus error (SIGBUS)",1);
		}
	}
}

	
/*** Initialise sockets on ports ***/
init_sockets()
{
struct sockaddr_in bind_addr;
int i,on,size;

printf(">> Initialising sockets on ports: %d, %d, %d\n",port[0],port[1],port[2]);
on=1;
size=sizeof(struct sockaddr_in);
bind_addr.sin_family=AF_INET;
bind_addr.sin_addr.s_addr=INADDR_ANY;
for(i=0;i<3;++i) {
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


/*** Initialise connections to remote servers. Basically this tries to connect
     to the services listed in the config file and it puts the open sockets in 
	the NL_OBJECT linked list which the talker then uses ***/
init_connections()
{
NL_OBJECT nl;
RM_OBJECT rm;
int ret,cnt=0;

printf(">> Connecting to remote servers...\n");
errno=0;
for(rm=room_first;rm!=NULL;rm=rm->next) {
	if ((nl=rm->netlink)==NULL) continue;
	++cnt;
     printf("   Trying service %s at %s %d: ",nl->service,nl->site,nl->port);
	fflush(stdout);
	if ((ret=connect_to_site(nl))) {
		if (ret==1) {
               printf(">> %s.\n",sys_errlist[errno]);
               sprintf(text,">> NETLINK: Failed to connect to %s: %s.\n",nl->service,sys_errlist[errno]);
			}
		else {
               printf(">> Unknown hostname.\n");
               sprintf(text,">> NETLINK: Failed to connect to %s: Unknown hostname.\n",nl->service);
			}
		write_syslog(text,1);
		}
	else {
          printf(">> CONNECTED.\n");
          sprintf(text,">> NETLINK: Connected to %s (%s %d).\n",nl->service,nl->site,nl->port);
		write_syslog(text,1);
		nl->connect_room=rm;
		}
	}
if (cnt) printf(">> See system log for any further information.\n");
else printf(">> No remote connections configured.\n");
}


/*** Do the actual connection ***/
connect_to_site(nl)
NL_OBJECT nl;
{
struct sockaddr_in con_addr;
struct hostent *he;
int inetnum;
char *sn;

sn=nl->site;
/* See if number address */
while(*sn && (*sn=='.' || isdigit(*sn))) sn++;

/* Name address given */
if(*sn) {
	if(!(he=gethostbyname(nl->site))) return 2;
	memcpy((char *)&con_addr.sin_addr,he->h_addr,(size_t)he->h_length);
	}
/* Number address given */
else {
	if((inetnum=inet_addr(nl->site))==-1) return 1;
	memcpy((char *)&con_addr.sin_addr,(char *)&inetnum,(size_t)sizeof(inetnum));
	}
/* Set up stuff and disable interrupts */
if ((nl->socket=socket(AF_INET,SOCK_STREAM,0))==-1) return 1;
con_addr.sin_family=AF_INET;
con_addr.sin_port=htons(nl->port);
signal(SIGALRM,SIG_IGN);

/* Attempt the connect. This is where the talker may hang. */
if (connect(nl->socket,(struct sockaddr *)&con_addr,sizeof(con_addr))==-1) {
	reset_alarm();  return 1;
	}
reset_alarm();
nl->type=OUTGOING;
nl->stage=VERIFYING;
nl->last_recvd=time(0);
return 0;
}

	

/************* WRITE FUNCTIONS ************/

/*** Write a NULL terminated string to a socket ***/
write_sock(sock,str)
int sock;
char *str;
{
write(sock,str,strlen(str));
}



/*** Send message to user ***/
write_user(user,str)
UR_OBJECT user;
char *str;
{
int buffpos,sock,i;
char *start,buff[OUT_BUFF_SIZE],mesg[ARR_SIZE],*colour_com_strip();

if (user==NULL) return;
if (user->type==REMOTE_TYPE) {
	if (user->netlink->ver_major<=3 
	    && user->netlink->ver_minor<2) str=colour_com_strip(str);
	if (str[strlen(str)-1]!='\n') 
		sprintf(mesg,"MSG %s\n%s\nEMSG\n",user->name,str);
	else sprintf(mesg,"MSG %s\n%sEMSG\n",user->name,str);
	write_sock(user->netlink->socket,mesg);
	return;
	}
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
		/* See if its a / before a ~ , if so then we print colour command
		   as text */
		if (*str=='/' && *(str+1)=='~') {  ++str;  continue;  }
		if (str!=start && *str=='~' && *(str-1)=='/') {
			*(buff+buffpos)=*str;  goto CONT;
			}
          /* Process colour commands eg ~FY. We have to strip out the commands 
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
write_level(level,above,str,user)
int level,above;
char *str;
UR_OBJECT user;
{
UR_OBJECT u;
int logmsgs,bcastmsgs,wizmsgs;

switch(above) {
	case 2:
		logmsgs=1;
		bcastmsgs=0;
		wizmsgs=0;
		above=0;
		break;
	case 3:
		logmsgs=1;
		bcastmsgs=0;
		wizmsgs=0;
		above=1;
		break;
	case 4:
          bcastmsgs=1;
		wizmsgs=0;
		logmsgs=0;
		above=0;
		break;
	case 5: 
		wizmsgs=1;
		logmsgs=0;
		bcastmsgs=0;
		above=1;
		break;
	}
for(u=user_first;u!=NULL;u=u->next) {
	if (u!=user && !u->login && u->type!=CLONE_TYPE) {
		if ((logmsgs && u->ignall) || (bcastmsgs && u->ignall) || (wizmsgs && u->ignall)) continue;
		if (logmsgs && u->ignore & LOGON_MSGS)  continue;
		if (bcastmsgs && u->ignore & BCAST_MSGS)  continue;
		if (wizmsgs && u->ignore & WIZARD_MSGS)  continue;
		if ((above && u->level>=level) || (!above && u->level<=level)) 
			write_user(u,str);
		}
	}
}



/*** Subsid function to below but this one is used the most ***/
write_room(rm,str)
RM_OBJECT rm;
char *str;
{
write_room_except(rm,str,NULL);
}



/*** Write to everyone in room rm except for "user". If rm is NULL write 
     to all rooms. ***/
write_room_except(rm,str,user)
RM_OBJECT rm;
char *str;
UR_OBJECT user;
{
UR_OBJECT u;
char text2[ARR_SIZE];

for(u=user_first;u!=NULL;u=u->next) {
	if (u->login 
	    || u->room==NULL 
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
write_syslog(str,write_time)
char *str;
int write_time;
{
FILE *fp;

if (!system_logging || !(fp=fopen(SYSLOG,"a"))) return;
if (!write_time) fputs(str,fp);
else fprintf(fp,"%02d/%02d %02d:%02d:%02d: %s",tmday,tmonth+1,thour,tmin,tsec,str);
fclose(fp);
}

/*** Write a string to login log ***/
write_loginlog(str,write_time)
char *str;
int write_time;
{
FILE *fp;

if (!system_logging || !(fp=fopen(LOGINLOG,"a"))) return;
if (!write_time) fputs(str,fp);
else fprintf(fp,"%02d/%02d %02d:%02d:%02d: %s",tmday,tmonth+1,thour,tmin,tsec,str);
fclose(fp);
}

/*** Write a string to user's arrest file ***/
write_arrestrecord(user,str,write_time)
UR_OBJECT user;
char *str;
int write_time;
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
login(user,inpstr)
UR_OBJECT user;
char *inpstr;
{
UR_OBJECT u;
int i;
char name[ARR_SIZE],passwd[ARR_SIZE],site[80];

name[0]='\0';  passwd[0]='\0';
switch(user->login) {
	case 3:
	sscanf(inpstr,"%s",name);
     strcpy(site,user->site);
     sprintf(text,"~OL~FW~BM[~FTPre-Login~FW]~RS~OL: ~RS%s ~FT(Site: %s)\n",name,site);
     write_level(WIZ,3,text,user);
	if(name[0]<33) {
          write_user(user,"\nMay I Please Have Your Name? ");  return;
		}     
	if (!strcmp(name,"quit")) {
          write_user(user,"\n\nYou change your mind and turn around and walk away...\n\n");
		disconnect_user(user);  return;
		}
	if (!strcmp(name,"who")) {
		who(user,0);  
          write_user(user,"\nMay I Please Have Your Name? ");
		return;
		}	
	if (!strcmp(name,"Who")) {
		who(user,0);  
          write_user(user,"\nMay I Please Have Your Name? ");
		return;
		}
	if (!strcmp(name,"version")) {
          sprintf(text,"\nNUTS version %s\n\nMay I Please Have Your Name? ",VERSION);
		write_user(user,text);  return;
		}
	if (strlen(name)<3) {
		write_user(user,"\nSorry, you're gonna have to think of a longer name then that!\n\n");  
		attempts(user);  return;
		}
	if (strlen(name)>USER_NAME_LEN) {
		write_user(user,"\nUmm... That name is a little toooo long!  How about a shorter one?\n\n");
		attempts(user);  return;
		}
	/* see if only letters in login */
	for (i=0;i<strlen(name);++i) {
		if (!isalpha(name[i])) {
			write_user(user,"\nYou can only use letters in the name!\n\n");
			attempts(user);  return;
			}
		}
	if (!allow_caps_in_name) strtolower(name);
	name[0]=toupper(name[0]);
	if (user_banned(name)) {
          write_user(user,"\nYou Have Been BANNED from this talker!\n\n");
		disconnect_user(user);
		sprintf(text,"Attempted login by banned user %s.\n",name);
		write_syslog(text,1);
		return;
		}
	strcpy(user->name,name);
	/* If user has hung on another login clear that session */
	for(u=user_first;u!=NULL;u=u->next) {
		if (u->login && u!=user && !strcmp(u->name,user->name)) {
			disconnect_user(u);  break;
			}
		}	
	if (!load_user_details(user)) {
		if (user->port==port[1]) {
			write_user(user,"\nSorry, new logins cannot be created on this port.\n\n");
			disconnect_user(user);  
			return;
			}
		if (minlogin_level>-1) {
			write_user(user,"\nSorry, new logins cannot be created at this time.\n\n");
			disconnect_user(user);  
			return;
			}
		strcpy(site,user->site);
          sprintf(text,"~OLSYSTEM: ~FTNew User:~FG %s ~RSis logging in from: ~FY(%s)\n",user->name,site);
          write_level(WIZ,3,text,NULL);    
		if (newsite_banned(site)) {
			write_user(user,"\nLogins from your site/domain are banned.\n\n");
			disconnect_user(user);
			sprintf(text,"Attempted login by: %s from banned site for NEW users %s.\n",name,site);
			write_syslog(text,1);
               sprintf(text,"~OLSYSTEM: ~FRNew User:~RS %s is from banned site: %s was disconected\n",name,site);
               write_level(WIZ,3,text,NULL);
			return;
			}
          write_user(user,"~FTWelcome To ~OL~FYThe Talker!\n");
		}
	else {
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
     write_user(user,"Please enter your Password:  ");
	echo_off(user);
	user->login=2;
	return;

	case 2:
	sscanf(inpstr,"%s",passwd);
	if (strlen(passwd)<3) {
          write_user(user,"\n\nSorry, Your password must be atleast 3 characters long!\n\n");  
		attempts(user);  return;
		}
	if (strlen(passwd)>PASS_LEN) {
          write_user(user,"\n\nSorry, Your password is too long, please choose a shorter one!\n");
		attempts(user);  return;
		}
	/* if new user... */
	if (!user->pass[0]) {
		strcpy(user->pass,(char *)crypt(passwd,"NU"));
		write_user(user,"\nPlease enter your password again to verify it: ");
		user->login=1;
		}
	else {
		if (!strcmp(user->pass,(char *)crypt(passwd,"NU"))) {
		  if (!(in_userlist(user->name))) { add_userlist(user->name); }
		  echo_on(user);  connect_user(user);  return;
		  }
		write_user(user,"\n\nOoops... Wrong Password!\n\n");
		attempts(user);
		}
	return;

	case 1:
	sscanf(inpstr,"%s",passwd);
	if (strcmp(user->pass,(char*)crypt(passwd,"NU"))) {
		write_user(user,"\n\nYour passwords did not match!\n\n");
		attempts(user);
		return;
		}
	echo_on(user);
        strcpy(user->desc," ~OL-> ~FYNew Around Here");
        strcpy(user->in_phrase,"arrives in the room."); 
        strcpy(user->out_phrase,"walks out of the room"); 
	user->last_site[0]='\0';
	user->level=0;
	user->muzzled=0;
	user->command_mode=0;
	user->prompt=prompt_def;
	user->colour=colour_def;
	user->charmode_echo=charecho_def;
	save_user_details(user,1);
	sprintf(text,"New user \"%s\" created.\n",user->name);
	write_syslog(text,1);
	sprintf(text,"New user \"%s\" created.\n",user->name);
	write_loginlog(text,1);
	add_userlist(user->name);
	connect_user(user);
	}
}
	


/*** Count up attempts made by user to login ***/
attempts(user)
UR_OBJECT user;
{
user->attempts++;
if (user->attempts==3) {
     write_user(user,"\nSorry, You have made too many attempts to login.\n\n");
	disconnect_user(user);  return;
	}
user->login=3;
user->pass[0]='\0';
write_user(user,"May I Please Have Your Name? ");
echo_on(user);
}


/*** Load the users details ***/
load_user_details(user)
UR_OBJECT user;
{
FILE *fp;
char line[ARR_SIZE],filename[80];
int temp1,temp2,temp3;

sprintf(filename,"%s/%s.D",USERFILES,user->name);
if (!(fp=fopen(filename,"r"))) return 0;

fscanf(fp,"%s",user->pass); /* password */
fscanf(fp,"%d %d %d %d %d %d %d %d %d %d",&temp1,&temp2,&user->last_login_len,&temp3,&user->level,&user->prompt,&user->muzzled,&user->charmode_echo,&user->command_mode,&user->colour);
fscanf(fp,"%d %d %d %d %d %d\n",&user->a1,&user->a2,&user->a3,&user->a4,&user->a5,&user->a6);
user->last_login=(time_t)temp1;
user->total_login=(time_t)temp2;
user->read_mail=(time_t)temp3;
user->gaged=user->a2;
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
strcpy(user->c1,line);
fgets(line,ARR_SIZE,fp);
line[strlen(line)-1]=0;
strcpy(user->c2,line);
fclose(fp);
return 1;
}



/*** Save a users stats ***/
save_user_details(user,save_current)
UR_OBJECT user;
int save_current;
{
FILE *fp;
char filename[80];

if (user->type==REMOTE_TYPE || user->type==CLONE_TYPE) return 0;
sprintf(filename,"%s/%s.D",USERFILES,user->name);
if (!(fp=fopen(filename,"w"))) {
	sprintf(text,"%s: failed to save your details.\n",syserror);	
	write_user(user,text);
	sprintf(text,"SAVE_USER_STATS: Failed to save %s's details.\n",user->name);
	write_syslog(text,1);
	return 0;
	}
fprintf(fp,"%s\n",user->pass);
if (save_current)
	fprintf(fp,"%d %d %d ",(int)time(0),(int)user->total_login,(int)(time(0)-user->last_login));
else fprintf(fp,"%d %d %d ",(int)user->last_login,(int)user->total_login,user->last_login_len);
fprintf(fp,"%d %d %d %d %d %d %d\n",(int)user->read_mail,user->level,user->prompt,user->muzzled,user->charmode_echo,user->command_mode,user->colour);
fprintf(fp,"%d %d %d %d %d %d\n",user->a1,user->a2,user->a3,user->a4,user->a5,user->a6);
if (save_current) fprintf(fp,"%s\n",user->site);
else fprintf(fp,"%s\n",user->last_site);
fprintf(fp,"%s\n",user->desc);
fprintf(fp,"%s\n",user->in_phrase);
fprintf(fp,"%s\n",user->out_phrase);
fprintf(fp,"%s\n",user->c1);
fprintf(fp,"%s\n",user->c2);
fclose(fp);
return 1;
}


/*** Connect the user to the talker proper ***/
connect_user(user)
UR_OBJECT user;
{
UR_OBJECT u,u2;
RM_OBJECT rm;
char temp[30];
char levelname[20];

/* See if user already connected */
strcpy(user->ignuser,"NONE");
for(u=user_first;u!=NULL;u=u->next) {
	if (user!=u && user->type!=CLONE_TYPE && !strcmp(user->name,u->name)) {
		rm=u->room;
		if (u->type==REMOTE_TYPE) {
			write_user(u,"\n~FB~OLYou are pulled back through cyberspace...\n");
			sprintf(text,"REMVD %s\n",u->name);
			write_sock(u->netlink->socket,text);
			sprintf(text,"%s vanishes.\n",u->name);
			destruct_user(u);
			write_room(rm,text);
			reset_access(rm);
			num_of_users--;
			break;
			}
		write_user(user,"\n\nYou are already connected - switching to old session...\n");
		sprintf(text,"%s swapped sessions. (%s:%s)\n",user->name,user->site,user->site_port);
		write_loginlog(text,1);
		close(u->socket);
		u->socket=user->socket;
		strcpy(u->ignuser,user->ignuser);
		strcpy(u->site,user->site);
		strcpy(u->site_port,user->site_port);
		destruct_user(user);
		num_of_logins--;
		if (strstr(user->site,user->site_port))
          sprintf(text,"~OL[SWAP]: ~RS %s ~RS~FT(%s)\n",user->name,user->site);
          else sprintf(text,"~OL[SWAP]:~RS %s ~RS~FT(%s:%s)\n",user->name,user->site,user->site_port);
          write_level(WIZ,3,text,NULL);
		if (rm==NULL) {
			sprintf(text,"ACT %s look\n",u->name);
			write_sock(u->netlink->socket,text);
			}
		else {
			look(u);  prompt(u);
			}
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
if (user->a3==USERINVIS) {
	user->vis=0;
	}
else {
	user->vis=1;
}

/* Set User's Room To The Right Room If They Are Jailed Upon Login */

if (user->muzzled & JAILED) {
	user->room=room_last;
	}
	else {
	user->room=room_first;
	}

strcpy(levelname,level_name[user->level]);
if (user->prompt & FEMALE) strcpy(levelname,level_name_fem[user->level]);

if (user->vis) {
     sprintf(text,"\n~OL~FW~BM[~FYEntering The Talker~FW]~RS~OL~FT: ~RS%s %s ~RS~FY<< ~OL%s~RS~FY >> \n",user->name,user->desc,levelname);
     write_level(NEW,3,text,user);
}
else {
     sprintf(text,"\n~OL~FW~BM[~FYEntering The Talker~FW]~RS~OL~FT: ~RS%s %s ~RS~FY<< ~OL%s~RS~FY >> ~OL~FT[ ~RS~FTInvisible ~OL]\n",user->name,user->desc,levelname);
     write_level(user->level,3,text,NULL);
	write_room(user->room,"~FTYou suddenly get this feeling that someone else is watching you...\n");
}
if (!user->vis && user->level>REG) {
     if (strstr(user->site,user->site_port)) sprintf(text,"~FYConnecting From Site~FW: ~FT%s\n",user->site);
     else sprintf(text,"~FYConnecting From Site~FW: ~FT%s:%s\n",user->site,user->site_port); 
	write_level(user->level,5,text,NULL);
}
else {
     if (strstr(user->site,user->site_port)) sprintf(text,"~FYConnecting From Site~FW: ~FT%s\n",user->site);
     else sprintf(text,"~FYConnecting From Site~FW: ~FT%s:%s\n",user->site,user->site_port); 
	write_level(WIZ,3,text,NULL);
}
if (user->muzzled & JAILED) {
     sprintf(text,"~FR~OLThe earth opens up and %s falls straight into hell...\n",user->name);
     write_room_except(room_first,text,user);
     sprintf(text,"~FR~OLThe earth opens up and you fall straight into hell...\n");
     write_user(user,text);     
	}

/* send post-login message and other logon stuff to user */

write_user(user,"\n");
more(user,user->socket,MOTD2); 

if (user->last_site[0]) {
	sprintf(temp,"%s",ctime(&user->last_login));
	temp[strlen(temp)-1]=0;     
     sprintf(text,"\n~OL~FW~BM[~FYEntering The Talker~FW]~RS~OL~FT: ~RS%s %s ~RS~FY<< ~OL%s~RS~FY >> \n~RS~FYYou were last on %s from %s.\n\n",user->name,user->desc,levelname,temp,user->last_site);
	}

else sprintf(text,"\n~OL~FW~BM[~FYEntering The Talker~FW]~RS~OL~FT: ~RS%s %s ~RS~FY<< ~OL%s~RS~FY >> \n",user->name,user->desc,levelname);
write_user(user,text);
user->last_login=time(0); /* set to now */
look(user);

/* Quote Of The Day */
write_user(user,"\n~OL~FYYour quote for today is:\n");
sprintf(text,"~FY> ~FM%s ~RS\n",quote_of_the_day[rand()%MAX_QUOTES]);
write_user(user,text);

/* Auto Promtoe Message */

if (user->level==NEW && !(user->muzzled & JAILED)) {
     write_user(user,"\n"); 
     write_user(user,"~OL~FTWelcome To ~FRThe Talker~FY!!~FG  Please take a moment and set the following items.\n");
     write_user(user,"~OL~FGThen you will be Auto-Promoted to the next level.\n\n");
     write_user(user,"~OL~FYStep ~FY1~FB... ~FGType:  ~FW.accreq ~RS~FT[Your Real E-Mail Address]\n");
     write_user(user,"~OL~FTStep ~FY2~FB... ~FGType:  ~FW.desc ~RS~FT<a new description>  i.e.  .desc is new at this.\n");
     write_user(user,"~OL~FTStep ~FY3~FB... ~FGType:  ~FW.gender ~RS~FTmale/female        (It is Required)\n");
     write_user(user,"~OL~FTStep ~FY4~FB... ~FGType:  ~FW.entpro ~RS~FTAnd enter a small profile of yourself.\n");
     write_user(user,"~OL~FTStep ~FY5~FB... ~FGType:  ~FW.set age ~RS~FT<~FR1 ~FTto ~FR99~FT>\n\n");
     write_user(user,"~OL~FGAnd you will be promoted to the next level where you will become a USER.\n");
     write_user(user,"** Note:  You can turn color on by typeing .color **\n");
}

if (has_unread_mail(user)) {
     write_user(user,"\n\n");
     write_user(user,"~OL~FYY88b    /                        888                                    \n");
     write_user(user,"~OL~FY Y88b  /   e88~-.  888  888      888-~88e   //~~~8e  Y88b    /  e88~~8e  \n");
     write_user(user,"~OL~FY  Y88b/   d888   i 888  888      888  888       88b  Y88b  /  d888  88b \n");
     write_user(user,"~OL~FY   Y8Y    8888   | 888  888      888  888  e88~-888   Y88b/   8888__888 \n");
     write_user(user,"~OL~FY    Y     Y888   | 888  888      888  888 C888  888    Y8/    Y888    , \n");
     write_user(user,"~OL~FY   /       '88__,' '88_-888      888  888  '88_-888     Y      '88___/  \n");
     write_user(user,"\n");
     write_user(user,"~OL~FY888b    | 888~~  Y88b         /                          ,e, 888  d8b   \n");
     write_user(user,"~OL~FY|Y88b   | 888___  Y88b       /   888-~88e-~88e   //~~~8e   '  888 !Y88!  \n");
     write_user(user,"~OL~FY| Y88b  | 888      Y88b  e  /    888  888  888       88b 888 888  Y8Y   \n");
     write_user(user,"~OL~FY|  Y88b | 888       Y88bd8b/     888  888  888  e88~-888 888 888   8    \n");
     write_user(user,"~OL~FY|   Y88b| 888        Y88Y8Y      888  888  888 C888  888 888 888   e    \n");
     write_user(user,"~OL~FY|    Y888 888___      Y  Y       888  888  888  '88_-888 888 888  '8'   \n\n");
     }
prompt(user);

/* write to loginlog and set up some vars */
sprintf(text,"%s logged in on port %d from %s : %s.\n",user->name,user->port,user->site,user->site_port);
write_loginlog(text,1);
num_of_users++;
num_of_logins--;
user->login=0;
/* Create user's Room */
if (user->level>NEW && user->level<WIZ) myroom(user);

}


/*** Disconnect user from talker ***/
disconnect_user(user)
UR_OBJECT user;
{
RM_OBJECT rm;
NL_OBJECT nl;

rm=user->room;
if (user->login) {
	close(user->socket);  
	destruct_user(user);
	num_of_logins--;  
	return;
	}
if (user->type!=REMOTE_TYPE) {
	if (!user->vis) {
		user->a3=USERINVIS;
		}
	else {
		user->a3=USERVIS;
	}
	save_user_details(user,1);  
     sprintf(text,"\n~OL~FYThank you for visiting our talker, %s!\nWe hope to see you again soon!!\n\n",user->name);
    	write_user(user,text);
	close(user->socket);
	if (user->vis) {
		sprintf(text,"%s logged out while visible.\n",user->name);
		write_loginlog(text,1);
               sprintf(text,"~OL~FW~BM[~FYLeaving The Talker~FW]~RS~OL~FW: ~FT%s %s \n",user->name,user->desc);
               write_level(OWNER,2,text,NULL);
		}
	if (!user->vis) {
		sprintf(text,"%s logged out while invisible.\n",user->name);
		write_loginlog(text,1);
          sprintf(text,"~OL~FW~BM[~FYLeaving The Talker~FW]~RS~OL~FW: ~FT%s %s ~RS~FT[ ~OLInvisible ~RS~FT] \n",user->name,user->desc);
		write_level(user->level,5,text,NULL);
		}		
	if (user->room==NULL) {
		sprintf(text,"REL %s\n",user->name);
		write_sock(user->netlink->socket,text);
		for(nl=nl_first;nl!=NULL;nl=nl->next) 
			if (nl->mesg_user==user) {  
				nl->mesg_user=(UR_OBJECT)-1;  break;  
				}
             }
	}
else {
	write_user(user,"\n~FR~OLYou are pulled back in disgrace to your own domain...\n");
	sprintf(text,"REMVD %s\n",user->name);
	write_sock(user->netlink->socket,text);
     sprintf(text,"~FY~OL%s is banished from here!\n",user->name);
	write_room_except(rm,text,user);
	sprintf(text,"NETLINK: Remote user %s removed.\n",user->name);
	write_loginlog(text,1);
	}
if (user->malloc_start!=NULL) free(user->malloc_start);
num_of_users--;

/* Destroy any clones */
destroy_user_clones(user);
destruct_user(user);
reset_access(rm);
destructed=0;
}

/*** Logoff user from the talaker QUIIETLY ***/
logoff_user(user)
UR_OBJECT user;
{
RM_OBJECT rm;
NL_OBJECT nl;

rm=user->room;
if (user->login) {
	close(user->socket);  
	destruct_user(user);
	num_of_logins--;  
	return;
	}
if (user->type!=REMOTE_TYPE) {
	if (!user->vis) {
		user->a3=USERINVIS;
		}
	else {
		user->a3=USERVIS;
	}
	save_user_details(user,1);
     sprintf(text,"%s quietly logged out.\n",user->name);
	write_loginlog(text,1);
     sprintf(text,"\n~OL~FYThank you for visiting our talker, %s!\n~OL~FYWe hope to see you again soon!! \n\n",user->name);
     write_user(user,text);
	close(user->socket);
     sprintf(text,"~OL~FW~BM[~FYLeaving The Talker~FW]~RS~OL~FW: ~FT%s %s ~RS~FT[ ~OLInvisible ~RS~FT] \n",user->name,user->desc);
     write_level(WIZ,5,text,user);
	if (user->room==NULL) {
		sprintf(text,"REL %s\n",user->name);
		write_sock(user->netlink->socket,text);
		for(nl=nl_first;nl!=NULL;nl=nl->next) 
			if (nl->mesg_user==user) {  
				nl->mesg_user=(UR_OBJECT)-1;  break;  
				}
		}
	}
else {
	write_user(user,"\n~FR~OLYou are pulled back in disgrace to your own domain...\n");
	sprintf(text,"REMVD %s\n",user->name);
	write_sock(user->netlink->socket,text);
     sprintf(text,"~FY~OL%s is banished from here!\n",user->name);
	write_room_except(rm,text,user);
	sprintf(text,"NETLINK: Remote user %s removed.\n",user->name);
	write_loginlog(text,1);
	}
if (user->malloc_start!=NULL) free(user->malloc_start);
num_of_users--;

/* Destroy any clones */
destroy_user_clones(user);
destruct_user(user);
reset_access(rm);
destructed=0;
}


/*** Tell telnet not to echo characters - for password entry ***/
echo_off(user)
UR_OBJECT user;
{
char seq[4];

if (password_echo) return;
sprintf(seq,"%c%c%c",255,251,1);
write_user(user,seq);
}


/*** Tell telnet to echo characters ***/
echo_on(user)
UR_OBJECT user;
{
char seq[4];

if (password_echo) return;
sprintf(seq,"%c%c%c",255,252,1);
write_user(user,seq);
}



/************ MISCELLANIOUS FUNCTIONS *************/

/*** Stuff that is neither speech nor a command is dealt with here ***/
misc_ops(user,inpstr)
UR_OBJECT user;
char *inpstr;
{
switch(user->misc_op) {
	case 1: 
	if (toupper(inpstr[0])=='Y') {
		if (rs_countdown && !rs_which) {
			if (rs_countdown>60) 
                    sprintf(text,"\n\07~OLSYSTEM: ~FY~LISHUTDOWN INITIATED, shutdown in %d minutes, %d seconds!\n\n",rs_countdown/60,rs_countdown%60);
               else sprintf(text,"\n\07~OLSYSTEM: ~FY~LISHUTDOWN INITIATED, shutdown in %d seconds!\n\n",rs_countdown);
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
	editor(user,inpstr);  return 1;
	
	case 6:
	if (toupper(inpstr[0])=='Y') delete_user(user,1); 
	else {  user->misc_op=0;  prompt(user);  }
	return 1;

	case 7:
	if (toupper(inpstr[0])=='Y') {
		if (rs_countdown && rs_which==1) {
			if (rs_countdown>60) 
				sprintf(text,"\n\07~OLSYSTEM: ~FY~LIREBOOT INITIATED, rebooting in %d minutes, %d seconds!\n\n",rs_countdown/60,rs_countdown%60);
			else sprintf(text,"\n\07~OLSYSTEM: ~FY~LIREBOOT INITIATED, rebooting in %d seconds!\n\n",rs_countdown);
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
	}
return 0;
}


/*** The editor used for writing profiles, mail and messages on the boards ***/
editor(user,inpstr)
UR_OBJECT user;
char *inpstr;
{
int cnt,line;
char *edprompt="\n~OL~FY[~FRS~FY]ave, ~FY[~FRR~FY]edo, ~FY[~FRA~FY]bort~FW: ~RS";
char *ptr;
char genderx[4];

strcpy(genderx,"its");
if (user->prompt & 4) strcpy(genderx,"her");
if (user->prompt & 8) strcpy(genderx,"his");

if (user->edit_op) {
	switch(toupper(*inpstr)) {
		case 'S':
          sprintf(text,"%s finishes writing %s message.\n",user->name,genderx);
		write_room_except(user->room,text,user);
		switch(user->misc_op) {
			case 3: write_board(user,NULL,1);  break;
			case 4: smail(user,NULL,1);  break;
			case 5: enter_profile(user,1);  break;
			case 8: room_desc(user,1);  break;
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
          sprintf(text,"   ~OL~FG[~FY--------~FG1~FY---------~FG2~FY---------~FG3~FY---------~FG4~FY---------~FG5~FY---------~FG6~FY---------~FG7~FY----~FG]\n");
          write_user(user,text);
          sprintf(text,"~OL~FR%2.2d~RS~FB>~RS",user->edit_line);
          write_user(user,text);
		return;

          case 'A': 
          write_user(user,"\n~OL~FG>> ~FWMessage aborted...\n");
          sprintf(text,"~FY%s decided to give up %s writing career.\n",user->name,genderx);
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
     sprintf(text,"\012   ~OL~FTYou can write a maximum of %d lines, Enter a '.' on a line by itself to end!\n\n",MAX_LINES);
	write_user(user,text);
     sprintf(text,"   ~OL~FG[~FY--------~FG1~FY---------~FG2~FY---------~FG3~FY---------~FG4~FY---------~FG5~FY---------~FG6~FY---------~FG7~FY----~FG]\n");
     write_user(user,text);
     sprintf(text,"~OL~FR%2.2d~RS~FB>~RS",user->edit_line);
	write_user(user,text);
     sprintf(text,"%s gets out %s pen and begins to write...\n",user->name,genderx);
	write_room_except(user->room,text,user);
	return;
	}

/* Check for empty line */
if (!word_count) {
	if (!user->charcnt) {
          sprintf(text,"~OL~FR%2.2d~RS~FB>~RS",user->edit_line);
		write_user(user,text);
		return;
		}
	*user->malloc_end++='\n';
	if (user->edit_line==MAX_LINES) goto END;
     sprintf(text,"~OL~FR%2.2d~RS~FB>~RS",++user->edit_line);
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
     sprintf(text,"%2.2d>%s",user->edit_line,ptr);
	write_user(user,text);
	user->charcnt=cnt;
	return;
	}
else {
	*user->malloc_end++='\n';
	user->charcnt=0;
	}
if (user->edit_line!=MAX_LINES) {
     sprintf(text,"~OL~FR%2.2d~RS~FB>~RS",++user->edit_line);
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
write_user(user,"\n~OL~FG>> ~FRNo text has been written...\n");
sprintf(text,"%s gives up %s writing career...\n",user->name,genderx);
write_room_except(user->room,text,user);
editor_done(user);
}


/*** Reset some values at the end of editing ***/
editor_done(user)
UR_OBJECT user;
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
     sprintf(text,"~OL~FG>~FY YOU HAVE NEW TELLS ~FG< ~FYtype ~FG.revtell ~FYto view them.\n");
	write_user(user,text);
	user->chkrev=0;
	}
}


/*** Record speech and emotes in the room. ***/
record(rm,str)
RM_OBJECT rm;
char *str;
{
strncpy(rm->revbuff[rm->revline],str,REVIEW_LEN);
rm->revbuff[rm->revline][REVIEW_LEN]='\n';
rm->revbuff[rm->revline][REVIEW_LEN+1]='\0';
rm->revline=(rm->revline+1)%REVIEW_LINES;
}


/*** Records tells and pemotes sent to the user. ***/
record_tell(user,str)
UR_OBJECT user;
char *str;
{
strncpy(user->revbuff[user->revline],str,REVIEW_LEN);
user->revbuff[user->revline][REVIEW_LEN]='\n';
user->revbuff[user->revline][REVIEW_LEN+1]='\0';
user->revline=(user->revline+1)%REVTELL_LINES;
}


/*** Records tells and pemotes sent to the user. ***/
record_wiz(user,str)
UR_OBJECT user;
char *str;
{
     strncpy(wizrevbuff[wrevline],str,REVIEW_LEN);
     wizrevbuff[wrevline][REVIEW_LEN]='\n';
     wizrevbuff[wrevline][REVIEW_LEN+1]='\0';
     wrevline=(wrevline+1)%REVIEW_LINES;
}

/*** Records tells and pemotes sent to the user. ***/
record_shout(user,str)
UR_OBJECT user;
char *str;
{
     strncpy(shoutrevbuff[srevline],str,REVIEW_LEN);
     shoutrevbuff[srevline][REVIEW_LEN]='\n';
     shoutrevbuff[srevline][REVIEW_LEN+1]='\0';
     srevline=(srevline+1)%REVIEW_LINES;
}


/*** Set room access back to public if not enough users in room ***/
reset_access(rm)
RM_OBJECT rm;
{
UR_OBJECT u;
int cnt;

if (rm==NULL || rm->access!=PRIVATE || rm->access & PERSONAL) return; 
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
boot_exit(code)
int code;
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
prompt(user)
UR_OBJECT user;
{
int hr,min;

if (no_prompt) return;
if (user->type==REMOTE_TYPE) {
	sprintf(text,"PRM %s\n",user->name);
	write_sock(user->netlink->socket,text);  
	return;
	}
if (user->command_mode && !user->misc_op) {  
	if (!user->vis) write_user(user,"~FTCOM+> ");
	else write_user(user,"~FTCOM> ");  
	return;  
	}
if (!(user->prompt & 1) || user->misc_op) return;
hr=(int)(time(0)-user->last_login)/3600;
min=((int)(time(0)-user->last_login)%3600)/60;
if (!user->vis)
	sprintf(text,"~FT<%02d:%02d, %02d:%02d, %s+>\n",thour,tmin,hr,min,user->name);
else sprintf(text,"~FT<%02d:%02d, %02d:%02d, %s>\n",thour,tmin,hr,min,user->name);
write_user(user,text);
}



/*** Page a file out to user. Colour commands in files will only work if 
     user!=NULL since if NULL we dont know if his terminal can support colour 
     or not. Return values: 
	        0 = cannot find file, 1 = found file, 2 = found and finished ***/
more(user,sock,filename)
UR_OBJECT user;
int sock;
char *filename;
{
int i,buffpos,num_chars,lines,retval,len,rows;
char buff[OUT_BUFF_SIZE],text2[83],*str,*colour_com_strip();
FILE *fp;

if (com_num==MAP)  rows=35;
else  rows=23;
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
	if (sock==-1) {
		lines++;  
		if (user->netlink->ver_major<=3 && user->netlink->ver_minor<2) 
			str=colour_com_strip(text2);
		else str=text2;
		if (str[strlen(str)-1]!='\n') 
			sprintf(text,"MSG %s\n%s\nEMSG\n",user->name,str);
		else sprintf(text,"MSG %s\n%sEMSG\n",user->name,str);
		write_sock(user->netlink->socket,text);
		num_chars+=strlen(str);
		fgets(text2,82,fp);
		continue;
		}
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
			if (*str=='/' && *(str+1)=='~') {  ++str;  continue;  }
			if (str!=text && *str=='~' && *(str-1)=='/') {
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
	/* We use E here instead of Q because when on a remote system and
	   in COMMAND mode the Q will be intercepted by the home system and 
	   quit the user */
     write_user(user,"              ~OL~BR~FT .oO [~FWENTER~FT] = Continue, [~FWE~FT] + [~FWENTER~FT] = Exit Oo. ~RS\n\n");
	no_prompt=1;
	}
fclose(fp);
return retval;
}



/*** Set global vars. hours,minutes,seconds,date,day,month,year ***/
set_date_time()
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
char *remove_first(inpstr)
char *inpstr;
{
char *pos=inpstr;
while(*pos<33 && *pos) ++pos;
while(*pos>32) ++pos;
while(*pos<33 && *pos) ++pos;
return pos;
}


/*** Get user struct pointer from name ***/
UR_OBJECT get_user(name)
char *name;
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


/*** Get room struct pointer from abbreviated name ***/
RM_OBJECT get_room(name)
char *name;
{
RM_OBJECT rm;

for(rm=room_first;rm!=NULL;rm=rm->next)
     if (!strncmp(rm->name,name,strlen(name))) return rm;
return NULL;
}


/*** Return level value based on level name ***/
get_level(name)
char *name;
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
has_room_access(user,rm)
UR_OBJECT user;
RM_OBJECT rm;
{
if ((rm->access & PRIVATE) 
    && user->level<gatecrash_level 
    && user->invite_room!=rm
    && !((rm->access & FIXED) && user->level>=WIZ)) return 0;
return 1;
}


/*** See if user has unread mail, mail file has last read time on its 
     first line ***/
has_unread_mail(user)
UR_OBJECT user;
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
send_mail(user,to,ptr)
UR_OBJECT user;
char *to,*ptr;
{
NL_OBJECT nl;
FILE *infp,*outfp;
char *c,d,*service,filename[80],line[DNL+1];

/* See if remote mail */
c=to;  service=NULL;
while(*c) {
	if (*c=='@') {  
		service=c+1;  *c='\0'; 
		for(nl=nl_first;nl!=NULL;nl=nl->next) {
			if (!strcmp(nl->service,service) && nl->stage==UP) {
				send_external_mail(nl,user,to,ptr);
				return;
				}
			}
		sprintf(text,"Service %s unavailable.\n",service);
		write_user(user,text); 
		return;
		}
	++c;
	}

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
if (infp=fopen(filename,"r")) {
	/* Discard first line of mail file. */
	fgets(line,DNL,infp);

	/* Copy rest of file */
	d=getc(infp);  
	while(!feof(infp)) {  putc(d,outfp);  d=getc(infp);  }
	fclose(infp);
	}

/* Put new mail in tempfile */
if (user!=NULL) {
	if (user->type==REMOTE_TYPE)
		fprintf(outfp,"From: %s@%s \n%s \n",user->name,user->netlink->service,long_date(0));
     else fprintf(outfp,"~BMFrom: %s \n~BM%s \n",user->name,long_date(0));
	}
else fprintf(outfp,"~BM From: MAILER  %s\n",long_date(0));

fputs(ptr,outfp);
fputs("\n",outfp);
fclose(outfp);
rename("tempfile",filename);
count_messages(user);
sprintf(text,"%s smailed %s\n",user->name,to);
write_syslog(text,1);

     sprintf(text,"~FT~OLYou walk over to the mailbox and drop your letter to ~FY%s~FT in it...Thank You!!!\n",to);

write_user(user,text);
if (user->type==REMOTE_TYPE) {
     sprintf(text,"\07\n~OLNew Mail Has Arived For You From: ~FY%s@%s\n~OLSent To You: ~FG%s \n",user->name,user->netlink->service,long_date(0));
     	write_user(get_user(to),text);
	}
else { 
     sprintf(text,"\07\n~OLNew Mail Has Arived For You From: ~FY%s \n~OLSent To You: ~FG%s \n",user->name,long_date(0));
	write_user(get_user(to),text);
	} 
}

/*** Spool mail file and ask for confirmation of users existence on remote
	site ***/
send_external_mail(nl,user,to,ptr)
NL_OBJECT nl;
UR_OBJECT user;
char *to,*ptr;
{
FILE *fp;
char filename[80];

/* Write out to spool file first */
sprintf(filename,"%s/OUT_%s_%s@%s",MAILSPOOL,user->name,to,nl->service);
if (!(fp=fopen(filename,"a"))) {
	sprintf(text,"%s: unable to spool mail.\n",syserror);
	write_user(user,text);
	sprintf(text,"ERROR: Couldn't open file %s to append in send_external_mail().\n",filename);
	write_syslog(text,0);
	return;
	}
putc('\n',fp);
fputs(ptr,fp);
fclose(fp);

/* Ask for verification of users existence */
sprintf(text,"EXISTS? %s %s\n",to,user->name);
write_sock(nl->socket,text);

/* Rest of delivery process now up to netlink functions */
sprintf(text,"%s smailed %s\n",user->name,to);
write_syslog(text,1);
sprintf(text,"~OL~FTYou walk over and drop you letter to ~FY%s~FT in it...  Thank You!!!\n",to);
write_user(user,text);
}


/*** See if string contains any swearing ***/
contains_swearing(str)
char *str;
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
colour_com_count(str)
char *str;
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
char *colour_com_strip(str)
char *str;
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
char *long_date(which)
int which;
{
static char dstr[80];

if (which) sprintf(dstr,"on %s %d %s %d at %02d:%02d",day[twday],tmday,month[tmonth],tyear,thour,tmin);
else sprintf(dstr,"[ %s %d %s %d at %02d:%02d ]",day[twday],tmday,month[tmonth],tyear,thour,tmin);
return dstr;
}


/*** Clear the review buffer in the room ***/
clear_revbuff(rm)
RM_OBJECT rm;
{
int c;

for(c=0;c<REVIEW_LINES;++c) rm->revbuff[c][0]='\0';
rm->revline=0;
}

/*** Clear the shout review buffer in the room ***/
clear_shoutrevbuff()
{
int c;

for(c=0;c<REVIEW_LINES;++c) shoutrevbuff[c][0]='\0';
srevline=0;
}

/*** Clear the wiz review buffer in the room ***/
clear_wizrevbuff()
{
int c;

for(c=0;c<REVIEW_LINES;++c) wizrevbuff[c][0]='\0';
wrevline=0;
}


/*** Clear the screen ***/
cls(user)
UR_OBJECT user;
{
int i;

for(i=0;i<5;++i) write_user(user,"\n\n\n\n\n\n\n\n\n\n");		
}
/*** Display Time And Date To User ***/

showtime(user)
UR_OBJECT user;
{
char bstr[40],localtm[40],text2[ARR_SIZE];
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
sprintf(text,"The current system time is : ~OL%s, %d %s, %02d:%02d:%02d %d~RS~FT~BK\n\n",day[twday],tmday,month[tmonth],thour,tmin,tsec,tyear);
write_user(user,text);
/*
sprintf(text,"\07\n~OL-> ~FYThe Current Time And Date Is: %s\n",long_date(0));
write_user(user,text);
*/
}

/*** Clear All The Screens In The Room screen ***/
cls_all(user)
UR_OBJECT user;
{
int i;

for(i=0;i<5;++i) write_room(user->room,"\n\n\n\n\n\n\n\n\n\n");    
sprintf(text,"\n~OL~FR%s has cleared everyone's screen...\n",user->name);
write_room(user->room,text);
}


/*** Convert string to upper case ***/
strtoupper(str)
char *str;
{
while(*str) {  *str=toupper(*str);  str++; }
}


/*** Convert string to lower case ***/
strtolower(str)
char *str;
{
while(*str) {  *str=tolower(*str);  str++; }
}


/*** Returns 1 if string is a positive number ***/
isnumber(str)
char *str;
{
while(*str) if (!isdigit(*str++)) return 0;
return 1;
}


/************ OBJECT FUNCTIONS ************/

/*** Construct user/clone object ***/
UR_OBJECT create_user()
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
user->netlink=NULL;
user->pot_netlink=NULL; 
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
for(i=0;i<REVTELL_LINES;++i) user->revbuff[i][0]='\0';
user->samesite_all_store=0;
user->samesite_check_store[0]='\0';
user->a1=0;
user->a2=0;
user->a3=0;
user->a4=0;
user->a5=0;
user->a6=0;
user->c1[0]='\0';
user->c2[0]='\0';
return user;
}



/*** Destruct an object. ***/
destruct_user(user)
UR_OBJECT user;
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


/*** Construct room object ***/
RM_OBJECT create_room()
{
RM_OBJECT room;
int i;

if ((room=(RM_OBJECT)malloc(sizeof(struct room_struct)))==NULL) {
	fprintf(stderr,"NUTS: Memory allocation failure in create_room().\n");
	boot_exit(1);
	}
room->name[0]='\0';
room->label[0]='\0';
room->desc[0]='\0';
room->topic[0]='\0';
room->owner[0]='\0';
room->map[0]='\0';
room->access=-1;
room->revline=0;
room->mesg_cnt=0;
room->inlink=0;
room->netlink=NULL;
room->netlink_name[0]='\0';
room->next=NULL;
for(i=0;i<MAX_LINKS;++i) {
	room->link_label[i][0]='\0';  room->link[i]=NULL;
	}
for(i=0;i<REVIEW_LINES;++i) room->revbuff[i][0]='\0';
if (room_first==NULL) room_first=room;
else room_last->next=room;
room_last=room;
return room;
}


/*** Construct link object ***/
NL_OBJECT create_netlink()
{
NL_OBJECT nl;

if ((nl=(NL_OBJECT)malloc(sizeof(struct netlink_struct)))==NULL) {
	sprintf(text,"NETLINK: Memory allocation failure in create_netlink().\n");
	write_syslog(text,1);
	return NULL;
	}
if (nl_first==NULL) { 
	nl_first=nl;  nl->prev=NULL;  nl->next=NULL;
	}
else {  
	nl_last->next=nl;  nl->next=NULL;  nl->prev=nl_last;
	}
nl_last=nl;

nl->service[0]='\0';
nl->site[0]='\0';
nl->verification[0]='\0';
nl->mail_to[0]='\0';
nl->mail_from[0]='\0';
nl->mailfile=NULL;
nl->buffer[0]='\0';
nl->ver_major=0;
nl->ver_minor=0;
nl->ver_patch=0;
nl->keepalive_cnt=0;
nl->last_recvd=0;
nl->port=0;
nl->socket=0;
nl->mesg_user=NULL;
nl->connect_room=NULL;
nl->type=UNCONNECTED;
nl->stage=DOWN;
nl->connected=0;
nl->lastcom=-1;
nl->allow=ALL;
nl->warned=0;
return nl;
}


/*** Destruct a netlink (usually a closed incoming one). ***/
destruct_netlink(nl)
NL_OBJECT nl;
{
if (nl!=nl_first) {
	nl->prev->next=nl->next;
	if (nl!=nl_last) nl->next->prev=nl->prev;
	else { nl_last=nl->prev; nl_last->next=NULL; }
	}
else {
	nl_first=nl->next;
	if (nl!=nl_last) nl_first->prev=NULL;
	else nl_last=NULL; 
	}
free(nl);
}


/*** Destroy all clones belonging to given user ***/
destroy_user_clones(user)
UR_OBJECT user;
{
UR_OBJECT u;

for(u=user_first;u!=NULL;u=u->next) {
	if (u->type==CLONE_TYPE && u->owner==user) {
		sprintf(text,"The clone of %s shimmers and vanishes.\n",u->name);
		write_room(u->room,text);
		destruct_user(u);
		}
	}
}


/************ NUTS PROTOCOL AND LINK MANAGEMENT FUNCTIONS ************/
/* Please don't alter these functions. If you do you may introduce 
   incompatabilities which may prevent other systems connecting or cause
   bugs on the remote site and yours. You may think it looks simple but
   even the newline count is important in some places. */

/*** Accept incoming server connection ***/
accept_server_connection(sock,acc_addr)
int sock;
struct sockaddr_in acc_addr;
{
NL_OBJECT nl,nl2,create_netlink();
RM_OBJECT rm;
char site[81];

/* Send server type id and version number */
sprintf(text,"NUTS %s\n",VERSION);
write_sock(sock,text);
strcpy(site,get_ip_address(acc_addr));
sprintf(text,"NETLINK: Received request connection from site %s.\n",site);
write_syslog(text,1);

/* See if legal site, ie site is in config sites list. */
for(nl2=nl_first;nl2!=NULL;nl2=nl2->next) 
	if (!strcmp(nl2->site,site)) goto OK;
write_sock(sock,"DENIED CONNECT 1\n");
close(sock);
write_syslog("NETLINK: Request denied, remote site not in valid sites list.\n",1);
return;

/* Find free room link */
OK:
for(rm=room_first;rm!=NULL;rm=rm->next) {
	if (rm->netlink==NULL && rm->inlink) {
		if ((nl=create_netlink())==NULL) {
			write_sock(sock,"DENIED CONNECT 2\n");  
			close(sock);  
			write_syslog("NETLINK: Request denied, unable to create netlink object.\n",1);
			return;
			}
		rm->netlink=nl;
		nl->socket=sock;
		nl->type=INCOMING;
		nl->stage=VERIFYING;
		nl->connect_room=rm;
		nl->allow=nl2->allow;
		nl->last_recvd=time(0);
		strcpy(nl->service,"<verifying>");
		strcpy(nl->site,site);
		write_sock(sock,"GRANTED CONNECT\n");
		write_syslog("NETLINK: Request granted.\n",1);
		return;
		}
	}
write_sock(sock,"DENIED CONNECT 3\n");
close(sock);
write_syslog("NETLINK: Request denied, no free room links.\n",1);
}
		

/*** Deal with netlink data on link nl ***/
exec_netcom(nl,inpstr)
NL_OBJECT nl;
char *inpstr;
{
int netcom_num,lev;
char w1[ARR_SIZE],w2[ARR_SIZE],w3[ARR_SIZE],*c,ctemp;

/* The most used commands have been truncated to save bandwidth, ie ACT is
   short for action, EMSG is short for end message. Commands that don't get
   used much ie VERIFICATION have been left long for readability. */
char *netcom[]={
"DISCONNECT","TRANS","REL","ACT","GRANTED",
"DENIED","MSG","EMSG","PRM","VERIFICATION",
"VERIFY","REMVD","ERROR","EXISTS?","EXISTS_NO",
"EXISTS_YES","MAIL","ENDMAIL","MAILERROR","KA",
"RSTAT","*"
};

/* The buffer is large (ARR_SIZE*2) but if a bug occurs with a remote system
   and no newlines are sent for some reason it may overflow and this will 
   probably cause a crash. Oh well, such is life. */
if (nl->buffer[0]) {
	strcat(nl->buffer,inpstr);  inpstr=nl->buffer;
	}
nl->last_recvd=time(0);

/* Go through data */
while(*inpstr) {
	w1[0]='\0';  w2[0]='\0';  w3[0]='\0';  lev=0;
	if (*inpstr!='\n') sscanf(inpstr,"%s %s %s %d",w1,w2,w3,&lev);
	/* Find first newline */
	c=inpstr;  ctemp=1; /* hopefully we'll never get char 1 in the string */
	while(*c) {
		if (*c=='\n') {  ctemp=*(c+1); *(c+1)='\0';  break; }
		++c;
		}
	/* If no newline then input is incomplete, store and return */
	if (ctemp==1) {  
		if (inpstr!=nl->buffer) strcpy(nl->buffer,inpstr);  
		return;  
		}
	/* Get command number */
	netcom_num=0;
	while(netcom[netcom_num][0]!='*') {
		if (!strcmp(netcom[netcom_num],w1))  break;
		netcom_num++;
		}
	/* Deal with initial connects */
	if (nl->stage==VERIFYING) {
		if (nl->type==OUTGOING) {
			if (strcmp(w1,"NUTS")) {
				sprintf(text,"NETLINK: Incorrect connect message from %s.\n",nl->service);
				write_syslog(text,1);
				shutdown_netlink(nl);
				return;
				}	
			/* Store remote version for compat checks */
			nl->stage=UP;
			w2[10]='\0'; 
			sscanf(w2,"%d.%d.%d",&nl->ver_major,&nl->ver_minor,&nl->ver_patch);
			goto NEXT_LINE;
			}
		else {
			/* Incoming */
			if (netcom_num!=9) {
				/* No verification, no connection */
				sprintf(text,"NETLINK: No verification sent by site %s.\n",nl->site);
				write_syslog(text,1);
				shutdown_netlink(nl);  
				return;
				}
			nl->stage=UP;
			}
		}
	/* If remote is currently sending a message relay it to user, don't
	   interpret it unless its EMSG or ERROR */
	if (nl->mesg_user!=NULL && netcom_num!=7 && netcom_num!=12) {
		/* If -1 then user logged off before end of mesg received */
		if (nl->mesg_user!=(UR_OBJECT)-1) write_user(nl->mesg_user,inpstr);   
		goto NEXT_LINE;
		}
	/* Same goes for mail except its ENDMAIL or ERROR */
	if (nl->mailfile!=NULL && netcom_num!=17) {
		fputs(inpstr,nl->mailfile);  goto NEXT_LINE;
		}
	nl->lastcom=netcom_num;
	switch(netcom_num) {
		case  0: 
		if (nl->stage==UP) {
			sprintf(text,"~OLSYSTEM:~FY~RS Disconnecting from service %s in the %s.\n",nl->service,nl->connect_room->name);
			write_room(NULL,text);
			}
		shutdown_netlink(nl);  
		break;

		case  1: nl_transfer(nl,w2,w3,lev,inpstr);  break;
		case  2: nl_release(nl,w2);  break;
		case  3: nl_action(nl,w2,inpstr);  break;
		case  4: nl_granted(nl,w2);  break;
		case  5: nl_denied(nl,w2,inpstr);  break;
		case  6: nl_mesg(nl,w2); break;
		case  7: nl->mesg_user=NULL;  break;
		case  8: nl_prompt(nl,w2);  break;
		case  9: nl_verification(nl,w2,w3,0);  break;
		case 10: nl_verification(nl,w2,w3,1);  break;
		case 11: nl_removed(nl,w2);  break;
		case 12: nl_error(nl);  break;
		case 13: nl_checkexist(nl,w2,w3);  break;
		case 14: nl_user_notexist(nl,w2,w3);  break;
		case 15: nl_user_exist(nl,w2,w3);  break;
		case 16: nl_mail(nl,w2,w3);  break;
		case 17: nl_endmail(nl);  break;
		case 18: nl_mailerror(nl,w2,w3);  break;
		case 19: /* Keepalive signal, do nothing */ break;
		case 20: nl_rstat(nl,w2);  break;
		default: 
			sprintf(text,"NETLINK: Received unknown command '%s' from %s.\n",w1,nl->service);
			write_syslog(text,1);
			write_sock(nl->socket,"ERROR\n"); 
		}
	NEXT_LINE:
	/* See if link has closed */
	if (nl->type==UNCONNECTED) return;
	*(c+1)=ctemp;
	inpstr=c+1;
	}
nl->buffer[0]='\0';
}


/*** Deal with user being transfered over from remote site ***/
nl_transfer(nl,name,pass,lev,inpstr)
NL_OBJECT nl;
char *name,*pass,*inpstr;
int lev;
{
UR_OBJECT u,create_user();

/* link for outgoing users only */
if (nl->allow==OUT) {
	sprintf(text,"DENIED %s 4\n",name);
	write_sock(nl->socket,text);
	return;
	}
if (strlen(name)>USER_NAME_LEN) name[USER_NAME_LEN]='\0';

/* See if user is banned */
if (user_banned(name)) {
	if (nl->ver_major==3 && nl->ver_minor>=3 && nl->ver_patch>=3) 
		sprintf(text,"DENIED %s 9\n",name); /* new error for 3.3.3 */
	else sprintf(text,"DENIED %s 6\n",name); /* old error to old versions */
	write_sock(nl->socket,text);
	return;
	}

/* See if user already on here */
if (u=get_user(name)) {
	sprintf(text,"DENIED %s 5\n",name);
	write_sock(nl->socket,text);
	return;
	}

/* See if user of this name exists on this system by trying to load up
   datafile */
if ((u=create_user())==NULL) {		
	sprintf(text,"DENIED %s 6\n",name);
	write_sock(nl->socket,text);
	return;
	}
u->type=REMOTE_TYPE;
strcpy(u->name,name);
if (load_user_details(u)) {
	if (strcmp(u->pass,pass)) {
		/* Incorrect password sent */
		sprintf(text,"DENIED %s 7\n",name);
		write_sock(nl->socket,text);
		destruct_user(u);
		destructed=0;
		return;
		}
	}
else {
	/* Get the users description */
	if (nl->ver_major<=3 && nl->ver_minor<=3 && nl->ver_patch<1) 
		strcpy(text,remove_first(remove_first(remove_first(inpstr))));
	else strcpy(text,remove_first(remove_first(remove_first(remove_first(inpstr)))));
	text[USER_DESC_LEN]='\0';
	terminate(text);
	strcpy(u->desc,text);
	strcpy(u->in_phrase,"enters");
	strcpy(u->out_phrase,"goes");
	if (nl->ver_major==3 && nl->ver_minor>=3 && nl->ver_patch>=1) {
		if (lev>rem_user_maxlevel) u->level=rem_user_maxlevel;
		else u->level=lev; 
		}
	else u->level=rem_user_deflevel;
	}
/* See if users level is below minlogin level */
if (u->level<minlogin_level) {
	if (nl->ver_major==3 && nl->ver_minor>=3 && nl->ver_patch>=3) 
		sprintf(text,"DENIED %s 8\n",u->name); /* new error for 3.3.3 */
	else sprintf(text,"DENIED %s 6\n",u->name); /* old error to old versions */
	write_sock(nl->socket,text);
	destruct_user(u);
	destructed=0;
	return;
	}
strcpy(u->site,nl->service);
sprintf(text,"%s enters from cyberspace.\n",u->name);
write_room(nl->connect_room,text);
sprintf(text,"NETLINK: Remote user %s received from %s.\n",u->name,nl->service);
write_syslog(text,1);
u->room=nl->connect_room;
u->netlink=nl;
u->read_mail=time(0);
u->last_login=time(0);
num_of_users++;
sprintf(text,"GRANTED %s\n",name);
write_sock(nl->socket,text);
}
		

/*** User is leaving this system ***/
nl_release(nl,name)
NL_OBJECT nl;
char *name;
{
UR_OBJECT u;

if ((u=get_user(name))!=NULL && u->type==REMOTE_TYPE) {
	sprintf(text,"%s leaves this plain of existence.\n",u->name);
	write_room_except(u->room,text,u);
	sprintf(text,"NETLINK: Remote user %s released.\n",u->name);
	write_syslog(text,1);
	destroy_user_clones(u);
	destruct_user(u);
	num_of_users--;
	return;
	}
sprintf(text,"NETLINK: Release requested for unknown/invalid user %s from %s.\n",name,nl->service);
write_syslog(text,1);
}


/*** Remote user performs an action on this system ***/
nl_action(nl,name,inpstr)
NL_OBJECT nl;
char *name,*inpstr;
{
UR_OBJECT u;
char *c,ctemp;

if (!(u=get_user(name))) {
	sprintf(text,"DENIED %s 8\n",name);
	write_sock(nl->socket,text);
	return;
	}
if (u->socket!=-1) {
	sprintf(text,"NETLINK: Action requested for local user %s from %s.\n",name,nl->service);
	write_syslog(text,1);
	return;
	}
inpstr=remove_first(remove_first(inpstr));
/* remove newline character */
c=inpstr; ctemp='\0';
while(*c) {
	if (*c=='\n') {  ctemp=*c;  *c='\0';  break;  }
	++c;
	}
u->last_input=time(0);
if (u->misc_op) {
	if (!strcmp(inpstr,"NL")) misc_ops(u,"\n");  
	else misc_ops(u,inpstr+4);
	return;
	}
if (u->afk) {
	write_user(u,"You are no longer AFK.\n");  
	if (u->vis) {
		sprintf(text,"%s comes back from being AFK.\n",u->name);
		write_room_except(u->room,text,u);
		}
	u->afk=0;
	}
word_count=wordfind(inpstr);
if (!strcmp(inpstr,"NL")) return; 
exec_com(u,inpstr);
if (ctemp) *c=ctemp;
if (!u->misc_op) prompt(u);
}


/*** Grant received from remote system ***/
nl_granted(nl,name)
NL_OBJECT nl;
char *name;
{
UR_OBJECT u;
RM_OBJECT old_room;

if (!strcmp(name,"CONNECT")) {
	sprintf(text,"NETLINK: Connection to %s granted.\n",nl->service);
	write_syslog(text,1);
	/* Send our verification and version number */
	sprintf(text,"VERIFICATION %s %s\n",verification,VERSION);
	write_sock(nl->socket,text);
	return;
	}
if (!(u=get_user(name))) {
	sprintf(text,"NETLINK: Grant received for unknown user %s from %s.\n",name,nl->service);
	write_syslog(text,1);
	return;
	}
/* This will probably occur if a user tried to go to the other site , got 
   lagged then changed his mind and went elsewhere. Don't worry about it. */
if (u->remote_com!=GO) {
	sprintf(text,"NETLINK: Unexpected grant for %s received from %s.\n",name,nl->service);
	write_syslog(text,1);
	return;
	}
/* User has been granted permission to move into remote talker */
write_user(u,"~FB~OLYou traverse cyberspace...\n");
if (u->vis) {
	sprintf(text,"%s %s to the %s.\n",u->name,u->out_phrase,nl->service);
	write_room_except(u->room,text,u);
	}
else write_room_except(u->room,invisleave,u);
sprintf(text,"NETLINK: %s transfered to %s.\n",u->name,nl->service);
write_syslog(text,1);
old_room=u->room;
u->room=NULL; /* Means on remote talker */
u->netlink=nl;
u->pot_netlink=NULL;
u->remote_com=-1;
u->misc_op=0;  
u->filepos=0;  
u->page_file[0]='\0';
reset_access(old_room);
sprintf(text,"ACT %s look\n",u->name);
write_sock(nl->socket,text);
}


/*** Deny received from remote system ***/
nl_denied(nl,name,inpstr)
NL_OBJECT nl;
char *name,*inpstr;
{
UR_OBJECT u,create_user();
int errnum;
char *neterr[]={
"this site is not in the remote services valid sites list",
"the remote service is unable to create a link",
"the remote service has no free room links",
"the link is for incoming users only",
"a user with your name is already logged on the remote site",
"the remote service was unable to create a session for you",
"incorrect password. Use '.go <service> <remote password>'",
"your level there is below the remote services current minlogin level",
"you are banned from that service"
};

errnum=0;
sscanf(remove_first(remove_first(inpstr)),"%d",&errnum);
if (!strcmp(name,"CONNECT")) {
	sprintf(text,"NETLINK: Connection to %s denied, %s.\n",nl->service,neterr[errnum-1]);
	write_syslog(text,1);
	/* If wiz initiated connect let them know its failed */
	sprintf(text,"~OLSYSTEM:~RS Connection to %s failed, %s.\n",nl->service,neterr[errnum-1]);
	write_level(com_level[CONN],1,text,NULL);
	close(nl->socket);
	nl->type=UNCONNECTED;
	nl->stage=DOWN;
	return;
	}
/* Is for a user */
if (!(u=get_user(name))) {
	sprintf(text,"NETLINK: Deny for unknown user %s received from %s.\n",name,nl->service);
	write_syslog(text,1);
	return;
	}
sprintf(text,"NETLINK: Deny %d for user %s received from %s.\n",errnum,name,nl->service);
write_syslog(text,1);
sprintf(text,"Sorry, %s.\n",neterr[errnum-1]);
write_user(u,text);
prompt(u);
u->remote_com=-1;
u->pot_netlink=NULL;
}


/*** Text received to display to a user on here ***/
nl_mesg(nl,name)
NL_OBJECT nl;
char *name;
{
UR_OBJECT u;

if (!(u=get_user(name))) {
	sprintf(text,"NETLINK: Message received for unknown user %s from %s.\n",name,nl->service);
	write_syslog(text,1);
	nl->mesg_user=(UR_OBJECT)-1;
	return;
	}
nl->mesg_user=u;
}


/*** Remote system asking for prompt to be displayed ***/
nl_prompt(nl,name)
NL_OBJECT nl;
char *name;
{
UR_OBJECT u;

if (!(u=get_user(name))) {
	sprintf(text,"NETLINK: Prompt received for unknown user %s from %s.\n",name,nl->service);
	write_syslog(text,1);
	return;
	}
if (u->type==REMOTE_TYPE) {
	sprintf(text,"NETLINK: Prompt received for remote user %s from %s.\n",name,nl->service);
	write_syslog(text,1);
	return;
	}
prompt(u);
}


/*** Verification received from remote site ***/
nl_verification(nl,w2,w3,com)
NL_OBJECT nl;
char *w2,*w3;
int com;
{
NL_OBJECT nl2;

if (!com) {
	/* We're verifiying a remote site */
	if (!w2[0]) {
		shutdown_netlink(nl);  return;
		}
	for(nl2=nl_first;nl2!=NULL;nl2=nl2->next) {
		if (!strcmp(nl->site,nl2->site) && !strcmp(w2,nl2->verification)) {
			switch(nl->allow) {
				case IN : write_sock(nl->socket,"VERIFY OK IN\n");  break;
				case OUT: write_sock(nl->socket,"VERIFY OK OUT\n");  break;
				case ALL: write_sock(nl->socket,"VERIFY OK ALL\n"); 
				}
			strcpy(nl->service,nl2->service);

			/* Only 3.2.0 and above send version number with verification */
			sscanf(w3,"%d.%d.%d",&nl->ver_major,&nl->ver_minor,&nl->ver_patch);
			sprintf(text,"NETLINK: Connected to %s in the %s.\n",nl->service,nl->connect_room->name);
			write_syslog(text,1);
			sprintf(text,"~OLSYSTEM:~RS New connection to service %s in the %s.\n",nl->service,nl->connect_room->name);
			write_room(NULL,text);
			return;
			}
		}
	write_sock(nl->socket,"VERIFY BAD\n");
	shutdown_netlink(nl);
	return;
	}
/* The remote site has verified us */
if (!strcmp(w2,"OK")) {
	/* Set link permissions */
	if (!strcmp(w3,"OUT")) {
		if (nl->allow==OUT) {
			sprintf(text,"NETLINK: WARNING - Permissions deadlock, both sides are outgoing only.\n");
			write_syslog(text,1);
			}
		else nl->allow=IN;
		}
	else {
		if (!strcmp(w3,"IN")) {
			if (nl->allow==IN) {
				sprintf(text,"NETLINK: WARNING - Permissions deadlock, both sides are incoming only.\n");
				write_syslog(text,1);
				}
			else nl->allow=OUT;
			}
		}
	sprintf(text,"NETLINK: Connection to %s verified.\n",nl->service);
	write_syslog(text,1);
	sprintf(text,"~OLSYSTEM:~RS New connection to service %s in the %s.\n",nl->service,nl->connect_room);
	write_room(NULL,text);
	return;
	}
if (!strcmp(w2,"BAD")) {
	sprintf(text,"NETLINK: Connection to %s has bad verification.\n",nl->service);
	write_syslog(text,1);
	/* Let wizes know its failed , may be wiz initiated */
	sprintf(text,"~OLSYSTEM:~RS Connection to %s failed, bad verification.\n",nl->service);
	write_level(com_level[CONN],1,text,NULL);
	shutdown_netlink(nl);  
	return;
	}
sprintf(text,"NETLINK: Unknown verify return code from %s.\n",nl->service);
write_syslog(text,1);
shutdown_netlink(nl);
}


/* Remote site only sends REMVD (removed) notification if user on remote site 
   tries to .go back to his home site or user is booted off. Home site doesn't
   bother sending reply since remote site will remove user no matter what. */
nl_removed(nl,name)
NL_OBJECT nl;
char *name;
{
UR_OBJECT u;

if (!(u=get_user(name))) {
	sprintf(text,"NETLINK: Removed notification for unknown user %s received from %s.\n",name,nl->service);
	write_syslog(text,1);
	return;
	}
if (u->room!=NULL) {
	sprintf(text,"NETLINK: Removed notification of local user %s received from %s.\n",name,nl->service);
	write_syslog(text,1);
	return;
	}
sprintf(text,"NETLINK: %s returned from %s.\n",u->name,u->netlink->service);
write_syslog(text,1);
u->room=u->netlink->connect_room;
u->netlink=NULL;
if (u->vis) {
	sprintf(text,"%s %s\n",u->name,u->in_phrase);
	write_room_except(u->room,text,u);
	}
else write_room_except(u->room,invisenter,u);
look(u);
prompt(u);
}


/*** Got an error back from site, deal with it ***/
nl_error(nl)
NL_OBJECT nl;
{
if (nl->mesg_user!=NULL) nl->mesg_user=NULL;
/* lastcom value may be misleading, the talker may have sent off a whole load
   of commands before it gets a response due to lag, any one of them could
   have caused the error */
sprintf(text,"NETLINK: Received ERROR from %s, lastcom = %d.\n",nl->service,nl->lastcom);
write_syslog(text,1);
}


/*** Does user exist? This is a question sent by a remote mailer to
     verifiy mail id's. ***/
nl_checkexist(nl,to,from)
NL_OBJECT nl;
char *to,*from;
{
FILE *fp;
char filename[80];

sprintf(filename,"%s/%s.D",USERFILES,to);
if (!(fp=fopen(filename,"r"))) {
	sprintf(text,"EXISTS_NO %s %s\n",to,from);
	write_sock(nl->socket,text);
	return;
	}
fclose(fp);
sprintf(text,"EXISTS_YES %s %s\n",to,from);
write_sock(nl->socket,text);
}


/*** Remote user doesnt exist ***/
nl_user_notexist(nl,to,from)
NL_OBJECT nl;
char *to,*from;
{
UR_OBJECT user;
char filename[80];
char text2[ARR_SIZE];

if ((user=get_user(from))!=NULL) {
	sprintf(text,"~OLSYSTEM:~RS User %s does not exist at %s, your mail bounced.\n",to,nl->service);
	write_user(user,text);
	}
else {
	sprintf(text2,"There is no user named %s at %s, your mail bounced.\n",to,nl->service);
	send_mail(NULL,from,text2);
	}
sprintf(filename,"%s/OUT_%s_%s@%s",MAILSPOOL,from,to,nl->service);
unlink(filename);
}


/*** Remote users exists, send him some mail ***/
nl_user_exist(nl,to,from)
NL_OBJECT nl;
char *to,*from;
{
UR_OBJECT user;
FILE *fp;
char text2[ARR_SIZE],filename[80],line[82];

sprintf(filename,"%s/OUT_%s_%s@%s",MAILSPOOL,from,to,nl->service);
if (!(fp=fopen(filename,"r"))) {
	if ((user=get_user(from))!=NULL) {
		sprintf(text,"~OLSYSTEM:~RS An error occured during mail delivery to %s@%s.\n",to,nl->service);
		write_user(user,text);
		}
	else {
		sprintf(text2,"An error occured during mail delivery to %s@%s.\n",to,nl->service);
		send_mail(NULL,from,text2);
		}
	return;
	}
sprintf(text,"MAIL %s %s\n",to,from);
write_sock(nl->socket,text);
fgets(line,80,fp);
while(!feof(fp)) {
	write_sock(nl->socket,line);
	fgets(line,80,fp);
	}
fclose(fp);
write_sock(nl->socket,"\nENDMAIL\n");
unlink(filename);
}


/*** Got some mail coming in ***/
nl_mail(nl,to,from)
NL_OBJECT nl;
char *to,*from;
{
char filename[80];

sprintf(text,"NETLINK: Mail received for %s from %s.\n",to,nl->service);
write_syslog(text,1);
sprintf(filename,"%s/IN_%s_%s@%s",MAILSPOOL,to,from,nl->service);
if (!(nl->mailfile=fopen(filename,"w"))) {
	sprintf(text,"ERROR: Couldn't open file %s to write in nl_mail().\n",filename);
	write_syslog(text,0);
	sprintf(text,"MAILERROR %s %s\n",to,from);
	write_sock(nl->socket,text);
	return;
	}
strcpy(nl->mail_to,to);
strcpy(nl->mail_from,from);
}


/*** End of mail message being sent from remote site ***/
nl_endmail(nl)
NL_OBJECT nl;
{
FILE *infp,*outfp;
char c,infile[80],mailfile[80],line[DNL+1];

fclose(nl->mailfile);
nl->mailfile=NULL;

sprintf(mailfile,"%s/IN_%s_%s@%s",MAILSPOOL,nl->mail_to,nl->mail_from,nl->service);

/* Copy to users mail file to a tempfile */
if (!(outfp=fopen("tempfile","w"))) {
	write_syslog("ERROR: Couldn't open tempfile in netlink_endmail().\n",0);
	sprintf(text,"MAILERROR %s %s\n",nl->mail_to,nl->mail_from);
	write_sock(nl->socket,text);
	goto END;
	}
fprintf(outfp,"%d\r",(int)time(0));

/* Copy old mail file to tempfile */
sprintf(infile,"%s/%s.M",USERFILES,nl->mail_to);
if (!(infp=fopen(infile,"r"))) goto SKIP;
fgets(line,DNL,infp);
c=getc(infp);
while(!feof(infp)) {  putc(c,outfp);  c=getc(infp);  }
fclose(infp);

/* Copy received file */
SKIP:
if (!(infp=fopen(mailfile,"r"))) {
	sprintf(text,"ERROR: Couldn't open file %s to read in netlink_endmail().\n",mailfile);
	write_syslog(text,0);
	sprintf(text,"MAILERROR %s %s\n",nl->mail_to,nl->mail_from);
	write_sock(nl->socket,text);
	goto END;
	}
fprintf(outfp,"~OLFrom: %s@%s  %s",nl->mail_from,nl->service,long_date(0));
c=getc(infp);
while(!feof(infp)) {  putc(c,outfp);  c=getc(infp);  }
fclose(infp);
fclose(outfp);
rename("tempfile",infile);
write_user(get_user(nl->mail_to),"\07~FT~OL~LI** YOU HAVE NEW MAIL **\n");

END:
nl->mail_to[0]='\0';
nl->mail_from[0]='\0';
unlink(mailfile);
}


/*** An error occured at remote site ***/
nl_mailerror(nl,to,from)
NL_OBJECT nl;
char *to,*from;
{
UR_OBJECT user;

if ((user=get_user(from))!=NULL) {
	sprintf(text,"~OLSYSTEM:~RS An error occured during mail delivery to %s@%s.\n",to,nl->service);
	write_user(user,text);
	}
else {
	sprintf(text,"An error occured during mail delivery to %s@%s.\n",to,nl->service);
	send_mail(NULL,from,text);
	}
}


/*** Send statistics of this server to requesting user on remote site ***/
nl_rstat(nl,to)
NL_OBJECT nl;
char *to;
{
char str[80];

gethostname(str,80);
if (nl->ver_major<=3 && nl->ver_minor<2)
	sprintf(text,"MSG %s\n\n*** Remote statistics ***\n\n",to);
else sprintf(text,"MSG %s\n\n~BM*** Remote statistics ***\n\n",to);
write_sock(nl->socket,text);
sprintf(text,"NUTS version         : %s\nHost                 : %s\n",VERSION,str);
write_sock(nl->socket,text);
sprintf(text,"Ports (Main/Wiz/Link): %d ,%d, %d\n",port[0],port[1],port[2]);
write_sock(nl->socket,text);
sprintf(text,"Number of users      : %d\nRemote user maxlevel : %s\n",num_of_users,level_name[rem_user_maxlevel]);
write_sock(nl->socket,text);
sprintf(text,"Remote user deflevel : %s\n\nEMSG\nPRM %s\n",level_name[rem_user_deflevel],to);
write_sock(nl->socket,text);
}


/*** Shutdown the netlink and pull any remote users back home ***/
shutdown_netlink(nl)
NL_OBJECT nl;
{
UR_OBJECT u;
char mailfile[80];

if (nl->type==UNCONNECTED) return;

/* See if any mail halfway through being sent */
if (nl->mail_to[0]) {
	sprintf(text,"MAILERROR %s %s\n",nl->mail_to,nl->mail_from);
	write_sock(nl->socket,text);
	fclose(nl->mailfile);
	sprintf(mailfile,"%s/IN_%s_%s@%s",MAILSPOOL,nl->mail_to,nl->mail_from,nl->service);
	unlink(mailfile);
	nl->mail_to[0]='\0';
	nl->mail_from[0]='\0';
	}
write_sock(nl->socket,"DISCONNECT\n");
close(nl->socket);  
for(u=user_first;u!=NULL;u=u->next) {
	if (u->pot_netlink==nl) {  u->remote_com=-1;  continue;  }
	if (u->netlink==nl) {
		if (u->room==NULL) {
			write_user(u,"~FB~OLYou feel yourself dragged back across the ether...\n");
			u->room=u->netlink->connect_room;
			u->netlink=NULL;
			if (u->vis) {
				sprintf(text,"%s %s\n",u->name,u->in_phrase);
				write_room_except(u->room,text,u);
				}
			else write_room_except(u->room,invisenter,u);
			look(u);  prompt(u);
			sprintf(text,"NETLINK: %s recovered from %s.\n",u->name,nl->service);
			write_syslog(text,1);
			continue;
			}
		if (u->type==REMOTE_TYPE) {
			sprintf(text,"%s vanishes!\n",u->name);
			write_room(u->room,text);
			destruct_user(u);
			num_of_users--;
			}
		}
	}
if (nl->stage==UP) 
	sprintf(text,"NETLINK: Disconnected from %s.\n",nl->service);
else sprintf(text,"NETLINK: Disconnected from site %s.\n",nl->site);
write_syslog(text,1);
if (nl->type==INCOMING) {
	nl->connect_room->netlink=NULL;
	destruct_netlink(nl);  
	return;
	}
nl->type=UNCONNECTED;
nl->stage=DOWN;
nl->warned=0;
}



/*************** START OF COMMAND FUNCTIONS AND THEIR SUBSIDS **************/

/*** Deal with user input ***/
exec_com(user,inpstr)
UR_OBJECT user;
char *inpstr;
{
int i,len;
char filename[80],*comword=NULL;

com_num=-1;
if (word[0][0]=='.') comword=(word[0]+1);
else comword=word[0];
if (!comword[0]) {
     write_user(user,"% What is it you are trying to do? %\n");  return;
	}
     if (user->gaged) {
          write_user(user,"~OL~FW>>>>~RS Silly POTATO HEAD!\n");
          return;
          }
/* get com_num */
if (!strcmp(word[0],">")) strcpy(word[0],"tell");
if (!strcmp(word[0],"<")) strcpy(word[0],"pemote");
if (!strcmp(word[0],"[")) strcpy(word[0],"csay");
if (!strcmp(word[0],"]")) strcpy(word[0],"cemote");
if (!strcmp(word[0],"/")) strcpy(word[0],"pemote");
if (!strcmp(word[0],"-")) strcpy(word[0],"echo"); 
if (!strcmp(word[0],"'")) strcpy(word[0],"to");
if (!strcmp(word[0],"#")) strcpy(word[0],"shout");
/* if (inpstr[0]==';' || inpstr[0]==':') strcpy(word[0],"emote"); */
if (inpstr[0]==';') strcpy(word[0],"emote");
else if (inpstr[0]=='!') strcpy(word[0],"semote");
	else inpstr=remove_first(inpstr);

i=0;
len=strlen(comword);
while(command[i][0]!='*') {
	if (!strncmp(command[i],comword,len)) {  com_num=i;  break;  }
	++i;
	}
if (user->room!=NULL && (com_num==-1 || com_level[com_num] > user->level)) {
     write_user(user,"% What is it you are trying to do? %\n");  return;
	}
/* See if user has gone across a netlink and if so then intercept certain 
   commands to be run on home site */
if (user->room==NULL) {
	switch(com_num) {
		case HOME: 
		case QUIT:
		case MODE:
		case PROMPT: 
		case COLOR:
/*          case REBOOT:     */
/*          case SUICIDE:    */
/*          case SHUTDOWN:   */
		case CHARECHO:
          write_user(user,"~FY~OL*** Home execution ***\n");  break;

		default:
		sprintf(text,"ACT %s %s %s\n",user->name,word[0],inpstr);
		write_sock(user->netlink->socket,text);
		no_prompt=1;
		return;
		}
	}
/* Dont want certain commands executed by remote users */
if (user->type==REMOTE_TYPE) {
	switch(com_num) {
		case PASSWD :
		case ENTPRO :
		case ACCREQ :
		case CONN   :
		case DISCONN:
			write_user(user,"Sorry, remote users cannot use that command.\n");
			return;
		}
	}

/* Main switch */
switch(com_num) {

     case QUIT:
		if (user->muzzled & JAILED) {
               write_user(user,"% What is it you are trying to do? %\n");  
			return;
			}
		disconnect_user(user);  break;
     case LOOK     : look(user);  break;
     case MODE     : toggle_mode(user);  break;
     case SAY      : 
		if (word_count<2) {
               write_user(user,"What is it you want to say?\n");  return;
			}
		say(user,inpstr);
		break;
     case SHOUT    : shout(user,inpstr);  break;
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
	case OUTPHRASE: 
		set_iophrase(user,inpstr);  break; 
     case PUBCOM   :
     case PRIVCOM  : set_room_access(user);  break;
     case KNOCK    : letmein(user);  break;
     case INVITE   : invite(user);   break;
     case TO       : to(user,inpstr);  break;
     case TOPIC    : set_topic(user,inpstr);  break;
     case MOVE     : move(user);  break;
     case BCAST    : bcast(user,inpstr);  break;
     case WHO      : who(user,0);  break;
     case PEOPLE   : who(user,1);  break;
     case HELP     : help(user);  break;
     case SHUTDOWN : shutdown_com(user);  break;
	case NEWS:
		if (user->muzzled & JAILED) {
               write_user(user,"% What is it you are trying to do? %\n");  
			return;
			}
		sprintf(filename,"%s/%s",DATAFILES,NEWSFILE);
		switch(more(user,user->socket,filename)) {
               case 0: write_user(user,"You pick up the newspaper, and to your surprise, Its Empty!!\n");  break;
			case 1: user->misc_op=2;
			}
		break;
	case RULES:
		sprintf(filename,"%s/%s",DATAFILES,RULESFILE);
		switch(more(user,user->socket,filename)) {
               case 0: write_user(user,"% The rules could not be found, try again later! %\n");  break;
			case 1: user->misc_op=2;
			}
		break;
     case READ     : read_board(user);  break;
     case WRITE    : write_board(user,inpstr,0);  break;
     case WIPE     : wipe_board(user);  break;
     case SEARCH   : search_boards(user);  break;
     case REVIEW   : review(user);  break;
     case HOME     : home(user);  break;
     case STATUS   : status(user);  break;
     case VER      :
          sprintf(text,"NUTS version %s\nCurrently serving ~OL~FY%d~RS users.\n",VERSION,user_count);
		write_user(user,text);  break;
     case RMAIL    : rmail(user);  break;
     case SMAIL    : smail(user,inpstr,0);  break;
     case DMAIL    : dmail(user);  break;
     case FROM     : mail_from(user);  break;
     case ENTPRO   : enter_profile(user,0);  break;
     case EXAMINE  : examine(user);  break;
     case RMST     : rooms(user,1);  break;
     case RMSN     : rooms(user,0);  break;
     case NETSTAT  : netstat(user);  break;
     case NETDATA  : netdata(user);  break;
     case CONN     : connect_netlink(user);  break;
     case DISCONN  : disconnect_netlink(user);  break;
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
     case SRULES   :
		if (user->muzzled & JAILED) {
               write_user(user,"% What is it you are trying to do? %\n");  
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
     case RSTAT    : remote_stat(user);  break;
     case SWBAN    : swban(user);  break;
     case AFK      : afk(user,inpstr);  break;
     case CLS      : cls(user);  break;
     case COLOR    : toggle_colour(user);  break;
     case LISTEN   : listening(user);  break;
     case IGNORE   : ignoreing(user);  break;
     case SUICIDE  : suicide(user);  break;
     case NUKE     : delete_user(user,0);  break;
     case REBOOT   : reboot_com(user);  break;
     case RECOUNT  : check_messages(user,2);  break;
     case REVTELL  : revtell(user);  break;
     case THINK    : think(user,inpstr);  break;
     case WEMOTE   : wemote(user,inpstr);  break;
     case IMNEW    :
		if (user->muzzled & JAILED) {
               write_user(user,"% What is it you are trying to do? %\n");  
			return;
			}
		sprintf(filename,"%s/%s",DATAFILES,NEWBIEFILE);
		switch(more(user,user->socket,filename)) {
               case 0: write_user(user,"% The New User File Is Not Currently Availble, Sorry. %\n");  break;
			case 1: user->misc_op=2;
			}
		break;
	case STAFF  :
		if (user->muzzled & JAILED) {
               write_user(user,"% What is it you are trying to do? %\n");  
			return;
			}
		sprintf(filename,"%s/%s",DATAFILES,STAFFFILE);
		switch(more(user,user->socket,filename)) {
               case 0: write_user(user,"% The Staff List Is Not Currently Available, Sorry! %\n");  break;
			case 1: user->misc_op=2;
			}
		break;
	case MANAGEMENT   : staff_on(user);  break;
	case ENTROOM      : room_desc(user,0);  break;
     case BOOT         : remove_user(user);  break;
	case ARREST       : arrest(user);  break;
	case UNARREST     : unarrest(user);  break;
	case GENDER       : gender(user);  break;
	case RECORD       : view_record(user);  break;
     case CHANGE       : changesys(user);  break;
     case REVWIZ       : revwiztell(user);  break;
     case RELOAD       : reloadroom(user);  break;
	case BEEP         : beep(user,inpstr);  break;
	case LOBBY        : lobby(user);  break;
	case JOIN         : join(user);  break;
     case GAG         : gag(user);  break;
	case FREEZE       : freeze(user);  break;
	case UNFREEZE     : unfreeze(user);  break;
     case SING         : sing(user,inpstr);  break;
     case NAKED        : naked(user,inpstr); break;
     case SSING        : ssing(user,inpstr); break;
     case DOH          : doh(user); break;
     case QECHO        : qecho(user,inpstr); break;
     case HUG          : hug(user,inpstr); break;
     case FORCE        : force(user,inpstr); break;
     case KISS         : kiss(user,inpstr); break;
     case FRENCH       : frenchkiss(user,inpstr); break;
     case NETSEX       : netsex(user); break;
     case NETSEXTWO    : netsextwo(user); break;
     case YELL         : yell(user,inpstr); break;
     case LOGOFF       : logoff_user(user); break;
     case CLSALL       : cls_all(user); break;
     case WELCOME      : welcome_user(user,inpstr); break;
     case TIME         : showtime(user); break;
     case RANKS        : show_ranks(user); break;
     case GREET        : greet(user,inpstr);  break;
     case SAMESITE     : samesite(user,0);  break;
     case WRAP         : linewrap(user); break;
     case UNINVITE     : uninvite(user); break; 
     case STREAK       : streak(user); break;
     case LICK         : lick(user); break;
     case BOP          : bop(user); break;     
     case SET          : setinfo(user,inpstr); break;
     case PADDLE       : paddle(user); break;     
	case TALKERS:
		sprintf(filename,"%s/%s",DATAFILES,TALKERFILE);
		switch(more(user,user->socket,filename)) {
		case 0: write_user(user,"There are no talkers to list.\n"); break;
		case 1: user->misc_op=2;
		}
		break;
	case MUDS:
		sprintf(filename,"%s/%s",DATAFILES,MUDFILE);
		switch(more(user,user->socket,filename)) {
		case 0: write_user(user,"There are no muds to list.\n"); break;
		case 1: user->misc_op=2;
		}
		break;
	case EWTOO:
		sprintf(filename,"%s/%s",DATAFILES,EWTOOFILE);
		switch(more(user,user->socket,filename)) {
		case 0: write_user(user,"There are no ewtoo sites to list.\n"); break;
		case 1: user->misc_op=2;
		}
		break;
	case NUTS:
		sprintf(filename,"%s/%s",DATAFILES,NUTSFILE);
		switch(more(user,user->socket,filename)) {
		case 0: write_user(user,"There are no NUTS talkers to list.\n"); break;
		case 1: user->misc_op=2;
		}
		break;
	case PICLIST	: piclist(user); break;
	case PICTELL	: pictell(user,inpstr); break;
	case ROOMPIC	: roompic(user,inpstr); break;
	case VIEWPIC	: viewpic(user,inpstr); break;
	case WREVCLR	: wizrevclr(user); break;
	case SREVCLR	: shoutrevclr(user); break;
	case REVSHOUT   : revshout(user); break;
	case MYROOM     : goto_myroom(user); break;
	case EDIT	: if (user->room->access==FIXED_PERSONAL) {
				room_desc(user,0); 
			  }
			else write_user(user,"~OL~FRThis is not a personal room... You cannot edit the room description here!\n");
			break;
	case RMKILL	: rmkill(user); break;
	case RMBAN	: rmban(user); break;
	case RMUNBAN	: rmunban(user); break;
	case SETPOSS	: setposs(user,inpstr); break;
	case SHACKLE    : shackle(user); break;
	case UNSHACKLE  : unshackle(user); break;
	case MAKEINVIS  : makeinvis(user); break;
	case MAKEVIS    : makevis(user); break;
	case HIDE       : toggle_hidden(user); break;	
	default         : write_user(user,"~OLERROR:  ~RSCommand not executed in ~OLexec_com()\n");
	}	
}

/*************************************************************************/
/******************************* Speech FUNTIONS *************************/
/*************************************************************************/

/*** Say user speech. ***/
say(user,inpstr)
UR_OBJECT user;
char *inpstr;
{
RM_OBJECT rm;
char type[10],*name;
char *clr[]={"~FR","~FG","~FY","~FB","~FY"};

if (user->room==NULL) {
	sprintf(text,"ACT %s say %s\n",user->name,inpstr);
	write_sock(user->netlink->socket,text);
	no_prompt=1;
	return;
	}
if (word_count<2 && user->command_mode) {
     write_user(user,"Usage: .say <message>\n");  return;
	}
if (user->gaged) {
     write_user(user,"~OL~FW>>>>~RS Silly POTATO HEAD\n");  
	return;
	}
switch(inpstr[strlen(inpstr)-1]) {
     case '?': strcpy(type,"ask");      break;
     case '!': strcpy(type,"exclaim");  break;
/*   case '.': strcpy(type,"mumble");   break;   */
     case ')': strcpy(type,"smile");    break; 
     case '(': strcpy(type,"frown");    break; 
     default : strcpy(type,"say"); 
     }

/**** Clone Of Prompt *****/

if (user->type==CLONE_TYPE) {
     sprintf(text,"%s%s %ss~OL: ~RS%s\n",clr[rand()%5],user->name,type,inpstr);
	write_room(user->room,text);
	record(user->room,text);
	return;
	}
rm=user->room;
if (rm->access==FIXED_PUBLIC && ban_swearing && contains_swearing(inpstr)) {
	write_user(user,noswearing);  return;
	}
if (user->muzzled & FROZEN) {
     sprintf(text,"%sYou %s~OL:~RS %s\n",clr[rand()%5],type,inpstr);
	write_user(user,text);
	return;
	}
sprintf(text,"%sYou %s~OL:~RS %s\n",clr[rand()%5],type,inpstr);
write_user(user,text);
if (user->vis) name=user->name; else name=invisname;
sprintf(text,"%s%s %ss~OL:~RS %s\n",clr[rand()%5],name,type,inpstr);
write_room_except(user->room,text,user);
record(user->room,text);
}

/*** Shout something ***/
shout(user,inpstr)
UR_OBJECT user;
char *inpstr;
{
char *name;

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
if (user->muzzled & 1) {
	write_user(user,"You are muzzled, you cannot shout.\n");  
	return;
	}
if (word_count<2) {
     write_user(user,"Usage:  .shout <message>\n");  return;
	}
if (ban_swearing && contains_swearing(inpstr)) {
	write_user(user,noswearing);  return;
	}
if (user->muzzled & FROZEN) {
     sprintf(text,"~OL~FGYou shout:~FT %s\n",inpstr);
	write_user(user,text);
	return;
	}
sprintf(text,"~OL~FGYou shout:~FT %s\n",inpstr);
write_user(user,text);
if (user->vis) name=user->name; else name=invisname;
sprintf(text,"~OL~FG%s shouts:~FT %s\n",name,inpstr);
write_room_except(NULL,text,user);
record_shout(user,text);
}


/*** Welcome A User ***/
welcome_user(user,inpstr)
UR_OBJECT user;
char *inpstr;
{
char *name;

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
if (user->muzzled & 1) {
     write_user(user,"You are muzzled, you cannot welcome anyone.\n");  
	return;
	}
if (word_count<2) {
     write_user(user,"Usage:  .welcome <message>\n");  return;
	}
if (ban_swearing && contains_swearing(inpstr)) {
	write_user(user,noswearing);  return;
	}
if (user->muzzled & FROZEN) {
	write_user(user,"\n");
        sprintf(text,"  ~OL~FY--==<~FW %s ~RS~OL~FY>==--\n",inpstr);
	write_user(user,text);
	write_user(user,"\n");
	return;
	}
write_user(user,"\n");
sprintf(text,"  ~OL~FY--==<~FW %s ~RS~OL~FY>==--\n",inpstr);
write_room(NULL,text);
record_shout(user,text);
write_user(user,"\n");
}


/*** Tell another user something ***/
sendtell(user,inpstr)
UR_OBJECT user;
char *inpstr;
{
UR_OBJECT u;
RM_OBJECT rm;
RM_OBJECT urm;
char type[5],*name;

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
if (user->muzzled & 1) {
	write_user(user,"You are muzzled, you cannot tell anyone anything.\n");  
	return;
	}
if (word_count<3) {
     write_user(user,"Usage:  .tell <user> <message>\n");  return;
	}
if (!(u=get_user(word[1]))) {
	write_user(user,notloggedon);  return;
	}
if (u==user) {
	write_user(user,"Talking to yourself is the first sign of madness.\n");
	return;
	}
if ((u->hidden && (!(u->vis))) && user->level<OWNER) write_user(user,notloggedon); return;
if (u->afk) {
     if (u->afk_mesg[0]) sprintf(text,"~OL~FR%s ~FGis ~FYAFK: %s\n",u->name,u->afk_mesg);
	else sprintf(text,"%s is AFK at the moment.\n",u->name);
	write_user(user,text);
	if (user->muzzled & FROZEN) {
		sprintf(text,"~OL~FTMESSAGE RECORDED! ~RS%s will get a mesage to do a .revtell.\n",u->name);
		write_user(user,text);
		return;
		}
	sprintf(text,"~OL~FTMESSAGE RECORDED! ~RS%s will get a mesage to do a .revtell.\n",u->name);
	write_user(user,text);
	inpstr=remove_first(inpstr);
	if (inpstr[strlen(inpstr)-1]=='?') strcpy(type,"ask");
	else strcpy(type,"tell");
	if (user->vis) sprintf(text,"~OL-> %s %ss you:~RS %s\n",name,type,inpstr);
        else sprintf(text,"~FR(invis)~FW~OL-> %s %ss you:~RS %s\n",name,type,inpstr);
	record_tell(u,text);
     write_user(u,text);
	u->chkrev=1;
	return;
	}
if (u->malloc_start!=NULL) {
	sprintf(text,"%s is using the editor at the moment.\n",u->name);
	write_user(user,text);
	if (user->muzzled & FROZEN) {
		sprintf(text,"~OL~FTMESSAGE RECORDED! ~RS%s will get a mesage to do a .revtell.\n",u->name);
		write_user(user,text);
		return;
		}
	sprintf(text,"~OL~FTMESSAGE RECORDED! ~RS%s will get a mesage to do a .revtell.\n",u->name);
	write_user(user,text);
	inpstr=remove_first(inpstr);
	if (inpstr[strlen(inpstr)-1]=='?') strcpy(type,"ask");
	else strcpy(type,"tell");
	if (user->vis) sprintf(text,"~OL-> %s %ss you:~RS %s\n",name,type,inpstr);
        else sprintf(text,"~FR(invis)~FW~OL-> %s %ss you:~RS %s\n",name,type,inpstr);
	record_tell(u,text);
	u->chkrev=1;    
	return;
	}
if (u->ignall && user->level<OWNER) {
     sprintf(text,"~OL~FT%s is ignoring everyone at the moment.\n",u->name);
	write_user(user,text);  
	return;
	}
if ((u->ignore & ALL_TELLS) && user->level<OWNER) {
  if (u->vis) {
     sprintf(text,"~OL~FT%s is ignoring private tells at the moment.\n",u->name);
	write_user(user,text);
	}
  else write_user(user,notloggedon);
	return;
	}
if (!strcmp(user->name,u->ignuser) && user->level<OWNER) {
  if (u->vis) {
     sprintf(text,"~OL~FT%s is ignoring private tells at the moment.\n",u->name);
	write_user(user,text);	
	}
  else write_user(user,notloggedon);
	return;
	}
rm=user->room;
urm=u->room;
if ((rm!=urm) && (u->ignore & OUT_ROOM_TELLS) && user->level<OWNER) {
  if (u->vis) {
	sprintf(text,"~OL~FT%s is ignoring private tells at the moment.\n",u->name);
	write_user(user,text);	
	}
  else write_user(user,notloggedon);
return;
	}
if (u->room==NULL) {
     sprintf(text,"~OL~FT%s is offsite and would not be able to reply to you.\n",u->name);
	write_user(user,text);
	return;
	}
if (user->muzzled & FROZEN) {
	inpstr=remove_first(inpstr);
	if (inpstr[strlen(inpstr)-1]=='?') strcpy(type,"ask");
	else strcpy(type,"tell");
        sprintf(text,"~OL-> You %s %s:~RS %s\n",type,u->name,inpstr);
	write_user(user,text);	
	return;
	}
inpstr=remove_first(inpstr);
if (inpstr[strlen(inpstr)-1]=='?') strcpy(type,"ask");
else strcpy(type,"tell");
sprintf(text,"~OL-> You %s %s:~RS %s\n",type,u->name,inpstr);
write_user(user,text);
if (user->vis) sprintf(text,"~OL-> %s %ss you:~RS %s\n",user->name,type,inpstr);
else sprintf(text,"~FR(invis)~FW~OL-> %s %ss you:~RS %s\n",user->name,type,inpstr);
write_user(u,text);
record_tell(u,text);
}


/*** Emote something ***/
emote(user,inpstr)
UR_OBJECT user;
char *inpstr;
{
RM_OBJECT rm;
char *name;

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");
	return;
	}
if (user->muzzled & 1) {
	write_user(user,"You are muzzled, you cannot emote.\n");  return;
	}
if (word_count<2 && inpstr[1]<33) {
     write_user(user,"Usage: .emote <emotion>\n");  return;
	}
/** CLONE OF **/
if (user->type==CLONE_TYPE) {
     sprintf(text,"~FG%s ~FM%s\n",user->name,inpstr);
	write_room(user->room,text);
	record(user->room,text);
	return;
	}
rm=user->room;
if (rm->access==FIXED_PUBLIC && ban_swearing && contains_swearing(inpstr)) {
	write_user(user,noswearing);  return;
	}
if (user->muzzled & FROZEN) {
	if (user->vis) name=user->name; else name=invisname;
     if (inpstr[0]==';') sprintf(text,"%s %s\n",name,inpstr+1);
     else sprintf(text,"~FG%s ~FM%s\n",name,inpstr);    write_user(user,text);
	write_user(user,text); 
	return;
	}
if (user->vis) name=user->name; else name=invisname;
if (inpstr[0]==';') sprintf(text,"~FG%s ~FM%s\n",name,inpstr+1);
else sprintf(text,"~FG%s ~FM%s  ~RS\n",name,inpstr);
write_room(user->room,text);
record(user->room,text);
}


/*** Do a shout emote ***/
semote(user,inpstr)
UR_OBJECT user;
char *inpstr;
{
char *name;

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
if (user->muzzled & 1) {
	write_user(user,"You are muzzled, you cannot emote.\n");  return;
	}
if (word_count<2 && inpstr[1]<33) {
     write_user(user,"Usage: .semote <emotion>\n");  return;
	}
if (user->muzzled & FROZEN) {
	if (user->vis) name=user->name; else name=invisname;
	if (inpstr[0]=='!') sprintf(text,"~OL~FT!! ~FY%s ~FG%s\n",name,inpstr+1);
     else sprintf(text,"~OL~FT!! ~FY%s ~FG%s\n",name,inpstr); 
	write_user(user,text); 
	return;
	}
if (user->vis) name=user->name; else name=invisname;
if (inpstr[0]=='!') sprintf(text,"~OL~FT!! ~FY%s ~FG%s  ~RS\n",name,inpstr+1);
else sprintf(text,"~OL~FT!! ~FY%s ~FG%s\n",name,inpstr);
write_room(NULL,text);
record_shout(user,text);
}


/*** Do a private emote ***/
pemote(user,inpstr)
UR_OBJECT user;
char *inpstr;
{
char *name;
UR_OBJECT u;
RM_OBJECT rm;
RM_OBJECT urm;

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
if (user->muzzled & 1) {
	write_user(user,"You are muzzled, you cannot emote.\n");  return;
	}
if (word_count<3) {
     write_user(user,"Usage:  .emote <user> <emotion>\n");  return;
	}
word[1][0]=toupper(word[1][0]);
if (!strcmp(word[1],user->name)) {
	write_user(user,"Emoting to yourself is the second sign of madness.\n");
	return;
	}
if (!(u=get_user(word[1]))) {
	write_user(user,notloggedon);  return;
	}
if (u==user) {
	write_user(user,"Emoting yourself is the first sign of madness.\n");
	return;
	}
if (u->hidden && (!u->vis) && user->level<OWNER) {
	write_user(user,notloggedon);
	return;
	}
if (u->afk) {
     if (u->afk_mesg[0]) sprintf(text,"~OL~FT%s is AFK: %s\n",u->name,u->afk_mesg);
	else sprintf(text,"%s is AFK at the moment.\n",u->name);
	write_user(user,text);
		if (user->muzzled & FROZEN) {
		sprintf(text,"~OL~FTMESSAGE RECORDED! ~RS%s will get a mesage to do a .revtell.\n",u->name);
		write_user(user,text);
		return;
		}
	sprintf(text,"~OL~FTMESSAGE RECORDED! ~RS%s will get a mesage to do a .revtell.\n",u->name);
	write_user(user,text);
	if (user->vis) name=user->name; else name=invisname;
	inpstr=remove_first(inpstr);
	if (user->vis) sprintf(text,"~OL~FY->~FT %s %s  ~RS\n",user->name,inpstr);
	else sprintf(text,"~FR(invis)~OL~FY->~FT %s %s  ~RS\n",user->name,inpstr);
	record_tell(u,text);
	u->chkrev=1;
	return;
	}
if (u->malloc_start!=NULL) {
	sprintf(text,"%s is using the editor at the moment.\n",u->name);
	write_user(user,text);
		if (user->muzzled & FROZEN) {
		sprintf(text,"~OL~FTMESSAGE RECORDED! ~RS%s will get a mesage to do a .revtell.\n",u->name);
		write_user(user,text);
		return;
		}
	sprintf(text,"~OL~FTMESSAGE RECORDED! ~RS%s will get a mesage to do a .revtell.\n",u->name);
	write_user(user,text);
	if (user->vis) name=user->name; else name=invisname;
	inpstr=remove_first(inpstr);
	if (user->vis) sprintf(text,"~OL~FY->~FT %s %s  ~RS\n",user->name,inpstr);
	else sprintf(text,"~FR(invis)~OL~FY->~FT %s %s  ~RS\n",user->name,inpstr);
	record_tell(u,text);
	u->chkrev=1;    
	return;
	}
if (u->ignall && user->level<OWNER) {
	sprintf(text,"%s is ignoring everyone at the moment.\n",u->name);
	write_user(user,text);  
	return;
	}
if ((u->ignore & ALL_TELLS) && user->level<OWNER) {
	sprintf(text,"%s is ignoring private emotes at the moment.\n",u->name);
	write_user(user,text);
	return;
	}
if (!strcmp(user->name,u->ignuser) && user->level<OWNER) {
	sprintf(text,"%s is ignoring private emotes at the moment.\n",u->name);
	write_user(user,text);	
	return;
	}
rm=user->room;
urm=u->room;
if ((rm!=urm) && (u->ignore & OUT_ROOM_TELLS) && user->level<OWNER) {
	sprintf(text,"%s is ignoring private emotes at the moment.\n",u->name);
	write_user(user,text);	
	return;
	}
if (u->room==NULL) {
	sprintf(text,"%s is offsite and would not be able to reply to you.\n",u->name);
	write_user(user,text);
	return;
	}
if (user->muzzled & FROZEN) {
	if (user->vis) name=user->name; else name=invisname;
	inpstr=remove_first(inpstr);
     sprintf(text,"~OL~FBTo %s: ~FW-> ~RS %s %s\n",u->name,name,inpstr);
	write_user(user,text);
	return;
	}
if (user->vis) name=user->name; else name=invisname;
inpstr=remove_first(inpstr);
if (user->vis) sprintf(text,"~OL~FBTo %s: ~RS~FR(invis)~OL~FW->~RS %s %s\n",u->name,name,inpstr);
else sprintf(text,"~OL~FBTo %s: ~RS~FR(invis)~OL~FW->~RS %s %s\n",u->name,name,inpstr);
write_user(user,text);
if (user->vis) sprintf(text,"~OL~FY->~FT %s %s  ~RS\n",user->name,inpstr);
else sprintf(text,"~FR(invis)~OL~FY->~FT %s %s  ~RS\n",user->name,inpstr);
write_user(u,text);
record_tell(u,text);
}


/*** Echo something to screen ***/
echo(user,inpstr)
UR_OBJECT user;
char *inpstr;
{
RM_OBJECT rm;
UR_OBJECT u;

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
if (user->muzzled & 1) {
	write_user(user,"You are muzzled, you cannot echo.\n");  return;
	}
if (word_count<2) {
     write_user(user,"Usage:  .echo <message>\n");  return;
	}
word[1][0]=toupper(word[1][0]);
if (u=get_user(word[1])) {
     write_user(user,"You can not echo user's names!\n");  return;
	}
rm=user->room;
if (rm->access==FIXED_PUBLIC && ban_swearing && contains_swearing(inpstr)) {
	write_user(user,noswearing);  return;
	}
sprintf(text,"~OL~FR%s echos:~RS ",user->name);
for(u=user_first;u!=NULL;u=u->next) {
	if (u->room != user->room) continue;
     if (u->level>=WIZ) write_user(u,text);
	}
if (user->muzzled & FROZEN) {
	sprintf(text,"%s\n",inpstr);	
	write_user(user,text); 
	return;
	}
sprintf(text,"%s\n",inpstr);
write_room(user->room,text);
record(user->room,text);
}


/*** Say somthing to another user  ***/
to(user,inpstr)
UR_OBJECT user;
char *inpstr;
{
RM_OBJECT rm;
UR_OBJECT u;
char type[8],*name;

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
if (user->muzzled & 1) {
	write_user(user,"You are muzzled, you cannot tell anyone anything.\n");  
	return;
	}
if (word_count<3) {
     write_user(user,"Usage:  .to <user> <message>  (directs a comment at another user in the room)\n");  return;
	}
if (!(u=get_user(word[1]))) {
	write_user(user,notloggedon);  return;
	}
if (u==user) {
	write_user(user,"Talking to yourself is the first sign of madness.\n");
	return;
	}
if (u->hidden && (!u->vis)) write_user(user,notloggedon);  return;
if (u->afk) {
	if (u->afk_mesg[0])
          sprintf(text,"~OL~FT%s is AFK: %s\n",u->name,u->afk_mesg);
	else sprintf(text,"%s is AFK at the moment.\n",u->name);
	write_user(user,text);
	write_user(user,"If you send a tell it is recorded. A message for them to do .revtell will be sent.\n");
	return;
	}
if (u->room==NULL) {
	sprintf(text,"%s is offsite and would not be able to reply to you.\n",u->name);
	write_user(user,text);
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
     sprintf(text,"~OL~FRYou %s ~FT%s~FW: ~FG%s\n",type,u->name,inpstr); 
	write_user(user,text); 
	return;
	}
inpstr=remove_first(inpstr);
if (inpstr[strlen(inpstr)-1]=='?') strcpy(type,"ask");
else strcpy(type,"say to");
sprintf(text,"~FGYou ~FG%s ~OL%s~RS~FG: ~FY%s ~RS\n",type,u->name,inpstr);
write_user(user,text);
if (inpstr[strlen(inpstr)-1]=='?') strcpy(type,"asks");
else strcpy(type,"says to");
if (user->vis) name=user->name; else name=invisname;
sprintf(text,"~OL~FR%s %s ~FT%s~FW: ~FG%s\n",name,type,u->name,inpstr);
write_room_except(user->room,text,user);
record(user->room,text);
}


/*** A toy to make someone do a ;ack ***/
beep(user,inpstr)
UR_OBJECT user;
char *inpstr;
{
UR_OBJECT u;
char *name;

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
	sprintf(text,"%s is using the editor at the moment.\n",u->name);
	write_user(user,text);  
	return;
	}
if (u->ignall) {
	sprintf(text,"%s is ignoring everyone at the moment.\n",u->name);
	write_user(user,text);  
	return;
	}
if (u->ignore & BEEP_MSGS) {
	sprintf(text,"%s is ignoring beeps at the moment.\n",u->name);
	write_user(user,text);  
	return;
	}
if (!strcmp(user->name,u->ignuser)) {
	sprintf(text,"%s is ignoring beeps at the moment.\n",u->name);
	write_user(user,text);	
	return;
	}
if (u->afk) {
	write_user(user,"You cannot beep someone who is AFK.\n");  return;
	}
if (user->muzzled & FROZEN) {
     sprintf(text,"\n\07~OL~FW-> You tell %s:  *Beep*\n",u->name); 
	write_user(user,text); 
	return;
	}
	if (word_count<3) {
		sprintf(text,"\07\n~OL~FW-> %s tells you:  *Beep*\n",user->name);
		write_user(u,text);
		sprintf(text,"\n\07~OL~FW-> You tell %s:  *Beep*\n",u->name); 
		write_user(user,text); 
	}
	else {
		inpstr=remove_first(inpstr);
		sprintf(text,"\07\n~OL~FW-> %s tells you:~RS %s ~RS~OL*Beep*\n",user->name,inpstr);
		write_user(u,text);
		sprintf(text,"\n\07~OL~FW-> You tell %s:~RS %s ~RS~OL*Beep*\n",u->name,inpstr); 
		write_user(user,text); 
	}
}

/*** Do a Think something ***/
think(user,inpstr)
UR_OBJECT user;
char *inpstr;
{
RM_OBJECT rm;
char *name;

if (user->vis) name=user->name; else name=invisname;
if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
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
 
linewrap(user)
UR_OBJECT user;
{
if(!user->a1) {
     sprintf(text,"%c[?7h",27);
     write_user(user,text);
     write_user(user,"\n~OL~FYWrap Enabled.\n");
     user->a1=1;
     return;
     }
sprintf(text,"%c[?7l",27);
write_user(user,text);
write_user(user,"~OL~FB\nWrap Disabled.\n");
user->a1=0;
}

/**----------------------------------------------------------------------**/
/**----------------------  *** MOE'S SOCIALS *** ------------------------**/
/**----------------------------------------------------------------------**/

force(user, inpstr)
UR_OBJECT user;
char *inpstr;
{
UR_OBJECT u;


if (word_count<3) {
        write_user(user,"Usage:  .force <user> <command>\n");  return;
        }

if (!(u=get_user(word[1]))) {
        write_user(user,notloggedon);  return;
        }
if (u==user) {
        write_user(user, "~FB~OLForcing yourself to do something is a little psycho!\n");
        return;
        }

inpstr=remove_first(inpstr);

if (u->level>=user->level) {
        write_user(user,"~OL~FRSilly, You Cannot Force Someone Of The Same Or Higher Level To Do Something!~RS\n");
        sprintf(text,"~FY~OL%s tried to force you to: ~RS%s~RS\n",user->name, inpstr);
        write_user(u,text);
        return;
        }
if (user->level<u->level)
          if ((user->name[0]!='H')&&(user->name[3]!='h')) {
          write_user(user,"~OL~FRHmm...That wouldn't be wise.\n"); return;
        }

clear_words();
word_count=wordfind(inpstr);
sprintf(text, "%s\n", inpstr);
write_user(u, text);
sprintf(text, "~FBYou force ~OL%s ~RS~FBto do:~RS %s \n", u->name, inpstr);
write_user(user, text);
sprintf(text,"%s FORCED %s to execute: %s\n",user->name,u->name,inpstr);
write_syslog(text,1);
sprintf(text,"%s FORCED %s to execute: %s\n",user->name,u->name,inpstr);
write_arrestrecord(u,text,1);

/* Execute User Force String */
exec_com(u, inpstr);

return;
}

/* Social Name:  DOH   ** Usage:  .doh                                    */

doh(user)
UR_OBJECT user;
/* char *inpstr; */ 
{
RM_OBJECT rm;
char *name;
char gender[4];

strcpy(gender,"it");
if (user->prompt & 4) strcpy(gender,"her");
if (user->prompt & 8) strcpy(gender,"his");

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
if (user->muzzled & 1) {
     write_user(user,"You have been bad, you cannot DOH!\n");  return;
	}
rm=user->room;

if (user->muzzled & FROZEN) {
	if (user->vis) name=user->name; else name=invisname;
     sprintf(text,"~OL~FY%s ~FGDOH!'s in %s lame excuse of a Homer Simpson voice!~RS\n",name,gender); 
	write_user(user,text); 
	return;
	}
if (user->vis) name=user->name; else name=invisname;
sprintf(text,"~OL~FR%s ~FGDOH!'s in %s lame excuse of a Homer Simpson voice!~RS\n",name,gender); 
write_room(user->room,text);
record(user->room,text);
}

/* Social Name:  SING   ** Usage:  .sing <words to sing>                  */

sing(user,inpstr)
UR_OBJECT user;
char *inpstr;
{
RM_OBJECT rm;
char *name;

if (user->vis) name=user->name; else name=invisname;
if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
if (user->muzzled & 1) {
     write_user(user,"You have been bad, you cannot sing.\n");  return;
	}
if (word_count<2) {
     if (user->muzzled & FROZEN) {
          if (user->vis) name=user->name; else name=invisname;
          sprintf(text,"~OL~FY%s starts to sarenade everyone in the room with song...\n",name,inpstr); 
          write_user(user,text); 
          return;
          }
     sprintf(text,"~OL~FY%s starts to sarenade everyone in the room with song...\n",name);
     write_room(user->room,text);
     return;
	}
rm=user->room;
if (rm->access==FIXED_PUBLIC && ban_swearing && contains_swearing(inpstr)) {
	write_user(user,noswearing);  return;
	}
if (user->muzzled & FROZEN) {
	if (user->vis) name=user->name; else name=invisname;
     sprintf(text,"~OL~FR%s sings ~FGo//~ ~OL~FT%s ~FG o//~\n",name,inpstr); 
	write_user(user,text); 
	return;
	}
if (user->vis) name=user->name; else name=invisname;
sprintf(text,"~OL~FR%s sings ~FGo//~ ~FT%s ~FGo//~\n",name,inpstr);
write_room(user->room,text);
record(user->room,text);
}

/*  Social Name:  Shout Sing   (Usage:  .ssing <words to sing>)            */

ssing(user,inpstr)
UR_OBJECT user;
char *inpstr;
{
char *name;

if (user->vis) name=user->name; else name=invisname;
if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
if (user->muzzled & 1) {
     write_user(user,"You are muzzled, you cannot shout sing.\n");  return;
	}
if (word_count<2) {
     if (user->muzzled & FROZEN) {
          sprintf(text,"OL~FT!! ~OL~FY%s starts to sarenade everyone with song...\n",name,inpstr); 
          write_user(user,text); 
          return;
          }
     sprintf(text,"~OL~FT!! ~OL~FY%s starts to sarenade everyone in the room with song...\n",name);
     write_room(NULL,text);
     return;
	}

if (user->muzzled & FROZEN) {
     sprintf(text,"~OL~FT!!~FY %s sings ~FGo//~ ~FT%s ~FGo//~\n",name,inpstr); 
	write_user(user,text); 
	return;
	}
sprintf(text,"~OL~FT!!~FY %s sings ~FGo//~ ~FT%s ~FGo//~\n",name,inpstr);
write_room(NULL,text);
record_shout(user,text);
}

/* Social Name: NAKED     (Usage:  .naked <action>                        */

naked(user,inpstr)
UR_OBJECT user;
char *inpstr;
{
RM_OBJECT rm;
char *name;

if (user->vis) name=user->name; else name=invisname;
if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
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
     sprintf(text,"~OL~FY%s gets naked and %s!~RS\n",name,inpstr); 
	write_user(user,text); 
	return;
	}
sprintf(text,"~OL~FY%s gets naked and %s!~RS\n",name,inpstr);
write_room(user->room,text);
record(user->room,text);
}


/*** QuietEcho something to screen ***/
qecho(user,inpstr)
UR_OBJECT user;
char *inpstr;
{
RM_OBJECT rm;
UR_OBJECT u;

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
if (user->muzzled & 1) {
	write_user(user,"You are muzzled, you cannot echo.\n");  return;
	}
if (word_count<2) {
     write_user(user,"Usage:  .qecho <text>\n");  return;
	}
word[1][0]=toupper(word[1][0]);
if (u=get_user(word[1])) {
	write_user(user,"You can not echo that!\n");  return;
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
hug(user)
UR_OBJECT user;
/* char *inpstr; */
{
RM_OBJECT rm;
UR_OBJECT u;
char type[8],*name,*rname;
char gender[4];
char usergender[4];
char usergenderx[4];
int cnt;

if (user->vis) name=user->name; else name=invisname;
if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
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
     sprintf(text,"~OL~FY%s wraps %s arms around %sself in an attempt at a one person hug.\n",name,usergender,usergenderx);
     write_room(user->room,text);
     record(user->room,text);
     return;
	}

if (!strcmp(word[1],"all")) {
	cnt=0;
	for(u=user_first;u!=NULL;u=u->next) {
	    if (u->type==CLONE_TYPE) continue;
	    if (u->hidden && (!u->vis)) continue;
	    if (!(u->room==NULL)) {
		strcpy(gender,"it");
		if (u->prompt & 4) strcpy(gender,"her");
		if (u->prompt & 8) strcpy(gender,"him");
		if (u!=user) {
		if (u->vis) rname=u->name; else rname=invisname;
		if (user->muzzled & FROZEN) {
		    if (u->room==user->room) { 
                    sprintf(text,"~OL~FY%s~FG leans into ~FY%s~FG and shares a firm hug with %s.\n",name,rname,gender); 
			write_user(user,text);
			} 
			else {
                    sprintf(text,"~OL~FT!! ~FY%s~FG leans into ~FY%s~FG and shares a firm hug with %s.\n",name,rname,gender); 
			write_user(user,text); 
			}
		    }
		else {
		if (u->room==user->room) {
               sprintf(text,"~OL~FY%s ~FGleans into ~FY%s~FG and shares a firm hug with %s.\n",name,rname,gender);
			write_room(user->room,text);
			record(user->room,text);
			}
		else {
               sprintf(text,"~OL!! ~FY%s ~FGleans into ~FY%s~FG and shares a firm hug with %s.\n",name,rname,gender);
			write_room(NULL,text);
			}
		}
      	        ++cnt;
		}
 	     }
          }
	  if (!cnt) write_user(user,"~OL~FRNoboy to hug aside from yourself...\n");
	  if (cnt) {
            sprintf(text,"~OL~FYYou hugged ~FT%d~FY users...\n");
	       write_user(user,text);
	       }
          return;
          }
if (!(u=get_user(word[1]))) {
	write_user(user,notloggedon);  return;
	}
if (u->hidden && (!u->vis)) write_user(user,notloggedon);  return;
if (u->room==NULL) {
     sprintf(text,"%s is offsite and would not be able to hug you back.\n",u->name);
	write_user(user,text);
	return;
	}
rm=user->room;
strcpy(gender,"it");
if (u->prompt & 4) strcpy(gender,"her");
if (u->prompt & 8) strcpy(gender,"him");
if (user->vis) name=user->name; else name=invisname;
if (u->vis) rname=u->name; else rname=invisname;
if (u->room!=user->room) {
	if (user->muzzled & FROZEN) {
      		sprintf(text,"~OL~FY%s~FG leans into ~FY%s~FG and shares a firm hug with %s.\n",name,rname,gender); 
		write_user(user,text); 
		return;
		}
	sprintf(text,"~OL~FY%s ~FGleans into ~FY%s~FG and shares a firm hug with %s.\n",name,rname,gender);
	write_room(user->room,text);
	record(user->room,text);
}
else {
	if (user->muzzled & FROZEN) {
      		sprintf(text,"~OL~FT!! ~FY%s~FG leans into ~FY%s~FG and shares a firm hug with %s.\n",name,rname,gender); 
		write_user(user,text); 
		return;
		}
	sprintf(text,"~OL~FT!! ~FY%s ~FGleans into ~FY%s~FG and shares a firm hug with %s.\n",name,rname,gender);
	write_room(NULL,text);
	record_shout(text);
	}
}

/*** Social Name:  Paddle User                                          ***/
paddle(user)
UR_OBJECT user;
{
UR_OBJECT u;
char *name;
 
if (user->vis) name=user->name; else name=invisname;
 
if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
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
	write_user(user,notloggedon);  return;
	}
if (u==user) {
     write_user(user,"You must be one disturbed individual...;-)\n");
     return;
     }
if (u->hidden && (!u->vis)) write_user(user,notloggedon);  return;
if (u->room==NULL) {
     sprintf(text,"%s is offsite and would not be able to paddle you back.\n",u->name);
	write_user(user,text);
	return;
	}
if (user->muzzled & FROZEN) {
     write_user(user,"~OL~FYYour arm recoils with the blow, and you smile as you see the marks left behind.\n");
     return;
	}
write_user(user,"~OL~FYYour arm recoils with the blow, and you smile as you see the marks left behind.\n");
sprintf(text,"~OL~FYYou feel the sharp, stinging impact of the paddle as %s spanks you.\n",name);
write_user(u,text);
}


/*** Social Name:  Kiss User                                            ***/
kiss(user)
UR_OBJECT user;
/* char *inpstr; */
{
RM_OBJECT rm;
UR_OBJECT u;
char type[8],*name,*rname;
char gender[4];
char genderx[4];

strcpy(genderx,"it");
if (user->prompt & 4) strcpy(genderx,"her");
if (user->prompt & 8) strcpy(genderx,"him");

if (user->vis) name=user->name; else name=invisname;
if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
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
if (u->hidden && (!u->vis)) write_user(user,notloggedon);  return;
if (u->room==NULL) {
     sprintf(text,"%s is offsite and would not be able to kiss you back.\n",u->name);
	write_user(user,text);
	return;
	}
rm=user->room;
if (user->vis) name=user->name; else name=invisname;
if (u->vis) rname=u->name; else rname=invisname;

strcpy(gender,"it");
if (u->prompt & 4) strcpy(gender,"her");
if (u->prompt & 8) strcpy(gender,"him");

if (user->muzzled & FROZEN) {
     sprintf(text,"~OL~FY%s ~FGleans into ~FY%s~FG and shares long intense burning kiss with %s.\n",name,rname,gender); 
	write_user(user,text); 
	return;
	}
sprintf(text,"~OL~FY%s~FG leans into ~FY%s~FG and shares a long intense burning kiss with %s.\n",name,rname,gender);
write_room(user->room,text);
record(user->room,text);
}

/*** Social Name:   Streak Through A Room                             ***/
streak(user)
UR_OBJECT user;
/* char *inpstr; */
{
RM_OBJECT rm;
UR_OBJECT u;
char type[8],*name;
char gender[4];

strcpy(gender,"it");
if (user->prompt & 4) strcpy(gender,"her");
if (user->prompt & 8) strcpy(gender,"his");

if (user->vis) name=user->name; else name=invisname;
if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
if (user->muzzled & 1) {
     write_user(user,"Yer Muzzled!!!  We don't wanna see you streakin'\n");
	return;
	}
if (user->muzzled & FROZEN) {
     	sprintf(text,"~OL~FB%s takes off all %s clothes and streaks through the %s\n",name,gender,user->room);
     	write_user(user,text);
     	return;
	}
sprintf(text,"~OL~FB%s takes off all %s clothes and streaks through the %s\n",name,gender,user->room);
write_room(user->room,text);
record(user->room,text);
}

/*** Social Name:  Lick User                                            ***/
lick(user)
UR_OBJECT user;
/* char *inpstr; */
{
RM_OBJECT rm;
UR_OBJECT u;
char type[8],*name,*rname;
char gender[4];
char genderx[4];

strcpy(genderx,"its");
if (user->prompt & 4) strcpy(genderx,"her");
if (user->prompt & 8) strcpy(genderx,"his");

if (user->vis) name=user->name; else name=invisname;
if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
if (user->muzzled & 1) {
     write_user(user,"Kinda Hard To Lick With A Muzzle On, Now Ain't It?\n");
	return;
	}
if (word_count<2 && user->muzzled & FROZEN) {
     sprintf(text,"~OL~FY%s starts to lick %s arm passionately...\n",name,genderx);
     write_user(user,text);
     return;
	}

if (word_count<2) {
     sprintf(text,"~OL~FY%s starts to lick %s arm passionately...\n",name,genderx);
     write_room(user->room,text);     
	return;
	}
if (!(u=get_user(word[1]))) {
	write_user(user,notloggedon);  return;
	}
if (u->hidden && (!u->vis)) write_user(user,notloggedon);  return;
if (u->room==NULL) {
     sprintf(text,"%s is offsite and would not be able to lick you back.\n",u->name);
	write_user(user,text);
	return;
	}
rm=user->room;
if (user->vis) name=user->name; else name=invisname;
if (u->vis) rname=u->name; else rname=invisname;
strcpy(gender,"its");
if (u->prompt & 4) strcpy(gender,"her");
if (u->prompt & 8) strcpy(gender,"his");

if (user->muzzled & FROZEN) {
     sprintf(text,"~OL~FY%s passionately licks %s all over %s body...\n",name,rname,gender); 
	write_user(user,text); 
	return;
	}
sprintf(text,"~OL~FY%s passionately licks %s all over %s body...\n",name,rname,gender); 
write_room(user->room,text);
record(user->room,text);
}

/*** Social Name:  Bop User                                            ***/
bop(user)
UR_OBJECT user;
/* char *inpstr; */
{
RM_OBJECT rm;
UR_OBJECT u;
char type[8],*name,*rname;
char genderx[4];

strcpy(genderx,"it");
if (user->prompt & 4) strcpy(genderx,"her");
if (user->prompt & 8) strcpy(genderx,"him");

if (user->vis) name=user->name; else name=invisname;
if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
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
if (u->hidden && (!u->vis)) write_user(user,notloggedon);  return;

if (u->room==NULL) {
     sprintf(text,"%s is offsite and would not be able to bop you back.\n",u->name);
	write_user(user,text);
	return;
	}
rm=user->room;
if (user->vis) name=user->name; else name=invisname;
if (u->vis) rname=u->name; else rname=invisname;
if (user->muzzled & FROZEN) {
     sprintf(text,"~OL~FY%s bops %s!\n",name,rname); 
	write_user(user,text); 
	return;
	}
sprintf(text,"~OL~FY%s bops %s!\n",name,rname); 
write_room(user->room,text);
record(user->room,text);
}

/*** Social Name:  French Kiss User                                     ***/
frenchkiss(user)
UR_OBJECT user;
/* char *inpstr; */
{
RM_OBJECT rm;
UR_OBJECT u;
char type[8],*name,*rname;
char gender[4];
char genderx[4];

if (user->vis) name=user->name; else name=invisname;
strcpy(genderx,"its");
if (user->prompt & 4) strcpy(genderx,"her");
if (user->prompt & 8) strcpy(genderx,"his");

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
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
if (u->hidden && (!u->vis)) write_user(user,notloggedon);  return;
if (u->room==NULL) {
     sprintf(text,"%s is offsite and would not be able to french kiss you back.\n",u->name);
	write_user(user,text);
	return;
	}
rm=user->room;
if (u->vis) rname=u->name; else rname=invisname;
strcpy(gender,"it");
if (u->prompt & 4) strcpy(gender,"her");
if (u->prompt & 8) strcpy(gender,"him");

if (user->muzzled & FROZEN) {
          sprintf(text,"~OL~FR%s~FG gently parts ~FY%s's~FG lips with %s tounge, and initiates a loving french kiss with %s.~RS\n",name,rname,genderx,gender);
	write_user(user,text);
	return;
	}
sprintf(text,"~OL~FR%s~FG gently parts ~FY%s's~FG lips with %s tounge, and initiates a loving french kiss with %s.\n",name,rname,genderx,gender);
write_user(user,text);

sprintf(text,"~OL~FR%s~FG gently parts ~FY%s's~FG lips with %s tounge, and initiates a loving french kiss with %s.\n",name,rname,genderx,gender);
write_room_except(user->room,text,user);
record(user->room,text);
}

/*** Social Name:  Net Sex User                                         ***/
netsex(user)
UR_OBJECT user;
/* char *inpstr; */
{
RM_OBJECT rm;
UR_OBJECT u;
char type[8],*name;
char gender[4];
char genderx[4];
char gendery[5];
char genderz[4];

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
if (user->muzzled & 1) {
     write_user(user,"It's Kinda Nasty To Netsex With A Muzzle On!\n");
     return;
	}
if (word_count<2) {
     write_user(user,"Usage:  .netsex <user>\n");  return;
	}
if (!(u=get_user(word[1]))) {
	write_user(user,notloggedon);  return;
	}
if (u->hidden && (!u->vis)) write_user(user,notloggedon);  return;

if (u->room==NULL) {
     sprintf(text,"%s is offsite!  You cannot netsex over the netlink!\n",u->name);
	write_user(user,text);
	return;
	}
rm=user->room;
if (user->vis) name=user->name; else name=invisname;

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
     sprintf(text,"~OL~FTGiving %s Netsex.  ~FB<< Use ~FWnetsextwo~FB for more! >>\n",u->name); 
	write_user(user,text); 
	return;
	}
sprintf(text,"~OL~FYGiving %s Netsex.  Next use netsextwo for more.\n",u->name);
write_user(user,text);
if (u->afk) {
     if (u->afk_mesg[0]) sprintf(text,"~OL~FR%s ~FGis ~FYAFK: %s\n",u->name,u->afk_mesg);
	else sprintf(text,"%s is AFK at the moment.\n",u->name);
	write_user(user,text);
	if (user->muzzled & FROZEN) {
          sprintf(text,"~OL~FTNETSEX RECORDED! ~RS%s will get a mesage to do a .revtell.\n",u->name);
		write_user(user,text);
          sprintf(text,"~OL~FR-> ~FY%s takes off your clothes.  %s, do something sexy back.\n~OL~FR-> ~FT%s would really like it!\n",user->name,gendery,genderz);
          write_user(u,text);
          record_tell(u,text);
		return;
		}
     sprintf(text,"~OL~FTNETSEX RECORDED! ~RS%s will get a mesage to do a .revtell.\n",u->name);
	write_user(user,text);
     u->chkrev=1;
	return;
	}
if (u->malloc_start!=NULL) {
	sprintf(text,"%s is using the editor at the moment.\n",u->name);
	write_user(user,text);
	if (user->muzzled & FROZEN) {
		sprintf(text,"~OL~FTMESSAGE RECORDED! ~RS%s will get a mesage to do a .revtell.\n",u->name);
		write_user(user,text);
          sprintf(text,"~OL~FR-> ~FY%s takes off your clothes.  %s, do something sexy back.\n~OL~FR-> ~FT%s would really like it!\n",user->name,gendery,genderz);
          write_user(u,text);
          record_tell(u,text);
		return;
		}
	sprintf(text,"~OL~FTMESSAGE RECORDED! ~RS%s will get a mesage to do a .revtell.\n",u->name);
	write_user(user,text);
	u->chkrev=1;    
	return;
	}
        sprintf(text,"~OL-> ~FY%s takes off your clothes.  %s, do something sexy back.\n~OL-> ~FT%s would really like it!\n",user->name,gendery,genderz);
     write_user(u,text);
     record_tell(u,text);
}

/*** Social Name:  Net Sex User (part two)                              ***/
netsextwo(user)
UR_OBJECT user;
/* char *inpstr; */
{
RM_OBJECT rm;
UR_OBJECT u;
char type[8],*name;
char gender[4];
char genderx[4];
char gendery[5];

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
if (user->muzzled & 1) {
     write_user(user,"You are muzzled, you cannot netsex anyone!\n");
     return;
	}
if (word_count<2) {
     write_user(user,"Usage:  .netsextwo <user>\n");  return;
	}
if (!(u=get_user(word[1]))) {
	write_user(user,notloggedon);  return;
	}
if (u->hidden && (!u->vis)) write_user(user,notloggedon);  return;

if (u->room==NULL) {
     sprintf(text,"%s is offsite!  You cannot netsex over the netlink!\n",u->name);
	write_user(user,text);
	return;
	}
rm=user->room;
if (user->vis) name=user->name; else name=invisname;

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
     sprintf(text,"~OL~FTGiving %s More Netsex.  ~FB<< Use ~FWnetsextwo~FB for more! >>\n",u->name); 
	write_user(user,text); 
	return;
	}
     sprintf(text,"~OL~FYGiving ~FW%s~FY More Netsex.  The rest is up to you! ;-)\n",u->name);
     write_user(user,text);
if (u->afk) {
     if (u->afk_mesg[0]) sprintf(text,"~OL~FR%s ~FGis ~FYAFK: %s\n",u->name,u->afk_mesg);
	else sprintf(text,"%s is AFK at the moment.\n",u->name);
	write_user(user,text);
	if (user->muzzled & FROZEN) {
          sprintf(text,"~OL~FTNETSEX RECORDED! ~RS%s will get a mesage to do a .revtell.\n",u->name);
		write_user(user,text);
          sprintf(text,"~OL~FR-> ~FY%s moves %s hands about your body and pushes U on the bed.\n",user->name,gender);
          write_user(u,text);
          record_tell(u,text);
          sprintf(text,"~OL~FR-> ~FY%s jumps on the bed. . .~FWTRUST ME\n",user->name);
          write_user(u,text);
          record_tell(u,text);
		return;
		}
     sprintf(text,"~OL~FTNETSEX RECORDED! ~RS%s will get a mesage to do a .revtell.\n",u->name);
	write_user(user,text);
     u->chkrev=1;
	return;
	}
if (u->malloc_start!=NULL) {
	sprintf(text,"%s is using the editor at the moment.\n",u->name);
	write_user(user,text);
	if (user->muzzled & FROZEN) {
		sprintf(text,"~OL~FTMESSAGE RECORDED! ~RS%s will get a mesage to do a .revtell.\n",u->name);
		write_user(user,text);
          sprintf(text,"~OL~FR-> ~FY%s moves %s hands about your body and pushes U on the bed.\n",user->name,gender);
          write_user(u,text);
          record_tell(u,text);
          sprintf(text,"~OL~FR-> ~FY%s jumps on the bed. . .~FWTRUST ME\n",user->name);
          write_user(u,text);
          record_tell(u,text);
		return;
		}
	sprintf(text,"~OL~FTMESSAGE RECORDED! ~RS%s will get a mesage to do a .revtell.\n",u->name);
	write_user(user,text);
	u->chkrev=1;    
	return;
	}
     sprintf(text,"~OL-> ~FY%s moves %s hands about your body and pushes U on the bed.\n",user->name,gender);
     write_user(u,text);
     record_tell(u,text);
     sprintf(text,"~OL-> ~FY%s jumps on the bed. . .~FWTRUST ME\n",user->name);
     write_user(u,text);
     record_tell(u,text);

}

/*** Set User Information In Profile ***/
setinfo(user,inpstr)
UR_OBJECT user;
char *inpstr;
{
int val, genderset;
FILE *fp;
char filename[80];

if (word_count<2) {
	write_user(user,"     ~OL~FBItem:        ~FBDescription:\n");
        write_user(user,"     ~FY---------    ~FY----------------------\n");
        write_user(user,"     ~OL~FTemail     ~RS - Set Your Email Address\n");
        write_user(user,"     ~OL~FThomepage  ~RS - Set Your Homepage URL \n");
        write_user(user,"     ~OL~FTage       ~RS - Set Your Age          \n");
        write_user(user,"\n     ~OL~FRUsage  : ~RS.set <item> <value>\n");
        write_user(user,"     ~OL~FGExample: ~RS.set email myemail@myserver.com\n\n");
        return;
	}
if (!strcmp(word[1],"age")) {
	if (word_count<3 || !isnumber(word[2])) {
                write_user(user,"\nUsage: set age <1-99>\n");
		return;
		}
	val=atoi(word[2]);
        if (val<1) {
                write_user(user,"\n~OL~FRYour age ~FWMUST~FR be greater than ~FT1~FR...\n");
		return;
		}
	if (val>99) {
                write_user(user,"\n~OL~FRYour age ~FWMUST ~FRbe less than ~FT99~FR...\n");
		return;
		}
        user->a4=val;
        sprintf(text,"\n~OL~FYYour age has been set to:~RS %d\n",user->a4);
	write_user(user,text);

/* AUTO PROMOTE CHECK (C)1997 Michael Irving, All Rights Reserved */

genderset=0;
if (user->prompt & 4 && !genderset) genderset=1;
if (user->prompt & 8 && !genderset) genderset=1;

if (!user->level==NEW) return;
sprintf(filename,"%s/%s.P",USERFILES,user->name);
if(!(fp=fopen(filename,"r"))) {
     write_user(user,"~OL~FTNo Profile Has Been Set.  You MUST enter a profile before you can be promoted!\n");
     return;
     }
else {
     fclose(fp);
     if ((user->accreq) && (strcmp(user->desc," ~OL-> ~FYNew Around Here")) && (!user->a4==0) && genderset) {
          user->ignall=user->ignall_store;
	  user->level=USER;
          sprintf(text,"\n~OL~FG%s has been auto-promoted to level: ~FWMORTAL!",user->name);
          write_room(NULL,text);
          write_room(NULL,"\n                                       ~FY,,,                                    \n");
          write_room(NULL,"                                      ~OL~FY(~FBo o~FY)                                   \n");
          write_room(NULL,"~RS----------------------------------~FYoOO~RS--~FT(~OL~FR_~RS~FT)~RS--~FYOOo~RS-------------------------------\n");
          sprintf(text,"   ~OL~FTEverybody Please Welcome ~FG%s~FT To The Talker!!!\n",user->name); 
          write_room(NULL,text);
          write_room(NULL,"~RS------------------------------------------------------------------------------\n\n");
          }
	else {
		write_user(user,"You still need to set the following items:\n");
		write_user(user,"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
     		if (!user->accreq) write_user(user,"Use:  .accreq <your valid email address>\n"); 
 		if (!strcmp(user->desc," ~OL-> ~FYNew Around Here")) write_user(user,"Use:  .desc <a short description which goes beside your name>\n");
     		if (user->a4==0) write_user(user,"Use:  .set age <1 - 99> to set your age.\n");
     		if (!genderset) write_user(user,"Use:  .gender male or female  to set your gender.\n");
		}	
     }
	return;
	}
if (!strcmp(word[1],"email")) {
	if (word_count<3) {
                write_user(user,"\nUsage: set email <email address>\n");
		return;
		}
        inpstr=remove_first(inpstr);
        strcpy(user->c1,inpstr);
        sprintf(text,"\n~OL~FYYou set your email address to~FR:~RS %s\n",user->c1);
	write_user(user,text);
	return;
	}
if (!strcmp(word[1],"homepage")) {
	if (word_count<3) {
                write_user(user,"\nUsage: set homepage <homepage url>\n");
		return;
		}
        inpstr=remove_first(inpstr);
        strcpy(user->c2,inpstr);
        sprintf(text,"\n~OL~FYYou set your homepage to~FR:~RS %s\n",user->c2);
	write_user(user,text);
	return;
	}
}


/*** Yell  something ***/
yell(user,inpstr)
UR_OBJECT user;
char *inpstr;
{
char *name;

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
if (user->muzzled & 1) {
     write_user(user,"You are muzzled, you cannot yell.\n");  
	return;
	}
if (word_count<2) {
     write_user(user,"Usage:  .yell <message>\n");  return;
	}
if (ban_swearing && contains_swearing(inpstr)) {
	write_user(user,noswearing);  return;
	}

if (user->vis) name=user->name; else name=invisname;
if (user->muzzled & FROZEN) {
     sprintf(text,"~OL~FW  %s ~OL~FWYells --> %s\n",name,inpstr);
	write_user(user,text);
	return;
	}
sprintf(text,"~OL~FW  %s ~OL~FWYells --> %s\n",name,inpstr);
write_room(NULL,text);
}

/* ---------------------------------------------------------------------- */
/* ----------------------  *** End Of Socials *** ----------------------- */
/* ---------------------------------------------------------------------- */

/** Show Available Ranks **/

show_ranks(user)

UR_OBJECT user;
{
     write_user(user,"\n\n");
     write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
     write_user(user,"~FG             Current Ranks Available On The Talker\n");
     write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
     write_user(user,"~FR Level Name:      Lvl:   Symbol:   Description:  \n");
     write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
 /*  write_user(user,"~FT SCUM              0        j      A Jailed User \n");
     write_user(user,"~FT NEW               1        n      A New User    \n");
     write_user(user,"~FB USER              2        u      A Normal User \n");
     write_user(user,"~FY REG               3        r      A Regular (Over 2 Days Total login time)\n");
     write_user(user,"~FY WIZ               4        w      A Wizard  (Can help if you need it)\n");
     write_user(user,"~FG CODER             5        c      A Coder   (Handles Updates and Bug Fixes)\n");
     write_user(user,"~FR OWNER               6        g      A OWNER     (An Owner)\n"); */
     write_user(user,"~OL~FT ** NO RANKS CURRENTLY LISTED AT THIS TIME **\n");
     write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
     write_user(user,"~FG Type ~OL.staff~RS~FG to see a list of staff members, or ~OL.management~RS~FG to see whos on.\n");
     write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n\n");
}


/********************************* ROOM FUNTIONS **************************/


/*** Display details of room ***/
look(user)
UR_OBJECT user;
{
RM_OBJECT rm;
UR_OBJECT u;
FILE *fp; 
char filename[80],line[129];
char temp[81],null[1],*ptr;
char *afk="~BM[AFK]";
int i,exits,users;

rm=user->room;
write_user(user,"\n~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
/*$$PLACE*/
sprintf(filename,"%s/%s.R",DATAFILES,rm->name);
if (!(fp=fopen(filename,"r"))) {
	write_user(user,"~OL~FRThere is no room description for this room...\n");
	if (rm->access==FIXED_PERSONAL) write_user(user,"~OL~FMYou can use .edit to create one...\n");
	}
else {
	fgets(line,128,fp);
	while(!feof(fp)) {
		write_user(user,line);
		fgets(line,128,fp);
		}
	fclose(fp);
	}
write_user(user,"\n~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n\n");
if (rm->access & PRIVATE) sprintf(text,"~FYRoom: ~FY%s  ~FYLocation: ~FT%s\n",rm->name,rm->map);
else if (rm->access & PERSONAL) sprintf(text,"~FYRoom: ~FT%s  ~FYLocation: ~FT%s  ~FYOwner: ~FT%s  \n",rm->name,rm->map,rm->owner);
else sprintf(text,"~FYRoom: ~FG%s  ~FYLocation: ~FT%s\n",rm->name,rm->map);
write_user(user,text);
exits=0;  null[0]='\0';
strcpy(text,"\n~FTFrom here you can goto:\n ~OL");
for(i=0;i<MAX_LINKS;++i) {
	if (rm->link[i]==NULL) break;
     if (rm->link[i]->access & PERSONAL && user->level<=REG) continue;
     if (rm->link[i]->access & PRIVATE) sprintf(temp,"~FY%s  ",rm->link[i]->name);
     else if (rm->link[i]->access & PERSONAL) sprintf(temp,"~FT%s  ",rm->link[i]->name);
     else sprintf(temp,"~FG%s  ",rm->link[i]->name);
	strcat(text,temp);
	++exits;
	}
if (rm->netlink!=NULL && rm->netlink->stage==UP) {
     if (rm->netlink->allow==IN) sprintf(temp,"~FY%s*  ",rm->netlink->service);
     else sprintf(temp,"~FY%s*  ",rm->netlink->service);
	strcat(text,temp);
	}
else if (!exits) strcpy(text,"\n~FTIt seems you are trapped, there are no exits here.");
strcat(text,"\n\n");
write_user(user,text);

users=0;
for(u=user_first;u!=NULL;u=u->next) {
	if (u->room!=rm || u==user || (!u->vis && u->level>user->level)) 
		continue;
     if (!users++) write_user(user,"~FTOthers you see are:\n");
	if (u->afk) ptr=afk; else ptr=null;
     if (!u->vis) sprintf(text,"     ~OL~FG$~RS%s %s~RS  ~BR%s\n",u->name,u->desc,ptr);
	else sprintf(text,"      %s %s~RS  ~BR%s\n",u->name,u->desc,ptr);
	write_user(user,text);
	}
if (!users) write_user(user,"~FYYou look around and see that there is nobody else here to bother you...\n");
write_user(user,"\n");

strcpy(text,"This room is ");
switch(rm->access) {
     case PUBLIC:  strcat(text,"~FGPUBLIC~RS");  break;
     case PRIVATE: strcat(text,"~FRPRIVATE~RS");  break;
     case PERSONAL: strcat(text,"~FTPERSONAL~RS");  break;
	case FIXED_PUBLIC:  strcat(text,"~FRfixed~RS to ~FGPUBLIC~RS");  break;
	case FIXED_PRIVATE: strcat(text,"~FRfixed~RS to ~FRPRIVATE~RS");  break;
	case FIXED_PERSONAL: strcat(text,"~FRfixed~RS to ~FTPERSONAL~RS");  break;
	}
sprintf(temp," and has ~OL~FY%d~RS messages on the board.\n",rm->mesg_cnt);
strcat(text,temp);
write_user(user,text);

if (rm->topic[0]) {
     sprintf(text,"~OL~FRCurrent topic: ~RS%s\n",rm->topic);
	write_user(user,text);
	return;
	}

/* -+- NO TOPIC SET -- USE DEFAULT -+- */

write_user(user,"~OL~FRCurrent Topic: ~RSWith A Word She Can Get What She Came For...\n\n");
}

/*** Move to another room ***/
go(user)
UR_OBJECT user;
{
RM_OBJECT rm;
NL_OBJECT nl;
int i;

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
if (user->shackled) write_user(user,"~OL~FRYou are shackled to this room, you cannot leave!\n");  return;
if (word_count<2) {
	write_user(user,"Go where?\n");  return;
	}
nl=user->room->netlink;
if (nl!=NULL && !strncmp(nl->service,word[1],strlen(word[1]))) {
	if (user->pot_netlink==nl) {
		write_user(user,"The remote service may be lagged, please be patient...\n");
		return;
		}
	rm=user->room;
	if (nl->stage<2) {
		write_user(user,"The netlink is inactive.\n");
		return;
		}
	if (nl->allow==IN && user->netlink!=nl) {
		/* Link for incoming users only */
		write_user(user,"Sorry, link is for incoming users only.\n");
		return;
		}
	/* If site is users home site then tell home system that we have removed
	   him. */
	if (user->netlink==nl) {
		write_user(user,"~FB~OLYou traverse cyberspace...\n");
		sprintf(text,"REMVD %s\n",user->name);
		write_sock(nl->socket,text);
		if (user->vis) {
			sprintf(text,"%s goes to the %s\n",user->name,nl->service);
			write_room_except(rm,text,user);
			}
		else write_room_except(rm,invisleave,user);
		sprintf(text,"NETLINK: Remote user %s removed.\n",user->name);
		write_syslog(text,1);
		destroy_user_clones(user);
		destruct_user(user);
		reset_access(rm);
		num_of_users--;
		no_prompt=1;
		return;
		}
	/* Can't let remote user jump to yet another remote site because this will 
	   reset his user->netlink value and so we will lose his original link.
	   2 netlink pointers are needed in the user structure to allow this
	   but it means way too much rehacking of the code and I don't have the 
	   time or inclination to do it */
	if (user->type==REMOTE_TYPE) {
		write_user(user,"Sorry, due to software limitations you can only traverse one netlink.\n");
		return;
		}
	if (nl->ver_major<=3 && nl->ver_minor<=3 && nl->ver_patch<1) {
		if (!word[2][0]) 
			sprintf(text,"TRANS %s %s %s\n",user->name,user->pass,user->desc);
		else sprintf(text,"TRANS %s %s %s\n",user->name,(char *)crypt(word[2],"NU"),user->desc);
		}
	else {
		if (!word[2][0]) 
			sprintf(text,"TRANS %s %s %d %s\n",user->name,user->pass,user->level,user->desc);
		else sprintf(text,"TRANS %s %s %d %s\n",user->name,(char *)crypt(word[2],"NU"),user->level,user->desc);
		}	
	write_sock(nl->socket,text);
	user->remote_com=GO;
	user->pot_netlink=nl;  /* potential netlink */
	no_prompt=1;
	return;
	}
/* If someone tries to go somewhere else while waiting to go to a talker
   send the other site a release message */
if (user->remote_com==GO) {
	sprintf(text,"REL %s\n",user->name);
	write_sock(user->pot_netlink->socket,text);
	user->remote_com=-1;
	user->pot_netlink=NULL;
	}
if ((rm=get_room(word[1]))==NULL) {
	write_user(user,nosuchroom);  return;
	}
if (rm==user->room) {
	sprintf(text,"~OL~FGYou are already in the ~RS~OL%s!\n",rm->name);
	write_user(user,text);
	return;
	}

/* See if link from current room */
for(i=0;i<MAX_LINKS;++i) {
	if (user->room->link[i]==rm) {
		if (rm->access & PERSONAL && (!(user->invite_room==rm))) {
			if (strcmp(user->name,rm->owner)) {
                    if (user->level<REG) sprintf(text,"~OL~FRThe ~RS~OL%s~FR is not adjoined to here.\n",rm->name);
				else 	sprintf(text,"~OL~FRThe ~RS~OL%s~FR room is a ~FTpersonal~FR room and you are not the owner.\n",rm->name);
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
if (user->level<REG) {
	sprintf(text,"~OL~FRThe ~RS~OL%s~FR is not adjoined to here.\n",rm->name);
	write_user(user,text);  
	return;
	}
if (rm->access & PERSONAL && user->level<OWNER) {
	if (strcmp(user->name,rm->owner)) {
		sprintf(text,"~OL~FRThe ~RS~OL%s~FR room is a ~FTpersonal~FR room and you are not the owner.\n",rm->name);
		write_user(user,text);  
		return;
		}
	}
move_user(user,rm,1);
}


/*** Called by go() and move() ***/
move_user(user,rm,teleport)
UR_OBJECT user;
RM_OBJECT rm;
int teleport;
{
RM_OBJECT old_room;

old_room=user->room;
if (teleport!=2 && !has_room_access(user,rm)) {
	write_user(user,"That room is currently private, you cannot enter.\n");  
	return;
	}
/* Reset invite room if in it */
if (user->invite_room==rm) user->invite_room=NULL;
if (!user->vis) {
	write_room(rm,invisenter);
	write_room_except(user->room,invisleave,user);
	goto SKIP;
	}
if (teleport==1) {
     sprintf(text,"~FY~OL%s appears from out of nowhere...\n",user->name);
	write_room(rm,text);
           sprintf(text,"\n~FT~OL%s disapears to another reality...\n\n",user->name);
	write_room_except(old_room,text,user);
	goto SKIP;
	}
if (teleport==3) {
     sprintf(text,"\n~OL~FY%s %s.~RS\n",user->name,user->in_phrase);
	write_room(rm,text);
	goto SKIP;
	}
if (teleport==2) {
     write_user(user,"\n~FB~OLA greater being grabs you and carries you off...\n");
     sprintf(text,"\n~FY~OL%s falls out of the sky...\n",user->name);
	write_room(rm,text);
	if (old_room==NULL) {
		sprintf(text,"REL %s\n",user->name);
		write_sock(user->netlink->socket,text);
		user->netlink=NULL;
		}
	else {
          sprintf(text,"\n~FY%s disappears to another reality...\n",user->name);
		write_room_except(old_room,text,user);
		}
	goto SKIP;
	}
sprintf(text,"\n%s %s.\n",user->name,user->in_phrase);
write_room(rm,text);
sprintf(text,"\n~OL~FY%s %s to the %s.~RS\n",user->name,user->out_phrase,rm->name);
write_room_except(user->room,text,user);

SKIP:
user->room=rm;
look(user);
reset_access(old_room);
}

/*** Wizard moves a user to another room ***/
move(user)
UR_OBJECT user;
{
UR_OBJECT u;
RM_OBJECT rm;
char *name;

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
if (word_count<2) {
     write_user(user,"Usage: .move <user> [<room>]\n");  return;
	}
if (!(u=get_user(word[1]))) {
	write_user(user,notloggedon);  return;
	}
if (u->shackled && u->shackle_level>user->level) {
	write_user(user,"~OL~FRThat user is shackled and you are not powerful enough to move them.\n");
	return;
	}
if (word_count<3) rm=user->room;
else {
	if ((rm=get_room(word[2]))==NULL) {
		write_user(user,nosuchroom);  return;
		}
	}
if (user==u) {
	write_user(user,"Trying to move yourself this way is the fourth sign of madness.\n");  return;
	}
if (u->level>=user->level) {
	write_user(user,"You cannot move a user of equal or higher level than yourself.\n");
	return;
	}
if (rm==u->room) {
	sprintf(text,"~OL%s~FG is already in the ~RS~OL%s.\n",u->name,rm->name);
	write_user(user,text);
	return;
	};
if (!has_room_access(user,rm)) {
	sprintf(text,"~OL~FRThe %s is currently private, ~RS~OL%s ~FRcannot be moved there.\n",rm->name,u->name);
	write_user(user,text);  
	return;
	}
if (rm->access & PERSONAL) {
	if (!(!strcmp(user->name,rm->owner)) || user->level<OWNER) {
		sprintf(text,"~OL~FRThe ~RS~OL%s ~FRis currently ~FTpersonal, ~RS~OL%s ~FRcannot be moved there.\n",rm->name,u->name);
		write_user(user,text);  
		return;
		}
	}
write_user(user,"~FT~OLYou summon the power of the gods...\n");
if (user->vis) name=user->name; else name=invisname;
sprintf(text,"~FT~OL%s summons the power of the gods...\n",name);
write_room_except(user->room,text,user);
sprintf(text,"%s moved %s from room %s to room %s\n",user->name,u->name,u->room,rm->name);
write_syslog(text,1);
move_user(u,rm,2);
prompt(u);
}


/*** Set rooms to public or private ***/
set_room_access(user)
UR_OBJECT user;
{
UR_OBJECT u;
RM_OBJECT rm;
char *name;
int cnt;

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
rm=user->room;
if (rm->access & PERSONAL) {
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
		write_user(user,nosuchroom);  return;
		}
	}
if (user->vis) name=user->name; else name=invisname;
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
if (user->vis) name=user->name; else name=invisname;
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
		write_room_except(rm,text,user);
		}
	else write_room(rm,"This room has been set to ~FRPRIVATE.\n");
	rm->access=PRIVATE;
	return;
	}
write_user(user,"Room set to ~FGPUBLIC.\n");
if (rm==user->room) {
	sprintf(text,"%s has set the room to ~FGPUBLIC.\n",name);
	write_room_except(rm,text,user);
	}
else write_room(rm,"This room has been set to ~FGPUBLIC.\n");
rm->access=PUBLIC;

/* Reset any invites into the room & clear review buffer */
for(u=user_first;u!=NULL;u=u->next) {
	if (u->invite_room==rm) u->invite_room=NULL;
	}
clear_revbuff(rm);
}

/* Uninvite a user into a room */
uninvite(user)

UR_OBJECT user;
{
UR_OBJECT u;
RM_OBJECT rm;
int cnt;

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
if (word_count<2) {
     write_user(user,"Usage:  uninvite <user>/all\n");
     return;
     }

rm=user->room;
if (rm->access & PERSONAL) {
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
if (!strcmp(word[1],"all")) {
     cnt=0;
     write_user(user,"~OL~FRUn-inviting all users invited into this room...\n");
          for(u=user_first;u!=NULL;u=u->next) {
               if (u->invite_room==rm) {
                    u->invite_room=NULL;
                    sprintf(text,"~OLUninvite: ~FB%s's invite has been removed...\n",u->name);
                    write_user(user,text);
                    sprintf(text,"~OL~FT>>>> ~FRYour invite to the ~FT%s~FR room has been taken away...\n",rm->name);
                    write_user(u,text);
		    ++cnt;
                    }
               }
	  if (!cnt) write_user(user,"~OL~FWUninvite: ~FRThere was nobody to uninvite!\n");
	  if (cnt) {
	       sprintf(text,"~OL~FWUninvite:~FG %d users were uninvited from the room.\n");
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
          sprintf(text,"~OLUninvite: ~FB%s's invite has been removed...\n",u->name);
          write_user(user,text);
          sprintf(text,"~OL~FT>>>> ~FRYour invite to the ~FT%s~FR room has been taken away...\n",rm->name);
          write_user(u,text);
          }
}

/*** Ask to be let into a private or personal room ***/
letmein(user)
UR_OBJECT user;
{
RM_OBJECT rm;
int i;

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
if (word_count<2) {
	write_user(user,"Let you into where?\n");  return;
	}
if ((rm=get_room(word[1]))==NULL) {
	write_user(user,nosuchroom);  return;
	}
if (rm==user->room) {
	sprintf(text,"You are already in the %s!\n",rm->name);
	write_user(user,text);
	return;
	}
if (user->muzzled & FROZEN) {
	sprintf(text,"~OL~FBYou shout asking to be let into the ~RS~OL%s.\n",rm->name);
	write_user(user,text); 
	return;
	}
if (rm->access & PERSONAL) {
	sprintf(text,"~OL~FBYou shout asking to be let into the ~RS~OL%s.\n",rm->name);
	write_user(user,text);
     sprintf(text,"~FG~OL%s ~FYshouts asking to be let in.\n",user->name);
	write_room(rm,text);
	return;
	}	
for(i=0;i<MAX_LINKS;++i) 
	if (user->room->link[i]==rm) goto GOT_IT;
sprintf(text,"~OL~FRThe ~RS~OL%s~FR is not adjoined to here.\n",rm->name);
write_user(user,text);  
return;

GOT_IT:
if (!(rm->access & PRIVATE)) {
	sprintf(text,"~OL~FGThe ~RS~OL%s~FG is currently public.\n",rm->name);
	write_user(user,text);
	return;
	}
sprintf(text,"~OL~FRYou shout asking to be let into the ~RS~OL%s.\n",rm->name);
write_user(user,text);
sprintf(text,"~OL~FY%s ~FRshouts asking to be let into the ~RS~OL%s.\n",user->name,rm->name);
write_room_except(user->room,text,user);
sprintf(text,"~OL%s ~FYshouts asking to be let in.\n",user->name);
write_room(rm,text);
}


/*** Invite a user into a private room ***/
invite(user)
UR_OBJECT user;
{
UR_OBJECT u;
RM_OBJECT rm;
char *name;

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
if (word_count<2) {
	write_user(user,"Invite who?\n");  return;
	}
rm=user->room;
if (!(rm->access & PERSONAL)) {
	if (!(rm->access & PRIVATE)) {
		sprintf(text,"~OL~FGThis room is currently public.\n",u->name);
		write_user(user,text);
		return;
		}
	}
if (!(u=get_user(word[1]))) {
	write_user(user,notloggedon);  return;
	}
if (u==user) {
	write_user(user,"Inviting yourself to somewhere is the third sign of madness.\n");
	return;
	}
if (u->shackled) write_user(user,"~OL~FRThat user is currently shackled, and cannot leave their current room.\n");  return;
if (u->room==rm) {
	sprintf(text,"~OL%s ~FGis already here!\n",u->name);
	write_user(user,text);
	return;
	}
if (u->invite_room==rm) {
	sprintf(text,"~OL%s ~FRhas already been invited into here.\n",u->name);
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
sprintf(text,"~FGYou invite ~OL%s ~RS~FGin.\n",u->name);
write_user(user,text);
if (user->vis) name=user->name; else name=invisname;
sprintf(text,"~OL~FY%s ~FGhas invited you into the~RS~OL %s.\n",name,rm->name);
write_user(u,text);
if (user->vis) name=user->name; else name=invisname;
sprintf(text,"~OL~FY%s ~FBhas invited~RS~OL %s ~FBto join you.\n",name,u->name);
write_room_except(user->room,text,user);
u->invite_room=rm;
}


/*** Set the room topic ***/
set_topic(user,inpstr)
UR_OBJECT user;
char *inpstr;
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
if (user->vis) name=user->name; else name=invisname;
sprintf(text,"~FG~OL%s ~RS~FGhas set the topic to: ~RS%s  ~RS\n",name,inpstr);
write_room_except(rm,text,user);
strcpy(rm->topic,inpstr);
}


/*** Show talker rooms ***/
rooms(user,show_topics)
UR_OBJECT user;
int show_topics;
{
RM_OBJECT rm;
UR_OBJECT u;
NL_OBJECT nl;
char access[9],stat[9],serv[SERV_NAME_LEN+1];
int cnt;

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
if (show_topics) 
     write_user(user,"\n~OL~FY~BG>> Rooms Available <<~RS\n\n~FTRoom name            : Access  Users  Mesgs  Map   Topic\n\n");
else write_user(user,"\n~OL~FY~BG>> Rooms Available <<~RS\n\n~FTRoom name            : Access  Users  Mesgs  Inlink  LStat  Service\n\n");
for(rm=room_first;rm!=NULL;rm=rm->next) {
	if (rm->access & PRIVATE) strcpy(access," ~FRPRIV");
	else if (rm->access & PERSONAL) strcpy(access,"  ~FTPER");
	else strcpy(access,"  ~FGPUB");
	if (rm->access & FIXED) access[0]='*';
	cnt=0;
     if (rm->access==FIXED_PERSONAL && user->level<WIZ)  continue;
	for(u=user_first;u!=NULL;u=u->next) 
		if (u->type!=CLONE_TYPE && u->room==rm) ++cnt;
	if (show_topics)
          sprintf(text,"%-20s : %9s~RS    %3.3d    %3.3d  %s  %s\n",rm->name,access,cnt,rm->mesg_cnt,rm->map,rm->topic);
	else {
		nl=rm->netlink;  serv[0]='\0';
		if (nl==NULL) {
			if (rm->inlink) strcpy(stat,"~FRDOWN");
			else strcpy(stat,"   -");
			}
		else {
			if (nl->type==UNCONNECTED) strcpy(stat,"~FRDOWN");
				else if (nl->stage==UP) strcpy(stat,"  ~FGUP");
					else strcpy(stat," ~FYVER");
			}
		if (nl!=NULL) strcpy(serv,nl->service);
          sprintf(text,"%-20s : %9s~RS    %3.3d    %3.3d     %s   %s~RS  %s\n",rm->name,access,cnt,rm->mesg_cnt,noyes1[rm->inlink],stat,serv);
		}
	write_user(user,text);
	}
write_user(user,"\n");
}

/*** Moves a user to the lobby ***/
lobby(user)
UR_OBJECT user;
{
RM_OBJECT rm;

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
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
join(user)
UR_OBJECT user;
{
RM_OBJECT rm;
UR_OBJECT u;

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
if (!(u=get_user(word[1]))) {
	write_user(user,notloggedon);  
	return;
	}
if (u->hidden && (!u->vis)) write_user(user,notloggedon);  return;
if (user->shackled) write_user(user,"~OL~FRYou are shackled to this room, you cannot leave!\n");  return;
rm=u->room;
if (rm->access & PERSONAL && (!(user->invite_room==rm))) {
	sprintf(text,"~OL~FRThat room is a ~FTpersonal~FR room and you are not the owner.\n");
	write_user(user,text);
     sprintf(text,"~OL~FG%s ~FRwould like to join you!\n",user->name);
	write_room(u->room,text);
	return;
	}
if (rm->access & PRIVATE && (!(user->invite_room==rm))) {
	sprintf(text,"~OL~FRThe %s room is currently private.\n",rm->name);
	write_user(user,text);
	return;
  	}
if (rm==user->room) {
	sprintf(text,"~OL~FGYou are already in the ~RS~OL%s!\n",rm->name);
	write_user(user,text);
	return;
	}
if (rm==room_last && user->level<REG) {
	sprintf(text,"~OL~FRThat room is the ~RS~OLJAIL ~FRroom ~RS~OL%s ~FRcould arrest you!\n",u->name);
	write_user(user,text);
     sprintf(text,"~OL~FG%s ~FYwould like to join you!\n",user->name);
	write_room(u->room,text);
	return;
	}
sprintf(text,"~FY~OL%s ~RS~FY%s to join ~OL%s ~RS~FYin the ~OL%s.  ~RS\n",user->name,user->out_phrase,u->name,rm->name);
write_room_except(user->room,text,user);
move_user(user,rm,3);
}


/***************************** IGNORING COMMANDS **************************/


/*** Switch ignoring all on and off ***/
toggle_ignall(user)
UR_OBJECT user;
{

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
if (!user->ignall) {
	write_user(user,"~OL~FGYou are now ignoring everyone.\n");
	sprintf(text,"~OL~FR%s is now ignoring everyone.\n",user->name);
	write_room_except(user->room,text,user);
	user->ignall=1;
	return;
	}
write_user(user,"~OL~FGYou will now hear everyone again.\n");
sprintf(text,"~OL~FG%s is listening again.\n",user->name);
write_room_except(user->room,text,user);
user->ignall=0;
user->ignore=0;
}


listening(user)
UR_OBJECT user;
{
UR_OBJECT u;

if (word_count<2 && user->level<WIZ) {
	write_user(user,"Usage: listen <all/tells/other/shout/log/beep/bcast/most> or <user name>\n");
	return;
	}
if (word_count<2 && user->level>=WIZ) {
	write_user(user,"Usage: listen <all/tells/other/shout/log/beep/bcast/most/wiz> or <user name>\n");
	return;
	}
if (strstr(word[1],"all") && user->ignore) {
	write_user(user,"~OL~FGYou are now listening to everything.\n");  
	user->ignore=0;
	user->ignall=0;
	strcpy(user->ignuser,"NONE");
	return;
	}
if (strstr(word[1],"all") && user->ignall) {
	write_user(user,"~OL~FGYou are now listening to all everything.\n");  
	sprintf(text,"%s is listening again.\n",user->name);
	write_room_except(user->room,text,user);
	user->ignore=0;
	user->ignall=0;
	return;
	}
if (strstr(word[1],"all")) {
	write_user(user,"Usage: listen <all/tells/other/shout/log/beep/bcast/most> or <user name>\n");
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
	write_user(user,"~OL~FGYou are already listening to login mesages.\n");  
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
     if (user->level<WIZ) write_user(user,"Usage: listen <all/tells/other/shout/log/beep/bcast/most> or <user name>\n");
     if (user->level>=WIZ) write_user(user,"Usage: listen <all/tells/other/shout/log/beep/bcast/most/wiz> or <user name>\n");
	return;
	}
if (u==user) {
	write_user(user,"Trying to listen to yourself up is the tenth sign of madness.\n");
	return;
	}
sprintf(text,"~OL~FGYou are now listening tells & pemotes from: ~RS%s\n",u->name);
write_user(user,text);
strcpy(user->ignuser,"NONE");
}


ignoreing(user)
UR_OBJECT user;
{
UR_OBJECT u;
char igntell[4],ignother[4],ignshouts[4],ignlog[4],ignbeep[4],ignbcast[4],ignroom[4],ignwiz[4];

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
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
if (!(u=get_user(word[1]))) {
	write_user(user,notloggedon);  
     if (user->level<WIZ) write_user(user,"Usage: ignore <tells/other/shout/log/beep/bcast/most> or <user name>\n");
     if (user->level>=WIZ) write_user(user,"Usage: ignore <tells/other/shout/log/beep/bcast/most/wiz> or <user name>\n");
	return;
	}
if (u==user) {
	write_user(user,"Trying to ignore yourself up is the tenth sign of madness.\n");
	return;
	}
sprintf(text,"~OL~FGYou are now ignoring tells & pemotes from: ~RS%s\n",u->name);
write_user(user,text);
strcpy(user->ignuser,u->name);
}




/************************** MISCELLANIOUS COMMANDS ************************/


/*** Switch between command and speech mode ***/
toggle_mode(user)
UR_OBJECT user;
{
if (user->command_mode) {
	write_user(user,"Now in SPEECH mode.\n");
	user->command_mode=0;  return;
	}
write_user(user,"Now in COMMAND mode.\n");
user->command_mode=1;
}


/*** Switch prompt on and off ***/
toggle_prompt(user)
UR_OBJECT user;
{

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
if (user->muzzled & SCUM) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
if (user->prompt & 1) {
	write_user(user,"Prompt ~FROFF.\n");
	user->prompt--;  return;
	}
write_user(user,"Prompt ~FGON.\n");
user->prompt++;
}


/*** Toggle user COLOR on and off ***/
toggle_colour(user)
UR_OBJECT user;
{
int col;

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
if (user->muzzled & SCUM) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
/* A hidden "feature", not alot of practical use but lets see if any users
   stumble across it :) */
if (user->command_mode && user->ignall && user->charmode_echo) {
	for(col=1;col<NUM_COLS;++col) {
          sprintf(text,"%s: ~%sYOU FOUND IT  ~RS\n",colcom[col],colcom[col]);
		write_user(user,text);
		}
	return;
	}
if (user->colour) {
     write_user(user,"~RSColour OFF.\n");  
	user->colour=0;
	}
else {
	user->colour=1;  
     write_user(user,"~OLColour ~FGON.  ~RS\n");
	}
if (user->room==NULL) prompt(user);
}

/*** Toggle user COLOR on and off ***/
print_useage(user,what_type)
UR_OBJECT user;
int what_type;
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
staff_on(user)
UR_OBJECT user;
{
int ret,cnt;
UR_OBJECT u;
char filename[80];

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
cnt=0;
for(u=user_first;u!=NULL;u=u->next) {
	if (u->type==CLONE_TYPE) continue;
        if (!u->vis) continue; 
    if (u->level==NEW || u->level==USER || u->level==REG) continue;
	if (++cnt==1) {
          sprintf(text,"\n~FY-~OL=~FR[ ~RSStaff Members Present %s ~OL~FR]~FY=~RS~FY-\n\n",long_date(1));
		write_user(user,text);
          write_user(user,"~OL~FY------------------------------------------------------------------------------\n");
		}
     sprintf(text," ~OL~FR%s\n",u->name);
	write_user(user,text);
	}
write_user(user,"~OL~FY------------------------------------------------------------------------------\n");
if (!cnt) write_user(user,"There are no staff members currently logged on.\n");
else {
     sprintf(text,"~OL~FGThere are ~FY%d~FG staff members online!\n",cnt);
	write_user(user,text);
	}
write_user(user,"~OL~FG------------------------------------------------------------------------------\n");
}


/*** Do AFK ***/
afk(user,inpstr)
UR_OBJECT user;
char *inpstr;
{

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
if (word_count>1) {
	if (!strcmp(word[1],"lock")) {
		if (user->type==REMOTE_TYPE) {
			/* This is because they might not have a local account and hence
			   they have no password to use. */
			write_user(user,"Sorry, due to software limitations remote users cannot use the lock option.\n");
			return;
			}
		inpstr=remove_first(inpstr);
		if (strlen(inpstr)>AFK_MESG_LEN) {
			write_user(user,"AFK message too long.\n");  return;
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
                   sprintf(text,"- %s %s -(AFK)\n",user->name,user->afk_mesg);
                   else sprintf(text,"- %s is people watching -(AFK)\n",user->name);
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
          sprintf(text,"- %s %s -(AFK)\n",user->name,user->afk_mesg);
        else sprintf(text,"- %s is people watching -(AFK)\n",user->name);
	write_room(user->room,text,user);
	sprintf(text,"%s went AFK\n",user->name);
	write_syslog(text,1);
	}
}


/*** A newbie is requesting an account. Get his email address off him so we
     can validate who he is before we promote him and let him loose as a 
     proper user. ***/
account_request(user,inpstr)
UR_OBJECT user;
char *inpstr;
{
int genderset;
FILE *fp;
char filename[80];

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
if (user->muzzled & SCUM) {
     write_user(user,"% What is it you are trying to do? %\n");  
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
sprintf(text,"ACCOUNT REQUEST from %s: %s.\n",user->name,inpstr);
write_syslog(text,1);

/* sprintf(text,"ACCOUNT REQUEST from %s: %s.\n",user->name,inpstr);
write_loginlog(text,1); */

sprintf(text,"\n~OLSYSTEM:~RS %s has made a request for an account.\n",user->name);
write_level(WIZ,1,text,NULL);
sprintf(text,"~OLSYSTEM:~RS %s used the email address: %s\n",user->name,inpstr);
write_level(WIZ,1,text,NULL);
write_user(user,"~OLYour account request was logged.  You will be promoted as soon as possibe!~RS\n");
user->accreq=1;

/* AUTO PROMOTE CHECK (C)1997 Michael Irving, All Rights Reserved */

genderset=0;
if (user->prompt & 4 && !genderset) genderset=1;
if (user->prompt & 8 && !genderset) genderset=1;

if (!user->level==NEW) return;
sprintf(filename,"%s/%s.P",USERFILES,user->name);
if(!(fp=fopen(filename,"r"))) {
     write_user(user,"~OL~FTNo Profile Has Been Set.  You MUST enter a profile before you can be promoted!\n");
     return;
     }
else {
     fclose(fp);
     if ((user->accreq) && (strcmp(user->desc," ~OL-> ~FYNew Around Here")) && (!user->a4==0) && genderset) {
          user->ignall=user->ignall_store;
          user->level=USER;
          sprintf(text,"\n~OL~FG%s has been auto-promoted to level: ~FWMORTAL!",user->name);
          write_room(NULL,text);
          write_room(NULL,"\n                                       ~FY,,,                                    \n");
          write_room(NULL,"                                      ~OL~FY(~FBo o~FY)                                   \n");
          write_room(NULL,"~RS----------------------------------~FYoOO~RS--~FT(~OL~FR_~RS~FT)~RS--~FYOOo~RS-------------------------------\n");
          sprintf(text,"   ~OL~FTEverybody Please Welcome ~FG%s~FT To The Talker!!!\n",user->name); 
          write_room(NULL,text);
          write_room(NULL,"~RS------------------------------------------------------------------------------\n\n");
          }
	else {
		write_user(user,"You still need to set the following items:\n");
		write_user(user,"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
     		if (!user->accreq) write_user(user,"Use:  .accreq <your valid email address>\n"); 
 		if (!strcmp(user->desc," ~OL-> ~FYNew Around Here")) write_user(user,"Use:  .desc <a short description which goes beside your name>\n");
     		if (user->a4==0) write_user(user,"Use:  .set age <1 - 99> to set your age.\n");
     		if (!genderset) write_user(user,"Use:  .gender male or female  to set your gender.\n");
		}	
     }
}


/*** Clear the review buffer ***/
revclr(user)
UR_OBJECT user;
{
RM_OBJECT rm;
char *name;

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
if (word_count<2 || user->level<WIZ) rm=user->room;
else {
	if ((rm=get_room(word[1]))==NULL) {
		write_user(user,nosuchroom);  return;
		}
	}
if (rm->access==FIXED_PUBLIC && user->level<WIZ) {
	write_user(user,"~OL~FRYou are not powerfull enough to clear the review buffer here.\n");
	return;
	}
if (user->level>=CODER) {
	clear_revbuff(rm); 
	sprintf(text,"~OL~FGReview buffer cleared in the ~RS~OL%s~OL~FG room.\n",rm->name);
	write_user(user,text);
	return;
	}
clear_revbuff(user->room); 
write_user(user,"~OL~FGYou lick the review buffer clean.\n");
if (user->vis) name=user->name; else name=invisname;
sprintf(text,"~OL~FY%s licked the review buffer clean with their tonge...\n",name);
write_room_except(user->room,text,user);
}

/*** Clear the review buffer ***/
wizrevclr(user)
UR_OBJECT user;
{

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
clear_wizrevbuff(); 
write_user(user,"~OL~FGYou lick the wiz review buffer clean.\n");
}

/*** Clear the review buffer ***/
shoutrevclr(user)
UR_OBJECT user;
{

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
clear_shoutrevbuff(); 
write_user(user,"~OL~FGYou lick the shout review buffer clean.\n");
}

/*** See review of conversation ***/
review(user)
UR_OBJECT user;
{
RM_OBJECT rm=user->room;
int i,line,cnt;

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
if (user->muzzled & FROZEN) {
     write_user(user,"~OL~FYI see nothing here worth showing you...\n");
	return;
	}
if (word_count<2 || user->level<WIZ) rm=user->room;
else {
	if ((rm=get_room(word[1]))==NULL) {
		write_user(user,nosuchroom);  return;
		}
	if (!has_room_access(user,rm)) {
		write_user(user,"~OL~FRThat room is currently private, you cannot review the conversation.\n");
		return;
		}
     if (rm->access & PERSONAL && rm!=user->room && user->level<CODER) {
          write_user(user,"~OL~FRThat room is set to ~FTpersonal~FR you cannot review the conversation.\n");
		return;
		}
	}
cnt=0;
for(i=0;i<REVIEW_LINES;++i) {
	line=(rm->revline+i)%REVIEW_LINES;
	if (rm->revbuff[line][0]) {
		cnt++;
		if (cnt==1) {
               sprintf(text,"\n~FY-~OL=~FR[ ~FTReview buffer for: ~FG%s ~FR]~FY=~RS~FY-\n\n",rm->name);
			write_user(user,text);
               write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
			}
		write_user(user,rm->revbuff[line]); 
		}
	}
if (!cnt) write_user(user,"~OL~FYI see nothing here worth showing you...\n");
else write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n\n");
}


/*** Show recorded tells and pemotes ***/
revtell(user)
UR_OBJECT user;
{
int i,cnt,line;

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
cnt=0;
for(i=0;i<REVTELL_LINES;++i) {
	line=(user->revline+i)%REVTELL_LINES;
	if (user->revbuff[line][0]) {
		cnt++;
          	if (cnt==1) write_user(user,"\n~FY-~OL=~FR[ ~FTReview Of Tells Sent To You ~FR]~FY=~RS~FY-\n");
          	if (cnt==1) write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
	  	write_user(user,user->revbuff[line]); 
	  	}
	}
if (!cnt) write_user(user,"~OL~FYYou Have No Tells To Review!\n");
else write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n\n");
}


/*** Show recorded wiztells and wemotes ***/
revwiztell(user)
UR_OBJECT user;
{
int i,cnt,line;

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
cnt=0;
for(i=0;i<REVIEW_LINES;++i) {
	line=(wrevline+i)%REVIEW_LINES;
	if (wizrevbuff[line][0]) {
		cnt++;
          if (cnt==1) write_user(user,"\n~FY-~OL=~FR[ ~FTReview Of Tells Sent To The Staff ~FR]~FY=~RS~FY-\n");
          if (cnt==1) write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
          write_user(user,wizrevbuff[line]);
		}
	}
if (!cnt) write_user(user,"~OL~FYWiztell buffer is empty.\n");
else write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n\n");
}

/*** Show recorded shouts and shout emotes ***/
revshout(user)
UR_OBJECT user;
{
int i,cnt,line;

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
cnt=0;
for(i=0;i<REVIEW_LINES;++i) {
	line=(srevline+i)%REVIEW_LINES;
	if (shoutrevbuff[line][0]) {
		cnt++;
          if (cnt==1) write_user(user,"\n~FY-~OL=~FR[ ~FTReview Of Shouts and Shout Emotes ~FR]~FY=~RS~FY-\n");
          if (cnt==1) write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
          write_user(user,shoutrevbuff[line]);
		}
	}
if (!cnt) write_user(user,"~OL~FYShout buffer is empty.\n");
else write_user(user,"~FY\n\n");
write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
}

/*** Return to home site ***/
home(user)
UR_OBJECT user;
{

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
if (user->room!=NULL) {
	write_user(user,"You are already on your home system.\n");
	return;
	}
write_user(user,"~FB~OLYou traverse cyberspace...\n");
sprintf(text,"REL %s\n",user->name);
write_sock(user->netlink->socket,text);
sprintf(text,"NETLINK: %s returned from %s.\n",user->name,user->netlink->service);
write_syslog(text,1);
user->room=user->netlink->connect_room;
user->netlink=NULL;
if (user->vis) {
	sprintf(text,"%s %s\n",user->name,user->in_phrase);
	write_room_except(user->room,text,user);
	}
else write_room_except(user->room,invisenter,user);
look(user);
}


/*** Set the character mode echo on or off. This is only for users logging in
     via a character mode client, those using a line mode client (eg unix
     telnet) will see no effect. ***/
toggle_charecho(user)
UR_OBJECT user;
{

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
if (user->muzzled & SCUM) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
if (!user->charmode_echo) {
	write_user(user,"Echoing for character mode clients ~FGON.\n");
	user->charmode_echo=1;
	}
else {
	write_user(user,"Echoing for character mode clients ~FROFF.\n");
	user->charmode_echo=0;
	}
if (user->room==NULL) prompt(user);
}


/*** Set user description ***/
set_desc(user,inpstr)
UR_OBJECT user;
char *inpstr;
{
FILE *fp;
int genderset;
char filename[80];

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
if (user->muzzled & SCUM) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
if (word_count<2) {
	sprintf(text,"Your current description is: %s\n",user->desc);
	write_user(user,text);
	return;
	}
if (strstr(word[1],"(CLONE)")) {
	write_user(user,"You cannot have that description.\n");  return;
	}
if (strlen(inpstr)>USER_DESC_LEN) {
     write_user(user,"~FRYour Description Is Too Long!\n~RS");  return;
	}
strcpy(user->desc,inpstr);
write_user(user,"~FGYour Description Has Been Set!  ~RS\n");

/* AUTO PROMOTE CHECK (C)1997 Michael Irving, All Rights Reserved */

genderset=0;
if (user->prompt & 4 && !genderset) genderset=1;
if (user->prompt & 8 && !genderset) genderset=1;

if (!user->level==NEW) return;
sprintf(filename,"%s/%s.P",USERFILES,user->name);
if(!(fp=fopen(filename,"r"))) {
     write_user(user,"~OL~FTNo Profile Has Been Set.  You MUST enter a profile before you can be promoted!\n");
     return;
     }
else {
     fclose(fp);
     if ((user->accreq) && (strcmp(user->desc," ~OL-> ~FYNew Around Here")) && (!user->a4==0) && genderset) {
          user->ignall=user->ignall_store;
          user->level=USER;
          sprintf(text,"\n~OL~FG%s has been auto-promoted to level: ~FWMORTAL!",user->name);
          write_room(NULL,text);
          write_room(NULL,"\n                                       ~FY,,,                                    \n");
          write_room(NULL,"                                      ~OL~FY(~FBo o~FY)                                   \n");
          write_room(NULL,"~RS----------------------------------~FYoOO~RS--~FT(~OL~FR_~RS~FT)~RS--~FYOOo~RS-------------------------------\n");
          sprintf(text,"   ~OL~FTEverybody Please Welcome ~FG%s~FT To The Talker!!!\n",user->name); 
          write_room(NULL,text);
          write_room(NULL,"~RS------------------------------------------------------------------------------\n\n");
          }
	else {
		write_user(user,"You still need to set the following items:\n");
		write_user(user,"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
     		if (!user->accreq) write_user(user,"Use:  .accreq <your valid email address>\n"); 
 		if (!strcmp(user->desc," ~OL-> ~FYNew Around Here")) write_user(user,"Use:  .desc <a short description which goes beside your name>\n");
     		if (user->a4==0) write_user(user,"Use:  .set age <1 - 99> to set your age.\n");
     		if (!genderset) write_user(user,"Use:  .gender male or female  to set your gender.\n");
		}	
     }

}


/*** Enter user profile ***/
enter_profile(user,done_editing)
UR_OBJECT user;
int done_editing;
{
/* RM_OBJECT rm; */
FILE *fp;
char *c,filename[80];
int genderset;

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
/* rm=user->room; */
if (!done_editing) {
     sprintf(text,"~OL~FT%s is entering a profile...\n",user->name);
	write_room_except(user->room,text,user);
     write_user(user,"\n   ~FG-~OL~FG=~FY]~FT You are writing a profile of yourself ~FY[~FG=~RS~FG-\n\n");
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
write_user(user,"~OL~FGProfile stored.\n");
sprintf(text,"~OL~FT%s has finnished entering a profile...\n",user->name);
write_room(user->room,text);

/* AUTO PROMOTE CHECK (C)1997 Michael Irving, All Rights Reserved */

genderset=0;
if (user->prompt & 4 && !genderset) genderset=1;
if (user->prompt & 8 && !genderset) genderset=1;

if (!user->level==NEW) return;
sprintf(filename,"%s/%s.P",USERFILES,user->name);
if(!(fp=fopen(filename,"r"))) {
     write_user(user,"~OL~FTNo Profile Has Been Set.  You MUST enter a profile before you can be promoted!\n");
     return;
     }
else {
     fclose(fp);
     if ((user->accreq) && (strcmp(user->desc," ~OL-> ~FYNew Around Here")) && (!user->a4==0) && genderset) {
          user->ignall=user->ignall_store;
          user->level=USER;
          sprintf(text,"\n~OL~FG%s has been auto-promoted to level: ~FWMORTAL!",user->name);
          write_room(NULL,text);
          write_room(NULL,"\n                                       ~FY,,,                                    \n");
          write_room(NULL,"                                      ~OL~FY(~FBo o~FY)                                   \n");
          write_room(NULL,"~RS----------------------------------~FYoOO~RS--~FT(~OL~FR_~RS~FT)~RS--~FYOOo~RS-------------------------------\n");
          sprintf(text,"   ~OL~FTEverybody Please Welcome ~FG%s~FT To The Talker!!!\n",user->name); 
          write_room(NULL,text);
          write_room(NULL,"~RS------------------------------------------------------------------------------\n\n");
          }
	else {
		write_user(user,"You still need to set the following items:\n");
		write_user(user,"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
     		if (!user->accreq) write_user(user,"Use:  .accreq <your valid email address>\n"); 
 		if (!strcmp(user->desc," ~OL-> ~FYNew Around Here")) write_user(user,"Use:  .desc <a short description which goes beside your name>\n");
     		if (user->a4==0) write_user(user,"Use:  .set age <1 - 99> to set your age.\n");
     		if (!genderset) write_user(user,"Use:  .gender male or female  to set your gender.\n");
		}	
     }
}

gender(user)
UR_OBJECT user;
{
FILE *fp;
int genderset;
char filename[80];

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
if (user->muzzled & SCUM) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
if (word_count<2) {
	sprintf(text,"Useage: gender <male/female>\n");
	write_user(user,text);
	return;
	}
if (!strcmp(word[1],"male") && (user->prompt & FEMALE)) {
	write_user(user,"Have You had a sex change operation lately?\n");
	return;
	}
if (!strcmp(word[1],"female") && (user->prompt & MALE)) {
	write_user(user,"Have You had a sex change operation lately?\n");
	return;
	}
if (!strcmp(word[1],"male") && (user->prompt & MALE)) {
	write_user(user,"Yes we already know you are a male!\n");
	return;
	}
if (!strcmp(word[1],"female") && (user->prompt & FEMALE)) {
	write_user(user,"Yes We already know you are a female\n");
	return;
	}
if (!strcmp(word[1],"male")) {
	user->prompt+=MALE;
	write_user(user,"Ok! You are a male.\n");

/* AUTO PROMOTE CHECK (C)1997 Michael Irving, All Rights Reserved */

genderset=0;
if (user->prompt & 4 && !genderset) genderset=1;
if (user->prompt & 8 && !genderset) genderset=1;

if (!user->level==NEW) return;
sprintf(filename,"%s/%s.P",USERFILES,user->name);
if(!(fp=fopen(filename,"r"))) {
     write_user(user,"~OL~FTNo Profile Has Been Set.  You MUST enter a profile before you can be promoted!\n");
     return;
     }
else {
     fclose(fp);
     if ((user->accreq) && (strcmp(user->desc," ~OL-> ~FYNew Around Here")) && (!user->a4==0) && genderset) {
          user->ignall=user->ignall_store;
          user->level=USER;
          sprintf(text,"\n~OL~FG%s has been auto-promoted to level: ~FWMORTAL!",user->name);
          write_room(NULL,text);
          write_room(NULL,"\n                                       ~FY,,,                                    \n");
          write_room(NULL,"                                      ~OL~FY(~FBo o~FY)                                   \n");
          write_room(NULL,"~RS----------------------------------~FYoOO~RS--~FT(~OL~FR_~RS~FT)~RS--~FYOOo~RS-------------------------------\n");
          sprintf(text,"   ~OL~FTEverybody Please Welcome ~FG%s~FT To The Talker!!!\n",user->name); 
          write_room(NULL,text);
          write_room(NULL,"~RS------------------------------------------------------------------------------\n\n");
          }
	else {
		write_user(user,"You still need to set the following items:\n");
		write_user(user,"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
     		if (!user->accreq) write_user(user,"Use:  .accreq <your valid email address>\n"); 
 		if (!strcmp(user->desc," ~OL-> ~FYNew Around Here")) write_user(user,"Use:  .desc <a short description which goes beside your name>\n");
     		if (user->a4==0) write_user(user,"Use:  .set age <1 - 99> to set your age.\n");
     		if (!genderset) write_user(user,"Use:  .gender male or female  to set your gender.\n");
		}	
     }

	return;
	}	

if (!strcmp(word[1],"female")) {
	user->prompt+=FEMALE;
	write_user(user,"Ok! You are a female.\n");

/* AUTO PROMOTE CHECK (C)1997 Michael Irving, All Rights Reserved */

genderset=0;
if (user->prompt & 4 && !genderset) genderset=1;
if (user->prompt & 8 && !genderset) genderset=1;

if (!user->level==NEW) return;
sprintf(filename,"%s/%s.P",USERFILES,user->name);
if(!(fp=fopen(filename,"r"))) {
     write_user(user,"~OL~FTNo Profile Has Been Set.  You MUST enter a profile before you can be promoted!\n");
     return;
     }
else {
     fclose(fp);
     if ((user->accreq) && (strcmp(user->desc," ~OL-> ~FYNew Around Here")) && (!user->a4==0) && genderset) {
          user->ignall=user->ignall_store;
          user->level=USER;
          sprintf(text,"\n~OL~FG%s has been auto-promoted to level: ~FWMORTAL!",user->name);
          write_room(NULL,text);   
          write_room(NULL,"\n                                       ~FY,,,                                    \n");
          write_room(NULL,"                                      ~OL~FY(~FBo o~FY)                                   \n");
          write_room(NULL,"~RS----------------------------------~FYoOO~RS--~FT(~OL~FR_~RS~FT)~RS--~FYOOo~RS-------------------------------\n");
          sprintf(text,"   ~OL~FTEverybody Please Welcome ~FG%s~FT To The Talker!!!\n",user->name); 
          write_room(NULL,text);
          write_room(NULL,"~RS------------------------------------------------------------------------------\n\n");
          }
	else {
		write_user(user,"You still need to set the following items:\n");
		write_user(user,"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
     		if (!user->accreq) write_user(user,"Use:  .accreq <your valid email address>\n"); 
 		if (!strcmp(user->desc," ~OL-> ~FYNew Around Here")) write_user(user,"Use:  .desc <a short description which goes beside your name>\n");
     		if (user->a4==0) write_user(user,"Use:  .set age <1 - 99> to set your age.\n");
     		if (!genderset) write_user(user,"Use:  .gender male or female  to set your gender.\n");
		}	
        }
	return;
	}
sprintf(text,"Usage:  .gender male / female\n");
write_user(user,text);
}


/*** Shows user the main map or one of 4 others ***/
map(user)
UR_OBJECT user;
{
char filename[80];

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
if (word_count<2) {
	sprintf(filename,"%s/%s",DATAFILES,MAPFILE);
	switch(more(user,user->socket,filename)) {
		case 0:
		write_user(user,"There is no map.\n\n");
		return;
		case 1: user->misc_op=2;
		}
	return;
	}
if (strstr(word[1],"hi")) {
	sprintf(filename,"%s/%s",DATAFILES,MAPAFILE);
	switch(more(user,user->socket,filename)) {
		case 0:
          write_user(user,"There is no hidden rooms map.\n\n");
		return;
		case 1: user->misc_op=2;
		}
	return;
	}
if (strstr(word[1],"so")) {
	sprintf(filename,"%s/%s",DATAFILES,MAPBFILE);
	switch(more(user,user->socket,filename)) {
		case 0:
		write_user(user,"There is no map south.\n\n");
		return;
		case 1: user->misc_op=2;
		}
	return;
	}
if (strstr(word[1],"ea")) {
	sprintf(filename,"%s/%s",DATAFILES,MAPCFILE);
	switch(more(user,user->socket,filename)) {
		case 0:
		write_user(user,"There is no map east.\n\n");
		return;
		case 1: user->misc_op=2;
		}
	return;
	}
if (strstr(word[1],"we")) {
	sprintf(filename,"%s/%s",DATAFILES,MAPDFILE);
	switch(more(user,user->socket,filename)) {
		case 0:
		write_user(user,"There is no map west.\n\n");
		return;
		case 1: user->misc_op=2;
		}
	return;
	}
if (strstr(word[1],"ce") && user->level>=WIZ) {
	sprintf(filename,"%s/%s",DATAFILES,MAPEFILE);
	switch(more(user,user->socket,filename)) {
		case 0:
		write_user(user,"There is no map for the center.\n\n");
		return;
		case 1: user->misc_op=2;
		}
	return;
	}
if (strstr(word[1],"st") && user->level>=WIZ) {
	sprintf(filename,"%s/%s",DATAFILES,MAPFFILE);
	switch(more(user,user->socket,filename)) {
		case 0:
		write_user(user,"There is no map for the staff.\n\n");
		return;
		case 1: user->misc_op=2;
		}
	return;
	}
write_user(user,"Usage:  .map  for the general map or:  map [area]\n"); 
}




/*** Change room description ***/
room_desc(user,done_editing)
UR_OBJECT user;
int done_editing;
{
RM_OBJECT rm;
FILE *fp;
int i;
char *c,filename[80],d;

rm=user->room;
if (!done_editing) {
     sprintf(text,"~OL~FT%s is entering a description for this room...\n",user->name);
     write_room_except(user->room,text,user);     
     write_user(user,"\n   ~FG-~OL=~FY] ~FWEntering A Room Description ~FY[~FG=~RS~FG-\n\n");
	user->misc_op=8;
	editor(user,NULL);
	return;
	}
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
sprintf(text,"~OL~FT%s has finnished entering a description for this room...\n",user->name);
write_room_except(user->room,text,user);
write_user(user,"Room description stored. Reloading room description\n");

/* Load room description 

sprintf(filename,"%s/%s.R",DATAFILES,rm->name);
	if (!(fp=fopen(filename,"r"))) {
		fprintf(stderr,"NUTS: Can't open description file for room %s.\n",rm->name);
		sprintf(text,"ERROR: Couldn't open description file for room %s.\n",rm->name);
		write_syslog(text,0);
		}
	i=0;
	d=getc(fp);
	while(!feof(fp)) {
		if (i==ROOM_DESC_LEN) {
			fprintf(stderr,"NUTS: Description too long for room %s.\n",rm->name);
			sprintf(text,"ERROR: Description too long for room %s.\n",rm->name);
			write_syslog(text,0);
			break;
			}
		rm->desc[i]=d;  
		d=getc(fp);  ++i;
		}
	rm->desc[i]='\0';
	fclose(fp);
*/
look(user);
}


/*** Reloads room description for room the user is in ***/
reloadroom(user)
UR_OBJECT user;
{
RM_OBJECT rm;
/* FILE *fp;
int i;
char *c,filename[80],d; */

rm=user->room;
write_user(user,"There is no need to call this function anymore!\n");

/* Load room description 
sprintf(filename,"%s/%s.R",DATAFILES,rm->name);
	if (!(fp=fopen(filename,"r"))) {
		fprintf(stderr,"NUTS: Can't open description file for room %s.\n",rm->name);
		sprintf(text,"ERROR: Couldn't open description file for room %s.\n",rm->name);
		write_syslog(text,0);
		}
	i=0;
	d=getc(fp);
	while(!feof(fp)) {
		if (i==ROOM_DESC_LEN) {
			fprintf(stderr,"NUTS: Description too long for room %s.\n",rm->name);
			sprintf(text,"ERROR: Description too long for room %s.\n",rm->name);
			write_syslog(text,0);
			break;
			}
		rm->desc[i]=d;  
		d=getc(fp);  ++i;
		}
	rm->desc[i]='\0';
	fclose(fp);
look(user); */
}




/*** Set in and out phrases ***/
set_iophrase(user,inpstr)
UR_OBJECT user;
char *inpstr;
{

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
if (strlen(inpstr)>PHRASE_LEN) {
	write_user(user,"Phrase too long.\n");  return;
	}
if (com_num==INPHRASE) {
	if (word_count<2) {
		sprintf(text,"Your current in phrase is: %s\n",user->in_phrase);
		write_user(user,text);
		return;
		}
	strcpy(user->in_phrase,inpstr);
	write_user(user,"In phrase set.\n");
	return;
	}
if (word_count<2) {
	sprintf(text,"Your current out phrase is: %s\n",user->out_phrase);
	write_user(user,text);
	return;
	}
strcpy(user->out_phrase,inpstr);
write_user(user,"Out phrase set.\n");
}


/******************************** DISPLAY COMMANDS ************************/


/*** Show who is on ***/
who(user,people)
UR_OBJECT user;
int people;
{
UR_OBJECT u;
UR_OBJECT cu;
RM_OBJECT rm;

int cnt,total,invis,mins,remote,idle,logins;
char line[USER_NAME_LEN+USER_DESC_LEN*2];
char rname[ROOM_NAME_LEN+1],portstr[16],idlestr[6],sockstr[3],levelname[20];
char status[6], gender[7];

total=0;  invis=0;  remote=0;  logins=0;
if (user->login) sprintf(text,"\n                People On %s \n",long_date(1));
else sprintf(text,"\n   ~OL~FMPeople Currently Connected ~FB%s \n\n",long_date(1));
write_user(user,text);
if (!people) sprintf(text,"   Name and Description                    m/f Level Room       Mins Idle\n");
if (!people) write_user(user,text);
sprintf(text,"~OL~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
write_user(user,text);
if (people) write_user(user,"~FY~OLName            ~FW:~FY  Level Line Visi Idle Mins Port    Connected From\n\r");
if (people) write_user(user,"~OL~FT------------------------------------------------------------------------------\n");
for(u=user_first;u!=NULL;u=u->next) {
	if (u->type==CLONE_TYPE && people) continue; 
	if (u->type==CLONE_TYPE) cu=get_user(u->name);
	mins=(int)(time(0) - u->last_login)/60;
	idle=(int)(time(0) - u->last_input)/60;
	if (u->type==CLONE_TYPE) {
		mins=(int)(time(0) - cu->last_login)/60;
                idle=(int)(time(0) - cu->last_input)/60; 
      		}
          if (!u->level && u->muzzled & SCUM) {
		 strcpy(levelname,"SINNER");
		 }
          else {
               strcpy(levelname,level_name[u->level]);
               if (u->prompt & FEMALE) strcpy(levelname,level_name_fem[u->level]);
               }
if (people) {
     if (!u->level && u->muzzled & SCUM) strcpy(levelname,"SINNER");
     else {
          strcpy(levelname,level_name[u->level]);
          if (u->prompt & FEMALE) strcpy(levelname,level_name_fem[u->level]);
          }
        }
	if (u->type==CLONE_TYPE) {
          if (!cu->level && cu->muzzled & SCUM) {
		 strcpy(levelname,"SINNER");
		 }
          else {
               strcpy(levelname,level_name[cu->level]);
               if (cu->prompt & FEMALE) strcpy(levelname,level_name_fem[cu->level]);
               }
	}
     if (u->muzzled & JAILED) strcpy(levelname,"SINNER");

     /* Process User's Gender */
     strcpy(gender,"Neuter");
     if (u->prompt & FEMALE) strcpy(gender,"Female");
     if (u->prompt & MALE)   strcpy(gender,"Male  ");
     if (u->type==CLONE_TYPE) {
          strcpy(gender,"Neuter");
          if (cu->prompt & FEMALE) strcpy(gender,"Female");
          if (cu->prompt & MALE)   strcpy(gender,"Male  ");
          }

     if (u->type==REMOTE_TYPE) strcpy(portstr,"~FRLINK ");
	else {
        	if (u->port==port[0]) strcpy(portstr,"~FYMAIN ");
        	else strcpy(portstr,"~FGSTAFF");
	 }
	if (u->login) {
		if (!people) continue;
	        sprintf(text,"~FT[~OLLogin stage %d~RS~FT]~FY :   -     %2.2d     -  %4.4d    -  %s  %s\n",4 - u->login,u->socket,idle,portstr,u->site);
		write_user(user,text);
		logins++;
		continue;
		}
	++total;
	if (u->type==REMOTE_TYPE) ++remote;
	if (!u->vis) { 
		--total;
        	++invis;
		if (u->level>user->level) continue;  
		}
	if (people) {
		if (u->afk) strcpy(idlestr," AFK");
		else sprintf(idlestr,"%4.4d",idle);
		if (u->type==REMOTE_TYPE) strcpy(sockstr," -");
		else sprintf(sockstr,"%2.2d",u->socket);
          	sprintf(text,"~OL~FY%-15.15s ~FW:~FB  %6.6s %s    %s %s %4.4d  %s ~FY%s\n",u->name,levelname,sockstr,noyes1[u->vis],idlestr,mins,portstr,u->site);
    		write_user(user,text);
		continue;
		}
     sprintf(line,"  %s %s",u->name,u->desc);
     if (!u->vis) line[0]='>';
     if (u->type==REMOTE_TYPE) line[1]='@';
     if (u->room==NULL) sprintf(rname,"@%s",u->netlink->service);
     else strcpy(rname,u->room->name);
     rm=u->room;
     if (user->level<WIZ && rm->access & PERSONAL) sprintf(rname,"Private");
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
     	/*| 43 name/desc |G|1234567890|1234567890|0000|0000|S| */
     	sprintf(text,"~OL~FK|~RS%-*s ~RS~OL~FK|~RS%-1.1s~OL~FK|~RS%-6.6s~OL~FK|~RS%-10.10s~OL~FK|~RS%4.4d~OL~FK|~RS%4.4d~OL~FK|~RS%-1.1s~OL~FK|\n",40+cnt*3,line,gender,levelname,rname,mins,idle,status);
	write_user(user,text);
     }
write_user(user,"~OL~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
if (user->level>WIZ) {
    sprintf(text,"~FGThere are ~OL%d~RS~FG invisible users and ~OL%d~RS~FG remote users.\n",invis,remote);
    write_user(user,text);
    } 
sprintf(text,"~FGThere are ~OL%d~RS~FG users signed on",total);
if (people) sprintf(text,"%s and ~OL~FG%d ~RS~FGlogins.\n",text,logins);
else strcat(text,".\n");
write_user(user,text);
write_user(user,"~OL~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n\n");
}

/*** Show some user stats ***/
status(user)
UR_OBJECT user;
{
UR_OBJECT u;
char ir[ROOM_NAME_LEN+1],levelname[20],isbad[16],gendertype[16],waittell[4],ignoringstuff[4];
int days,hours,mins,hs;

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
if (user->muzzled & SCUM) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
if (word_count<2 || user->level<WIZ) {
	u=user;
     write_user(user,"\n~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
     write_user(user,"                    ~FY-~OL=~FR[ ~FTYour stats on The Talker ~FR]~FY=~RS~FY-\n\n");
     write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
	}
else {
	if (!(u=get_user(word[1]))) {
		write_user(user,notloggedon);  return;
		}
	if (u->level>user->level) {
		write_user(user,"You cannot stat a user of a higher level than yourself.\n");
		return;
		}
     sprintf(text,"\n~FY-~OL=~FR[ ~FT%s's ~FBstatus ~FR]~FY=~RS~FY-\n\n",u->name);
	write_user(user,text);
	}
strcpy(ignoringstuff,"NO ");
if (u->ignore)  strcpy(ignoringstuff,"YES");
strcpy(waittell,"NO ");
if (u->chkrev)  strcpy(waittell,"YES");
strcpy(isbad,"GOOD");
switch(u->level) {
	case  0: if (u->muzzled & SCUM) {
               strcpy(levelname,"~OL~FWCRIM  ");
               strcpy(isbad,"~FRBAD");
			}
               else strcpy(levelname,"~OL~FBNEW  ");  break;
     case  1: strcpy(levelname,"~OL~FTUSER  ");  break;
     case  2: strcpy(levelname,"~OL~FYREG   ");  break;
     case  3: strcpy(levelname,"~OL~FYWIZ   ");  break;
     case  4: strcpy(levelname,"~OL~FBWIZSUP");  break;
     case  5: strcpy(levelname,"~OL~FGOWNER   ");  break;
     case  6: strcpy(levelname,"~OL~FROWNER ");  break;

	}
if (u->muzzled & JAILED) {
     strcpy(levelname,"~FYJAILED");
     strcpy(isbad,"~FYJAILED");
	}
if (u->gaged) {           
     strcpy(isbad,"~FGGAGED");
	}
if (u->muzzled & 1 && u->gaged) {
     strcpy(isbad,"~FRMUZZLED & GAGED");
	}
strcpy(gendertype,"~OL~FTUNKNOWN");
if (u->prompt & 4) strcpy(gendertype,"~OL~FRFEMALE");
if (u->prompt & 8) strcpy(gendertype,"~OL~FBMALE");
if (u->invite_room==NULL) strcpy(ir,"<nowhere>");
else strcpy(ir,u->invite_room->name);
sprintf(text,"Level         : %s\nIgnoring all  : %s\n",levelname,noyes2[u->ignall]);
write_user(user,text);
sprintf(text,"Waiting tells : %s\nIgnoring      : %s\n",waittell,ignoringstuff);
write_user(user,text);
if (u->type==REMOTE_TYPE || u->room==NULL) hs=0; else hs=1;
sprintf(text,"Gender        : %s\n",gendertype);
write_user(user,text);
sprintf(text,"On home site  : %s\nVisible       : %s\n",noyes2[hs],noyes2[u->vis]);
write_user(user,text);
sprintf(text,"Status        : %s\nUnread mail   : %s\n",isbad,noyes2[has_unread_mail(u)]);
write_user(user,text);
sprintf(text,"Char echo     : %s\nColour        : %s\nInvited to    : %s\n",offon[u->charmode_echo],offon[u->colour],ir);
write_user(user,text);
sprintf(text,"Description   : %s\nIn phrase     : %s\nOut phrase    : %s\n",u->desc,u->in_phrase,u->out_phrase);
write_user(user,text);
mins=(int)(time(0) - u->last_login)/60;
sprintf(text,"Online for  : %d minutes\n",mins);
days=u->total_login/86400;
hours=(u->total_login%86400)/3600;
mins=(u->total_login%3600)/60;
sprintf(text,"Total login   : %d days, %d hours, %d minutes\n",days,hours,mins);
write_user(user,text);
write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
}


/*** Show talker system parameters etc ***/
system_details(user)
UR_OBJECT user;
{
NL_OBJECT nl;
RM_OBJECT rm;
UR_OBJECT u;
char bstr[40],minlogin[5];
char *ca[]={ "NONE  ","IGNORE","REBOOT" };
int days,hours,mins,secs;
int netlinks,live,inc,outg;
int rms,inlinks,num_clones,mem,size;

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}

write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
sprintf(text,"~FG   NUTS %s - System status\n",VERSION);
write_user(user,text);
write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");

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
inlinks=0;
size=sizeof(struct room_struct);
for(rm=room_first;rm!=NULL;rm=rm->next) {
	if (rm->inlink) ++inlinks;
	++rms;  mem+=size;
	}

netlinks=0;  
live=0;
inc=0; 
outg=0;
size=sizeof(struct netlink_struct);
for(nl=nl_first;nl!=NULL;nl=nl->next) {
	if (nl->type!=UNCONNECTED && nl->stage==UP) live++;
	if (nl->type==INCOMING) ++inc;
	if (nl->type==OUTGOING) ++outg;
	++netlinks;  mem+=size;
	}
if (minlogin_level==-1) strcpy(minlogin,"NONE");
else strcpy(minlogin,level_name[minlogin_level]);

/* Show header parameters */
write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
sprintf(text,"~FTProcess ID   : ~FG%d\n~FTTalker booted: ~FG%s~FTUptime       : ~FG%d days, %d hours, %d minutes, %d seconds\n",getpid(),bstr,days,hours,mins,secs);
write_user(user,text);
sprintf(text,"~FTPorts (M/W/L): ~FG%d,  %d,  %d\n",port[0],port[1],port[2]);
write_user(user,text);
write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
/* Show others */
sprintf(text,"Max users              : %4.4d         Current num. of users  : %4.4d\n",max_users,num_of_users);
write_user(user,text);
sprintf(text,"Max clones             : %4.4d         Current num. of clones : %4.4d\n",max_clones,num_clones);
write_user(user,text);
sprintf(text,"Current minlogin level : %-11.11s  Login idle time out    : %4.4d secs.\n",minlogin,login_idle_time);
write_user(user,text);
sprintf(text,"User idle time out     : %4.4d secs.   Heartbeat              : %4.4d\n",user_idle_time,heartbeat);
write_user(user,text);
sprintf(text,"Remote user maxlevel   : %-11.11s  Remote user deflevel   : %-13.13s\n",level_name[rem_user_maxlevel],level_name[rem_user_deflevel]);
write_user(user,text);
sprintf(text,"Wizport min login level: %-11.11s  Gatecrash level        : %-13.13s\n",level_name[wizport_level],level_name[gatecrash_level]);
write_user(user,text);
sprintf(text,"Time out maxlevel      : %-11.11s  Private room min count : %-4.4d\n",level_name[time_out_maxlevel],min_private_users);
write_user(user,text);
sprintf(text,"Message lifetime       : %2.2d days      Message check time     : %2.2d:%2.2d\n",mesg_life,mesg_check_hour,mesg_check_min);
write_user(user,text);
sprintf(text,"Net idle time out      : %-4.4d secs.   Number of rooms        : %-4.4d\n",net_idle_time,rms);
write_user(user,text);
sprintf(text,"Num. accepting connects: %-4.4d         Total netlinks         : %-4.4d\n",inlinks,netlinks);
write_user(user,text);
sprintf(text,"Number which are live  : %-4.4d         Number incoming        : %-4.4d\n",live,inc);
write_user(user,text);
sprintf(text,"Number outgoing        : %-4.4d         Ignoring sigterm       : %-13.13s\n",outg,noyes2[ignore_sigterm]);
write_user(user,text);
sprintf(text,"Echoing passwords      : %-11.11s  Swearing banned        : %-13.13s\n",noyes2[password_echo],noyes2[ban_swearing]);
write_user(user,text);
sprintf(text,"Time out afks          : %-11.11s  Allowing caps in name  : %-13.13s\n",noyes2[time_out_afks],noyes2[allow_caps_in_name]);
write_user(user,text);
sprintf(text,"New user prompt default: %-11.11s  New user color default : %-13.13s\n",offon[prompt_def],offon[colour_def]);
write_user(user,text);
sprintf(text,"New user charecho def. : %-11.11s  System logging         : %-13.13s\n",offon[charecho_def],offon[system_logging]);
write_user(user,text);
sprintf(text,"Crash action           : %-11.11s  Object memory allocated: %-4.4d Bytes\n",ca[crash_action],mem);
write_user(user,text);
write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
}


/*** Examine a user ***/
examine(user)
UR_OBJECT user;
{
UR_OBJECT u,u2;
FILE *fp;
char filename[80],line[129],ir[30],levelname[20],gendertype[16],genderx[4];
int new_mail,days,hours,mins,timelen,days2,hours2,mins2,idle;

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
if (word_count<2) {
	write_user(user,"Examine yourself?\n"); 
	strcpy(word[1],user->name); 
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
	u2=NULL;
	}
else u2=u;

sprintf(text,"\n~FG~OL.oO( ~RS%s %s ~RS~FG~OL)Oo.~RS\n",u->name,u->desc);
write_user(user,text);
write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
sprintf(filename,"%s/%s.P",USERFILES,u->name);
if (!(fp=fopen(filename,"r"))) write_user(user,"~OL~FYEeek!  ~FRNo Profile! :-( ~RS\n");
else {
	fgets(line,128,fp);
	while(!feof(fp)) {
		write_user(user,line);
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
switch(u->level) {
     case  0: if (u->muzzled & SCUM) strcpy(levelname,"~OL~FWCRIM");
         else strcpy(levelname,"~OL~FBNEW   ");  break;
     case  1: strcpy(levelname,"~OL~FTUSER  ");  break;
     case  2: strcpy(levelname,"~OL~FYREG   ");  break;
     case  3: strcpy(levelname,"~OL~FYWIZ   ");  break;
     case  4: strcpy(levelname,"~OL~FBWIZSUP");  break;
     case  5: strcpy(levelname,"~OL~FGOWNER   ");  break;
     case  6: strcpy(levelname,"~OL~FROWNER ");  break;
	}
if (u->muzzled & JAILED) strcpy(levelname,"JAILED ");
strcpy(gendertype,"~FRUNKNOWN");
if (u->prompt & 4) strcpy(gendertype,"~OL~FRFEMALE");
if (u->prompt & 8) strcpy(gendertype,"~OL~FBMALE");
if (u->invite_room==NULL) strcpy(ir,"<nowhere>");
write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
if (u2==NULL) {
     sprintf(text,"~OL~FGLevel      ~FW: %s\n~OL~FGLast login ~FW: %s",levelname,ctime((time_t *)&(u->last_login)));
	write_user(user,text);
     sprintf(text,"~OL~FGWhich was  ~FW: %d days, %d hours, %d minutes ago\n",days2,hours2,mins2);
	write_user(user,text);
     sprintf(text,"~OL~FGWas on for ~FW: %d hours, %d minutes\n~OL~FGTotal login~FW:~FY %d days, %d hours, %d minutes\n",u->last_login_len/3600,(u->last_login_len%3600)/60,days,hours,mins);
	write_user(user,text);
     sprintf(text,"~OL~FGGender     ~FW: %s\n",gendertype);
	write_user(user,text);
     if (user->level>=WIZ) {
          sprintf(text,"~OL~FGLast site  ~FW: %s\n",u->last_site);
		write_user(user,text);
		}
     strcpy(genderx,"its");
     if (u->prompt & 4) strcpy(genderx,"her");
     if (u->prompt & 8) strcpy(genderx,"his");
     if (new_mail>u->read_mail) {
          sprintf(text,"~OL~FGMail Status~FW: ~FY%s hasn't read %s new mail yet!\n",u->name,genderx);
		write_user(user,text);
		}
     sprintf(text,"~OL~FGEmail Addy ~FW: ~FT%s\n",u->c1);
     write_user(user,text);
     sprintf(text,"~OL~FGWebpage URL~FW: ~FT%s\n",u->c2);
     write_user(user,text);
     sprintf(text,"~OL~FGAge        ~FW: ~FT%d\n",u->a4);
     write_user(user,text);
     write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
	destruct_user(u);
	destructed=0;
     return;
	}
idle=(int)(time(0) - u->last_input)/60;
sprintf(text,"~OL~FGLevel       ~FW: %s\n~OL~FGIgnoring all~FW:~FT %s\n",levelname,noyes2[u->ignall]);
write_user(user,text);
sprintf(text,"~OL~FGOn since    ~FW:~FY %s~OL~FGOn for      ~FW:~FT %d hours, %d minutes\n",ctime((time_t *)&u->last_login),hours2,mins2);
write_user(user,text);
if (u->afk) {
     sprintf(text,"~OL~FGIdle for    ~FW:~FY %d minutes ~BR(AFK)\n",idle);
	write_user(user,text);
	if (u->afk_mesg[0]) {
          sprintf(text,"~OL~FGAFK message ~FW: ~FT%s\n",u->afk_mesg);
		write_user(user,text);
		}
	}
else {
     sprintf(text,"~OL~FGIdle for    ~FW: ~FY%d minutes\n",idle);
	write_user(user,text);
	}
sprintf(text,"~OL~FGTotal login ~FW: ~FY%d days, %d hours, %d minutes\n",days,hours,mins);
write_user(user,text);
sprintf(text,"~OL~FGGender      ~FW: %s\n",gendertype);
write_user(user,text);
if (u->socket==-1) {
     sprintf(text,"~OL~FBHome service~FW: ~FT%s\n",u->netlink->service);
	write_user(user,text);
	}
else {
     if (user->level>=WIZ) {
          sprintf(text,"~OL~FYSite        ~FW: ~FT%s:%s\n",u->site,u->site_port);
		write_user(user,text);
		}
	}
if (new_mail>u->read_mail) {
     sprintf(text,"~OL~FGMail Status ~FW: ~FY%s has unread mail.\n",u->name);
	write_user(user,text);
	}
     sprintf(text,"~OL~FGEmail Addy  ~FW: ~FT%s\n",u->c1);
     write_user(user,text);
     sprintf(text,"~OL~FGWebpage URL ~FW: ~FT%s\n",u->c2);
     write_user(user,text);
     sprintf(text,"~OL~FGAge         ~FW: ~FT%d\n",u->a4);
     write_user(user,text);
write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
write_user(user,"\n");
}


/****************************** HELP COMMANDS *****************************/


/*** Do the help ***/
help(user)
UR_OBJECT user;
{
int ret,len,i;
char filename[80];
char *c,*comword=NULL;

if (user->muzzled & SCUM) {
     write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
     write_user(user,"~FG                  Commands available for level ~OL~FTCRIM\n");
     write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
     write_user(user,"~FY[ ~OL~FTSCUM~RS~FY ]\n");
     write_user(user,"~RSquit      look      who        help       say        mode       rules\n\n");
     write_user(user,"~RSranks     time\n");
     write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
     write_user(user,"~FG For help on a command, type ~OL.help <command>~RS~FG.\n");
     write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
	return;
	}
if (user->muzzled & JAILED) {
     write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
     write_user(user,"~FG                  Commands available for level ~OL~FTJAILED\n");
     write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
     write_user(user,"~FY[ ~OL~FTJAILED~RS~FY ]\n");
     write_user(user,"~RSlook      who        help       say        mode       rules       ranks\n\n");
     write_user(user,"~RStime\n");
     write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
     write_user(user,"~FG For help on a command, type ~OL.help <command>~RS~FG.\n");
     write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
     return;
	}
if (word_count<2) {
/*	sprintf(filename,"%s/mainhelp",HELPFILES);
	if (!(ret=more(user,user->socket,filename))) {
		write_user(user,"There is no main help at the moment.\n");
		return;
		}
	if (ret==1) user->misc_op=2; */
	help_commands(user);
     return;
	}
if (!strcmp(word[1],"commands")) {  help_commands(user);  return;  }
if (!strcmp(word[1],"com")) {  help_commands(user);  return;  }
if (!strcmp(word[1],"gen")) {  help_general(user);  return;  }
if (!strcmp(word[1],"general")) {  help_general(user);  return;  }
if (!strcmp(word[1],"speech")) {  help_speech(user);  return;  }
if (!strcmp(word[1],"short")) {  help_short(user);  return;  }
if (!strcmp(word[1],"movement")) {  help_move(user);  return;  }
if (!strcmp(word[1],"mail")) {  help_mail(user);  return;  }
if (!strcmp(word[1],"misc")) {  help_misc(user);  return;  }
if (!strcmp(word[1],"link")) {  help_link(user);  return;  }
if (!strcmp(word[1],"credits")) {  help_credits(user);  return;  }
if (!strcmp(word[1],"nuts")) {  help_nuts(user);  return;  }


/* Check for any illegal crap in searched for filename so they cannot list 
   out the /etc/passwd file for instance. */
c=word[1];
while(*c) {
	if (*c=='.' || *c++=='/') {
		write_user(user,"Sorry, that command has not been invented yet.\n");
		sprintf(text,"%s tryed to to use %s to hack the system in the helpfiles.\n",user,word[1]);
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
if (user->room!=NULL && (com_num==-1 || com_level[com_num] > user->level)) {
	sprintf(text,"Sorry, the %s command has not been invented yet.\n",word[1]);
	write_user(user,text);
	return;
	}

sprintf(filename,"%s/%s",HELPFILES,word[1]);
if (!(ret=more(user,user->socket,filename)))
	sprintf(text,"Sorry, the %s command has not been invented yet.\n",word[1]);
	write_user(user,text);
if (ret==1) user->misc_op=2;
}


/*** Show the command available ***/
help_commands(user)
UR_OBJECT user;
{
int com,cnt,lev;
char temp[20];

write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
sprintf(text,"~FG                 Commands available for level ~OL~FT%s\n",level_name[user->level]);
write_user(user,text);
write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
for(lev=NEW;lev<=user->level;++lev) {
     sprintf(text,"~FY[ ~OL~FT%s~RS~FY ]~RS\n",level_name[lev]);
	write_user(user,text);
	com=0;  cnt=0;  text[0]='\0';
	while(command[com][0]!='*') {
		if (com_level[com]!=lev) {  com++;  continue;  }
		sprintf(temp,"%-10s ",command[com]);
		strcat(text,temp);
		if (cnt==6) {  
			strcat(text,"\n");  
			write_user(user,text);  
			text[0]='\0';  cnt=-1;  
			}
		com++; cnt++;
		}
	if (cnt) {
		strcat(text,"\n");  write_user(user,text);
		}
	}
write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
sprintf(text,"~OL~FY There are %d commands total.  You have %d commands available to you.\n",com,lev);
write_user(user,text);
write_user(user,"~FG For help on a command, type ~OL.help <command>~RS~FG.\n");
write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
}

help_general(user)
UR_OBJECT user;
{
int ret;
char filename[80];

sprintf(filename,"%s/generalhelp",HELPFILES);
if (!(ret=more(user,user->socket,filename)))
	write_user(user,"Sorry, that help file could not be found.\n");
if (ret==1) user->misc_op=2;
}


help_speech(user)
UR_OBJECT user;
{
int ret;
char filename[80];

sprintf(filename,"%s/speechhelp",HELPFILES);
if (!(ret=more(user,user->socket,filename)))
	write_user(user,"Sorry, that help file could not be found.\n");
if (ret==1) user->misc_op=2;
}


help_short(user)
UR_OBJECT user;
{
int ret;
char filename[80];

sprintf(filename,"%s/shorthelp",HELPFILES);
if (!(ret=more(user,user->socket,filename)))
	write_user(user,"Sorry, that help file could not be found.\n");
if (ret==1) user->misc_op=2;
}


help_move(user)
UR_OBJECT user;
{
int ret;
char filename[80];

sprintf(filename,"%s/movehelp",HELPFILES);
if (!(ret=more(user,user->socket,filename)))
	write_user(user,"Sorry, that help file could not be found.\n");
if (ret==1) user->misc_op=2;
}


help_mail(user)
UR_OBJECT user;
{
int ret;
char filename[80];

sprintf(filename,"%s/mailhelp",HELPFILES);
if (!(ret=more(user,user->socket,filename)))
	write_user(user,"Sorry, that help file could not be found.\n");
if (ret==1) user->misc_op=2;
}


help_misc(user)
UR_OBJECT user;
{
int ret;
char filename[80];

sprintf(filename,"%s/mischelp",HELPFILES);
if (!(ret=more(user,user->socket,filename)))
	write_user(user,"Sorry, that help file could not be found.\n");
if (ret==1) user->misc_op=2;
}

help_link(user)
UR_OBJECT user;
{
int ret;
char filename[80];

sprintf(filename,"%s/linkhelp",HELPFILES);
if (!(ret=more(user,user->socket,filename)))
	write_user(user,"Sorry, that help file could not be found.\n");
if (ret==1) user->misc_op=2;
}


help_credits(user)
UR_OBJECT user;
{
int ret;
char filename[80];

sprintf(filename,"%s/credithelp",HELPFILES);
if (!(ret=more(user,user->socket,filename)))
	write_user(user,"Sorry, that help file could not be found.\n");
if (ret==1) user->misc_op=2;
}


/*** Show info on the NUTS code ***/
help_nuts(user)
UR_OBJECT user;
{
write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
sprintf(text,"~BR~OL~FYMoenuts version %s ~RS\n",VERSION);
write_user(user,text);
write_user(user,"~FGCopyright (C)1996 Neil Robertson, (C)1997 Reddawg, (C)1997 Mike Irving (Moe)\n");
write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
write_user(user,"NUTS stands for Neils Unix Talk Server, a program which started out as a\nuniversity project in autumn 1992 and has progressed from thereon. In no\nparticular order thanks go to the following people who helped me develop or\n");
write_user(user,"debug this code in one way or another over the years:\n   ~FTDarren Seryck, Steve Guest, Dave Temple, Satish Bedi, Tim Bernhardt,\n   ~FTKien Tran, Jesse Walton, Pak Chan, Scott MacKenzie and Bryan McPhail.\n"); 
write_user(user,"Also thanks must go to anyone else who has emailed me with ideas and/or bug\nreports and all the people who have used NUTS over the intervening years.\n");
write_user(user,"I know I've said this before but this time I really mean it - this is the final\nversion of NUTS 3. In a few years NUTS 4 may spring forth but in the meantime\nthat, as they say, is that. :)\n");
write_user(user,"If you wish to email me my address is '~FGneil@ogham.demon.co.uk~RS' and should\nremain so for the forseeable future.\nNeil Robertson - November 1996.\n");
write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
write_user(user,"Moenuts is customized NUTS 3.3.3.  It was customized by Mike Irving.  I would\n");
write_user(user,"like to send out SPECIAL thanks to Reddawg and Nerfingdummy and Andy for\n");
write_user(user,"helping me with many additions to this code which by myself wouldn't be\n");
write_user(user,"possible.\n");
write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
write_user(user,"~FGThis version by Moe, Portions by Reddawg.  Thanks Reddawg for all your help!\n");
write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n\n");
}

/**************************** MESSAGE BOARD COMMANDS **********************/


/*** Read the message board ***/
read_board(user)
UR_OBJECT user;
{
RM_OBJECT rm;
char filename[80],*name;
int ret;

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
if (user->muzzled & SCUM) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
if (word_count<2) rm=user->room;
else {
	if ((rm=get_room(word[1]))==NULL) {
		write_user(user,nosuchroom);  return;
		}
	if (!has_room_access(user,rm)) {
		write_user(user,"That room is currently private, you cannot read the board.\n");
		return;
		}
	}	
write_user(user,"\n~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
sprintf(text,"             ~FY-~OL=~FR[ ~OL~FTThe ~FG%s~FT message board ~FR]~FY=~RS~FY-\n",rm->name);
write_user(user,text);
write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
sprintf(filename,"%s/%s.B",DATAFILES,rm->name);
if (!(ret=more(user,user->socket,filename))) 
     write_user(user,"~OL~FGUmm... No messages on the board!  Write Some!! =:-)\n\n");
else if (ret==1) user->misc_op=2;
if (user->vis) name=user->name; else name=invisname;
if (rm==user->room) {
     sprintf(text,"~FY~OL%s~RS~FT looks up and decides to read the message board.  ~RS\n",name);
	write_room_except(user->room,text,user);
	}
}


/*** Write on the message board ***/
write_board(user,inpstr,done_editing)
UR_OBJECT user;
char *inpstr;
int done_editing;
{
RM_OBJECT rm;
FILE *fp;
int cnt,inp,newmsg;
char *ptr,*name,filename[80];

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
if (user->muzzled & 1) {
	write_user(user,"You are muzzled, you cannot write on the board.\n");  
	return;
	}
if (!done_editing) {
	if (word_count<2) {
		if (user->type==REMOTE_TYPE) {
			/* Editor won't work over netlink cos all the prompts will go
			   wrong, I'll address this in a later version. */
			write_user(user,"Sorry, due to software limitations remote users cannot use the line editor.\nUse the '.write <mesg>' method instead.\n");
			return;
			}
          write_user(user,"\n  ~FY-~OL=~FR[ ~FGWriting Babble On The Board! ~FR]~FY=~RS~FY-\n\n");
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
     write_user(user,"~OL~FYYou scribble something on the board...\n");
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
if (user->vis) name=user->name; else name=invisname;
/* The posting time (PT) is the time its written in machine readable form, this 
   makes it easy for this program to check the age of each message and delete 
   as appropriate in check_messages() */
check_messages(user,3);
newmsg=user->room->mesg_cnt+1;
if (user->type==REMOTE_TYPE)
     sprintf(text,"PT: %d %d\r~FY~OLFrom~FW : ~FT%s@%s\n~OL~FYWrote~FW:~FT %s\n",(int)(time(0)),newmsg,name,user->netlink->service,long_date(0));
else sprintf(text,"PT: %d %d\r~FY~OLFrom~FW : ~FT%s\n~OL~FYWrote~FW:~FT %s\n",(int)(time(0)),newmsg,name,long_date(0));
fputs(text,fp);
fputs("~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n",fp);
cnt=0;
while(*ptr!='\0') {
	putc(*ptr,fp);
	if (*ptr=='\n') cnt=0; else ++cnt;
     if (cnt==77) { putc('\n',fp); cnt=0; }
	++ptr;
	}
if (inp) fputs("\n~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n\n",fp);
else fputs("~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n\n",fp);
/* if (inp) fputs("\n\n",fp); else putc('\n',fp); */
fclose(fp);
write_user(user,"~OL~FYYou write some scribble on the board.\n");
sprintf(text,"~OL~FY%s writes some scribble on the board.\n",name);
write_room_except(user->room,text,user);
user->room->mesg_cnt++;
}



/*** Wipe some messages off the board ***/
wipe_board(user)
UR_OBJECT user;
{
int cnt,cnt_um,valid,just_one,range,total,done;
int num,num_one,num_two,num_three,num_four,num_five,num_six;
char infile[80],line[82],id[82],hell[8],*name;
FILE *infp,*outfp;
RM_OBJECT rm;

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n"); 
	return;
	}
just_one=1;
total=1;
strcpy(hell,"Hot");
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
if (user->vis) name=user->name; else name=invisname;
sprintf(infile,"%s/%s.B",DATAFILES,rm->name);
if (!(infp=fopen(infile,"r"))) {
	write_user(user,"The message board is empty.\n");
	return;
	}
if (!strcmp(word[1],"all")) {
	fclose(infp);
	unlink(infile);
	write_user(user,"~OL~FRAll messages deleted.\n");
     sprintf(text,"~OL~FG%s ~RS~FYwipes some babble from board.\n",name);
	write_room_except(rm,text,user);
     sprintf(text,"%s wiped all the babbling from the board in the %s.\n",user->name,rm->name);
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
		sprintf(text,"~OL~FRThere were only ~RS~OL%d ~FRmessages on the board, all now deleted.\n",cnt);
		write_user(user,text);
		sprintf(text,"~OL%s ~FYwipes the message board.\n",name);
		write_room_except(rm,text,user);
		sprintf(text,"%s wiped all messages from the board in the %s.\n",user->name,rm->name);
		write_syslog(text,1);
		rm->mesg_cnt=0;
		return;
		}
	if (cnt==num) {
		unlink("tempfile"); /* cos it'll be empty anyway */
		write_user(user,"~OL~FRAll messages deleted.\n");
		user->room->mesg_cnt=0;
		sprintf(text,"%s wiped all messages from the board in the %s.\n",user->name,rm->name);
		}
	else {
		rename("tempfile",infile);
		check_messages(user,1);
		sprintf(text,"~OL%d ~FYmessages deleted from the top.\n",num);
		write_user(user,text);
		user->room->mesg_cnt-=num;
		sprintf(text,"%s wiped %d messages from the board in the %s.\n",user->name,num,rm->name);
		}
	write_syslog(text,1);
	sprintf(text,"~OL%s ~FYwipes the message board.\n",name);
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
		sprintf(text,"~OL~FRThere were only %d messages on the board, all now deleted.\n",total);
		write_user(user,text);
		sprintf(text,"~OL%s ~FYwipes the message board.\n",name);
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
		write_user(user,"~OL~FRAll messages deleted.\n");
		user->room->mesg_cnt=0;
		sprintf(text,"%s wiped all messages from the board in the %s.\n",user->name,rm->name);
		}
	else {
		rename("tempfile",infile);
		check_messages(user,1);
		sprintf(text,"~OL%d ~FYmessages deleted from the bottom.\n",total);
		write_user(user,text);
		user->room->mesg_cnt-total;
		sprintf(text,"%s wiped %d messages from the board in the %s.\n",user->name,total,rm->name);
		}
	write_syslog(text,1);
	sprintf(text,"~OL%s ~FYwipes the message board.\n",name);
	write_room_except(rm,text,user);
	return;
	}
if (just_one) {
	cnt=0; 
	valid=1;
	if (cnt_um==1) {
		unlink(infile); /* cos it'll be empty anyway */
          write_user(user,"~OL~FRAll Messages Deleted.\n");
		user->room->mesg_cnt=0;
		sprintf(text,"%s wiped all messages from the board in the %s.\n",user->name,rm->name);
		write_syslog(text,1);
		return;
		}
	if (num>cnt_um) {
		sprintf(text,"~OL~FRThere were only %d messages on the board. There is not a # %d message to delete.\n",cnt_um,num);
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
	sprintf(text,"~OL1 ~FYmessage deleted from board.\n");
	write_user(user,text);
	sprintf(text,"%s wiped 1 message from the board in the %s.\n",user->name,rm->name);
	write_syslog(text,1);
	sprintf(text,"~OL%s ~FYwipes the message board.\n",name);
	write_room_except(rm,text,user);
	return;
	}
if (range) {
	cnt=0; 
	valid=1;
	if (cnt_um==total) {
		unlink(infile); /* cos it'll be empty anyway */
		write_user(user,"~OL~FRAll messages deleted.\n");
		user->room->mesg_cnt=0;
		sprintf(text,"%s wiped all messages from the board in the %s.\n",user->name,rm->name);
		write_syslog(text,1);
		return;
		}
	if (num_one>cnt_um) {
		sprintf(text,"~OL~FRThere were only %d messages on the board. There is not a # %d message to delete.\n",cnt_um,num_one);
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
	sprintf(text,"~OL%d ~FYmessage deleted from board.\n",total);
	write_user(user,text);
	sprintf(text,"%s wiped %d message from the board in the %s.\n",user->name,total,rm->name);
	write_syslog(text,1);
	sprintf(text,"~OL%s ~FYwipes the message board.\n",name);
	write_room_except(rm,text,user);
	return;
	}
if (!just_one || !range) {
	cnt=0; 
	valid=1;
	if (num>cnt_um) {
		sprintf(text,"~OL~FRThere were only %d messages on the board. There is not a # %d message to delete.\n",cnt_um,num);
		write_user(user,text);
		print_useage(user,1); 
		return;
		}
	if (num_one>cnt_um) {
		sprintf(text,"~OL~FRThere were only %d messages on the board. There is not a # %d message to delete.\n",cnt_um,num_one);
		write_user(user,text);
		print_useage(user,1); 
		return;
		}
	if (num_two>cnt_um ) {
		sprintf(text,"~OL~FRThere were only %d messages on the board. There is not a # %d message to delete.\n",cnt_um,num_two);
		write_user(user,text);
		print_useage(user,1); 
		return;
		}
	if (num_three>cnt_um) {
		sprintf(text,"~OL~FRThere were only %d messages on the board. There is not a # %d message to delete.\n",cnt_um,num_three);
		write_user(user,text);
		print_useage(user,1); 
		return;
		}
	if (num_four>cnt_um) {
		sprintf(text,"~OL~FRThere were only %d messages on the board. There is not a # %d message to delete.\n",cnt_um,num_four);
		write_user(user,text);
		print_useage(user,1); 
		return;
		}
	if (cnt_um==total) {
		unlink(infile); /* cos it'll be empty anyway */
		write_user(user,"~OL~FRAll messages deleted.\n");
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
	sprintf(text,"~OL%d ~FYmessage deleted from board.\n",total);
	write_user(user,text);
	sprintf(text,"%s wiped %d message from the board in the %s.\n",user->name,total,rm->name);
	write_syslog(text,1);
	sprintf(text,"~OL%s ~FBwipes some babble from the message board.\n",name);
	write_room_except(rm,text,user);
	return;
	}
write_user(user,"~OL~FRUnknown error in wipe\n");
print_useage(user,1); 
}

	

/*** Search all the boards for the words given in the list. Rooms fixed to
	private will be ignore if the users level is less than gatecrash_level ***/
search_boards(user)
UR_OBJECT user;
{
RM_OBJECT rm;
FILE *fp;
char filename[80],line[82],buff[(MAX_LINES+1)*82],w1[81];
int w,cnt,message,yes,room_given;

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
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
rmail(user)
UR_OBJECT user;
{
FILE *infp,*outfp;
int ret;
char c,filename[80],line[DNL+1];

sprintf(filename,"%s/%s.M",USERFILES,user->name);
if (!(infp=fopen(filename,"r"))) {
     write_user(user,"~FBYou have no mail.  ~RS\n");  return;
	}
/* Update last read / new mail received time at head of file */
if (outfp=fopen("tempfile","w")) {
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
write_user(user,"\n~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
write_user(user,"~FG                          Your Smail Box\n");
write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
ret=more(user,user->socket,filename);
if (ret==1) user->misc_op=2;
}



/*** Send mail message ***/
smail(user,inpstr,done_editing)
UR_OBJECT user;
char *inpstr;
int done_editing;
{
UR_OBJECT u;
FILE *fp;
int remote,has_account;
char *c,filename[80];

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
if (user->muzzled & 1) {
	write_user(user,"You are muzzled, you cannot mail anyone.\n");  return;
	}
if (done_editing) {
	send_mail(user,user->mail_to,user->malloc_start);
	user->mail_to[0]='\0';
	return;
	}
if (word_count<2) {
	write_user(user,"Smail who?\n");  return;
	}
if (user->muzzled & FROZEN) {
	word[1][0]=toupper(word[1][0]);
     sprintf(text,"~FY~OLYou send ~FY%s~FY some mail.\n",word[1]);
	write_user(user,text);
	return;
	}
/* See if its to another site */
remote=0;
has_account=0;
c=word[1];
while(*c) {
	if (*c=='@') {  
		if (c==word[1]) {
			write_user(user,"Users name missing before @ sign.\n");  
			return;
			}
		remote=1;  break;  
		}
	++c;
	}
word[1][0]=toupper(word[1][0]);
/* See if user exists */
if (!remote) {
	u=NULL;
	if (!(u=get_user(word[1]))) {
		sprintf(filename,"%s/%s.D",USERFILES,word[1]);
		if (!(fp=fopen(filename,"r"))) {
			write_user(user,nosuchuser);  return;
			}
		has_account=1;
		fclose(fp);
		}
/*	if (u==user) {
		write_user(user,"Trying to mail yourself is the fifth sign of madness.\n");
		return;
		} */
	if (u!=NULL) strcpy(word[1],u->name); 
	if (!has_account) {
		/* See if user has local account */
		sprintf(filename,"%s/%s.D",USERFILES,word[1]);
		if (!(fp=fopen(filename,"r"))) {
			sprintf(text,"%s is a remote user and does not have a local account.\n",u->name);
			write_user(user,text);  
			return;
			}
		fclose(fp);
		}
	}
if (word_count>2) {
	/* One line mail */
	strcat(inpstr,"\n"); 
	send_mail(user,word[1],remove_first(inpstr));
	return;
	}
if (user->type==REMOTE_TYPE) {
	write_user(user,"Sorry, due to software limitations remote users cannot use the line editor.\nUse the '.smail <user> <mesg>' method instead.\n");
	return;
	}
sprintf(text,"\n   ~FY-~OL=~FR[ ~FTWriting mail message to ~FG%s ~FR]~FY=~RS~FY-\n\n",word[1]);
write_user(user,text);
user->misc_op=4;
strcpy(user->mail_to,word[1]);
editor(user,NULL);
}


dmail(user)
UR_OBJECT user;
{
int cnt,cnt_um,valid,just_one,range,total,done;
int num,num_one,num_two,num_three,num_four,num_five,num_six;
char infile[80],line[82],id[82],hell[8],*name;
FILE *infp,*outfp;
RM_OBJECT rm;

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n"); 
	return;
	}
just_one=1;
total=1;
strcpy(hell,"Hot");
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
     write_user(user,"~FRYou have no mail to delete.  ~RS\n");  
	return;
	}
if (!strcmp(word[1],"all")) {
	fclose(infp);
	unlink(infile);
     write_user(user,"~OL~FRAll messages deleted.  ~RS\n");
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
               if (valid && (!strcmp(id,"~OLFrom:") || !strcmp(id,"From:")|| !strcmp(id,"~BMFrom:"))) {
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
		sprintf(text,"~OL~FRThere were only ~RS~OL%d ~FRmessages in your mailbox, all now deleted.\n",cnt);
		write_user(user,text);
		return;
		}
	if (cnt==num) {
		unlink("tempfile"); /* cos it'll be empty anyway */
		write_user(user,"~OL~FRAll messages deleted.\n");
		return;
		}
	rename("tempfile",infile);
	sprintf(text,"~OL%d ~FYmessages deleted from the top of your mailbox.\n",num);
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
     if (valid && (!strcmp(id,"~OLFrom:") || !strcmp(id,"From:")|| !strcmp(id,"~BMFrom:"))) {
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
		sprintf(text,"~OL~FRThere were only %d messages in your mailbox, all now deleted.\n",total);
		write_user(user,text);
		return;
		}
	fgets(line,DNL,infp);
	fputs(line,outfp);
	fgets(line,82,infp); /* max of 80+newline+terminator = 82 */
	while(!feof(infp)) {
		if (*line=='\n') valid=1;
		sscanf(line,"%s",id);
          if (valid && (!strcmp(id,"~OLFrom:") || !strcmp(id,"From:")|| !strcmp(id,"~BMFrom:"))) {
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
	sprintf(text,"~OL%d ~FYmessages deleted from the bottom of your mailbox.\n",total);
	write_user(user,text);
	return;
	}
if (just_one) {
	cnt=0; 
	valid=1;
	if (num>cnt_um) {
		sprintf(text,"~OL~FRThere were only %d messages in your mailbox. There is not a # %d message to delete.\n",cnt_um,num);
		write_user(user,text);
		print_useage(user,0); 
		return;
		}
	if (cnt_um==1) {
		unlink(infile); /* cos it'll be empty anyway */
		write_user(user,"~OL~FRAll messages deleted.\n");
		return;
		}
	fgets(line,DNL,infp);
	fputs(line,outfp);
	fgets(line,82,infp); /* max of 80+newline+terminator = 82 */
	while(!feof(infp)) {
		if (*line=='\n') valid=1;
		sscanf(line,"%s",id);
          if (valid && (!strcmp(id,"~OLFrom:") || !strcmp(id,"From:")|| !strcmp(id,"~BMFrom:"))) {
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
	sprintf(text,"~OL1 ~FYmessage deleted from your mailbox.\n");
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
		sprintf(text,"~OL~FRThere were only %d messages in your mailbox. There is not a # %d message to delete.\n",cnt_um,num_one);
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
          if (valid && (!strcmp(id,"~OLFrom:") || !strcmp(id,"From:")|| !strcmp(id,"~BMFrom:"))) {
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
	sprintf(text,"~OL%d ~FYmessage deleted from your mailbox.\n",total);
	write_user(user,text);
	return;
	}
if (!just_one || !range) {
	cnt=0; 
	valid=1;
	if (num>cnt_um) {
		sprintf(text,"~OL~FRThere were only %d messages in your mailbox. There is not a # %d message to delete.\n",cnt_um,num);
		write_user(user,text);
		print_useage(user,0); 
		return;
		}
	if (num_one>cnt_um) {
		sprintf(text,"~OL~FRThere were only %d messages in your mailbox. There is not a # %d message to delete.\n",cnt_um,num_one);
		write_user(user,text);
		print_useage(user,0); 
		return;
		}
	if (num_two>cnt_um ) {
		sprintf(text,"~OL~FRThere were only %d messages in your mailbox. There is not a # %d message to delete.\n",cnt_um,num_two);
		write_user(user,text);
		print_useage(user,0); 
		return;
		}
	if (num_three>cnt_um) {
		sprintf(text,"~OL~FRThere were only %d messages in your mailbox. There is not a # %d message to delete.\n",cnt_um,num_three);
		write_user(user,text);
		print_useage(user,0); 
		return;
		}
	if (num_four>cnt_um) {
		sprintf(text,"~OL~FRThere were only %d messages in your mailbox. There is not a # %d message to delete.\n",cnt_um,num_four);
		write_user(user,text);
		print_useage(user,0); 
		return;
		}
	if (cnt_um==total) {
		unlink(infile); /* cos it'll be empty anyway */
		write_user(user,"~OL~FRAll messages deleted.\n");
		return;
		}
	fgets(line,DNL,infp);
	fputs(line,outfp);
	fgets(line,82,infp); /* max of 80+newline+terminator = 82 */
	while(!feof(infp)) {
		if (*line=='\n') valid=1;
		sscanf(line,"%s",id);
          if (valid && (!strcmp(id,"~OLFrom:") || !strcmp(id,"From:")|| !strcmp(id,"~BMFrom:"))) {
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
	sprintf(text,"~OL%d ~FYmessage deleted from your mailbox.\n",total);
	write_user(user,text);
	return;
	}
write_user(user,"~OL~FRUnknown error in wipe\n");
print_useage(user,1); 
}




/*** Show list of people your mail is from without seeing the whole lot ***/
mail_from(user)
UR_OBJECT user;
{
FILE *fp;
int valid,cnt;
char id[ARR_SIZE],line[ARR_SIZE],filename[80];

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
sprintf(filename,"%s/%s.M",USERFILES,user->name);
if (!(fp=fopen(filename,"r"))) {
	write_user(user,"You have no mail.\n");  return;
	}
write_user(user,"\n~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
write_user(user,"~FG                  You have mail from the following people\n\n");
write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
valid=1;  cnt=0;
fgets(line,DNL,fp); 
fgets(line,ARR_SIZE-1,fp);
while(!feof(fp)) {
	if (*line=='\n') valid=1;
	sscanf(line,"%s",id);
     if (valid && (!strcmp(id,"~OLFrom:") || !strcmp(id,"From:")|| !strcmp(id,"~BMFrom:"))) {
		write_user(user,remove_first(line));  
		cnt++;  valid=0;
		}
	fgets(line,ARR_SIZE-1,fp);
	}
fclose(fp);
write_user(user,"\n~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
   sprintf(text,"~FGThere was a total of %d messages found in your mailbox.\n",cnt);
write_user(user,text);
write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n\n");
}


count_messages(user)
UR_OBJECT user;
{
FILE *infp,*outfp;
char id[82],filename[80],line[82],old_filename[80],name[80],dash[80];
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
     if (valid && (!strcmp(id,"~OLFrom:") || !strcmp(id,"From:")|| !strcmp(id,"~BMFrom:"))) {
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



/******************************* NET LINK COMMANDS ************************/


/*** List defined netlinks and their status ***/
netstat(user)
UR_OBJECT user;
{
NL_OBJECT nl;
UR_OBJECT u;
char *allow[]={ "  ?","ALL"," IN","OUT" };
char *type[]={ "  -"," IN","OUT" };
char portstr[6],stat[9],vers[8];
int iu,ou,a;

if (nl_first==NULL) {
	write_user(user,"No remote connections configured.\n");  return;
	}
write_user(user,"\n~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
write_user(user,"~FG                         NUTS Netlink Data And Status\n");
write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n\n");
write_user(user,"~FTService name    : Allow Type Status IU OU Version  Site\n\n"); 

for(nl=nl_first;nl!=NULL;nl=nl->next) {
	iu=0;  ou=0;
	if (nl->stage==UP) {
		for(u=user_first;u!=NULL;u=u->next) {
			if (u->netlink==nl) {
				if (u->type==REMOTE_TYPE)  ++iu;
				if (u->room==NULL) ++ou;
				}
			}
		}
	if (nl->port) sprintf(portstr,"%d",nl->port);  else portstr[0]='\0';
	if (nl->type==UNCONNECTED) {
		strcpy(stat,"~FRDOWN");  strcpy(vers,"-");
		}
	else {
		if (nl->stage==UP) strcpy(stat,"  ~FGUP");
		else strcpy(stat," ~FYVER");
		if (!nl->ver_major) strcpy(vers,"3.?.?"); /* Pre - 3.2 version */  
		else sprintf(vers,"%d.%d.%d",nl->ver_major,nl->ver_minor,nl->ver_patch);
		}
	/* If link is incoming and remoter vers < 3.2 we have no way of knowing 
	   what the permissions on it are so set to blank */
	if (!nl->ver_major && nl->type==INCOMING && nl->allow!=IN) a=0; 
	else a=nl->allow+1;
     sprintf(text,"%-15.15s :   %s  %s   %s~RS %2.2d %2.2d %7.7s  %s %s\n",nl->service,allow[a],type[nl->type],stat,iu,ou,vers,nl->site,portstr);
	write_user(user,text);
	}
write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n\n");     
}



/*** Show type of data being received down links (this is usefull when a
     link has hung) ***/
netdata(user)
UR_OBJECT user;
{
NL_OBJECT nl;
char from[80],name[USER_NAME_LEN+1];
int cnt;

cnt=0;
write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n\n");
write_user(user,"~FG                       NUTS Netlink Mail Receiving Status\n");
write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n\n");

for(nl=nl_first;nl!=NULL;nl=nl->next) {
	if (nl->type==UNCONNECTED || nl->mailfile==NULL) continue;
	if (++cnt==1) write_user(user,"To              : From                       Last recv.\n\n");
	sprintf(from,"%s@%s",nl->mail_from,nl->service);
     sprintf(text,"%-15.15s : %-25.25s  %2.2d seconds ago.\n",nl->mail_to,from,(int)(time(0)-nl->last_recvd));
	write_user(user,text);
	}
if (!cnt) write_user(user,"\n~FRNo mail being received.\n\n");
else write_user(user,"\n");

cnt=0;
write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
write_user(user,"~FG                      NUTS Netlink Message Receiving Status\n");
write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n\n");
for(nl=nl_first;nl!=NULL;nl=nl->next) {
	if (nl->type==UNCONNECTED || nl->mesg_user==NULL) continue;
	if (++cnt==1) write_user(user,"To              : From             Last recv.\n\n");
	if (nl->mesg_user==(UR_OBJECT)-1) strcpy(name,"<unknown>");
	else strcpy(name,nl->mesg_user->name);
	sprintf(text,"%-15s : %-15s  %d seconds ago.\n",name,nl->service,(time(0)-nl->last_recvd));
	write_user(user,text);
	}
if (!cnt) write_user(user,"\n~FRNo messages being received.\n\n");
else write_user(user,"\n");
write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n\n");
}


/*** Connect a netlink. Use the room as the key ***/
connect_netlink(user)
UR_OBJECT user;
{
RM_OBJECT rm;
NL_OBJECT nl;
int ret,tmperr;

if (word_count<2) {
	write_user(user,"Usage: connect <room service is linked to>\n");  return;
	}
if ((rm=get_room(word[1]))==NULL) {
	write_user(user,nosuchroom);  return;
	}
if ((nl=rm->netlink)==NULL) {
	write_user(user,"That room is not linked to a service.\n");
	return;
	}
if (nl->type!=UNCONNECTED) {
	write_user(user,"That rooms netlink is already up.\n");  return;
	}
write_user(user,"Attempting connect (this may cause a temporary hang)...\n");
sprintf(text,"NETLINK: Connection attempt to %s initiated by %s.\n",nl->service,user->name);
write_syslog(text,1);
errno=0;
if (!(ret=connect_to_site(nl))) {
	write_user(user,"~FGInitial connection made...\n");
	sprintf(text,"NETLINK: Connected to %s (%s %d).\n",nl->service,nl->site,nl->port);
	write_syslog(text,1);
	nl->connect_room=rm;
	return;
	}
tmperr=errno; /* On Linux errno seems to be reset between here and sprintf */
write_user(user,"~FRConnect failed: ");
write_syslog("NETLINK: Connection attempt failed: ",1);
if (ret==1) {
	sprintf(text,"%s.\n",sys_errlist[tmperr]);
	write_user(user,text);
	write_syslog(text,0);
	return;
	}
write_user(user,"Unknown hostname.\n");
write_syslog("Unknown hostname.\n",0);
}



/*** Disconnect a link ***/
disconnect_netlink(user)
UR_OBJECT user;
{
RM_OBJECT rm;
NL_OBJECT nl;

if (word_count<2) {
	write_user(user,"Usage: disconnect <room service is linked to>\n");  return;
	}
if ((rm=get_room(word[1]))==NULL) {
	write_user(user,nosuchroom);  return;
	}
nl=rm->netlink;
if (nl==NULL) {
	write_user(user,"That room is not linked to a service.\n");
	return;
	}
if (nl->type==UNCONNECTED) {
	write_user(user,"That rooms netlink is not connected.\n");  return;
	}
/* If link has hung at verification stage don't bother announcing it */
if (nl->stage==UP) {
	sprintf(text,"~OLSYSTEM:~RS Disconnecting from %s in the %s.\n",nl->service,rm->name);
	write_room(NULL,text);
	sprintf(text,"NETLINK: Link to %s in the %s disconnected by %s.\n",nl->service,rm->name,user->name);
	write_syslog(text,1);
	}
else {
	sprintf(text,"NETLINK: Link to %s disconnected by %s.\n",nl->service,user->name);
	write_syslog(text,1);
	}
shutdown_netlink(nl);
write_user(user,"Disconnected.\n");
}


/********************************** CLONE COMMANDS ************************/

/*** Clone a user in another room ***/
create_clone(user)
UR_OBJECT user;
{
UR_OBJECT u;
RM_OBJECT rm;
char *name;
int cnt;

/* Check room */
if (word_count<2) rm=user->room;
else {
	if ((rm=get_room(word[1]))==NULL) {
		write_user(user,nosuchroom);  return;
		}
	}	
/* If room is private then nocando */
if (!has_room_access(user,rm)) {
	write_user(user,"That room is currently private, you cannot create a clone there.\n");  
	return;
	}
/* If room is personal and you are not invited in there */
if (((rm->access==FIXED_PERSONAL && user->invite_room!=rm) && user->level<OWNER) && strcmp(user->name,rm->owner)) {
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

if (rm==user->room)
     write_user(user,"\n~FY~OLYou whisper a haunting spell and create a clone of yourself here.\n");
else {
     sprintf(text,"\n~FY~OLYou whisper a haunting spell create a clone of yourself in the %s.\n",rm->name);
	write_user(user,text);
	}

/* if (user->vis) name=user->name; else name=invisname;
sprintf(text,"~FB~OL%s whispers a haunting spell...\n",name);
write_room_except(user->room,text,user); */

sprintf(text,"%s %s \n",user->name,user->in_phrase);
write_room_except(rm,text,user);
}


/*** Destroy user clone ***/
destroy_clone(user)
UR_OBJECT user;
{
UR_OBJECT u,u2;
RM_OBJECT rm;
char *name;

/* Check room and user */
if (word_count<2) rm=user->room;
else {
	if ((rm=get_room(word[1]))==NULL) {
		write_user(user,nosuchroom);  return;
		}
	}
if (word_count>2) {
	if ((u2=get_user(word[2]))==NULL) {
		write_user(user,notloggedon);  return;
		}
	if (u2->level>=user->level) {
		write_user(user,"You cannot destroy the clone of a user of an equal or higher level.\n");
		return;
		}
	}
else u2=user;
for(u=user_first;u!=NULL;u=u->next) {
	if (u->type==CLONE_TYPE && u->room==rm && u->owner==u2) {
		destruct_user(u);
		reset_access(rm);
          write_user(user,"~FY~OLYou whisper a sharp spell and the clone is destroyed.\n");
		if (user->vis) name=user->name; else name=invisname;
                sprintf(text,"~OL~FT%s disappears to another reality...\n",u2->name);
		write_room(rm,text);
		if (u2!=user) {
               sprintf(text,"\n~OLSYSTEM: ~FY%s has destroyed your clone in the %s.\n",user->name,rm->name);
			write_user(u2,text);
			}
		destructed=0;
		return;
		}
	}
if (u2==user) sprintf(text,"You do not have a clone in the %s.\n",rm->name);
else sprintf(text,"%s does not have a clone the %s.\n",u2->name,rm->name);
write_user(user,text);
}


/*** Show users own clones ***/
myclones(user)
UR_OBJECT user;
{
UR_OBJECT u;
int cnt;

cnt=0;
for(u=user_first;u!=NULL;u=u->next) {
	if (u->type!=CLONE_TYPE || u->owner!=user) continue;
	if (++cnt==1)
     write_user(user,"\n~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
     write_user(user,"~FG                       Rooms You Have Clones In Currently\n");
     write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n\n");
     sprintf(text,"Room     : %s\n",u->room);
	write_user(user,text);
	}
if (!cnt) write_user(user,"You have no clones.\n");
else {
write_user(user,"\n~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
sprintf(text,"~FRYou have a total of %d clones.\n",cnt);
write_user(user,text);
write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n\n");
	}
}


/*** Show all clones on the system ***/
allclones(user)
UR_OBJECT user;
{
UR_OBJECT u;
int cnt;

cnt=0;
for(u=user_first;u!=NULL;u=u->next) {
	if (u->type!=CLONE_TYPE) continue;
	if (++cnt==1) {
          write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
          sprintf(text," ~OL~FTCurrent clones %s \n",long_date(1));
          write_user(user,text);
          write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
          }
     sprintf(text," %-15s : %s\n",u->name,u->room);
	write_user(user,text);
	}
     write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");     
if (!cnt) write_user(user," ~FRThere are no clones on the system.\n");
else {
     sprintf(text," ~FGTotal of %d clones.\n",cnt);
	write_user(user,text);
     }
write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
}


/*** User swaps places with his own clone. All we do is swap the rooms the
	objects are in. ***/
clone_switch(user)
UR_OBJECT user;
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
		write_user(user,"\n~FB~OLYou experience a strange sensation...\n");
		u->room=user->room;
		user->room=rm;
		/* Clone Prompts 
		sprintf(text,"%s shimmered momentarilly...\n",u->name);
		write_room_except(user->room,text,user); */
		/* Turns Into A Clone Prompt */
/*		sprintf(text,"%s shimmered momentarilly...\n",u->name);
		write_room_except(u->room,text,u); */
		look(user);
		return;
		}
	}
write_user(user,"You do not have a clone in that room.\n");
}


/*** Make a clone speak ***/
clone_say(user,inpstr)
UR_OBJECT user;
char *inpstr;
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


/*** Make a clone speak ***/
clone_emote(user,inpstr)
UR_OBJECT user;
char *inpstr;
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
	write_user(user,nosuchroom);  return;
	}
for(u=user_first;u!=NULL;u=u->next) {
	if (u->type==CLONE_TYPE && u->room==rm && u->owner==user) {
		emote(u,remove_first(inpstr));  return;
		}
	}
write_user(user,"You do not have a clone in that room.\n");
}


/*** Set what a clone will hear, either all Speech , just bad language
	or nothing. ***/
clone_hear(user)
UR_OBJECT user;
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
kill_user(user)
UR_OBJECT user;
{
UR_OBJECT victim;
RM_OBJECT rm;
char *name;

if (word_count<2) {
	write_user(user,"Usage: kill <user>\n");  return;
	}
if (!(victim=get_user(word[1]))) {
	write_user(user,notloggedon);  return;
	}
if (user==victim) {
	write_user(user,"Trying to commit suicide this way is the sixth sign of madness.\n");
	return;
	}
if (victim->level>=user->level) {
	write_user(user,"You cannot kill a user of equal or higher level than yourself.\n");
	sprintf(text,"%s tried to kill you!\n",user->name);
	write_user(victim,text);
	return;
	}
sprintf(text,"%s KILLED %s.\n",user->name,victim->name);
write_syslog(text,1);
sprintf(text,"%s KILLED %s.\n",user->name,victim->name);
write_arrestrecord(victim,text,1);
write_user(user,"~FY~OLYou chant an evil incantation...\n");
if (user->vis) name=user->name; else name=invisname;
sprintf(text,"~FY~OL%s chants an evil incantation...\n",name);
write_room_except(user->room,text,user);
write_user(victim,"~FY~OLA shrieking furie rises up out of the ground, and devours you!!!\n");
sprintf(text,"~FY~OLA shrieking furie rises up out of the ground, devours %s and vanishes!!!\n",victim->name);
rm=victim->room;
write_room_except(rm,text,victim);
disconnect_user(victim);
write_room(NULL,"~FY~OLYou hear insane laughter from the beyond the grave...\n");
}

/*** Removes a user silently ***/
remove_user(user)
UR_OBJECT user;
{
UR_OBJECT victim;
RM_OBJECT rm;
char text2[80],*name;

if (word_count<2) {
	write_user(user,"Usage: remove <user>\n");  return;
	}
if (!(victim=get_user(word[1]))) {
	write_user(user,notloggedon);  return;
	}
if (user==victim) {
	write_user(user,"Trying to commit suicide this way is the sixth sign of madness.\n");
	return;
	}
if (victim->level>=user->level) {
	write_user(user,"You cannot rmove a user of equal or higher level than yourself.\n");
	sprintf(text,"%s tried to remove you!\n",user->name);
	write_user(victim,text);
	return;
	}
sprintf(text,"%s REMOVED %s.\n",user->name,victim->name);
write_syslog(text,1);
sprintf(text,"%s REMOVED %s.\n",user->name,victim->name);
write_arrestrecord(victim,text,1);
if (user->vis) name=user->name; else name=invisname;
sprintf(text,"~FY~OL%s ~FRREMOVED ~FY%s!!!\n",user->name,victim->name);
write_level(WIZ,1,text,user);
/* sprintf(text2,"~FY~OL%s Has ~FRREMOVED~FY you from this talker! Were you IDLEING to long?\n",name); */
/* send_mail(user,word[1],text2); */
disconnect_user(victim);
}


/*** Promote a user ***/
promote(user)
UR_OBJECT user;
{
UR_OBJECT u;
RM_OBJECT rm;
char text2[80],*name;

if (word_count<2) {
	write_user(user,"Usage: promote <user>\n");  return;
	}

/* See if user is on atm */
if ((u=get_user(word[1]))!=NULL) {
	if (u->level==NEW && u->muzzled & SCUM) {
          sprintf(text,"~FG~OLYou promote %s to level: ~RS~OLNEW!~RS.\n",u->name);
		write_user(user,text);
		if (user->vis) name=user->name; else name=invisname;	
          sprintf(text,"~FG~OL%s has promotes you to level: ~RS~OLNEW!\n",name);
		write_user(u,text);
          sprintf(text,"%s PROMOTED %s to level NEW.\n",user->name,u->name);
		write_syslog(text,1);
          sprintf(text,"%s PROMOTED %s to level NEW.\n",user->name,u->name);
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
     if (u->level==REG && user->level==WIZ) {
		write_user(user,"You cannot promote a user to the same level as your own.\n");
		return;
		}
      if (u->level==USER && user->level==WIZ && u->total_login<(86400)) {
          write_user(user,"Users cannot be promoted to REG untill they have a total login time of one day.\n");
		return;
		}
     if (u->level==USER && user->level==CODER && u->total_login<(86400)) {
		write_user(user,"You cannot promote a USER till total login time is more than 1 day.\n");
		return;
          }
	if (user->vis) name=user->name; else name=invisname;
	u->level++;
	sprintf(text,"~FG~OLYou promote %s to level: ~RS~OL%s!\n",u->name,level_name[u->level]);
	write_user(user,text);
	rm=user->room;
	user->room=NULL;
	sprintf(text,"~FG~OL%s promotes %s to level: ~RS~OL%s!\n",name,u->name,level_name[u->level]);
	write_room_except(NULL,text,u);
	user->room=rm;
	sprintf(text,"~FG~OL%s has promoted you to level: ~RS~OL%s!\n",name,level_name[u->level]);
	write_user(u,text);
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
	sprintf(text,"~FG~OLYou promote %s to level: ~RS~OLNEW!~RS.\n",u->name);
	write_user(user,text);
	if (user->vis) name=user->name; else name=invisname;	
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
if (u->level==USER && user->level==WIZ && u->total_login<(86400)) {
	write_user(user,"You cannot promote a USER till total login time is more than 2 days.\n");
	destruct_user(u);
	destructed=0;
	return;
	}
if (u->level==USER && user->level==CODER && u->total_login<(86400)) {
	write_user(user,"You cannot promote a USER till total login time is more than 1 day.\n");
	destruct_user(u);
	destructed=0;
	return;
     } 
if (u->level==REG && user->level==WIZ) {
	write_user(user,"You cannot promote a user to the same level as your own.\n");
	destruct_user(u);
	destructed=0;	
	return;
	}
if (u->level==WIZ && user->level==CODER) {
	write_user(user,"You cannot promote a user to the same level as your own. If they are not loged in.\n");
	destruct_user(u);
	destructed=0;
	return;
	}
u->level++;  
u->socket=-2;
strcpy(u->site,u->last_site);
save_user_details(u,0);
sprintf(text,"~OL~FGYou promote %s to level: ~RS~OL%s.\n",u->name,level_name[u->level]);
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
demote(user)
UR_OBJECT user;
{
UR_OBJECT u;
RM_OBJECT rm;
char text2[80],*name;

if (word_count<2) {
	write_user(user,"Usage: demote <user>\n");  return;
	}
/* See if user is on atm */
if ((u=get_user(word[1]))!=NULL) {
	if (u->level==NEW && u->muzzled & SCUM) {
		write_user(user,"You can not demote a user to level lower than~OL~FR CRIM~RS.\n");
		return;
		}
	if (u->level==NEW) {
          sprintf(text,"~FY~OLYou demote %s to level: ~RS~OLCRIM~RS.\n",u->name);
		write_user(user,text);
		if (user->vis) name=user->name; else name=invisname;	
          sprintf(text,"~FY~OL%s has demoted you to level: ~RS~OLCRIM!\n",name);
		write_user(u,text);
		sprintf(text,"%s DEMOTED %s to level CRIM.\n",user->name,u->name);
		write_syslog(text,1);
		sprintf(text,"%s DEMOTED %s to level CRIM.\n",user->name,u->name);
		write_arrestrecord(u,text,1);
		u->muzzled+=SCUM;
		return;
		}
	if (u->level>=user->level) {
		write_user(user,"You cannot demote a user of an equal or higher level than yourself.\n");
		return;
		}
	if (user->vis) name=user->name; else name=invisname;
	u->level--;
     sprintf(text,"~FY~OLYou demote %s to level: ~RS~OL%s.\n",u->name,level_name[u->level]);
	write_user(user,text);
	rm=user->room;
	user->room=NULL;
     sprintf(text,"~FY~OL%s demotes %s to level: ~RS~OL%s.\n",name,u->name,level_name[u->level]);
	write_room_except(NULL,text,u);
	user->room=rm;
     sprintf(text,"~FY~OL%s has demoted you to level: ~RS~OL%s!\n",name,level_name[u->level]);
	write_user(u,text);
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
          sprintf(text,"~FY~OLYou demote %s to level: ~RS~OLCRIM~RS.\n",u->name);
		write_user(user,text);
		if (user->vis) name=user->name; else name=invisname;	
          sprintf(text2,"~FY~OL%s has demoted you to level: ~RS~OLCRIM!\n",name);
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
sprintf(text,"~OL~FRYou demote %s to level: ~RS~OL%s.\n",u->name,level_name[u->level]);
write_user(user,text);
sprintf(text2,"~FY~OLYou have been demoted to level: ~RS~OL%s.\n",level_name[u->level]);
send_mail(user,word[1],text2);
sprintf(text,"%s DEMOTED %s to level %s.\n",user->name,word[1],level_name[u->level]);
write_syslog(text,1);
sprintf(text,"%s DEMOTED %s to level %s.\n",user->name,word[1],level_name[u->level]);
write_arrestrecord(u,text,1);
destruct_user(u);
destructed=0;
}


/*** List banned sites or users ***/
listbans(user)
UR_OBJECT user;
{
int i;
char filename[80];

if (!strcmp(word[1],"sites")) {
     write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
     write_user(user,"~FG                      Listing of Banned Sites and Domains\n");
     write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
	sprintf(filename,"%s/%s",DATAFILES,SITEBAN);
	switch(more(user,user->socket,filename)) {
		case 0:
          write_user(user,"~FRThere are currently no banned sites or domains.\n");
          write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
		return;
		case 1: user->misc_op=2;
		}
          write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
	return;
	}
if (!strcmp(word[1],"users")) {
     write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
     write_user(user,"~FG                      Listing of Banned Users From The Talker\n");
     write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
	sprintf(filename,"%s/%s",DATAFILES,USERBAN);
	switch(more(user,user->socket,filename)) {
		case 0:
          write_user(user,"~FRThere are currently no banned users.\n\n");
          write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
		return;
		case 1: user->misc_op=2;
		}
          write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
	return;
	}
if (!strcmp(word[1],"swears")) {
     write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
     write_user(user,"~FG                 Listing of Banned Swear Words at The Talker\n");
     write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
     write_user(user,"\n~BM*** Banned swear words ***\n\n");
	i=0;
	while(swear_words[i][0]!='*') {
		write_user(user,swear_words[i]);
		write_user(user,"\n");
		++i;
		}
	if (!i) write_user(user,"There are no banned swear words.\n");
	if (ban_swearing) write_user(user,"\n");
	else write_user(user,"\n(Swearing ban is currently off)\n\n");
     write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
	return;
	}
if (!strcmp(word[1],"new")) {
     write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
     write_user(user,"~FG            Listing of Banned Sites And Domains For New Users Only\n");
     write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
	sprintf(filename,"%s/%s",DATAFILES,NEWSITEBAN);
	switch(more(user,user->socket,filename)) {
		case 0:
          write_user(user,"~FRThere are currently no banned sites or domains for new users.\n");
          write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
		return;
		case 1: user->misc_op=2;
		}
	return;
     write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
	}
write_user(user,"Usage: listbans sites/new/users/swears\n"); 
}


/*** Ban a site/domain or user ***/
ban(user)
UR_OBJECT user;
{
/* char *usage="Usage: ban site/new/user <site/user name>\n"; */

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


ban_site(user)
UR_OBJECT user;
{
FILE *fp;
char filename[80],host[81],site[80];

gethostname(host,80);
if (!strcmp(word[2],host)) {
	write_user(user,"You cannot ban the machine that this program is running on.\n");
	return;
	}
sprintf(filename,"%s/%s",DATAFILES,SITEBAN);

/* See if ban already set for given site */
if (fp=fopen(filename,"r")) {
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


ban_new(user)
UR_OBJECT user;
{
FILE *fp;
char filename[80],host[81],site[80];

gethostname(host,80);
if (!strcmp(word[2],host)) {
	write_user(user,"You cannot ban the machine that this program is running on.\n");
	return;
	}
sprintf(filename,"%s/%s",DATAFILES,NEWSITEBAN);

/* See if ban already set for given site */
if (fp=fopen(filename,"r")) {
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



ban_user(user)
UR_OBJECT user;
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

/* See if ban already set for given user */
sprintf(filename,"%s/%s",DATAFILES,USERBAN);
if (fp=fopen(filename,"r")) {
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
		write_user(user,nosuchuser);  return;
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
unban(user)
UR_OBJECT user;
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


unban_site(user)
UR_OBJECT user;
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


unban_new(user)
UR_OBJECT user;
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



unban_user(user)
UR_OBJECT user;
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
visibility(user,vis)
UR_OBJECT user;
int vis;
{
if (vis) {
	if (user->vis) {
		write_user(user,"You are already visible.\n");  return;
		}
/*     write_user(user,"~FRYou appear from a ball of light...\n"); */
     sprintf(text,"~OL~FTA ball of light forms and %s appears from within...\n",user->name);
     write_room(user->room,text);
	user->vis=1;
	return;
	}
if (!user->vis) {
	write_user(user,"You are already invisible.\n");  return;
	}
write_user(user,"~FB~OLYou disapear into a ball of light...\n");
sprintf(text,"~FB~OL%s disapears into a ball of bright light...\n",user->name);
write_room_except(user->room,text,user); user->vis=0;
}


/*** Site a user ***/
site(user)
UR_OBJECT user;
{
UR_OBJECT u;

if (word_count<2) {
	write_user(user,"Usage: site <user>\n");  return;
	}
/* User currently logged in */
if (u=get_user(word[1])) {
	if (u->type==REMOTE_TYPE) sprintf(text,"%s is remotely connected from %s.\n",u->name,u->site);
	else if (strstr(u->site,u->site_port)) sprintf(text,"%s is logged in from %s:%s.\n",u->name,u->site,u->site_port);
	else sprintf(text,"%s is logged in from %s.\n",u->name,u->site);
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
sprintf(text,"%s was last logged in from %s.\n",word[1],u->last_site);
write_user(user,text);
destruct_user(u);
destructed=0;
}


/*** Shows wizard the record of a user ***/
view_record(user)
UR_OBJECT user;
{
UR_OBJECT u;
char filename[80];

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
if (word_count<2) {
	sprintf(text,"Useage: record <user name>\n");
	write_user(user,text);
	return;
	}
if ((u=get_user(word[1]))!=NULL) {
	if (u->level>=user->level) {
		write_user(user,"You cannot view the record a user of equal or higher level than yourself.\n");
		return;
		}
          write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
          sprintf(text,"~FG Viewing %s's Record\n",u->name);
          write_user(user,text);
          write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
     sprintf(filename,"%s/%s.A",USERFILES,u->name);
	switch(more(user,user->socket,filename)) {
		case 0:
		sprintf(text,"~OL%s ~FRdoes not have a record to view.\n",u->name);
		write_user(user,text);
		return;
		case 1: user->misc_op=2;
		}
          write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
          write_user(user,"~FG End of User's Record \n");
          write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n\n");
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
if (u->level>=user->level) {
	write_user(user,"You cannot view the record a user of equal or higher level than yourself.\n");
	destruct_user(u);
	destructed=0;
	return;
	}
     write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
     sprintf(text,"~FG Viewing %s's Record\n",u->name);
     write_user(user,text);
     write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
     sprintf(filename,"%s/%s.A",USERFILES,u->name);
     switch(more(user,user->socket,filename)) {
	case 0:
	sprintf(text,"~OL%s ~FRdoes not have a record to view.\n",u->name);
	write_user(user,text);
	destruct_user(u);
	destructed=0;
	return;
     case 1: user->misc_op=2;
     write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
     write_user(user,"~FG End of User's Record \n");
     write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n\n");
	destruct_user(u);
	destructed=0;
	return;
	}
destruct_user(u);
destructed=0;
} 



/*** Wake up some sleepy herbert ***/
wake(user)
UR_OBJECT user;
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
	sprintf(text,"%s is using the editor at the moment.\n",u->name);
	write_user(user,text);  
	return;
	}
if (user->vis) name=user->name; else name=invisname;
sprintf(text,"\07\n~FY>> ~OL~FY%s wants you to ~OL~LIWAKE UP!!!~RS~FY <<\n\n",name);
write_user(u,text);
sprintf(text,"~OL~FYYou bellow at %s to wake up!!!\n",u->name);
write_user(user,text);
}


/*** Shout something to other wizes and OWNERs. If the level isnt given it
     defaults to WIZ level. ***/
wizshout(user,inpstr)
UR_OBJECT user;
char *inpstr;
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
     sprintf(text,"~OL%s wiztells to level %s:~RS %s\n",user->name,level_name[lev],inpstr);
	write_level(lev,5,text,user);
	return;
	}
sprintf(text,"~OLYou wiztell:~RS %s\n",inpstr);
write_user(user,text);
sprintf(text,"~OL%s wiztells:~RS %s\n",user->name,inpstr);
write_level(WIZ,5,text,user);
record_wiz(user,text);
}


/*** Emote something to all wizs***/
wemote(user,inpstr)
UR_OBJECT user;
char *inpstr;
{
char *name;

if (user->muzzled) {
	write_user(user,"You are a muzzled Wizard, shame on you!\n");  return;
	}
if (word_count<2 && inpstr[1]<33) {
	write_user(user,"Wizemote what?\n");  return;
	}
name=user->name;
sprintf(text,"~OL~FGYou emote to all Wiz's:~RS %s %s  ~RS\n",name,inpstr);
write_user(user,text);
sprintf(text,"~OL~FGTo all Wiz's: ~RS%s %s  ~RS\n",name,inpstr);
write_level(WIZ,5,text,user);
record_wiz(user,text);
}


/*** Broadcast an important message ***/
bcast(user,inpstr)
UR_OBJECT user;
char *inpstr;
{
if (word_count<2) {
	write_user(user,"Usage: bcast <message>\n");  return;
	}
if (user->muzzled) {
	write_user(user,"You are muzzled, you cannot broadcast anything.\n");  
	return;
	}
force_listen=1;
if (user->vis) 
	sprintf(text,"\07\n~BR*** Broadcast message from %s ***\n%s\n\n",user->name,inpstr);
else sprintf(text,"\07\n~BR*** Broadcast message ***\n%s\n\n",inpstr);
write_user(user,text);  
write_level(OWNER,4,text,user);
}

/*** Mash a user into a silly potato head ***/

gag(user)

UR_OBJECT user;
{
UR_OBJECT u;

if (word_count<2) {
     write_user(user,"Usage: mash <user name>\n");
     return;
	}
if (!(u=get_user(word[1]))) {
	write_user(user,notloggedon);  return;
	}
if (u==user) {
     write_user(user,"You are a silly potato head for trying to mash yourself!!\n");
	return;
	}
if (u->level>=user->level) {
     write_user(user,"Trying to gag someone of a equal or higher level than yourself isn't nice!\n");
     sprintf(text,"~FR%s thought of mashing you!\n",user->name);
     write_user(u,text);
     return;
	}

if (u->gaged) {
     sprintf(text,"~OL~FW>>>>~RS %s has been re-habilitated and forgiven!\n",u->name);
     write_room(NULL,text);
     strcpy(u->desc,"re-habilitated POTATO HEAD");
     sprintf(text,"%s unmashed %s.\n",user->name,u->name);
	write_syslog(text,1);
        sprintf(text,"%s unmashed %s.\n",user->name,u->name);
	write_arrestrecord(u,text,1);
	u->gaged=0;
        u->a2=u->gaged;
	return;
	}
sprintf(text,"~OL~FW>>>>~RS %s has been MASHED into a Silly POTATO HEAD!\n",u->name);
write_room(NULL,text);
sprintf(text,"%s mashed %s.\n",user->name,u->name);
write_syslog(text,1);
sprintf(text,"%s mashed %s.\n",user->name,u->name);
write_arrestrecord(u,text,1);
u->gaged=1;
strcpy(u->desc,"~OL~FW-> Silly POTATO HEAD");
u->a2=u->gaged;
}


/*** Muzzle an annoying user so he cant speak, emote, echo, write, smail
     or bcast. Muzzles have levels from WIZ to OWNER so for instance a WIZ
     cannot remove a muzzle set by a OWNER.  ***/

muzzle(user)
UR_OBJECT user;
{
UR_OBJECT u;
int i;

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
          sprintf(text,"~FR%s thought about muzzling you.\n",user->name);
		write_user(u,text);  
		return;
		}
	if (u->muzzled & 1) {
		sprintf(text,"%s is already muzzled.\n",u->name);
		write_user(user,text);  return;
		}
     sprintf(text,"~FY~OLYou muzzle: ~RS~OL%s.\n",u->name);
	write_user(user,text);
     write_user(u,"~FY~OLYou have been muzzled!\n");
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
	sprintf(text,"%s is already muzzled.\n",u->name);
	write_user(user,text); 
	destruct_user(u);
	destructed=0;
	return;
	}
u->socket=-2;
u->muzzled++;
strcpy(u->site,u->last_site);
save_user_details(u,0);
sprintf(text,"~FY~OLHas muzzled you: ~RS~OL%s.\n",u->name);
write_user(user,text);
send_mail(user,word[1],"~FY~OLYou have been muzzled!\n");
sprintf(text,"%s muzzled %s.\n",user->name,u->name);
write_syslog(text,1);
sprintf(text,"%s muzzled %s.\n",user->name,u->name);
write_arrestrecord(u,text,1);
destruct_user(u);
destructed=0;
}



/*** Umuzzle the bastard now he's apologised and grovelled enough via email ***/
unmuzzle(user)
UR_OBJECT user;
{
UR_OBJECT u;
char levelname[16];
int i;

if (word_count<2) {
	write_user(user,"Usage: unmuzzle <user>\n");  return;
	}
if ((u=get_user(word[1]))!=NULL) {
	if (u==user) {
		write_user(user,"Trying to unmuzzle yourself is the tenth sign of madness.\n");
		return;
		}
	if (!(u->muzzled & 1)) {
		sprintf(text,"%s is not muzzled.\n",u->name);
		write_user(user,text);
		return;
		}
	sprintf(text,"~FG~OLYou remove %s's muzzle.\n",u->name);
	write_user(user,text);
	write_user(u,"~FG~OLYou have been unmuzzled!\n");
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
	sprintf(text,"%s is not muzzled.\n",u->name,levelname);
	write_user(user,text);  
	destruct_user(u);
	destructed=0;
	return;
	}
u->socket=-2;
u->muzzled--;
strcpy(u->site,u->last_site);
save_user_details(u,0);
sprintf(text,"~FG~OLYou remove %s's muzzle.\n",u->name);
write_user(user,text);
send_mail(user,word[1],"~FG~OLYou have been unmuzzled.\n");
sprintf(text,"%s unmuzzled %s.\n",user->name,u->name);
write_syslog(text,1);
sprintf(text,"%s unmuzzled %s.\n",user->name,u->name);
write_arrestrecord(u,text,1);
destruct_user(u);
destructed=0;
}

/* Thow the user in the brink!! */
arrest(user)
UR_OBJECT user;
{
UR_OBJECT u;
RM_OBJECT rm;
int i;

if (word_count<2) {
	write_user(user,"Usage: arrest <user>\n");  return;
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
     sprintf(text,"~FY~OLYou arrest: ~RS~OL%s.\n",u->name);
	write_user(user,text);
     write_user(u,"~FY~OLYou have been arrested!\n");
	sprintf(text,"%s arrested %s.\n",user->name,u->name);
	write_syslog(text,1);
	sprintf(text,"%s arrested %s.\n",user->name,u->name);
	write_arrestrecord(u,text,1);
	u->muzzled+=JAILED;
	rm=room_last;
	move_user(u,rm,2);
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
sprintf(text,"~FY~OLYou have arrested: ~RS~OL%s.\n",u->name);
write_user(user,text);
send_mail(user,word[1],"~FY~OLAn angry mob of hitmen grab you and carry you off!!!\n");
sprintf(text,"%s arrested %s.\n",user->name,u->name);
write_syslog(text,1);
sprintf(text,"%s arrested %s.\n",user->name,u->name);
write_arrestrecord(u,text,1);
destruct_user(u);
destructed=0;
}


/*** Unarrest the bastard now he's apologised and grovelled enough via email ***/
unarrest(user)
UR_OBJECT user;
{
UR_OBJECT u;
RM_OBJECT rm;
char levelname[16];
int i;

if (word_count<2) {
	write_user(user,"Usage: unarrest <user>\n");  return;
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
	sprintf(text,"~FG~OLYou have released %s from jail.\n",u->name);
	write_user(user,text);
	write_user(u,"~FG~OLYou wake up just as someone opens the trunk of the car!\n");
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
save_user_details(u,0);
sprintf(text,"~FG~OLYou unarrest %s.\n",u->name);
write_user(user,text);
send_mail(user,word[1],"~FG~OLYou have been unarrested.\n");
sprintf(text,"%s unarrested %s.\n",user->name,u->name);
write_syslog(text,1);
sprintf(text,"%s unarrested %s.\n",user->name,u->name);
write_arrestrecord(u,text,1);
destruct_user(u);
destructed=0;
}


/**************************** HIGH LEVEL COMMANDS *************************/


freeze(user)
UR_OBJECT user;
{
UR_OBJECT u;
RM_OBJECT rm;
int i;

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
		write_user(user,text);  return;
		}
     sprintf(text,"~FY~OLYou freeze: ~RS~OL%s.\n",u->name);
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
sprintf(text,"~FY~OLYou have frozen: ~RS~OL%s.\n",u->name);
write_user(user,text);
sprintf(text,"%s freeze's %s.\n",user->name,u->name);
write_syslog(text,1);
sprintf(text,"%s freezes's %s.\n",user->name,u->name);
write_arrestrecord(u,text,1);
destruct_user(u);
destructed=0;
}


/*** Umuzzle the bastard now he's apologised and grovelled enough via email ***/
unfreeze(user)
UR_OBJECT user;
{
UR_OBJECT u;
RM_OBJECT rm;
char levelname[16];
int i;

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
change_pass(user)
UR_OBJECT user;
{
UR_OBJECT u;
char *name;

if (word_count<3) {
     if (user->level<CODER)
		write_user(user,"Usage: passwd <old password> <new password>\n");
	else write_user(user,"Usage: passwd <old password> <new password> [<user>]\n");
	return;
	}
if (strlen(word[2])<3) {
	write_user(user,"New password too short.\n");  return;
	}
if (strlen(word[2])>PASS_LEN) {
	write_user(user,"New password too long.\n");  return;
	}
/* Change own password */
if (word_count==3) {
	if (strcmp((char *)crypt(word[1],"NU"),user->pass)) {
		write_user(user,"Old password incorrect.\n");  return;
		}
	if (!strcmp(word[1],word[2])) {
		write_user(user,"Old and new passwords are the same.\n");  return;
		}
	strcpy(user->pass,(char *)crypt(word[2],"NU"));
	save_user_details(user,0);
	cls(user);
	write_user(user,"Password changed.\n");
	return;
	}
/* Change someone elses */
if (user->level<CODER) {
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
if (u=get_user(word[3])) {
	if (u->type==REMOTE_TYPE) {
		write_user(user,"You cannot change the password of a user logged on remotely.\n");
		return;
		}
	if (u->level>=user->level) {
		write_user(user,"You cannot change the password of a user of equal or higher level than yourself.\n");
		return;
		}
	strcpy(u->pass,(char *)crypt(word[2],"NU"));
	cls(user);
	sprintf(text,"%s's password has been changed.\n",u->name);
	write_user(user,text);
	if (user->vis) name=user->name; else name=invisname;
     sprintf(text,"~FY~OLYour password has been changed by %s!\n",name);
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

/*** Switch system logging on and off ***/
changesys(user)
UR_OBJECT user;
{
int temp,val;

if (word_count<2) {
     sprintf(text,"MAX Users               : %-4.4d          Usage: change max <10-99>\n",max_users);
	write_user(user,text);
     sprintf(text,"MAX Clones              : %-4.4d          Usage: change clones <1-6>\n",max_clones);
	write_user(user,text);
     sprintf(text,"User Idle Time Out      : %-4.4d secs.    Usage: change idle <600-7200>\n",user_idle_time);
	write_user(user,text);
     sprintf(text,"Message Life Time       : %-4.4d days.    Usage: change life <1-15>\n",mesg_life);
	write_user(user,text);
     sprintf(text,"Time Out Afks           : %4.4s           Usage: change afks <yes/no>\n",noyes2[time_out_afks]);
	write_user(user,text);
	return;
	}
if (!strcmp(word[1],"max")) {
	temp=max_users;
	if (word_count<3 || !isnumber(word[2])) {
		write_user(user,"Usage: change max <10-99>\n");
		return;
		}
	val=atoi(word[2]);
	if (val<10) {
		sprintf(text,"The Value %d is to ~OL~FRLow~RS for Max users. Range: 10-99.\n",val);
		write_user(user,text);
		return;
		}
	if (val>99) {
		sprintf(text,"The Value %d is to ~OL~FRHigh~RS for Max users. Range: 10-99.\n",val);
		write_user(user,text);
		return;
		}
	max_users=val;
     sprintf(text,"~OL~FYYou changed Max users from:~RS %d ~OL~FYto~RS %d.\n",temp,max_users);
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
     sprintf(text,"~OL~FYYou changed Max clones from:~RS %d ~OL~FYto~RS %d.\n",temp,max_clones);
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
     sprintf(text,"~OL~FYYou changed User Idle Time Out from:~RS %d sec. ~OL~FYto~RS %d sec.\n",temp,user_idle_time);
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
     sprintf(text,"~OL~FYYou changed Message Life Time from:~RS %d days ~OL~FYto~RS %d days.\n",temp,mesg_life);
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
	if (!strcmp(word[2],"yes") && (time_out_afks!=0)) {
		sprintf(text,"Time Out Afks is set to ~OL~FRYES~RS Usage: change afks <yes/no>.\n");
		write_user(user,text);
		return;
		}
	if (!strcmp(word[2],"no") && (time_out_afks==0)) {
		sprintf(text,"Time Out Afks is set to ~OL~FRNO~RS Usage: change afks <yes/no>.\n");
		write_user(user,text);
		return;
		}
	if (!strcmp(word[2],"yes") && (time_out_afks==0)) {
		time_out_afks=1;
          sprintf(text,"~OL~FYYou changed Time Out Afks from: ~RSNO ~OL~FYto ~RSYES.\n"); 
		write_user(user,text);
		sprintf(text,"%s changed Time Out Afks from: NO to YES.\n");
		write_syslog(text,1);
		return;
		}
	if (!strcmp(word[2],"no") && (time_out_afks!=0)) {
		time_out_afks=0;
          sprintf(text,"~OL~FYYou Time Out Afks from: ~RSYES ~OL~FYto ~RSNO.\n"); 
		write_user(user,text);
		sprintf(text,"%s Changed Time Out Afks from: YES to NO.\n");
		write_syslog(text,1);
		return;
		}
	write_user(user,"Usage: change afks <yes/no>\n");
	return;
	}
write_user(user,"Type .change for Useage\n");
}



/*** Switch system logging on and off ***/
logging(user)
UR_OBJECT user;
{
if (system_logging) {
	write_user(user,"System logging ~FROFF.\n");
	sprintf(text,"%s switched system logging OFF.\n",user->name);
	write_syslog(text,1);
	system_logging=0;
	return;
	}
system_logging=1;
write_user(user,"System logging ~FGON.\n");
sprintf(text,"%s switched system logging ON.\n",user->name);
write_syslog(text,1);
}


/*** Set minlogin level ***/
minlogin(user)
UR_OBJECT user;
{
UR_OBJECT u,next;
char *usage="Usage: minlogin NONE/<user level>\n";
char levstr[5],*name;
int lev,cnt;

if (word_count<2) {
	write_user(user,usage);  return;
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
	write_user(user,"It is already set to that.\n");  return;
	}
minlogin_level=lev;
sprintf(text,"Minlogin level set to: ~OL%s.\n",levstr);
write_user(user,text);
if (user->vis) name=user->name; else name=invisname;
sprintf(text,"%s has set the minlogin level to: ~OL%s.\n",name,levstr);
write_room_except(NULL,text,user);
sprintf(text,"%s set the minlogin level to %s.\n",user->name,levstr);
write_syslog(text,1);

/* Now boot off anyone below that level */
cnt=0;
u=user_first;
while(u) {
	next=u->next;
	if (!u->login && u->type!=CLONE_TYPE && u->level<lev) {
		write_user(u,"\n~FY~OLYour level is now below the minlogin level, disconnecting you...\n");
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
clearline(user)
UR_OBJECT user;
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
change_room_fix(user,fix)
UR_OBJECT user;
int fix;
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
viewloginlog(user)
UR_OBJECT user;
{
FILE *fp;
char c,*emp="\nThe login log is empty.\n";
int lines,cnt,cnt2;

if (word_count==1) {
     write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
     write_user(user,"~FG                       Viewing the Login Log\n");
     write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
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
     write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
	return;
	}
if (cnt==lines) {
     write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
     write_user(user,"~FG                       Viewing the Login Log\n");
     write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
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
          write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
          sprintf(text,"~FG                 Viewing Last %d lines of the Login log\n",lines);
		write_user(user,text);
          write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
		user->filepos=ftell(fp)-1;
		fclose(fp);
		if (more(user,user->socket,LOGINLOG)!=1) user->filepos=0;
		else user->misc_op=2;
		return;
		}
	}
write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
fclose(fp);
sprintf(text,"%s: Line count error.\n",syserror);
write_user(user,text);
write_syslog("ERROR: Line count error in viewloginlog().\n",0);
write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
}


/*** View the system log ***/
viewsyslog(user)
UR_OBJECT user;
{
FILE *fp;
char c,*emp="~FRThe system log is empty.\n";
int lines,cnt,cnt2;

if (word_count==1) {
     write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
     write_user(user,"~FG                      Viewing the System Log\n");
     write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
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
     write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
	return;
	}
if (cnt==lines) {
     write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
     write_user(user,"~FG                      Viewing the System Log\n");
     write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
	fclose(fp);  more(user,user->socket,SYSLOG);
     write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
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
          write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
          sprintf(text,"~FG                 Viewing Last %d lines of the System log\n",lines);
		write_user(user,text);
          write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
		user->filepos=ftell(fp)-1;
		fclose(fp);
		if (more(user,user->socket,SYSLOG)!=1) user->filepos=0;
		else user->misc_op=2;
		return;
		}
	}
write_user(user,"~FY..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..oOo..\n");
fclose(fp);
sprintf(text,"%s: Line count error.\n",syserror);
write_user(user,text);
write_syslog("ERROR: Line count error in viewsyslog().\n",0);
}


/*** Stat a remote system ***/
remote_stat(user)
UR_OBJECT user;
{
NL_OBJECT nl;
RM_OBJECT rm;

if (word_count<2) {
	write_user(user,"Usage: rstat <room service is linked to>\n");  return;
	}
if ((rm=get_room(word[1]))==NULL) {
	write_user(user,nosuchroom);  return;
	}
if ((nl=rm->netlink)==NULL) {
	write_user(user,"That room is not linked to a service.\n");
	return;
	}
if (nl->stage!=2) {
	write_user(user,"Not (fully) connected to service.\n");
	return;
	}
if (nl->ver_major<=3 && nl->ver_minor<1) {
	write_user(user,"The NUTS version running that service does not support this facility.\n");
	return;
	}
sprintf(text,"RSTAT %s\n",user->name);
write_sock(nl->socket,text);
write_user(user,"Request sent.\n");
}


/*** Switch swearing ban on and off ***/
swban(user)
UR_OBJECT user;
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


/*** This command is kept High level for a reason ***/
suicide(user)
UR_OBJECT user;
{
if (word_count<2) {
	write_user(user,"Usage: suicide <your password>\n");  return;
	}
if (strcmp((char *)crypt(word[1],"NU"),user->pass)) {
	write_user(user,"Password incorrect.\n");  return;
	}
write_user(user,"\n\07~FR~OL~LI*** WARNING - This will delete your account! ***\n\nAre you sure about this (y/n)? ");
user->misc_op=6;  
no_prompt=1;
}


/*** NUKE a user ***/
delete_user(user,this_user)
UR_OBJECT user;
int this_user;
{
UR_OBJECT u;
char filename[80],name[USER_NAME_LEN+1];

if (this_user) {
	/* User structure gets destructed in disconnect_user(), need to keep a
	   copy of the name */
	strcpy(name,user->name); 
	write_user(user,"\n~FR~LI~OLACCOUNT DELETED!\n");
	sprintf(text,"~OL~LI%s commits suicide!\n",user->name);
	write_room_except(user->room,text,user);
	sprintf(text,"%s SUICIDED.\n",name);
	write_syslog(text,1);
	disconnect_user(user);
	sprintf(filename,"%s/%s.D",USERFILES,name);
	unlink(filename);
	sprintf(filename,"%s/%s.M",USERFILES,name);
	unlink(filename);
	sprintf(filename,"%s/%s.P",USERFILES,name);
	unlink(filename);
	sprintf(filename,"%s/%s.A",USERFILES,name);
	unlink(filename);
	clean_userlist(name);
	return;
	}
if (word_count<2) {
	write_user(user,"Usage: nuke <user>\n");  return;
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
sprintf(filename,"%s/%s.A",USERFILES,word[1]);
unlink(filename);
sprintf(text,"\07~FR~OL~LIUser %s deleted!\n",word[1]);
write_user(user,text);
sprintf(text,"%s DELETED %s.\n",user->name,word[1]);
write_syslog(text,1);
}


/*** Shutdown talker interface func. Countdown time is entered in seconds so
	we can specify less than a minute till reboot. ***/
shutdown_com(user)
UR_OBJECT user;
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
write_user(user,"\n\07~FR~OL~LI*** WARNING - This will shutdown the talker! ***\n\nAre you sure about this (y/n)? ");
user->misc_op=1;  
no_prompt=1;  
}


/*** Reboot talker interface func. ***/
reboot_com(user)
UR_OBJECT user;
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
write_user(user,"\n\07~FY~OL~LI*** WARNING - This will reboot the talker! ***\n\nAre you sure about this (y/n)? ");
user->misc_op=7;  
no_prompt=1;  
}


/*** Shutdown the talker ***/
talker_shutdown(user,str,reboot)
UR_OBJECT user;
char *str;
int reboot;
{
UR_OBJECT u;
NL_OBJECT nl;
int i;
char *ptr;
char *args[]={ progname,confile,NULL };

if (user!=NULL) ptr=user->name; else ptr=str;
if (reboot) {
	write_room(NULL,"\07\n~OLSYSTEM:~FY~LI Rebooting now!!\n\n");
	sprintf(text,"*** REBOOT initiated by %s ***\n",ptr);
	}
else {
     write_room(NULL,"\07\n~OLSYSTEM:~FY~LI Shutting down now!!\n\n");
	sprintf(text,"*** SHUTDOWN initiated by %s ***\n",ptr);
	}
write_syslog(text,0);
for(nl=nl_first;nl!=NULL;nl=nl->next) shutdown_netlink(nl);
for(u=user_first;u!=NULL;u=u->next) disconnect_user(u);
for(i=0;i<3;++i) close(listen_sock[i]); 
if (reboot) {
	/* If someone has changed the binary or the config filename while this 
	   prog has been running this won't work */
	execvp(progname,args);
	/* If we get this far it hasn't worked */
	sprintf(text,"*** REBOOT FAILED %s: %s ***\n\n",long_date(1),sys_errlist[errno]);
	write_syslog(text,0);
	exit(12);
	}
sprintf(text,"*** SHUTDOWN complete %s ***\n\n",long_date(1));
write_syslog(text,0);
exit(0);
}


/***********************************************************************/
/**************************** EVENT FUNCTIONS **************************/

void do_events()
{
set_date_time();
check_reboot_shutdown();
check_idle_and_timeout();
check_nethangs_send_keepalives(); 
check_messages(NULL,0);
reset_alarm();
}


reset_alarm()
{
signal(SIGALRM,do_events);
alarm(heartbeat);
}



/*** See if timed reboot or shutdown is underway ***/
check_reboot_shutdown()
{
int secs;
char *w[]={ "~FRShutdown","~FYRebooting" };

if (rs_user==NULL) return;
rs_countdown-=heartbeat;
if (rs_countdown<=0) talker_shutdown(rs_user,NULL,rs_which);

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
check_idle_and_timeout()
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
	if (user->login && tm>=login_idle_time) {
		write_user(user,"\n\n*** Time out ***\n\n");
		disconnect_user(user);
		user=next;
		continue;
		}
	if (user->warned) {
		if (tm<user_idle_time-60) {  user->warned=0;  continue;  }
		if (tm>=user_idle_time) {
			write_user(user,"\n\n\07~FR~OL~LI*** You have been timed out. ***\n\n");
			disconnect_user(user);
			user=next;
			continue;
			}
		}
	if ((!user->afk || (user->afk && time_out_afks)) 
	    && !user->login 
	    && !user->warned
	    && tm>=user_idle_time-60) {
		write_user(user,"\n\07~FY~OL~LI*** WARNING - Input within 1 minute or you will be disconnected. ***\n\n");
		user->warned=1;
		}
	user=next;
	}
}
	


/*** See if any net connections are dragging their feet. If they have been idle
     longer than net_idle_time the drop them. Also send keepalive signals down
     links, this saves having another function and loop to do it. ***/
check_nethangs_send_keepalives()
{
NL_OBJECT nl;
int secs;

for(nl=nl_first;nl!=NULL;nl=nl->next) {
	if (nl->type==UNCONNECTED) {
		nl->warned=0;  continue;
		}

	/* Send keepalives */
	nl->keepalive_cnt+=heartbeat;
	if (nl->keepalive_cnt>=keepalive_interval) {
		write_sock(nl->socket,"KA\n");
		nl->keepalive_cnt=0;
		}

	/* Check time outs */
	secs=(int)(time(0) - nl->last_recvd);
	if (nl->warned) {
		if (secs<net_idle_time-60) nl->warned=0;
		else {
			if (secs<net_idle_time) continue;
			sprintf(text,"~OLSYSTEM:~RS Disconnecting hung netlink to %s in the %s.\n",nl->service,nl->connect_room->name);
			write_room(NULL,text);
			shutdown_netlink(nl);
			nl->warned=0;
			}
		continue;
		}
	if (secs>net_idle_time-60) {
		sprintf(text,"~OLSYSTEM:~RS Netlink to %s in the %s has been hung for %d seconds.\n",nl->service,nl->connect_room->name,secs);
          write_level(CODER,1,text,NULL);
		nl->warned=1;
		}
	}
destructed=0;
}

/*** Remove any expired messages from boards unless force = 2 in which case
	just do a recount. ***/
check_messages(user,force)
UR_OBJECT user;
int force;
{
RM_OBJECT rm;
FILE *infp,*outfp;
char id[82],filename[80],line[82],old_filename[80],color[80],from[80],name[80];
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
	if (bad_cnt) 
		sprintf(text,"CHECK_MESSAGES: %d files checked, %d had an incorrect message count, %d messages deleted.\n",board_cnt,bad_cnt,old_cnt);
	else sprintf(text,"CHECK_MESSAGES: %d files checked, %d messages deleted.\n",board_cnt,old_cnt);
	write_syslog(text,1);
	sprintf(filename,"%s.%02d%02d.%02d%02d",SYSLOG,tmonth+1,tmday,thour,tmin);	
	rename(SYSLOG,filename);
	sprintf(text,"SYSLOG Renamed to: %s.%02d%02d.%02d%02d\n",SYSLOG,tmonth+1,tmday,thour,tmin);
	write_syslog(text,1);
	sprintf(filename,"%s.%02d%02d.%02d%02d",LOGINLOG,tmonth+1,tmday,thour,tmin);	
	rename(LOGINLOG,filename);
	sprintf(text,"LOGINLOG Renamed to: %s.%02d%02d.%02d%02d\n",LOGINLOG,tmonth+1,tmday,thour,tmin);
	write_syslog(text,1);
	write_loginlog(text,1);
	break;

	case 1:
	printf(">> %4.4d board files checked, %4.4d out of date messages found.\n",board_cnt,old_cnt);
	break;

	case 2:
	sprintf(text,"%d board files checked, %d had an incorrect message count.\n",board_cnt,bad_cnt);
	write_user(user,text);
	sprintf(text,"%s forced a recount of the message boards.\n",user->name);
	write_syslog(text,1);
	}
}
/**************************** Made in England *******************************/




/*** print out greeting in large letters ***/
greet(user,inpstr)
UR_OBJECT user;
char *inpstr;
{
char pbuff[ARR_SIZE],temp[8];
int slen,lc,c,i,j;
char *clr[]={"~OL~FR","~OL~FG","~OL~FY","~OL~FB","~OL~FY"};

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
}


/* adds a name to the userlist */
add_userlist(name)
char *name;
{
FILE *fp;
char filename[80];

sprintf(filename,"%s/%s",USERFILES,USERLIST);
if (fp=fopen(filename,"a")) {
	fprintf(fp,"%s\n",name);
	fclose(fp);
	}
}

/* takes a name out of the userlist file */
clean_userlist(name)
char *name;
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
	if (strcmp(name,check)) fprintf(fpo,"%s\n",check);
	fscanf(fpi,"%s",check);
	}
fclose(fpi);  fclose(fpo);
unlink(filename);
rename("templist",filename);
}


/* checks a name to see if it's in the userlist - incase of a bug, or userlst
   gets buggered up somehow */
in_userlist(name)
char *name;
{
char filename[80], check[USER_NAME_LEN+1];
FILE *fp;

sprintf(filename,"%s/%s",USERFILES,USERLIST);
if (!(fp=fopen(filename,"r"))) return 0;

name[0]=toupper(name[0]);
fscanf(fp,"%s",check);
while(!(feof(fp))) {
	check[0]=toupper(check[0]);
	if (!strcmp(name,check)) {
		fclose(fp); return 1;
		}
	fscanf(fp,"%s",check);
	}
fclose(fp);
return 0;
}


/* Display all the people logged on from the same site as user */
samesite(user,stage)
UR_OBJECT user;
int stage;
{
UR_OBJECT u,u_loop;
int found,cnt,same,on;
char line[82],filename[80],name[USER_NAME_LEN+1];
FILE *fpi,*fpu;

if (!stage) {
  if (word_count<2) {
    write_user(user,"Usage: samesite user/site [all]\n");
    return;
    }
  strtolower(word[1]); strtolower(word[2]);
  if (word_count==3 && !strcmp(word[2],"all")) user->samesite_all_store=1;
  else user->samesite_all_store=0;
  if (!strcmp(word[1],"user")) {
    write_user(user,"Enter the name of the user to be checked against: ");
    user->misc_op=9;
    return;
    }
  if (!strcmp(word[1],"site")) {
    write_user(user,"~OL~FRNOTE:~RS Partial site strings can be given, but NO wildcards.\n");
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
	  sprintf(text,"\n~BB~FG*** Users logged on from the same site as ~OL%s~RS~BB~FG ***\n\n",u->name);
	  write_user(user,text);
          }
	sprintf(text,"    %s %s\n",u_loop->name,u_loop->desc);
	if (u_loop->type==REMOTE_TYPE) text[2]='@';
	if (!u_loop->vis) text[3]='*';
	write_user(user,text);
        }
      }
    if (!found) {
      sprintf(text,"No users currently logged on have that same site as %s.\n",u->name);
      write_user(user,text);
      }
    else {
      sprintf(text,"\nChecked ~FY~OL%d~RS users, ~FY~OL%d~RS had the site as ~FG~OL%s~RS ~FG(%s)\n\n",cnt,same,u->name,u->site);
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
	sprintf(text,"\n~BB~FG*** All users from the same site as ~OL%s~RS~BB~FG ***\n\n",u->name);
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
    sprintf(text,"No users have the same site as %s.\n",u->name);
    write_user(user,text);
    }
  else {
    if (!on) sprintf(text,"\nChecked ~FY~OL%d~RS users, ~FY~OL%d~RS had the site as ~FG~OL%s~RS ~FG(%s)\n\n",cnt,same,u->name,u->last_site);
    else sprintf(text,"\nChecked ~FY~OL%d~RS users, ~FY~OL%d~RS had the site as ~FG~OL%s~RS ~FG(%s)\n\n",cnt,same,u->name,u->site);
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
	sprintf(text,"\n~BB~FG*** Users logged on from the same site as ~OL%s~RS~BB~FG ***\n\n",user->samesite_check_store);
	write_user(user,text);
        }
      sprintf(text,"    %s %s\n",u->name,u->desc);
      if (u->type==REMOTE_TYPE) text[2]='@';
      if (!u->vis) text[3]='*';
      write_user(user,text);
      }
    if (!found) {
      sprintf(text,"No users currently logged on have that same site as %s.\n",user->samesite_check_store);
      write_user(user,text);
      }
    else {
      sprintf(text,"\nChecked ~FY~OL%d~RS users, ~FY~OL%d~RS had the site as ~FG~OL%s\n\n",cnt,same,user->samesite_check_store);
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
	sprintf(text,"\n~BB~FG*** All users that have the site ~OL%s~RS~BB~FG ***\n\n",user->samesite_check_store);
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
    if (!on) sprintf(text,"\nChecked ~FY~OL%d~RS users, ~FY~OL%d~RS had the site as ~FG~OL%s~RS\n\n",cnt,same,user->samesite_check_store);
    else sprintf(text,"\nChecked ~FY~OL%d~RS users, ~FY~OL%d~RS had the site as ~FG~OL%s~RS\n\n",cnt,same,user->samesite_check_store);
    write_user(user,text);
    }
  return;
  } /* end of stage 2 */
}


/** count number of users listed in USERLIST for a global count **/
count_users()
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

/************************** MORE COMMANDS ******************/

/** Pictell (C)1997 Mike Irving, All rights Reserved **/

pictell(user,inpstr)

UR_OBJECT user;
char *inpstr;
{
UR_OBJECT u;
RM_OBJECT rm;
RM_OBJECT urm;
FILE *fp;
char filename[80],line[129];
char *name,*c;

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
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
if (u->hidden && (!u->vis)) write_user(user,notloggedon); return;
if (user->gaged) {
     write_user(user,"~OL~FW>>>>~RS Silly POTATO HEAD\n");  
	return;
	}
if (u->afk) {
     if (u->afk_mesg[0]) {
	 sprintf(text,"~OL~FR%s ~FGis ~FYAFK: %s\n",u->name,u->afk_mesg);
	}
     else {
	 sprintf(text,"%s is AFK at the moment, They cannot be sent pictures!\n",u->name);
	write_user(user,text);
	return;
	}
     }
if (u->malloc_start!=NULL) {
     sprintf(text,"%s is using the editor at the moment and cannot be sent pictures!\n",u->name);
	write_user(user,text);
	return;
	}
if (u->ignall && user->level<CODER) {
     sprintf(text,"~OL~FT%s is ignoring everyone at the moment.\n",u->name);
	write_user(user,text);  
	return;
	}
if ((u->ignore & ROOM_PICTURE) && user->level<CODER) {
     sprintf(text,"~OL~FT%s is ignoring pictures at the moment.\n",u->name);
	write_user(user,text);
	return;
	}
if (!strcmp(user->name,u->ignuser) && user->level<CODER) {
     sprintf(text,"~OL~FT%s is ignoring pictures at the moment.\n",u->name);
	write_user(user,text);	
	return;
	}
rm=user->room;
urm=u->room;
if ((rm!=urm) && (u->ignore & OUT_ROOM_TELLS) && user->level<CODER) {
     sprintf(text,"~OL~FT%s is ignoring private tells at the moment.\n",u->name);
	write_user(user,text);	
	return;
	}
if (u->room==NULL) {
     sprintf(text,"~OL~FT%s is offsite ~BR~OL~FY<*> Pictures Not Allowed Over The Netlinl! <*>~RS\n",u->name);
	write_user(user,text);
	return;
	}
if (user->muzzled & FROZEN) {
	inpstr=remove_first(inpstr);
     sprintf(text,"\n~OL~BM~FY[ ~FTYou Send ~FW%s~FG: %s ~FY]~RS\n",u->name,inpstr);
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
		sprintf(text,"%s tryed to to use %s to hack the system in Pictell.\n",user,word[1]);
		write_syslog(text,1);
		return;
		}
	}
sprintf(filename,"%s/%s",PICFILES,inpstr);
if (!(fp=fopen(filename,"r"))) {
     sprintf(text,"~OLPictell~FB:~FT %s ~FYcould not be found...\n",inpstr);
     write_user(user,text);
     return;
     }
else {
     sprintf(text,"\n~OL~BM~FY[ ~FTYou Send ~FW%s~FG: %s ~FY]~RS\n",u->name,inpstr);
     write_user(user,text);
     if (user->vis) name=user->name; else name=invisname;
     sprintf(text,"\n~OL~BM~FY[ ~FT%s sends you~FG: %s ~FY]~RS\n\n",name,inpstr);
     write_user(u,text);

     /* show the file */

     fgets(line,128,fp);
	while(!feof(fp)) {
          write_user(u,line);
		fgets(line,128,fp);
		}
	fclose(fp);
	}
write_user(user,"~OL~FGPicture Has Been sent...\n");
}

/** Roompic (C)1997 Mike Irving, All rights Reserved **/

roompic(user,inpstr)
UR_OBJECT user;
char *inpstr;
{
FILE *fp;
char filename[80],line[129];
char *name,*c;

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
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
		sprintf(text,"%s tryed to to use %s to hack the system in Room Picture.\n",user,word[1]);
		write_syslog(text,1);
		return;
		}
	}
if (user->muzzled & FROZEN) {
	inpstr=remove_first(inpstr);
     sprintf(text,"\n~OL~BM~FY[ ~FTYou Send The Room~FW:~FG %s ~FY]~RS\n",inpstr);
	write_user(user,text);	
	return;
	}
     sprintf(filename,"%s/%s",PICFILES,inpstr);
     if (!(fp=fopen(filename,"r"))) {
          sprintf(text,"~OLRoompic~FB:~FT %s ~FYcould not be found...\n",inpstr);
          write_user(user,text);
          piclist(user);
          return;
          }
     else {
          sprintf(text,"\n~OL~BM~FY[ ~FTYou Send The Room~FW:~FG %s ~FY]~RS\n",inpstr);
          write_user(user,text);
          if (user->vis) name=user->name; else name=invisname;
          sprintf(text,"\n~OL~BM~FY[ ~FT%s sends everyone in the room~FG: %s ~FY]~RS\n\n",name,inpstr);
          write_room_except(user->room,text,user);
     
          /* show the file */
     
          fgets(line,128,fp);
          while(!feof(fp)) {
               write_room(user->room,line);
               fgets(line,128,fp);
               }
          fclose(fp);
          }
}
     
/* Show the piclist to the user who requests it */

piclist(user)

UR_OBJECT user;
{
char filename[80];

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");
	return;
	}
sprintf(filename,"%s/%s",PICFILES,PICLISTFILE);
switch(more(user,user->socket,filename)) {
     case 0: write_user(user,"I was unable to find a pictures list.  Sorry...\n"); return;
     case 1: user->misc_op=2;
     }
}

/** Viewpic (C)1997 Mike Irving, All rights Reserved **/

viewpic(user,inpstr)
UR_OBJECT user;
char *inpstr;
{
FILE *fp;
char filename[80],line[129];
char *c;

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
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
		sprintf(text,"%s tryed to to use %s to hack the system in View Pic.\n",user,word[1]);
		write_syslog(text,1);
		return;
		}
	}
     sprintf(filename,"%s/%s",PICFILES,inpstr);
     if (!(fp=fopen(filename,"r"))) {
          sprintf(text,"~OLViewpic~FB:~FT %s ~FYcould not be found...\n",inpstr);
          write_user(user,text);
          piclist(user);
          return;
          }
     else {
          sprintf(text,"\n~OL~BM~FY[ ~FTYou View The Following Picture~FW:~FG %s ~FY]~RS\n",inpstr);
          write_user(user,text);
     
          /* show the file */
     
          fgets(line,128,fp);
          while(!feof(fp)) {
               write_user(user,line);
               fgets(line,128,fp);
               }
          fclose(fp);
          }
}

/*** Go to home room ***/
myroom(user)
UR_OBJECT user;
{
RM_OBJECT room;
/* char filename[80],line[81];
FILE *fp; */

room=get_room(user->name);

if (room==NULL) {
  room=create_room();
  if (room==NULL)
    return;
  strcpy(room->name,user->name);
  strcpy(room->label,"sTh");
  room->link[0]=room_first;
  room->access=FIXED_PERSONAL;
  strcpy(room->map,"users");
  strcpy(room->owner,user->name);
  if (!strcmp(user->name,"Genny")) sprintf(text,"Live, Love and Learn");
  else sprintf(text,"~OL~FTWelcome to %s's Room!",user->name); 
  strcpy(room->topic,text); 
}
if (room==user->room) {
  write_user(user,"You are already in your home room.\n");
  return;
}
sprintf(text,"~OL~FTA room has been created for you called ~FM%s~FT, use ~FR.home ~FT to enter it.\n",user->name);
write_user(user,text);
/*
if (!user_ownroom(user,user->room)) {
        sprintf(filename,"%s/%s.RM",DATAFILES,user->name);
        if (fp=fopen(filename,"a")) {
                fprintf(fp,"%s\n",user->name);
                fclose(fp);
                }
        }
*/
}


/*** Set Possesion of a room to a user ***/
setposs(user,inpstr)
UR_OBJECT user;
{
UR_OBJECT u;
RM_OBJECT rm;
char filename[80];
FILE *fp;

if (word_count<3) {
        write_user(user,"Usage: setposs <user> <room>\n");  return;
        }
word[1][0]=toupper(word[1][0]);
if ((u=get_user(word[1]))) {
        if ((rm=get_room(word[2]))==NULL) {
                write_user(user,nosuchroom);  return;
                }
        if (user_ownroom(u,word[2])) {
                sprintf(text,"%s already owns that room.\n",u->name);
                return;
                }
        sprintf(filename,"%s/%s.RM",DATAFILES,u->name);
        if (fp=fopen(filename,"a")) {
                fprintf(fp,"%s\n",word[2]);
                fclose(fp);
                }
        sprintf(text,"You have given possession of %s to %s\n",word[2],u->name);
        write_user(user,text);
        return;
        }
if (!(u=get_user(word[1]))) {
	if ((u=create_user())==NULL) {
		sprintf(text,"%s: unable to create temporary user object.\n",syserror);
		write_user(user,text);
		write_syslog("ERROR: Unable to create temporary user object in examine().\n",0,SYSLOG);
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
destruct_user(u);
destructed=0;
if ((rm=get_room(word[2]))==NULL) {
        write_user(user,nosuchroom);  return;
        }
if (user_ownroom(u,word[2])) {
        sprintf(text,"%s already owns that room.\n",u->name);
        return;
        }
sprintf(filename,"%s/%s.RM",DATAFILES,u->name);
if (fp=fopen(filename,"a")) {
        fprintf(fp,"%s\n",word[2]);
	fclose(fp);
	}
sprintf(text,"You have given possession of %s to %s\n",word[2],u->name);
write_user(user,text);
return;
}

/*** See if user owns the room ***/
user_ownroom(user,name)
char *name;
{
FILE *fp;
char line[82],filename[80];

sprintf(filename,"%s/%s.RM",DATAFILES,user);
if (!(fp=fopen(filename,"r"))) return 0;
fscanf(fp,"%s",line);
while(!feof(fp)) {
	if (!strcmp(line,name)) {  fclose(fp);  return 1;  }
	fscanf(fp,"%s",line);
	}
fclose(fp);
return 0;
}

/*** Room kill ***/
rmkill(user)
UR_OBJECT user;
{
UR_OBJECT u;
RM_OBJECT rm;

if (word_count<2) {
        write_user(user,"Usage: rmkill <user>\n");  return;
        }
if (!user_ownroom(user,user->room)) {
        write_user(user,"This is not your room, you cannot do that.\n");
return;
        }
word[1][0]=toupper(word[1][0]);
if (!(u=get_user(word[1]))) {
	write_user(user,notloggedon);  return;
	}
if (u==user) {
        write_user(user,"You cannot room kill yourself.\n");
        return;
        }
if (u->room!=user->room) {
        sprintf(text,"%s isn't in this room with you.\n",u->name);
        write_user(user,text);
        return;
        }
if (!(rm=get_room(room_first))) {
        write_user(user,"You cannot move someone to the main room at this time.\n");
        return;
        }
move_user(u,rm,2);
sprintf(text,"You room kill %s\n",u->name);
write_user(user,text);
write_user(u,"You have been room killed.\n");
return;
}

/*** See if user is banned from a room ***/
user_banroom(room,name)
char *name;
{
FILE *fp;
char line[82],filename[80];

sprintf(filename,"%s/%s.BAN",DATAFILES,room);
if (!(fp=fopen(filename,"r"))) return 0;
fscanf(fp,"%s",line);
while(!feof(fp)) {
        if (!strcmp(line,name)) {  fclose(fp);  return 1;  }
	fscanf(fp,"%s",line);
	}
fclose(fp);
return 0;
}

/*** Ban a user from a room ***/
rmban(user)
UR_OBJECT user;
{
UR_OBJECT u;
RM_OBJECT rm;
char filename[80];
FILE *fp;

if (word_count<2) {
        write_user(user,"Usage: rmban <user>\n");  return;
        }
if (!user_ownroom(user,user->room)) {
        write_user(user,"This is not your room, you cannot do that.\n");
return;
        }
word[1][0]=toupper(word[1][0]);
if ((u=get_user(word[1]))) {
        if (u==user) {
                write_user(user,"You cannot ban yourself from your room.\n");
                return;
                }
        if (user_banroom(user->room,u)) {
                sprintf(text,"%s is already banned from this room.",u->name);
                write_user(user,text);
                return;
                }
        sprintf(filename,"%s/%s.BAN",DATAFILES,user->room);
        if (fp=fopen(filename,"a")) {
                fprintf(fp,"%s\n",u->name);
                fclose(fp);
                }
        sprintf(text,"You have banned %s from %s\n",u->name,user->room);
        write_user(user,text);
        sprintf(text,"You have been banned from %s\n",user->room);
        write_user(u,text);
        return;
        }
if (!(u=get_user(word[1]))) {
	if ((u=create_user())==NULL) {
		sprintf(text,"%s: unable to create temporary user object.\n",syserror);
		write_user(user,text);
		write_syslog("ERROR: Unable to create temporary user object in examine().\n",0,SYSLOG);
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
destruct_user(u);
destructed=0;
if (u==user) {
        write_user(user,"You cannot ban yourself from your room.\n");
        return;
        }
if (user_banroom(user->room,u)) {
        sprintf(text,"%s is already banned from this room.",u->name);
        write_user(user,text);
        return;
        }
sprintf(filename,"%s/%s.BAN",DATAFILES,user->room);
if (fp=fopen(filename,"a")) {
        fprintf(fp,"%s\n",u->name);
	fclose(fp);
	}
sprintf(text,"You have banned %s from %s\n",u->name,user->room);
write_user(user,text);
return;
}

/*** Unban a user from a room ***/
rmunban(user)
UR_OBJECT user;
{
FILE *infp,*outfp;
char filename[80],name[USER_NAME_LEN+1];
int found,cnt;

if (word_count<2) {
        write_user(user,"Usage: rmunban <user>\n");  return;
        }
if (!user_ownroom(user,user->room)) {
        write_user(user,"This is not your room, you cannot do that.\n");
return;
        }
sprintf(filename,"%s/%s.BAN",DATAFILES,user->room);
if (!(infp=fopen(filename,"r"))) {
	write_user(user,"That user is not currently banned.\n");
	return;
	}
if (!(outfp=fopen("tempfile","w"))) {
	sprintf(text,"%s: Couldn't open tempfile.\n",syserror);
	write_user(user,text);
	write_syslog("ERROR: Couldn't open tempfile to write in
unban_user().\n",0,SYSLOG);
	fclose(infp);
	return;
	}
found=0;  cnt=0;
word[1][0]=toupper(word[1][0]);
fscanf(infp,"%s",name);
while(!feof(infp)) {
        if (strcmp(word[1],name)) {
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
write_user(user,"User room ban removed.\n");
return;
}

/** Goto User's Room **/
goto_myroom(user)

UR_OBJECT user;
{
RM_OBJECT rm;

if (user->muzzled & JAILED) {
        write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}

if ((rm=get_room(user->name))==NULL) {
	write_user(user,nosuchroom);
	return;
	}
if (rm==user->room) {
	write_user(user,"~OL~FGYou are already in your room!\n");
	return;
	}
sprintf(text,"~OL~FY%s %s to their room...\n",user->name,user->out_phrase);
write_room_except(user->room,text,user);
move_user(user,rm,3); 
}

/*** Toggle hidden attribute for a user ***/
toggle_hidden(user)
UR_OBJECT user;
{

if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
	return;
	}
if (user->muzzled & SCUM) {
     write_user(user,"% What is it you are trying to do? %\n");  
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
shackle(user)
UR_OBJECT user;
{
UR_OBJECT u;
char *name, *rname;
 
if (user->vis) name=user->name; else name=invisname;
 
if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
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
	write_user(user,notloggedon);  return;
	}
if (u->vis) rname=u->name; else rname=invisname;
if (u->shackled) {
	sprintf(text,"~OL~FT%s has already been shackled by a %s!\n",u->name,level_name[u->shackle_level]);
	write_user(user,text);
	return;
	}
if (u==user) {
     write_user(user,"Trying to shackle yourself is universally stupid!\n");
     return;
     }
if (u->room==NULL) {
     sprintf(text,"%s cannot be shackled as they are offsite.\n",u->name);
	write_user(user,text);
	return;
	}
if (user->level<=u->level) {
	write_user(user,"~OL~FRYou cannot shackle someone of equal or greater level than youself\n");
	sprintf(text,"~OL~FR%s thought about shackleing you to this room.\n");
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
sprintf(text,"%s was SHACKLED by %s.  Shackle Level = %s\n",u->name,user->name,level_name[user->level]);
write_syslog(text,1);
write_arrestrecord(u,text,1);
}


/*** Shackle a user to their current room so they cannot move ***/
unshackle(user)
UR_OBJECT user;
{
UR_OBJECT u;
char *name, *rname;
 
if (user->vis) name=user->name; else name=invisname;
 
if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
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
	write_user(user,notloggedon);  return;
	}
if (u->vis) rname=u->name; else rname=invisname;
if (!u->shackled) {
	sprintf(text,"~OL~FT%s is not currently shackled.\n",u->name);
	write_user(user,text);
	return;
	}
if (u==user) {
     write_user(user,"Trying to remove your own shackles, shake on you...\n");
     return;
     }
if (u->room==NULL) {
     sprintf(text,"%s cannot be un-shackled as they are offsite.\n",u->name);
	write_user(user,text);
	return;
	}
if (user->level<=u->level) {
	write_user(user,"~OL~FRYou cannot un-shackle someone of equal or greater level than youself\n");
	sprintf(text,"~OL~FR%s wanted to un-shackle you.\n");
	write_user(u,text);
	sprintf(text,"%s attempted to remove %s's SHACKLE.\n",user->name,u->name);
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
sprintf(text,"%s was UN-SHACKLED by %s (%s)\n",u->name,user->name,level_name[user->level]);
write_syslog(text,1);
write_arrestrecord(u,text,1);
}


/*** Make someone Invisible ***/
makeinvis(user)
UR_OBJECT user;
{
UR_OBJECT u;
char *name, *rname;
 
if (user->vis) name=user->name; else name=invisname;
 
if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
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
	write_user(user,notloggedon);  return;
	}
if (u->vis) rname=u->name; else rname=invisname;
if (u==user) {
     write_user(user,"Use .invisible instead!\n");
     return;
     }
if (u->room==NULL) {
     sprintf(text,"%s cannot be made invisible as they are offsite.\n",u->name);
	write_user(user,text);
	return;
	}
if (user->level<=u->level) {
	write_user(user,"~OL~FRYou cannot make someone of equal or greater level than youself invisible.\n");
	sprintf(text,"~OL~FR%s thought about making you disapear.\n");
	write_user(u,text);
	sprintf(text,"%s attempted to make %s INVISIBLE.\n",user->name,u->name);
	write_syslog(text,1);
	return;
	}
if (!u->vis) write_user(user,"~OL~FTThat user is already invisible.\n"); return;

if (user->muzzled & FROZEN) {
     sprintf(text,"~OL~FTYou make %s invisible.\n",rname);
     write_user(user,text);
     return;
	}
sprintf(text,"~OL~FTYou summon the power of the gods and make %s disappear.\n",rname);
write_user(user,text);
visibility(u,0);
sprintf(text,"%s was made invisible by %s.  Level = %s\n",u->name,user->name,level_name[user->level]);
write_syslog(text,1);
write_arrestrecord(u,text,1);
}

/*** Make a user visible again ***/
makevis(user)
UR_OBJECT user;
{
UR_OBJECT u;
char *name, *rname;
 
if (user->vis) name=user->name; else name=invisname;
 
if (user->muzzled & JAILED) {
     write_user(user,"% What is it you are trying to do? %\n");  
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
	write_user(user,notloggedon);  return;
	}
if (u->vis) rname=u->name; else rname=invisname;
if (u==user) {
     write_user(user,"Use .visible instead!\n");
     return;
     }
if (u->room==NULL) {
     sprintf(text,"%s cannot be made visible while they are offsite.\n",u->name);
	write_user(user,text);
	return;
	}
if (u->vis) write_user(user,"~OL~FTThat user is already visible.\n"); return;
if (user->level<=u->level) {
	write_user(user,"~OL~FRYou cannot make someone of equal or greater level than youself visible\n");
	sprintf(text,"~OL~FR%s thought about making you visible.\n");
	write_user(u,text);
	sprintf(text,"%s attempted to make %s VISIBLE.\n",user->name,u->name);
	write_syslog(text,1);
	return;
	}
if (user->muzzled & FROZEN) {
     sprintf(text,"~OL~FTYou summon the power of the gods and make %s re-appear.\n",rname);
     write_user(user,text);
     return;
	}
sprintf(text,"~OL~FTYou summon the power of the gods and make %s re-appear.\n",rname);
write_user(user,text);
visibility(u,1);
sprintf(text,"%s was made visible by %s.  Level = %s\n",u->name,user->name,level_name[user->level]);
write_syslog(text,1);
write_arrestrecord(u,text,1);
}
