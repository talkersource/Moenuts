/********************* Header file for Moenuts v1.73 ************************
 
  Note: This header file differs from earlier versions as it has many new
  additions to the user structure etc.  Do not use it with earlier versions
  of Moenuts, or with other NUTS dirivatives.  You will end up with problems
  with your talker.  This code has been tested under Linux 2.0.35 on my own
  server, but should also work with any other Unix based OS's like FreeBSD,
  NetBSD, SunOS, Unix, IRIX, Solaris etc.

 ****************************************************************************/

/* Directory Definitions */

#define DATAFILES 	"datafiles"
#define ROOMFILES 	"roomfiles"
#define USERFILES 	"userfiles"
#define HELPFILES 	"helpfiles"
#define MAILSPOOL 	"mailspool"
#define MISCFILES 	"miscfiles"
#define SCREENFILES     "screens"
#define PICFILES 	"pictures"
#define LOGFILES 	"logfiles"
#define TEMPFILES	"tempfiles"  /* Process Tempfiles Stored Here! */
#define INFOFILESDIR    "infofiles"
#define BOTLIST         "botlist"
#define USERPROTECT     "protect_list" /* Protected USer List */

/* Screen Files */

#define MOTD1 		"motd1"      /* Opening Screen (Pre-Login) [motd1.x] */
#define MOTD2 		"motd2"      /* Opening Screen (Post-Login) */
#define NEWMAILFILE     "newmail"
#define LEAVEFILE       "logoff"
#define NEWSFILE 	"news"
#define NEWSWIRE	"/tmp/news.ascii"  /* JKB's Underground News Service */
#define NEWBIEFILE 	"new-user"
#define STAFFFILE 	"staff-list"
#define SRULESFILE 	"staff-rules"
#define RULESFILE 	"user-rules"
#define RANKSFILE 	"ranks"
#define SETMENUFILE     "setmenu"
#define KILLEDFILE 	"killed"
#define QUITSCREEN	"logoff"
#define WELCOMEUSER     "welcome" /* Shown when a new user logs on, otherwise */
                                 /*  auto-promote steps are shown!           */
/* Other Files */

#define CONFIGFILE   "config"    
#define SITEBAN      "siteban"   
#define SITEMASK     "sitemask"
#define NEWSITEBAN   "newsiteban"
#define WHOSITEBAN   "whositeban"
#define USERBAN      "userban"
#define SYSLOG       "syslog"
#define LOGINLOG     "loginlog"
#define USERLIST     "userlist"
#define POKERFILE    "pokerhelp"
#define PICLISTFILE  "piclist"
#define HANGDICT     "hangman_words"
#define WELCOMEMAIL  "welcome-mail"
#define SOCIALFILE   "socials.dat"
#define TALKERFILE   "talkerlist"
#define MUDFILE      "mudlist"
#define EWTOOFILE    "ewtoolist"
#define NUTSFILE     "nutslist"
#define ITEMFILE     "itemlist"  /* Future Use */
#define PIDFILE      "moenuts.pid"  /* Used For The Kill Script Etc */

/* Object Files */
#define OBJFILES   "objectfiles"
#define USEROBJECTS "objects"

/* Object Stuff */

#define MAX_USER_OBJECTS 30
#define MAX_ROOM_OBJECTS 15

/* Talker Global Variables */

#define TALKERNAME    "A New Talker"             /* Talker's Full Name       */
#define SHORTNAME     "A New Talker"             /* Talker's Short Name      */
#define TALKERADDR    "myhost.com:9996"          /* Talker's Address         */
#define TALKERURL     "http://myhost.com/~me"    /* Talker's Homepage URL    */
#define DEFAULTDESC   "is new here."             /* Default New User Desc    */
#define BANKBALANCE   500                        /* Starting Bank Balance    */
#define CREDIT_RATE   3                          /* Credits Per Minute       */
#define MAX_IDLE_TIME 15                         /* Mins To Idle & Get Creds */
#define USE_FORTUNE   0                          /* 0 = QOTD, 1 = Fortune    */
#define FORTUNE_COMMAND    "/usr/games/fortune"  /* Fortune Path/Command     */
#define SENDMAILCMD        "/usr/sbin/sendmail %s < %s" /* Mail Send Command */
#define DICTIONARY_SCRIPT  "scripts/dictlookup"  /* Dictionary Script        */
#define USE_SOCIALS_ON_SELF 1  /* Can a user use a social on themselves 1=yes 0=no */

/* Note: SENDMAILCMD -> You Might Also Try:  "sendmail %s < %s" if your */
/*        server does not use Qmail like tdf.ca does.                    */

/* Email Signature For Smail Forwarding */

char *email_signature="(A Moenuts Talker)\n";

/* Email Signature For Gateway Email Messages */

char *gate_email_signature="(myhost.com:9996)\n";

/*****************************************************************************/
/* Note, the talker address variable is used in sending Internet Email as    */
/* a reference to where the message originated from so the user has a little */
/* more of a clue incase somehow the user's set email address slips through  */
/* the email checker as a valid email address when it's really bogus!        */
/* This way a person getting the email knows they can telnet to the talker's */
/* Address and get in contact with the named user somehow.                   */
/*****************************************************************************/
/* A good idea, if you can swing it, is to ask your system admin for a       */
/* Directory elsewhere on the server where you can keep the auto-backup      */
/* Zip File incase your talker's account ever got hacked and the hacker      */
/* Erases everything, you'll still have that backup elsewhere that the       */
/* Hacker hopefully doesn't kow about :-), I.e  /users/backups/talker        */
/*****************************************************************************/

#define BACKUPFILES   1        /* 1 = Do Nightly Backups, 0 = Don't Bother   */
#define BACKUPDIR     "backups"     /* Directory To Store Backups in         */
#define BACKUPFILE    "talkerbak"   /* Name of archive w/o extention         */

/* Note: BACKUPCMD not implimented yet -- talker still uses .zip! - Moe      */
/* 1st %s = BACKUPDIR, 2nd = BACKUPFILE, 3rd = Logfile Name (BACKUPFILE.log) */
/* Some Versions Of Tar Don't Allow The 'z' option, if not, you can try      */
/* Something like:  #define BACKUPCMD "tar -cvf %s.tar ../moenuts ||         */
/* gzip talkerfiles.tar"   (Or whatever your backupfile name is defined as.  */

#define BACKUPEXT     "tgz"         /* Backup File Extention                 */
#define BACKUPCMD     "tar -zcvf %s/%s.%s ../moenuts/* > %s" /* Archive Cmd  */

/*      System Allowances - Enable Feature = 1, Disable Feature = 0          */

#define ALLOW_USER_ROOMS      1  /* Set to 1 to allow user room creation     */
#define ALLOW_SUICIDE         0  /* Allow the use of the .suicide command    */
#define ALLOW_SYSLOG_TOGGLE   0  /* Allow System Logging To Be Turned Off    */
#define ALLOW_AUTO_PROMOTE    1  /* Allow Auto Promote System                */
#define ALLOW_EXTERNAL_REVIEW 1  /* Allow Reviewing In Other Rooms           */
#define EXTERNAL_IF_CLONE     1  /* Allow Clone Room Reviewing               */
#define ALLOW_EXTERNAL_CBUFF  1  /* Allow .cbuff In Other Rooms              */
#define ALLOW_PUBLIC_CBUFF    1  /* Allow Everyone To .cbuff in Pub Rooms    */
#define ALLOW_ONLINE_STORE    0  /* Future Use                               */

/*                    Other Talker Values And Settings                       */

#define MAX_MOTD1         5    /* Maximum Number Of Random Pre-Login Screens */
#define OUT_BUFF_SIZE     1024 /* Output Buffer Size (1KB)                   */
#define MAX_WORDS         10   /* Maximum Words                              */
#define WORD_LEN          40   /* Maximum Word Length (Miscelaneous)         */
#define ARR_SIZE          1024 /* Miscelaneous Text Buffer Storage Size      */
#define MAX_LINES         30   /* Maximum Lines To Allow In The Editor       */
#define USER_NAME_LEN     12   /* Maximum Size A User's Name Can Be          */
#define USER_ALIAS_LEN    30   /* Maximum User's Level Alias Can Be          */
#define USER_RECAP_LEN    60   /* User's Recap Size (Name + Color Codes)     */
#define USER_DESC_LEN     60   /* Maximum Size User's Desc. Size (w/ Color)  */
#define USER_DESC_TEXT    30   /* Maximum Desc. Size (Not Including Colors)  */
#define USER_PREDESC_LEN  18   /* Maximum Predesc Size                       */
#define BRAND_DESC_LEN    128  /* 255 Chars Should Be More Than Enough?      */
#define AFK_MESG_LEN      60   /* Maximum Size User's AFK Message Can Be     */
#define PHRASE_LEN        60   /* Maximum Size Of Users In/Out Phrase        */
#define PASS_LEN          20   /* (Only First 8 Chars Are Encrypted Though)  */
#define BUFSIZE           1024 /* Editor's Line Buffer Size                  */
#define ROOM_NAME_LEN     30   /* Maximum Size Of A Room's Name (Chars)      */
#define ROOM_RECAP_LEN    120  /* Maximum Room Recap Length (120 = lots)     */
#define ROOM_LABEL_LEN    5    /* Maximum Size Of A Room's Label In Config   */
#define ROOM_DESC_LEN     12   /* Not Really Used Anymore                    */
#define TOPIC_LEN         120  /* Maximum Size Of A Room Topic (Chars)       */
#define MAX_LINKS         20   /* Maximum Number Of Links A Room May Have    */
#define SERV_NAME_LEN     80   /* Domain Name Resolver Buffer Size #1        */
#define SITE_NAME_LEN     80   /* Domain Name Resolver Buffer Size #2        */
#define VERIFY_LEN        20   /* Verification Buffer Size                   */
#define REVIEW_LINES      40   /* Maximum Number Of Room Review Lines        */
#define REVTELL_LINES     20   /* Maximum Number of Tell Review Lines        */
#define REVIEW_LEN        255  /* Review Buffer Maximum Line Length          */ 
#define FAKE_AGE_LEN      30   /* Fake Age Length (Novelty;)                 */
#define ICQ_NUMBER_LEN    30   /* ICQ Number Length (If it's > 30, YIKES! :) */
#define BIRTHDAY_LEN      10   /* Birth Date Length (MM/DD/YYYY)             */
#define EMAIL_SUBJECT_LEN 80   /* 80 Character Subject Is More Than Enough   */
#define URL_LEN           160  /* Who has a URL bigger than 160 Chars?       */
#define EMAIL_ADDR_LEN    160  /* One Hell Of An Email Address :-)           */
#define DNL               12   /* Date Number Length                         */
#define GUARD_PHRASE_LEN  120  /* Guard Dog Phrase Length                    */
#define MAX_FRIENDS       50   /* How Many Friends Maximum On Friend List    */
/*****************************************************************************/

#define SEPERATOR1 "~FT.---------------------------------------------------------------------------."
#define SEPERATOR2 "~FT|---------------------------------------------------------------------------|"
#define SEPERATOR3 "~FT`---------------------------------------------------------------------------'"

/* Some Miscelaneous Costs */

#define TRIVIA_POINT_AMOUNT	100
#define ROOM_KEY_COST           500
#define ROOM_KEY_SELL           500
#define ROOM_OWNER_COST         1500
#define ROOM_OWNER_SELL         1000

/* Login Log Definitians */

#define LASTLOG_LINES	20
#define LASTLOG_LEN     1024

/* Room Access Setup & IDs */

#define PUBLIC 		 0
#define PRIVATE          1
#define FIXED            2
#define FIXED_PUBLIC 	 2
#define FIXED_PRIVATE 	 3
#define PERSONAL         4
#define FIXED_PERSONAL 	 6

/* User Status Flags */

#define SCUM 		16
#define JAILED          32
#define FROZEN		64

/* Gender Flags (Pre 1.73pr) */
#define NEUTER          2
#define FEMALE          4 
#define MALE 		8

/* New Gender Defines */
#define GEN_NEUTER      0
#define GEN_MALE        1
#define GEN_FEMALE      2

/* Invisible Flags */
#define USERINVIS	4
#define USERVIS 	8

/* Message Type Flags */

#define ALL_TELLS        1
#define OUT_ROOM_TELLS   2
#define SHOUT_MSGS       4
#define LOGON_MSGS       8
#define BEEP_MSGS        16
#define BCAST_MSGS       32
#define ROOM_PICTURE 	 64
#define MOST_MSGS        126
#define WIZARD_MSGS 	 128
#define ATMOS_MSGS       256

/* User Type & True/False Flags */

#define USER_TYPE        0
#define CLONE_TYPE       1
#define BOT_TYPE	 2
#define ZERO             0
#define TRUE             1
#define FALSE            0

/* Clone Flags */

#define CLONE_HEAR_NOTHING 0
#define CLONE_HEAR_SWEARS  1
#define CLONE_HEAR_ALL     2

/* Neil: The elements vis, ignall, prompt, command_mode etc could all be
   bits in one flag variable as they're only ever 0 or 1, but I tried it
   and it made the code unreadable. Better to waste a few bytes */

/* Spod */
struct user_dir_struct {
  char name[USER_NAME_LEN+1],date[80];
  short int level;
  struct user_dir_struct *next,*prev;
  };
struct user_dir_struct *first_dir_entry,*last_dir_entry;

struct user_struct {
	char name[USER_NAME_LEN+1];
	char desc[USER_DESC_LEN+1];
	char pass[PASS_LEN+6];
	char in_phrase[PHRASE_LEN+1],out_phrase[PHRASE_LEN+1],ignuser[USER_NAME_LEN];
	char buff[BUFSIZE],site[81],last_site[81],site_port[21],page_file[81];
	char mail_to[WORD_LEN+1],revbuff[REVTELL_LINES][REVIEW_LEN+2];
	char afk_mesg[AFK_MESG_LEN+1],inpstr_old[REVIEW_LEN+1],samesite_check_store[ARR_SIZE];
	struct room_struct *room,*invite_room;
	int type,port,login,socket,attempts,buffpos,filepos;
	int vis,ignall,prompt,command_mode,muzzled,gaged,charmode_echo; 
	int level,misc_op,remote_com,edit_line,charcnt,warned;
	int accreq,last_login_len,ignall_store,clone_hear,afk;
	int edit_op,colour,chkrev,ignore,revline,samesite_all_store;
	int drunk,gender;
	time_t last_input,last_login,total_login,read_mail;
        int c4_board[8][7], c4_turn, c4_moves;
	char *malloc_start,*malloc_end;
	struct user_struct *prev,*next,*owner,*c4_opponent;
	int wrap,mashed,invis,age,whostyle,last_room;
        char email[EMAIL_ADDR_LEN+1],homepage[URL_LEN+1];
	int rules,news,default_wrap,shackled,shackle_level,hidden;
        int high_ascii,fmail,start_script,temp_level,confirm;
        int first,twin,tlose,tdraw,roombanned;
        char level_alias[USER_ALIAS_LEN+1];
        char recap[USER_RECAP_LEN+1];
        char alias[USER_RECAP_LEN+1];
	char login_room[ROOM_NAME_LEN+1];
	char roomname[ROOM_NAME_LEN+1];
	char roomtopic[TOPIC_LEN+1];
	char roomgiver[USER_NAME_LEN+1];
	int hideroom;
	char array[10];
	struct user_struct *opponent;
	struct po_player *pop; /*** poker ***/
	int poker_wins;
        char birthday[BIRTHDAY_LEN+1];
        char icq[ICQ_NUMBER_LEN+1];
        /* Branding */
        int branded;
        char branded_by[USER_NAME_LEN+1];
        char brand_desc[BRAND_DESC_LEN+1];
        /* Callaring */
        char callared_by[USER_NAME_LEN+1];
        char callared_desc[BRAND_DESC_LEN+1];
        int callared;
	int bdsm_type;
	int bdsm_life_type;
	int mailbox_limit;
        char fakeage[FAKE_AGE_LEN+1];
        char married[USER_NAME_LEN+1];
	char predesc[USER_PREDESC_LEN+1];
	/* Misc-Input Storage */
	char inpstr[ARR_SIZE+2];
	/* Craps Varaibles */
	int cpot, cwager;
	/* Hangman Variables */
	char hang_word[WORD_LEN],hang_word_show[WORD_LEN],hang_guess[WORD_LEN];
	int hang_stage;
	/* Fight Variables */
        int fight_win,fight_lose,fight_draw;
	/* Paintball Variables */
	int paintballs,hps,painted,splatters;
	/* Email Gateway Variables */
	char email_subject[EMAIL_SUBJECT_LEN];
	char to_email[121];
        /* Bank Account Info */
        int bank_balance;
        int bank_update;
        int bank_temp;
        /* Other Stuff */
	int hide_email,bet,win;
	char owned_by[USER_NAME_LEN+1];
	/* Steal */
	int attempttime;
	int stealtime;
	int textcolor;
        /* Objects */
        int object_count[MAX_USER_OBJECTS+1];
        struct object_struct *objects[MAX_USER_OBJECTS+1];
        struct object_struct *wrap_object;
	/* Spod Stuff */
	int logons,stolen;
        /* Affection Stuff */
	int affection, afftime;
        char friends[MAX_FRIENDS][USER_NAME_LEN+1];
};

typedef struct user_struct* UR_OBJECT;
UR_OBJECT user_first,user_last,botuser;

struct room_struct {
	char name[ROOM_NAME_LEN+1];
	char recap[ROOM_RECAP_LEN+1];
	char owner2[USER_NAME_LEN+1];
	char label[ROOM_LABEL_LEN+1];
	char desc[ROOM_DESC_LEN+1];
	char topic[TOPIC_LEN+1];
	char owner[USER_NAME_LEN+1];
	char map[ROOM_LABEL_LEN+1];
	char revbuff[REVIEW_LINES][REVIEW_LEN+2];
	char atmos[ATMOSPHERES][ATMOS_LEN+2];
	int access; /* public , private etc */
	int revline; /* line number for review */
	int mesg_cnt;
        int hidden;
	int nobuff;
	char link_label[MAX_LINKS][ROOM_LABEL_LEN+1]; /* temp store for parse */
	struct room_struct *link[MAX_LINKS];
	struct room_struct *next,*prev;
        /* Objects */
        int object_count[MAX_ROOM_OBJECTS+1];
        struct object_struct *objects[MAX_ROOM_OBJECTS+1];
        /* Guard Dog Stuff */
	int guarddog;
        char guarddog_name[USER_RECAP_LEN+1];
        char guarddog_phrase[GUARD_PHRASE_LEN+1];
	};

typedef struct room_struct *RM_OBJECT;

RM_OBJECT room_first,room_last;
RM_OBJECT create_room();

/* Object Stuff */
char *nosuchobject="There is no such object.\n";

/* Login Room Config For Special Cases */
char *jail_room="Hell";
char *newbie_room="None";

/*
Colcode values equal the following:

RESET,BOLD,BLINK,REVERSE

Foreground & background colours in order..

BLACK,RED,GREEN,YELLOW/ORANGE,
BLUE,MAGENTA,TURQUIOSE,WHITE 
*/

#define NUM_COLS 36

char *colcode[NUM_COLS]={
/* Standard stuff */
"\033[0m","\033[1m","\033[4m","\033[5m","\033[7m",
/* Foreground colour */
"\033[0;30m","\033[0;31m","\033[0;32m","\033[0;33m",
"\033[0;34m","\033[0;35m","\033[0;36m","\033[0;37m",
/* Background colour */
"\033[40m","\033[41m","\033[42m","\033[43m",
"\033[44m","\033[45m","\033[46m","\033[47m",
/* Ascii Bell */
"\07",
/* Ansi Music -- Doesn't work in most terminals - has dif. meaning in vt100 */
"\033[M","\0x0E",
/* Clear Screen */
"\033[2J\033[H",  
/* Foreground colour */
"\033[1;30m","\033[1;31m","\033[1;32m","\033[1;33m",
"\033[1;34m","\033[1;35m","\033[1;36m","\033[1;37m",
/* addict adds some null ptrs here for a very good reason ;) */
"", "", ""
};

/* Codes used in a string to produce the colours when prepended with a '~' */
char *colcom[NUM_COLS]={
/* Special Attributes */
"RS","OL","UL","LI","RV",
/* Forground Colors */
"FK","FR","FG","FY",
"FB","FM","FT","FW",
/* Background Colors */
"BK","BR","BG","BY",
"BB","BM","BT","BW",
/* Sound & Music */
"BP","MS","ME",
/* Special Attributes */
"CS",
"CK","CR","CG","CY",
"CB","CM","CT","CW",
/* Addict adds some random stuffs */
"RF", "RB", "RC"
};

char *month[12]={
"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec" };

char *month_long[12]={
"January", "February",  "March",     "April",    "May",      "June",
"July",     "August",   "September", "October",  "November", "December" };

char *day[7]={ "Sun","Mon","Tue","Wed","Thu","Fri","Sat" };

char *day_long[7]={
"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday" };

char *noyes1[]  = { " No","Yes" };
char *noyes2[]  = { "No ","Yes" };
char *offon[]   = { "Off","On " };

/* Extentions for screen files                                             */

/* Note, the 'anim' file type isn't used at the moment, but I may decide
   to use it in the future if it's shown that some people have a harder
   time viewing ansi animation, but can view regular ansi graphics
   and/or high ascii characters.  This depends on the telnet client they're
   using to connect to the Moenuts talker.  I may find a need to separate
   ANSI files into two catagories.                                         */

char *fileext[] = { "ascii","ansi","anim" }; 

/* These MUST be in lower case - the contains_swearing() function converts
   the string to be checked to lower case before it compares it against
   these. Also even if you dont want to ban any words you must keep the 
   star as the first element in the array. */

/* These words will also be used in the valid_email() sub */
char *swear_words[]={
"nigger","niger","sigh","sighs","blah","*"
/* "nigger","niger","fuck","shit","whore","bitch","cunt","asshole","*" */
};

/* Define Who List Names */

#define MAX_WHOS 	8

char *who_list_style[MAX_WHOS+1]={
"~CRNone (System Default)",
"~CMShort Who ~FB-> ~FGNames Only",
"~CMHouse Of Pleasure Who List Style",
"~CYStairway To Heaven Who List Style",
"~CGMo~CGenuts Byroom Who List Style",
"~CMShackles Who List",
"~CTMedival Times Who List",
"~RSCamelot Who List",
"~FGDark ~CRGarden ~FGWho List"
};

/* Note - The extra ~CG in the moenuts who name is to stop the recap when */
/* Moe Is Online ;) */

/* BDSM Caste Types */

#define MAX_BDSM_TYPES 	6

char *bdsm_types[MAX_BDSM_TYPES]={
"None","Dominant","submissive","Slave","X S/switch","Vanilla"
};

char *bdsm_type_desc[MAX_BDSM_TYPES]={
"Not Caste Set",
"A Dominant person.",
"A submissive person.",
"A Dom/Domme's slave.",
"Both Dominant/submissive (switches)",
"Someone who isn't into the BDSM Lifestyle."
};

/* BDSM Lifestyle Types */

#define MAX_BDSM_LIFE_TYPES 3

char *bdsm_life_types[MAX_BDSM_LIFE_TYPES]={
"Straight","Gay","Bi-Sexual"
};

char *bdsm_life_type_desc[MAX_BDSM_LIFE_TYPES]={
"A Straight Person","A Gay/Lesbian Person","A Bi-Sexual Person"
};

/* gender arrays to make text easily for socials and strings  */

/* Lowercase */
char *himher[]={"it","him","her"};
char *hisher[]={"its","his","her"};
char *heshe[]={"it","he","she"};
char *kingqueen[]={"jester","king","queen"};
/* Uppercase */
char *uchimher[]={"It","Him","Her"};
char *uchisher[]={"Its","His","Her"};
char *ucheshe[]={"It","He","She"};
char *uckingqueen[]={"Jester","King","Queen"};

/* More Variable Stuffs */

char file[ARR_SIZE];
char verification[SERV_NAME_LEN+1];
char wizrevbuff[REVIEW_LINES][REVIEW_LEN+2];
char shoutrevbuff[REVIEW_LINES][REVIEW_LEN+2];
char mutterrevbuff[REVIEW_LINES][REVIEW_LEN+2];
char lastlogbuff[LASTLOG_LINES][LASTLOG_LEN+2];
char text[(ARR_SIZE*3)+2];
char afktext[ARR_SIZE];
char word[MAX_WORDS][WORD_LEN+1];
char wrd[15][81];
char progname[40],confile[40];
char tempstr[(ARR_SIZE*2)+1];
char newstopic[TOPIC_LEN+2];
time_t boot_time, rs_announce,rs_which;
jmp_buf jmpvar;

int port[2],listen_sock[2],wizport_level,minlogin_level;
int colour_def,password_echo,ignore_sigterm,wrevline,srevline,mutrevline;
int max_users,max_clones,num_of_users,num_of_logins,heartbeat;
int login_idle_time,user_idle_time,config_line,word_count;
int tyear,tmonth,tday,tmday,twday,thour,tmin,tsec;
int mesg_life,system_logging,prompt_def,no_prompt;
int force_listen,gatecrash_level,min_private_users;
int ignore_mp_level,rem_user_maxlevel,rem_user_deflevel;
int destructed,mesg_check_hour,mesg_check_min;
int keepalive_interval,ban_swearing,crash_action;
int time_out_afks,allow_caps_in_name,kill_idle_users,rs_countdown;
int charecho_def,time_out_maxlevel,num_maps,user_count,atmos_trigger,credit_timer;
int lastlogline,sys_allow_autopromote,sys_allow_suicide;
int SYS_FORTUNE=USE_FORTUNE; /* Set Fortune Flag */
int system_trivia=0;

UR_OBJECT rs_user;

/* If your compiler complains about the sys_errlist, you may need to
   either remove the #if and #endif lines, (Comment them out or whatever)
   or comment out the entire block of three lines below.  I know Redhat
   Linux has a problem with this, in which you need to remove the
   #if and #endif lines but leave the extern statement there. */

/* extern char *const sys_errlist[]; */

char *long_date();

/* Big letter array map - for greet() */
int biglet[26][5][5]={
	{{0,1,1,1,0},{1,0,0,0,1},{1,1,1,1,1},{1,0,0,0,1},{1,0,0,0,1}},
	{{1,1,1,1,0},{1,0,0,0,1},{1,1,1,1,0},{1,0,0,0,1},{1,1,1,1,0}},
	{{0,1,1,1,1},{1,0,0,0,0},{1,0,0,0,0},{1,0,0,0,0},{0,1,1,1,1}},
	{{1,1,1,1,0},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{1,1,1,1,0}},
	{{1,1,1,1,1},{1,0,0,0,0},{1,1,1,1,0},{1,0,0,0,0},{1,1,1,1,1}},
	{{1,1,1,1,1},{1,0,0,0,0},{1,1,1,1,0},{1,0,0,0,0},{1,0,0,0,0}},
	{{0,1,1,1,0},{1,0,0,0,0},{1,0,1,1,0},{1,0,0,0,1},{0,1,1,1,0}},
	{{1,0,0,0,1},{1,0,0,0,1},{1,1,1,1,1},{1,0,0,0,1},{1,0,0,0,1}},
	{{0,1,1,1,0},{0,0,1,0,0},{0,0,1,0,0},{0,0,1,0,0},{0,1,1,1,0}},
	{{0,0,0,0,1},{0,0,0,0,1},{0,0,0,0,1},{1,0,0,0,1},{0,1,1,1,0}},
	{{1,0,0,0,1},{1,0,0,1,0},{1,0,1,0,0},{1,0,0,1,0},{1,0,0,0,1}},
	{{1,0,0,0,0},{1,0,0,0,0},{1,0,0,0,0},{1,0,0,0,0},{1,1,1,1,1}},
	{{1,0,0,0,1},{1,1,0,1,1},{1,0,1,0,1},{1,0,0,0,1},{1,0,0,0,1}},
	{{1,0,0,0,1},{1,1,0,0,1},{1,0,1,0,1},{1,0,0,1,1},{1,0,0,0,1}},
	{{0,1,1,1,0},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{0,1,1,1,0}},
	{{1,1,1,1,0},{1,0,0,0,1},{1,1,1,1,0},{1,0,0,0,0},{1,0,0,0,0}},
	{{0,1,1,1,0},{1,0,0,0,1},{1,0,1,0,1},{1,0,0,1,1},{0,1,1,1,0}},
	{{1,1,1,1,0},{1,0,0,0,1},{1,1,1,1,0},{1,0,0,1,0},{1,0,0,0,1}},
	{{0,1,1,1,1},{1,0,0,0,0},{0,1,1,1,0},{0,0,0,0,1},{1,1,1,1,0}},
	{{1,1,1,1,1},{0,0,1,0,0},{0,0,1,0,0},{0,0,1,0,0},{0,0,1,0,0}},
	{{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{1,0,0,0,1},{1,1,1,1,1}},
	{{1,0,0,0,1},{1,0,0,0,1},{0,1,0,1,0},{0,1,0,1,0},{0,0,1,0,0}},
	{{1,0,0,0,1},{1,0,0,0,1},{1,0,1,0,1},{1,1,0,1,1},{1,0,0,0,1}},
	{{1,0,0,0,1},{0,1,0,1,0},{0,0,1,0,0},{0,1,0,1,0},{1,0,0,0,1}},
	{{1,0,0,0,1},{0,1,0,1,0},{0,0,1,0,0},{0,0,1,0,0},{0,0,1,0,0}},
	{{1,1,1,1,1},{0,0,0,1,0},{0,0,1,0,0},{0,1,0,0,0},{1,1,1,1,1}}
	};

/* Symbol array map - for greet() */
int bigsym[32][5][5]={
	{{0,0,1,0,0},{0,0,1,0,0},{0,0,1,0,0},{0,0,0,0,0},{0,0,1,0,0}},
	{{0,1,0,1,0},{0,1,0,1,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0}},
	{{0,1,0,1,0},{1,1,1,1,1},{0,1,0,1,0},{1,1,1,1,1},{0,1,0,1,0}},
	{{0,1,1,1,1},{1,0,1,0,0},{0,1,1,1,0},{0,0,1,0,1},{1,1,1,1,0}},
	{{1,1,0,0,1},{1,1,0,1,0},{0,0,1,0,0},{0,1,0,1,1},{1,0,0,1,1}},
	{{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0}},
	{{0,0,1,0,0},{0,0,1,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0}},
	{{0,0,1,1,0},{0,1,0,0,0},{0,1,0,0,0},{0,1,0,0,0},{0,0,1,1,0}},
	{{0,1,1,0,0},{0,0,0,1,0},{0,0,0,1,0},{0,0,0,1,0},{0,1,1,0,0}},
	{{1,0,1,0,1},{0,1,1,1,0},{1,1,1,1,1},{0,1,1,1,0},{1,0,1,0,1}},
	{{0,0,1,0,0},{0,0,1,0,0},{1,1,1,1,1},{0,0,1,0,0},{0,0,1,0,0}},
	{{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,1,0,0,0},{1,1,0,0,0}},
	{{0,0,0,0,0},{0,0,0,0,0},{1,1,1,1,1},{0,0,0,0,0},{0,0,0,0,0}},
	{{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{1,1,0,0,0},{1,1,0,0,0}},
	{{0,0,0,0,1},{0,0,0,1,0},{0,0,1,0,0},{0,1,0,0,0},{1,0,0,0,0}},
	{{0,1,1,1,0},{1,0,0,1,1},{1,0,1,0,1},{1,1,0,0,1},{0,1,1,1,0}},
	{{0,0,1,0,0},{0,1,1,0,0},{0,0,1,0,0},{0,0,1,0,0},{0,1,1,1,0}},
	{{1,1,1,1,0},{0,0,0,0,1},{0,1,1,1,0},{1,0,0,0,0},{1,1,1,1,1}},
	{{1,1,1,1,0},{0,0,0,0,1},{0,1,1,1,0},{0,0,0,0,1},{1,1,1,1,0}},
	{{0,0,1,1,0},{0,1,0,0,0},{1,0,0,1,0},{1,1,1,1,1},{0,0,0,1,0}},
	{{1,1,1,1,1},{1,0,0,0,0},{1,1,1,1,0},{0,0,0,0,1},{1,1,1,1,0}},
	{{0,1,1,1,0},{1,0,0,0,0},{1,1,1,1,0},{1,0,0,0,1},{0,1,1,1,0}},
	{{1,1,1,1,1},{0,0,0,0,1},{0,0,0,1,0},{0,0,1,0,0},{0,1,0,0,0}},
	{{0,1,1,1,0},{1,0,0,0,1},{0,1,1,1,0},{1,0,0,0,1},{0,1,1,1,0}},
	{{1,1,1,1,1},{1,0,0,0,1},{1,1,1,1,1},{0,0,0,0,1},{0,0,0,0,1}},
	{{0,0,0,0,0},{0,0,1,0,0},{0,0,0,0,0},{0,0,1,0,0},{0,0,0,0,0}},
	{{0,0,0,0,0},{0,0,1,0,0},{0,0,0,0,0},{0,0,1,0,0},{0,1,0,0,0}},
	{{0,0,0,1,0},{0,0,1,0,0},{0,1,0,0,0},{0,0,1,0,0},{0,0,0,1,0}},
	{{0,0,0,0,0},{1,1,1,1,1},{0,0,0,0,0},{1,1,1,1,1},{0,0,0,0,0}},
	{{0,1,0,0,0},{0,0,1,0,0},{0,0,0,1,0},{0,0,1,0,0},{0,1,0,0,0}},
	{{0,1,1,1,1},{0,0,0,0,1},{0,0,1,1,1},{0,0,0,0,0},{0,0,1,0,0}},
	{{0,1,0,0,0},{1,0,1,1,1},{1,0,1,0,1},{1,0,1,1,1},{0,1,1,1,0}}
	};

/**** Poker ****/

#define GAME_NAME_LEN 12

struct po_player {
  int hand[5];
  int putin;
  int touched;
  int rank;
  struct user_struct *user;
  struct po_game *game;
  struct po_player *prev, *next;
};

struct po_game {
  char name[GAME_NAME_LEN+1];
  struct room_struct *room;
  struct po_player *players;
  struct po_player *dealer;
  struct po_player *opened;
  int num_players;
  int num_raises;
  int in_players;
  int newdealer;
  int deck[52];
  int top_card;
  int state;
  int bet;
  int pot;
  struct po_player *curr_player, *first_player, *last_player;
  struct po_game *prev, *next;
};

struct po_game *po_game_first, *po_game_last;

/******* Cards **********/

#define CARD_LENGTH 5
#define CARD_WIDTH  7  

static char *cards[52][CARD_LENGTH]={
  {".-----.",
   "| ~OL2~RS   |",
   "|  ~OLC~RS  |",
   "|   ~OL2~RS |",
   "`-----'"},
  {".-----.",
   "| ~OL3~RS   |",
   "|  ~OLC~RS  |",
   "|   ~OL3~RS |",
   "`-----'"},
  {".-----.",
   "| ~OL4~RS   |",
   "|  ~OLC~RS  |",
   "|   ~OL4~RS |",
   "`-----'"},
  {".-----.",
   "| ~OL5~RS   |",
   "|  ~OLC~RS  |",
   "|   ~OL5~RS |",
   "`-----'"},
  {".-----.",
   "| ~OL6~RS   |",
   "|  ~OLC~RS  |",
   "|   ~OL6~RS |",
   "`-----'"},
  {".-----.",
   "| ~OL7~RS   |",
   "|  ~OLC~RS  |",
   "|   ~OL7~RS |",
   "`-----'"},
  {".-----.",
   "| ~OL8~RS   |",
   "|  ~OLC~RS  |",
   "|   ~OL8~RS |",
   "`-----'"},
  {".-----.",
   "| ~OL9~RS   |",
   "|  ~OLC~RS  |",
   "|   ~OL9~RS |",
   "`-----'"},
  {".-----.",
   "| ~OL10~RS  |",
   "|  ~OLC~RS  |",
   "|  ~OL10~RS |",
   "`-----'"},
  {".-----.",
   "| ~OLJ~RS   |",
   "|  ~OLC~RS  |",
   "|   ~OLJ~RS |",
   "`-----'"},
  {".-----.",
   "| ~OLQ~RS   |",
   "|  ~OLC~RS  |",
   "|   ~OLQ~RS |",
   "`-----'"},
  {".-----.",
   "| ~OLK~RS   |",
   "|  ~OLC~RS  |",
   "|   ~OLK~RS |",
   "`-----'"},
  {".-----.",
   "| ~OLA~RS   |",
   "|  ~OLC~RS  |",
   "|   ~OLA~RS |",
   "`-----'"},
  {".-----.",
   "| ~FR2~RS   |",
   "|  ~FRd~RS  |",
   "|   ~FR2~RS |",
   "`-----'"},
  {".-----.",
   "| ~FR3~RS   |",
   "|  ~FRd~RS  |",
   "|   ~FR3~RS |",
   "`-----'"},
  {".-----.",
   "| ~FR4~RS   |",
   "|  ~FRd~RS  |",
   "|   ~FR4~RS |",
   "`-----'"},
  {".-----.",
   "| ~FR5~RS   |",
   "|  ~FRd~RS  |",
   "|   ~FR5~RS |",
   "`-----'"},
  {".-----.",
   "| ~FR6~RS   |",
   "|  ~FRd~RS  |",
   "|   ~FR6~RS |",
   "`-----'"},
  {".-----.",
   "| ~FR7~RS   |",
   "|  ~FRd~RS  |",
   "|   ~FR7~RS |",
   "`-----'"},
  {".-----.",
   "| ~FR8~RS   |",
   "|  ~FRd~RS  |",
   "|   ~FR8~RS |",
   "`-----'"},
  {".-----.",
   "| ~FR9~RS   |",
   "|  ~FRd~RS  |",
   "|   ~FR9~RS |",
   "`-----'"},
  {".-----.",
   "| ~FR10~RS  |",
   "|  ~FRd~RS  |",
   "|  ~FR10~RS |",
   "`-----'"},
  {".-----.",
   "| ~FRJ~RS   |",
   "|  ~FRd~RS  |",
   "|   ~FRJ~RS |",
   "`-----'"},
  {".-----.",
   "| ~FRQ~RS   |",
   "|  ~FRd~RS  |",
   "|   ~FRQ~RS |",
   "`-----'"},
  {".-----.",
   "| ~FRK~RS   |",
   "|  ~FRd~RS  |",
   "|   ~FRK~RS |",
   "`-----'"},
  {".-----.",
   "| ~FRA~RS   |",
   "|  ~FRd~RS  |",
   "|   ~FRA~RS |",
   "`-----'"},
  {".-----.",
   "| ~FR2~RS   |",
   "|  ~FRh~RS  |",
   "|   ~FR2~RS |",
   "`-----'"},
  {".-----.",
   "| ~FR3~RS   |",
   "|  ~FRh~RS  |",
   "|   ~FR3~RS |",
   "`-----'"},
  {".-----.",
   "| ~FR4~RS   |",
   "|  ~FRh~RS  |",
   "|   ~FR4~RS |",
   "`-----'"},
  {".-----.",
   "| ~FR5~RS   |",
   "|  ~FRh~RS  |",
   "|   ~FR5~RS |",
   "`-----'"},
  {".-----.",
   "| ~FR6~RS   |",
   "|  ~FRh~RS  |",
   "|   ~FR6~RS |",
   "`-----'"},
  {".-----.",
   "| ~FR7~RS   |",
   "|  ~FRh~RS  |",
   "|   ~FR7~RS |",
   "`-----'"},
  {".-----.",
   "| ~FR8~RS   |",
   "|  ~FRh~RS  |",
   "|   ~FR8~RS |",
   "`-----'"},
  {".-----.",
   "| ~FR9~RS   |",
   "|  ~FRh~RS  |",
   "|   ~FR9~RS |",
   "`-----'"},
  {".-----.",
   "| ~FR10~RS  |",
   "|  ~FRh~RS  |",
   "|  ~FR10~RS |",
   "`-----'"},
  {".-----.",
   "| ~FRJ~RS   |",
   "|  ~FRh~RS  |",
   "|   ~FRJ~RS |",
   "`-----'"},
  {".-----.",
   "| ~FRQ~RS   |",
   "|  ~FRh~RS  |",
   "|   ~FRQ~RS |",
   "`-----'"},
  {".-----.",
   "| ~FRK~RS   |",
   "|  ~FRh~RS  |",
   "|   ~FRK~RS |",
   "`-----'"},
  {".-----.",
   "| ~FRA~RS   |",
   "|  ~FRh~RS  |",
   "|   ~FRA~RS |",
   "`-----'"},
  {".-----.",
   "| ~OL2~RS   |",
   "|  ~OLS~RS  |",
   "|   ~OL2~RS |",
   "`-----'"},
  {".-----.",
   "| ~OL3~RS   |",
   "|  ~OLS~RS  |",
   "|   ~OL3~RS |",
   "`-----'"},
  {".-----.",
   "| ~OL4~RS   |",
   "|  ~OLS~RS  |",
   "|   ~OL4~RS |",
   "`-----'"},
  {".-----.",
   "| ~OL5~RS   |",
   "|  ~OLS~RS  |",
   "|   ~OL5~RS |",
   "`-----'"},
  {".-----.",
   "| ~OL6~RS   |",
   "|  ~OLS~RS  |",
   "|   ~OL6~RS |",
   "`-----'"},
  {".-----.",
   "| ~OL7~RS   |",
   "|  ~OLS~RS  |",
   "|   ~OL7~RS |",
   "`-----'"},
  {".-----.",
   "| ~OL8~RS   |",
   "|  ~OLS~RS  |",
   "|   ~OL8~RS |",
   "`-----'"},
  {".-----.",
   "| ~OL9~RS   |",
   "|  ~OLS~RS  |",
   "|   ~OL9~RS |",
   "`-----'"},
  {".-----.",
   "| ~OL10~RS  |",
   "|  ~OLS~RS  |",
   "|  ~OL10~RS |",
   "`-----'"},
  {".-----.",
   "| ~OLJ~RS   |",
   "|  ~OLS~RS  |",
   "|   ~OLJ~RS |",
   "`-----'"},
  {".-----.",
   "| ~OLQ~RS   |",
   "|  ~OLS~RS  |",
   "|   ~OLQ~RS |",
   "`-----'"},
  {".-----.",
   "| ~OLK~RS   |",
   "|  ~OLS~RS  |",
   "|   ~OLK~RS |",
   "`-----'"},
  {".-----.",
   "| ~OLA~RS   |",
   "|  ~OLS~RS  |",
   "|   ~OLA~RS |",
   "`-----'"}
};

static char *cards_ansi[52][CARD_LENGTH]={
  {"ÚÄÄÄÄÄ¿",
   "³ ~OL2~RS   ³",
   "³  ~OLC~RS  ³",
   "³   ~OL2~RS ³",
   "ÀÄÄÄÄÄÙ"},
  {"ÚÄÄÄÄÄ¿",
   "³ ~OL3~RS   ³",
   "³  ~OLC~RS  ³",
   "³   ~OL3~RS ³",
   "ÀÄÄÄÄÄÙ"},
  {"ÚÄÄÄÄÄ¿",
   "³ ~OL4~RS   ³",
   "³  ~OLC~RS  ³",
   "³   ~OL4~RS ³",
   "ÀÄÄÄÄÄÙ"},
  {"ÚÄÄÄÄÄ¿",
   "³ ~OL5~RS   ³",
   "³  ~OLC~RS  ³",
   "³   ~OL5~RS ³",
   "ÀÄÄÄÄÄÙ"},
  {"ÚÄÄÄÄÄ¿",
   "³ ~OL6~RS   ³",
   "³  ~OLC~RS  ³",
   "³   ~OL6~RS ³",
   "ÀÄÄÄÄÄÙ"},
  {"ÚÄÄÄÄÄ¿",
   "³ ~OL7~RS   ³",
   "³  ~OLC~RS  ³",
   "³   ~OL7~RS ³",
   "ÀÄÄÄÄÄÙ"},
  {"ÚÄÄÄÄÄ¿",
   "³ ~OL8~RS   ³",
   "³  ~OLC~RS  ³",
   "³   ~OL8~RS ³",
   "ÀÄÄÄÄÄÙ"},
  {"ÚÄÄÄÄÄ¿",
   "³ ~OL9~RS   ³",
   "³  ~OLC~RS  ³",
   "³   ~OL9~RS ³",
   "ÀÄÄÄÄÄÙ"},
  {"ÚÄÄÄÄÄ¿",
   "³ ~OL10~RS  ³",
   "³  ~OLC~RS  ³",
   "³  ~OL10~RS ³",
   "ÀÄÄÄÄÄÙ"},
  {"ÚÄÄÄÄÄ¿",
   "³ ~OLJ~RS   ³",
   "³  ~OLC~RS  ³",
   "³   ~OLJ~RS ³",
   "ÀÄÄÄÄÄÙ"},
  {"ÚÄÄÄÄÄ¿",
   "³ ~OLQ~RS   ³",
   "³  ~OLC~RS  ³",
   "³   ~OLQ~RS ³",
   "ÀÄÄÄÄÄÙ"},
  {"ÚÄÄÄÄÄ¿",
   "³ ~OLK~RS   ³",
   "³  ~OLC~RS  ³",
   "³   ~OLK~RS ³",
   "ÀÄÄÄÄÄÙ"},
  {"ÚÄÄÄÄÄ¿",
   "³ ~OLA~RS   ³",
   "³  ~OLC~RS  ³",
   "³   ~OLA~RS ³",
   "ÀÄÄÄÄÄÙ"},
  {"ÚÄÄÄÄÄ¿",
   "³ ~FR2~RS   ³",
   "³  ~FRd~RS  ³",
   "³   ~FR2~RS ³",
   "ÀÄÄÄÄÄÙ"},
  {"ÚÄÄÄÄÄ¿",
   "³ ~FR3~RS   ³",
   "³  ~FRd~RS  ³",
   "³   ~FR3~RS ³",
   "ÀÄÄÄÄÄÙ"},
  {"ÚÄÄÄÄÄ¿",
   "³ ~FR4~RS   ³",
   "³  ~FRd~RS  ³",
   "³   ~FR4~RS ³",
   "ÀÄÄÄÄÄÙ"},
  {"ÚÄÄÄÄÄ¿",
   "³ ~FR5~RS   ³",
   "³  ~FRd~RS  ³",
   "³   ~FR5~RS ³",
   "ÀÄÄÄÄÄÙ"},
  {"ÚÄÄÄÄÄ¿",
   "³ ~FR6~RS   ³",
   "³  ~FRd~RS  ³",
   "³   ~FR6~RS ³",
   "ÀÄÄÄÄÄÙ"},
  {"ÚÄÄÄÄÄ¿",
   "³ ~FR7~RS   ³",
   "³  ~FRd~RS  ³",
   "³   ~FR7~RS ³",
   "ÀÄÄÄÄÄÙ"},
  {"ÚÄÄÄÄÄ¿",
   "³ ~FR8~RS   ³",
   "³  ~FRd~RS  ³",
   "³   ~FR8~RS ³",
   "ÀÄÄÄÄÄÙ"},
  {"ÚÄÄÄÄÄ¿",
   "³ ~FR9~RS   ³",
   "³  ~FRd~RS  ³",
   "³   ~FR9~RS ³",
   "ÀÄÄÄÄÄÙ"},
  {"ÚÄÄÄÄÄ¿",
   "³ ~FR10~RS  ³",
   "³  ~FRd~RS  ³",
   "³  ~FR10~RS ³",
   "ÀÄÄÄÄÄÙ"},
  {"ÚÄÄÄÄÄ¿",
   "³ ~FRJ~RS   ³",
   "³  ~FRd~RS  ³",
   "³   ~FRJ~RS ³",
   "ÀÄÄÄÄÄÙ"},
  {"ÚÄÄÄÄÄ¿",
   "³ ~FRQ~RS   ³",
   "³  ~FRd~RS  ³",
   "³   ~FRQ~RS ³",
   "ÀÄÄÄÄÄÙ"},
  {"ÚÄÄÄÄÄ¿",
   "³ ~FRK~RS   ³",
   "³  ~FRd~RS  ³",
   "³   ~FRK~RS ³",
   "ÀÄÄÄÄÄÙ"},
  {"ÚÄÄÄÄÄ¿",
   "³ ~FRA~RS   ³",
   "³  ~FRd~RS  ³",
   "³   ~FRA~RS ³",
   "ÀÄÄÄÄÄÙ"},
  {"ÚÄÄÄÄÄ¿",
   "³ ~FR2~RS   ³",
   "³  ~FRh~RS  ³",
   "³   ~FR2~RS ³",
   "ÀÄÄÄÄÄÙ"},
  {"ÚÄÄÄÄÄ¿",
   "³ ~FR3~RS   ³",
   "³  ~FRh~RS  ³",
   "³   ~FR3~RS ³",
   "ÀÄÄÄÄÄÙ"},
  {"ÚÄÄÄÄÄ¿",
   "³ ~FR4~RS   ³",
   "³  ~FRh~RS  ³",
   "³   ~FR4~RS ³",
   "ÀÄÄÄÄÄÙ"},
  {"ÚÄÄÄÄÄ¿",
   "³ ~FR5~RS   ³",
   "³  ~FRh~RS  ³",
   "³   ~FR5~RS ³",
   "ÀÄÄÄÄÄÙ"},
  {"ÚÄÄÄÄÄ¿",
   "³ ~FR6~RS   ³",
   "³  ~FRh~RS  ³",
   "³   ~FR6~RS ³",
   "ÀÄÄÄÄÄÙ"},
  {"ÚÄÄÄÄÄ¿",
   "³ ~FR7~RS   ³",
   "³  ~FRh~RS  ³",
   "³   ~FR7~RS ³",
   "ÀÄÄÄÄÄÙ"},
  {"ÚÄÄÄÄÄ¿",
   "³ ~FR8~RS   ³",
   "³  ~FRh~RS  ³",
   "³   ~FR8~RS ³",
   "ÀÄÄÄÄÄÙ"},
  {"ÚÄÄÄÄÄ¿",
   "³ ~FR9~RS   ³",
   "³  ~FRh~RS  ³",
   "³   ~FR9~RS ³",
   "ÀÄÄÄÄÄÙ"},
  {"ÚÄÄÄÄÄ¿",
   "³ ~FR10~RS  ³",
   "³  ~FRh~RS  ³",
   "³  ~FR10~RS ³",
   "ÀÄÄÄÄÄÙ"},
  {"ÚÄÄÄÄÄ¿",
   "³ ~FRJ~RS   ³",
   "³  ~FRh~RS  ³",
   "³   ~FRJ~RS ³",
   "ÀÄÄÄÄÄÙ"},
  {"ÚÄÄÄÄÄ¿",
   "³ ~FRQ~RS   ³",
   "³  ~FRh~RS  ³",
   "³   ~FRQ~RS ³",
   "ÀÄÄÄÄÄÙ"},
  {"ÚÄÄÄÄÄ¿",
   "³ ~FRK~RS   ³",
   "³  ~FRh~RS  ³",
   "³   ~FRK~RS ³",
   "ÀÄÄÄÄÄÙ"},
  {"ÚÄÄÄÄÄ¿",
   "³ ~FRA~RS   ³",
   "³  ~FRh~RS  ³",
   "³   ~FRA~RS ³",
   "ÀÄÄÄÄÄÙ"},
  {"ÚÄÄÄÄÄ¿",
   "³ ~OL2~RS   ³",
   "³  ~OLS~RS  ³",
   "³   ~OL2~RS ³",
   "ÀÄÄÄÄÄÙ"},
  {"ÚÄÄÄÄÄ¿",
   "³ ~OL3~RS   ³",
   "³  ~OLS~RS  ³",
   "³   ~OL3~RS ³",
   "ÀÄÄÄÄÄÙ"},
  {"ÚÄÄÄÄÄ¿",
   "³ ~OL4~RS   ³",
   "³  ~OLS~RS  ³",
   "³   ~OL4~RS ³",
   "ÀÄÄÄÄÄÙ"},
  {"ÚÄÄÄÄÄ¿",
   "³ ~OL5~RS   ³",
   "³  ~OLS~RS  ³",
   "³   ~OL5~RS ³",
   "ÀÄÄÄÄÄÙ"},
  {"ÚÄÄÄÄÄ¿",
   "³ ~OL6~RS   ³",
   "³  ~OLS~RS  ³",
   "³   ~OL6~RS ³",
   "ÀÄÄÄÄÄÙ"},
  {"ÚÄÄÄÄÄ¿",
   "³ ~OL7~RS   ³",
   "³  ~OLS~RS  ³",
   "³   ~OL7~RS ³",
   "ÀÄÄÄÄÄÙ"},
  {"ÚÄÄÄÄÄ¿",
   "³ ~OL8~RS   ³",
   "³  ~OLS~RS  ³",
   "³   ~OL8~RS ³",
   "ÀÄÄÄÄÄÙ"},
  {"ÚÄÄÄÄÄ¿",
   "³ ~OL9~RS   ³",
   "³  ~OLS~RS  ³",
   "³   ~OL9~RS ³",
   "ÀÄÄÄÄÄÙ"},
  {"ÚÄÄÄÄÄ¿",
   "³ ~OL10~RS  ³",
   "³  ~OLS~RS  ³",
   "³  ~OL10~RS ³",
   "ÀÄÄÄÄÄÙ"},
  {"ÚÄÄÄÄÄ¿",
   "³ ~OLJ~RS   ³",
   "³  ~OLS~RS  ³",
   "³   ~OLJ~RS ³",
   "ÀÄÄÄÄÄÙ"},
  {"ÚÄÄÄÄÄ¿",
   "³ ~OLQ~RS   ³",
   "³  ~OLS~RS  ³",
   "³   ~OLQ~RS ³",
   "ÀÄÄÄÄÄÙ"},
  {"ÚÄÄÄÄÄ¿",
   "³ ~OLK~RS   ³",
   "³  ~OLS~RS  ³",
   "³   ~OLK~RS ³",
   "ÀÄÄÄÄÄÙ"},
  {"ÚÄÄÄÄÄ¿",
   "³ ~OLA~RS   ³",
   "³  ~OLS~RS  ³",
   "³   ~OLA~RS ³",
   "ÀÄÄÄÄÄÙ"}
};

/* Dice for used with Moesoft Development's Craps, Online Dice Game */

#define DICE_WIDTH 9
#define DICE_HEIGHT 5
#define RAN_MAX 6
#define CRAPS_MINWAGER 10

static char *asciidice[6][DICE_HEIGHT]={
  {"~FB~BW.-------.~RS",
   "~FB~BW|       |~RS",
   "~FB~BW|   *   |~RS",
   "~FB~BW|       |~RS",
   "~FB~BW`-------'~RS"},
  {"~FB~BW.-------.~RS",
   "~FB~BW| *     |~RS",
   "~FB~BW|       |~RS",
   "~FB~BW|     * |~RS",
   "~FB~BW`-------'~RS"},
  {"~FB~BW.-------.~RS",
   "~FB~BW| *     |~RS",
   "~FB~BW|   *   |~RS",
   "~FB~BW|     * |~RS",
   "~FB~BW`-------'~RS"},
  {"~FB~BW.-------.~RS",
   "~FB~BW| *   * |~RS",
   "~FB~BW|       |~RS",
   "~FB~BW| *   * |~RS",
   "~FB~BW`-------'~RS"},
  {"~FB~BW.-------.~RS",
   "~FB~BW| *   * |~RS",
   "~FB~BW|   *   |~RS",
   "~FB~BW| *   * |~RS",
   "~FB~BW`-------'~RS"},
  {"~FB~BW.-------.~RS",
   "~FB~BW| * * * |~RS",
   "~FB~BW|       |~RS",
   "~FB~BW| * * * |~RS",
   "~FB~BW`-------'~RS"}
};

static char *ansidice[6][DICE_HEIGHT]={
  {"~FB~BWÚÄÄÄÄÄÄÄ¿~RS",
   "~FB~BW³       ³~RS",
   "~FB~BW³   *   ³~RS",
   "~FB~BW³       ³~RS",
   "~FB~BWÀÄÄÄÄÄÄÄÙ~RS"},
  {"~FB~BWÚÄÄÄÄÄÄÄ¿~RS",
   "~FB~BW³ *     ³~RS",
   "~FB~BW³       ³~RS",
   "~FB~BW³     * ³~RS",
   "~FB~BWÀÄÄÄÄÄÄÄÙ~RS"},
  {"~FB~BWÚÄÄÄÄÄÄÄ¿~RS",
   "~FB~BW³ *     ³~RS",
   "~FB~BW³   *   ³~RS",
   "~FB~BW³     * ³~RS",
   "~FB~BWÀÄÄÄÄÄÄÄÙ~RS"},
  {"~FB~BWÚÄÄÄÄÄÄÄ¿~RS",
   "~FB~BW³ *   * ³~RS",
   "~FB~BW³       ³~RS",
   "~FB~BW³ *   * ³~RS",
   "~FB~BWÀÄÄÄÄÄÄÄÙ~RS"},
  {"~FB~BWÚÄÄÄÄÄÄÄ¿~RS",
   "~FB~BW³ *   * ³~RS",
   "~FB~BW³   *   ³~RS",
   "~FB~BW³ *   * ³~RS",
   "~FB~BWÀÄÄÄÄÄÄÄÙ~RS"},
  {"~FB~BWÚÄÄÄÄÄÄÄ¿~RS",
   "~FB~BW³ * * * ³~RS",
   "~FB~BW³       ³~RS",
   "~FB~BW³ * * * ³~RS",
   "~FK~BWÀÄÄÄÄÄÄÄÙ~RS"}
};

/*-----------------------------------------------------------------*/
/* constants used for the fight command                            */
/*-----------------------------------------------------------------*/

#define TIE           3
#define BOTH_LOSE     0
#define CLOSE_NUMBER 50   /* make this 100 -no ties, 0- all ties              */
#define FIGHT_ROOM   -1   /* used for specifying a room to fight in, -1 = any */

int   odds[]={10, 30, 90,            /* use the super as the index   */
              270, 750, 2150,        /* used for thing like gunfight */
              7000, 21000, 60000};   /* or joust                     */

#define CHAL_LINE "\n\n~CROH MY GOD! You have been challenged to a fight!\n\07"
#define CHAL_LINE2 "~CTRespond with: .fight [yes | no]\n"
#define CHAL_ISSUED "~CB-=- You have issued the challenge..it is time to wait -=-\n"

char chal_text[][132] = {
  "~CR%s ~CWspits on ~CR%s ~CWand says LETS GO!\n",
  "~CR%s ~CWtakes some activator and sprays it on ~CR%s ~CW...it's GO TIME!\n",
  "~CR%s ~CWpulls out a salami and says \"~CR%s, ~CWYOU WANT SOME OF THIS?!?!\"\n",
  "~CR%s ~CWhas had enough of ~CR%s's~CW crap and smacks them in the kisser!\n",
  "~CR%s ~CWshouts \"~CR%s~CW, you suck, I'm gonna make you look like Play-Doh!\"\n",
  "~CR%s ~CWsucker punches ~CR%s~CW.  Uh-oh, Someone is going to bleed.\n",
  "~CR%s ~CWdraws a line in the dirt and says: \"~CR%s~CW, go ahead, cross me.\"\n",
  "~CR%s ~CWand ~CR%s ~CWcircle each other like dogs. FIGHT FIGHT FIGHT!!\n",
  "~CR%s ~CWgrabs ~CR%s ~CWand says \"I HATE YOU! I HATE YOU SO MUCH! DIE DIE DIE!\"\n",
  "~CR%s ~CWwhispers \"I slept with your sister\" to ~CR%s~CW and they get REALLY MAD!\n",
  "~CR%s ~CWshouts \"~CR%s ~FWis a CHICKEN, CLUCK CLUCK CLUCK!!\" It's Clobberin time!\n",
  "~CR%s ~CWjust stole all of ~CR%s's ~CWmoney....looks like someone is getting a ass whoopin.\n",
  "~CR%s ~CWpokes ~CR%s ~CWand says \"you fight like my dead grandmother!\"\n"
  };
int num_chal_text = 13;                              

char tie1_text[][132] = {
"~CTThere is a mutual agreement struck and both, ~CY%s ~CTand ~CY%s ~CTend the fight winners.\n",
"~CTThey both decide to open a hair salon instead...HALF PRICE HAIR CUTS!\n",
"~CTDOH! They Fooled you! ~CY%s ~CTshakes ~CY%s ~CThand. No way.\n",
"~CTIn a flash, both ~CY%s ~CTand ~CY%s ~CTknock each other out.\n~CTThey awake in each others arms (WHAT?!)\n",
"~CY%s ~CTand ~CY%s ~CTmake up and decide to open a brothel together instead.\n",
"~CY%s ~CTcracks a fart and they both start laughing so hard...they cannot fight.\n"
};
int num_tie1_text = 6;

char tie2_text[][132] = {
"~CBWow! They killed each other at exactly the same time. YAY! No one liked them anyway.\n",
"~CBBoth ~CW%s ~CBand ~CW%s ~CBdie....seems that some higher power hated them both.\n",
"~CW%s ~CBkilled ~CW%s, but was killed by a flying turd....the irony of it all.\n",
"~CWA run-away circus truck mows down ~CT%s ~CBand ~CT%s ~CBwhile they were fighting in the street. DOH!\n",
"~CW%s ~CBdecides to go suicidal and papercuts ~CW%s ~CBto death killing them both.\n",
"~CW%s ~CBand ~CW%s ~CBare mistaken as lawyers by a group of doctors...and are sliced open!\n",
"~CBA plane drops out of the sky on top of both ~CW%s ~CBand ~CW%s ~CBkilling them dead.  What luck.\n"
};
int num_tie2_text = 7;

char wins1_text[][132] = {
"~CT%s ~CRbends over and gasses ~CT%s ~CRto death with the STINK OF AGES.\n",
"~CT%s ~CRforces ~CT%s ~CRto eat taco bell for 5 straight hours till they die.\n",
"~CT%s ~CRpulls out a 9mm and pops a cap in ~CT%s~CR.\n",
"~CT%s ~CRpulls down their pants and ~CT%s~CR dies laughing!\n",
"~CT%s ~CRchokes ~CT%s ~CRto death with a piano wire.\n",
"~CT%s ~CRspits some CHEW on ~CT%s ~CRand watches them die from vomiting.\n",
"~CT%s ~CRplays with some matches and burns ~CT%s~CR to death.\n",
"~CT%s ~CRrams a taser up ~CT%s's YOU KNOW WHERE ~CRuntil smoke comes out of their ears.\n",
"~CT%s ~CRcuts the brake wires on ~CT%s's ~CRcar causing them to crash into a meat truck and die.\n",
"~CT%s ~CRdrops some bad ACID and FrEaKs Out killing ~CT%s~CR.\n",
"~CT%s ~CRwaits until ~CT%s ~CRis asleep... WHACKS them in the head with a garbage pan lid.\n",
"~CT%s ~CRgives ~CT%s ~CRtwo wires to hold then plugs them in. *ZAP*\n",
"~CT%s ~CRsings the theme to The Brady bunch over and over, killing ~CT%s~CR in record time.\n",
"~CT%s ~CRbooby traped the toilet seat so when ~CT%s ~CRsat down they exploded like blood sasauge.\n",
"~CT%s ~CRpulls a huge metal fan out and slices and dices ~CT%s ~CRinto pieces!.\n"
};
int num_wins1_text = 15;

char wins2_text[][132] = {
"~CR%s ~CTsays that they will kick ~CR%s's ~CTass, and they do!\n",
"~CR%s ~CTslits ~CR%s's ~CTthroat cause they wont SHUT UP.\n",
"~CR%s ~CTflicks mutant snot on ~CR%s, ~CTmaking them melt into goo.\n",
"~CR%s ~CTrips off ~CR%s's~CT head and ~CR*CENSORED* ~CTdown their neck.\n",
"~CR%s ~CTcalls the cops and has ~CR%s ~CTRodney Kinged!\n",
"~CR%s ~CTpretends they are OJ Simpson and makes a Pez dispenser out of ~CR%s~CT.\n",
"~CR%s ~CTthrows ~CR%s ~CTin molten lava and chuckles.\n",
"~CR%s ~CTsnaps ~CR%s's ~CTneck like a wishbone.\n",
"~CR%s ~CTputs a bullet in ~CR%s~CT brain!\n~CTAllways Put one in the brain kid..\n",
"~CR%s ~CTforces ~CR%s ~CTto drink draino as an after dinner treat!\n",
"~CR%s ~CTwhips out a HUGE mallet and bashes ~CR%s's ~CTface in for 3 hours.\n",
"~CR%s ~CTguts ~CR%s ~CTand proceeds to hickory smoke the corpse.\n",
"~CR%s ~CTsticks a trashbag over ~CR%s's ~CThead till death sets in.\n",
"~CR%s ~CTdoes a silly fresh backflip onto ~CR%s's ~CTskull, crushing it.\n",
"~CR%s ~CTslips ~CR%s ~CTa micky then grinds the body up into hotdog meat.\n"
};
int num_wins2_text =15;

char wimp_text[][132] = {
"~CY%s ~CMis a puss and hides!\n",
"~CMHEY LOOK!  ~CY%s ~CMsays their mommy is calling and runs away!\n",
"~CY%s ~CMis out on parole and can't fight.\n",
"~CY%s ~CMbribed their way out of the fight.\n",
"~CY%s ~CMis hiding behind the power of the cops and wont fight.\n",
"~CY%s ~CMran to get their big brother so there is no fight.\n",
"~CY%s ~CMpees their pants in fear and begs not to be beat.\n",
"~CY%s ~CMbegs for forgiveness so there wont be a fight.\n",
"~CY%s ~CMhas to go pick up a pack of smokes so they can't fight today.\n",
"~CY%s ~CMremembers they have to wash their hair so they can't fight now.\n"
};
int num_wimp_text = 10;
               
struct fight_struct {
        int issued,time;
        UR_OBJECT first_user,second_user;
        };

typedef struct fight_struct FI_OBJECT;
FI_OBJECT fight;

/*  Random 8ball messages   */
#define NUM_BALL_LINES 10
char ball_text[][132] = {
 "~CRHELL NO!!!!!",
 "~CMAre you a moron?  What kind of question is that!!!",
 "~CTIn a word ~CB- ~CWyes",
 "~CYWhat was that question again?",
 "~CBBetter ask your mommy to be on the safe side!",
 "~CGHAHAHAHAHA, ~CWThats a good one. ~CRNO.",
 "~CTBut of course!",
 "~CWOH MY GOD, ~CGYES!",
 "~CBNah, I don't think SO!",
 "~CMStop wasting my time with that trash!"
 };

#define MAX_TEXTCOLOR 17

char textcolor[MAX_TEXTCOLOR][5]=
{ 
/* Dull Colors */
"~FM","~FR","~FY","~FG","~FB","~FT","~FW",
/* Bright Colours */
"~CK","~CM","~CR","~CY","~CG","~CB","~CT","~CW",
"~RF","~RC"
};

char textcolorname[MAX_TEXTCOLOR][20]=
{  
/* Dull Color Names */
"~FMPurple","~FRRed","~FYYellow","~FGGreen","~FBBlue","~FTCyan","~FWWhite",
/* Bright Color Names */
"~CKBlack","~CMPurple","~CRRed","~CYYellow","~CGGreen","~CBBlue","~CTCyan",
"~CWWhite","~RFRandom Dull","~RCRandom Bright"
};

/****************************************
 * Ranklist system for Amnuts
 * written by Ardant (ardant@ardant.net)
 ****************************************/

struct rank_entry_struct {
  char name[USER_NAME_LEN + 1];
  unsigned long int data;
  struct rank_entry_struct *next, *prev;
  int flags;
};

struct rank_head_struct {
  int offset;
  char *name;
  struct rank_entry_struct *first, *last;
  int flags;
  int count;
  char *format;
  char *long_name;
  char *stats;
};

struct rank_return_struct {
  struct rank_entry_struct *spod;
  int rank;
};

