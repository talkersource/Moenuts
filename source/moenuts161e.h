 /********************* Header file for Moenuts v1.61e **********************

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
#define PO_HIST_FILE "pokerhistory"
#define HANGDICT     "hangman_words"
#define WELCOMEMAIL  "welcome-mail"
#define SOCIALFILE   "socials.dat"
#define TALKERFILE   "talkerlist"
#define MUDFILE      "mudlist"
#define EWTOOFILE    "ewtoolist"
#define NUTSFILE     "nutslist"
#define ITEMFILE     "itemlist"  /* Future Use */
#define PIDFILE      "moenuts.pid"  /* Used For The Kill Script Etc */

/* Talker Global Variables */

#define TALKERNAME  "A New Talker"                    /* Talker's Full Name    */
#define SHORTNAME   "A New Talker"                    /* Talker's Short Name   */
#define TALKERADDR  "myhost.com:5000"                 /* Talker's Address      */
#define TALKERURL   "http://user.myhost.com"          /* Talker's Homepage URL */
#define DEFAULTDESC "is a new user!"                  /* Default New User Desc */
#define BANKBALANCE 500                               /* Starting Bank Balance */
#define CREDIT_RATE 10                                /* Not Implimented Yet   */
#define USE_FORTUNE 1                                 /* 0 = QOTD, 1 = Fortune */
#define FORTUNE_COMMAND "/usr/games/fortune -o"       /* Fortune Path/Command  */

/* Email Signature For Smail Forwarding */

char *email_signature="
 .-='^`=-.-='^`=-.-='^`=-.-='^`=-.-='^`=-.-='^`=-.-='^`=-.-='^`=-.-='^`=-.
(  This email was sent to you from Moesoft's Forgotten Island Talker      )
 ) This email is a copy of your current S-Mail sent to you because you   (
(  Possibly have Mail Forwarding turned on.  Please DO NOT reply to       )
 ) This Email message.   -  Please visit the originating talker to       (
(  Reply to any SMail recieved in this Email message.                     )
 `-=~+~=-.-=~+~=-.-=~+~=-.-=~+~=-.-=~+~=-.-=~+~=-.-=~+~=-.-=+~=-.-=~+~=-.'
";

/* Email Signature For Gateway Email Messages */

char *gate_email_signature="
.--------------------------------------------------------------------------.
:  This email message has been sent to you via Moesoft's Internet E-Mail   |
|  Gateway via a Telnet Chat Server running at telnet://talkernet.net:8100 :
:  Please DO NOT reply to this email message as it will not reach the      |
|  Person who wrote this email message.  Instead, if available, use the    :
:  Supplied 'Reply-To' address, or address supplied in their message.      |
`--------------------------------------------------------------------------'
";

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
#define BACKUPFILE    "fisland"     /* Name of archive w/o extention         */
#define BACKUPEXT     "tgz"         /* Backup File Extention                 */
#define BACKUPCMD     "tar -zcvf %s/%s.%s ../moenuts/* > %s" /* Archive Cmd  */

/* 1st %s = BACKUPDIR, 2nd = BACKUPFILE, 3rd = Logfile Name (BACKUPFILE.log) */
/* Some Versions Of Tar Don't Allow The 'z' option, if not, you can try      */
/* Something like:  #define BACKUPCMD "tar -cvf %s.tar ../moenuts ||         */
/*   gzip talkerfiles.tar"   (Or whatever your backupfile name is defined as.*/

/*      System Allowances - Enable Feature = 1, Disable Feature = 0       */

#define ALLOW_USER_ROOMS      1  /* Set to 1 to allow user room creation  */
#define ALLOW_SUICIDE         0  /* Allow the use of the .suicide command */
#define ALLOW_SYSLOG_TOGGLE   0  /* Allow System Logging To Be Turned Off */
#define ALLOW_AUTO_PROMOTE    1  /* Allow Auto Promote System             */
#define ALLOW_EXTERNAL_REVIEW 1  /* Allow Reviewing In Other Rooms        */
#define EXTERNAL_IF_CLONE     1  /* Allow Clone Room Reviewing            */
#define ALLOW_EXTERNAL_CBUFF  1  /* Allow .cbuff In Other Rooms           */
#define ALLOW_PUBLIC_CBUFF    1  /* Allow Everyone To .cbuff in Pub Rooms */
#define ALLOW_ONLINE_STORE    0  /* Future Use                            */

/*                    Other Talker Values And Settings                    */

#define MAX_MOTD1        5    /* Maximum Number Of Random Pre-Login Screens*/
#define OUT_BUFF_SIZE    1024 /* Output Buffer Size (1KB)                  */
#define MAX_WORDS        10   /* Maximum Words                             */
#define WORD_LEN         40   /* Maximum Word Length (Miscelaneous)        */
#define ARR_SIZE         1024 /* Miscelaneous Text Buffer Storage Size     */
#define MAX_LINES        30   /* Maximum Lines To Allow In The Editor      */
#define USER_NAME_LEN    12   /* Maximum Size A User's Name Can Be         */
#define USER_ALIAS_LEN   30   /* Maximum User's Level Alias Can Be         */
#define USER_RECAP_LEN   30   /* User's Recap Size (Name + Color Codes)    */
#define USER_DESC_LEN    60   /* Maximum Size User's Desc. Size (w/ Color) */
#define USER_DESC_TEXT   30   /* Maximum Desc. Size (Not Including Colors) */
#define USER_PREDESC_LEN 18   /* Maximum Predesc Size                      */
#define BRAND_DESC_LEN   220  /* Should be more than enough                */
#define AFK_MESG_LEN     60   /* Maximum Size User's AFK Message Can Be    */
#define PHRASE_LEN       60   /* Maximum Size Of Users In/Out Phrase       */
#define PASS_LEN         20   /* (Only First 8 Chars Are Encrypted Though) */
#define BUFSIZE          1024 /* Editor's Line Buffer Size                 */
#define ROOM_NAME_LEN    20   /* Maximum Size Of A Room's Name (Chars)     */
#define ROOM_LABEL_LEN   5    /* Maximum Size Of A Room's Label In Config  */
#define ROOM_DESC_LEN    12   /* Not Really Used Anymore                   */
#define TOPIC_LEN        60   /* Maximum Size Of A Room Topic (Chars)      */
#define MAX_LINKS        20   /* Maximum Number Of Links A Room May Have   */
#define SERV_NAME_LEN    80   /* Domain Name Resolver Buffer Size #1       */
#define SITE_NAME_LEN    80   /* Domain Name Resolver Buffer Size #2       */
#define VERIFY_LEN       20   /* Verification Buffer Size                  */
#define REVIEW_LINES     75   /* Maximum Number Of Room Review Lines       */
#define REVTELL_LINES    10   /* Maximum Number of Tell Review Lines       */
#define REVIEW_LEN       250  /* Review Buffer Maximum Line Length         */ 
#define FAKE_AGE_LEN     60   /* Fake Age Length (Novelty;)                */
#define DNL              12   /* Date Number Length                        */
/***************************************************************************/

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
#define NEUTER          2
#define FEMALE          4 
#define MALE 		8
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
#define ZERO             0
#define TRUE             1

/* Clone Flags */

#define CLONE_HEAR_NOTHING 0
#define CLONE_HEAR_SWEARS  1
#define CLONE_HEAR_ALL     2

/* Neil: The elements vis, ignall, prompt, command_mode etc could all be
   bits in one flag variable as they're only ever 0 or 1, but I tried it
   and it made the code unreadable. Better to waste a few bytes */

struct user_struct {
	char name[USER_NAME_LEN+1];
	char desc[USER_DESC_LEN+1];
	char pass[PASS_LEN+6];
	char in_phrase[PHRASE_LEN+1],out_phrase[PHRASE_LEN+1],ignuser[USER_NAME_LEN];
	char buff[BUFSIZE],site[81],last_site[81],site_port[20],page_file[81];
	char mail_to[WORD_LEN+1],revbuff[REVTELL_LINES][REVIEW_LEN+2];
	char afk_mesg[AFK_MESG_LEN+1],inpstr_old[REVIEW_LEN+1],samesite_check_store[ARR_SIZE];
	struct room_struct *room,*invite_room;
	int type,port,login,socket,attempts,buffpos,filepos;
	int vis,ignall,prompt,command_mode,muzzled,gaged,charmode_echo; 
	int level,misc_op,remote_com,edit_line,charcnt,warned;
	int accreq,last_login_len,ignall_store,clone_hear,afk;
	int edit_op,colour,chkrev,ignore,revline,samesite_all_store;
	time_t last_input,last_login,total_login,read_mail;
	char *malloc_start,*malloc_end;
	struct user_struct *prev,*next,*owner;
	int wrap,mashed,invis,age,whostyle,last_room;
        char email[ARR_SIZE],homepage[ARR_SIZE];
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
        char birthday[ARR_SIZE];
        char icq[ARR_SIZE];
        /* Branding */
        int branded;
        char branded_by[USER_NAME_LEN+1];
        char brand_desc[BRAND_DESC_LEN+1];
        /* Callaring */
        char callared_by[USER_NAME_LEN+1];
        char callared_desc[BRAND_DESC_LEN+1];
        int callared;
	int bdsm_type;
	int mailbox_limit;
        char fakeage[FAKE_AGE_LEN+1];
        char married[ARR_SIZE+1];
	char predesc[USER_PREDESC_LEN+1];
	/* Misc-Input Storage */
	char inpstr[ARR_SIZE+2];
	/* Craps Varaibles */
	int cpot, cwager;
	/* Hangman Variables */
	char hang_word[WORD_LEN],hang_word_show[WORD_LEN],hang_guess[WORD_LEN];
	int hang_stage;
	/* Email Gateway Variables */
	char email_subject[81];
	char to_email[121];
        /* Bank Account Info */
        int bank_balance;
        int bank_update;
        int bank_temp;
};

typedef struct user_struct* UR_OBJECT;
UR_OBJECT user_first,user_last;

struct room_struct {
	char name[ROOM_NAME_LEN+1];
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
	char link_label[MAX_LINKS][ROOM_LABEL_LEN+1]; /* temp store for parse */
	struct room_struct *link[MAX_LINKS];
	struct room_struct *next,*prev;
	};

typedef struct room_struct *RM_OBJECT;
RM_OBJECT room_first,room_last;
RM_OBJECT create_room();

/* Login Room Config For Special Cases */
char *jail_room="Jail";
char *newbie_room="Newby_Room";

char *command[]={
"quit",      "look",       "mode",       "say",       "shout",
"tell",      "emote",      "semote",     "pemote",    "echo",
"go",        "ignore",     "prompt",     "lounge",    "listen",
"ignall",    "public",     "private",    "knock",     "invite",
"to",        "move",       "bcast",      "who",       "people",
"help",      "shutdown",   "news",       "read",      "write",
"wipe",      "search",     "review",     "email",     "ustat",
"version",   "rmail",      "smail",      "dmail",     "from",
"entpro",    "examine",    "lastlogin",  "rooms",     "makeuser",
"finduser",  "lastcbuff",  "nslookup",   "passwd",    "kill",
"promote",   "demote",     "listbans",   "ban",       "unban",
"vis",       "invis",      "site",       "wake",      "wiztell",
"muzzle",    "unmuzzle",   "map",        "logging",   "minlogin",
"system",    "charecho",   "clearline",  "fix",       "unfix",
"viewlog",   "viewsys",    "cbuff",      "clone",     "destroy",
"myclones",  "allclones",  "switch",     "csay",      "chear",
"cemote",    "swban",      "desc",       "cls",       "color",
"inphr",     "outphr",     "suicide",    "nuke",      "reboot",
"recount",   "revtell",    "think",      "wemote",    "imnew", 
"staff",     "management", "calender",   "entroom",   "topic",
"accreq",    "boot",       "arrest",     "unarrest",  "gender",  
"rules",     "srules",     "record",     "change",    "revwiz",
"chrank",    "beep",       "afk",        "join",      "mash", 
"freeze",    "unfreeze",   "sing",       "naked",     "ssing",
"doh",       "qecho",      "hug",        "force",     "kiss",
"french",    "netsex",     "netsextwo",  "yell",      "logoff",
"clsall",    "welcome",    "time",       "ranks",     "greet",
"samesite",  "wrap",       "uninvite",   "streak",    "lick",
"bop",       "set",        "paddle",     "tictactoe", "finger",
"pokerrules","startpoker", "joinpoker",  "leavepoker","gamespoker",
"dealpoker", "foldpoker",  "betpoker",   "checkpoker","raisepoker",
"discpoker", "seepoker",   "handpoker",  "rankpoker", "chipspoker",
"scorepoker","talkers",    "plist",      "ptell",     "rpic",
"vpic",      "wcbuff",     "scbuff",     "revshout",  "homeroom",
"edit",      "rmkill",     "rmban",      "rmunban",   "giveroom",
"muds",      "ewtoo",      "nuts",       "shackle",   "unshackle",
"makeinvis", "makevis",    "hide",       "show",      "execall",
"craps",     "givecash",   "lend",       "atmosedit", "backup",
"hangman",   "guess",      "givechips",  "tpromote",  "socials",
"icqpage",   "toemote",    "addnews",    "figlet",    "sreboot",
"ctopic",    "alias",      "wire",	 "blah",      "*"
};

/* Values of commands , used in switch in exec_com() */
enum comvals {
QUIT,        LOOK,         MODE,         SAY,         SHOUT,
TELL,        EMOTE,        SEMOTE,       PEMOTE,      ECHO,
GO,          IGNORE,       PROMPT,       LOBBY,       LISTEN,
IGNALL,      PUBCOM,       PRIVCOM,      KNOCK,       INVITE,
TO,          MOVE,         BCAST,        WHO,         PEOPLE,
HELP,        SHUTDOWN,     NEWS,         READ,        WRITE,
WIPE,        SEARCH,       REVIEW,       EMAIL,       STATUS,
VER,         RMAIL,        SMAIL,        DMAIL,       FROM,
ENTPRO,      EXAMINE,      LASTLOGIN,    RMST,        MAKEUSER,
FINDUSER,    CLASTLOGIN,   NSLOOKUP,     PASSWD,      KILL,
PROMOTE,     DEMOTE,       LISTBANS,     BAN,         UNBAN,
VIS,         INVIS,        SITE,         WAKE,        WIZTELL,
MUZZLE,      UNMUZZLE,     MAP,          LOGGING,     MINLOGIN,
SYSTEM,      CHARECHO,     CLEARLINE,    FIX,         UNFIX,
VIEWLOG,     VIEWSYS,      CBUFF,        CREATE,      DESTROY,
MYCLONES,    ALLCLONES,    SWITCH,       CSAY,        CHEAR,
CEMOTE,      SWBAN,        DESC,         CLS,         COLOR,
INPHRASE,    OUTPHRASE,    SUICIDE,      NUKE,        REBOOT,
RECOUNT,     REVTELL,      THINK,        WEMOTE,      IMNEW,
STAFF,       MANAGEMENT,   CALENDER,     ENTROOM,     TOPIC,
ACCREQ,      BOOT,         ARREST,       UNARREST,    GENDER,
RULES,       SRULES,       RECORD,       CHANGE,      REVWIZ,
SETRANK,     BEEP,         AFK,          JOIN,        GAG,
FREEZE,      UNFREEZE,     SING,         NAKED,       SSING,
DOH,         QECHO,        HUG,          FORCE,       KISS,
FRENCH,      NETSEX,       NETSEXTWO,    YELL,        LOGOFF,
CLSALL,      WELCOME,      TIME,         RANKS,       GREET,
SAMESITE,    WRAP,         UNINVITE,     STREAK,      LICK,
BOP,         SET,          PADDLE,       TICTAC,      FINGER,
POKER,       STARTPO,      JOINPO,       LEAVEPO,     GAMESPO,
DEALPO,      FOLDPO,       BETPO,        CHECKPO,     RAISEPO,
DISCPO,      SEEPO,        HANDPO,       RANKPO,      CHIPSPO,
SCOREPO,     TALKERS,      PICLIST,      PICTELL,     ROOMPIC,
VIEWPIC,     WREVCLR,      SREVCLR,      REVSHOUT,    MYROOM,
EDIT,        RMKILL,       RMBAN,        RMUNBAN,     GIVEROOM,
MUDS,        EWTOO,        NUTS,         SHACKLE,     UNSHACKLE,
MAKEINVIS,   MAKEVIS,      HIDE,         SHOW,        ALLEXEC,
CRAPS,       GIVECASH,     LENDCASH,     ATMOS,       BACKUP,
HANGMAN,     GUESS,        GIVEPOCHIPS,  TPROMOTE,    SOCIALS,
ICQPAGE,     TOEMOTE,      ADDNEWS,      FIGLET,      SREBOOT,
CTOPIC,      ALIAS,        WIRE,	 BLAH
} com_num;

/* These are the minimum levels at which the commands can be executed.
   Alter to suit. */

int com_level[]={
NEW,        NEW,         NEW,         NEW,        USER,
USER,       USER,        USER,        USER,       USER,
USER,       USER,        NEW,         NEW,        USER,
USER,       USER,        USER,        USER,       USER,
USER,       WIZ,         WIZ,         NEW,        WIZ,
NEW,        ARCH,        USER,        NEW,        USER,
WIZ,        USER,        USER,        USER,       NEW,
NEW,        NEW,         USER,        USER,       USER,
NEW,        USER,        USER,        USER,       ARCH,
USER,       ARCH,        USER,        USER,       ARCH,
WIZ,        ARCH,        WIZ,         WIZ,        WIZ,
USER,       USER,        WIZ,         WIZ,        WIZ,
WIZ,        WIZ,         USER,        ARCH,       ARCH,
ARCH,       NEW,         WIZ,         ARCH,       ARCH,
WIZ,        ARCH,        USER,        WIZ,        WIZ,
WIZ,        WIZ,         WIZ,         WIZ,        WIZ,
WIZ,        ARCH,        NEW,         NEW,        NEW,
USER,       USER,        NEW,         ARCH,       ARCH,
ARCH,       USER,        USER,        WIZ,        NEW,
NEW,        NEW,         NEW,         WIZ,        NEW,
NEW,        WIZ,         WIZ,         WIZ,        ARCH,
NEW,        WIZ,         WIZ,         ARCH,       WIZ,
WIZ,        USER,        USER,        USER,       WIZ,
ARCH,       ARCH,        USER,        USER,       USER,
USER,       ARCH,        USER,        ARCH,       USER,
USER,       USER,        USER,        WIZ,        WIZ,
WIZ,        USER,        NEW,         NEW,        USER,
WIZ,        NEW,         USER,        USER,       USER,
USER,       NEW,         USER,        USER,       GOD,
USER,       USER,        USER,        USER,       USER,
USER,       USER,        USER,        USER,       USER,
USER,       USER,        USER,        USER,       USER,
USER,       USER,        USER,        USER,       WIZ,
USER,       ARCH,        WIZ,         USER,       USER,
USER,       USER,        GOD,         GOD,        GOD,
USER,       USER,        USER,        WIZ,        WIZ,
WIZ,        WIZ,         USER,        WIZ,        USER,
USER,       GOD,         USER,        USER,       GOD,
USER,       USER,        ARCH,        ARCH,       MEMBER,
USER,       USER,        ARCH,        USER,       GOD,
USER,       WIZ,	 USER,        USER
};

/*
Colcode values equal the following:

RESET,BOLD,BLINK,REVERSE

Foreground & background colours in order..

BLACK,RED,GREEN,YELLOW/ORANGE,
BLUE,MAGENTA,TURQUIOSE,WHITE 
*/

#define NUM_COLS 33

char *colcode[NUM_COLS]={
/* Standard stuff */
/* "\033[0m","\033[1m","\033[4m","\033[5m","\033[7m", */

/* Because It Bothers Most People, For Now Untill I get .set blink done,
   I've defined ~LI = ~UL to stop people from using blinking! :) */

"\033[0m","\033[1m","\033[4m","\033[7m","\033[7m",
/* Foreground colour */
"\033[30m","\033[31m","\033[32m","\033[33m",
"\033[34m","\033[35m","\033[36m","\033[37m",
/* Background colour */
"\033[40m","\033[41m","\033[42m","\033[43m",
"\033[44m","\033[45m","\033[46m","\033[47m",
/* Ascii Bell */
"\07",
/* Ansi Music */
"\033[M","\0x0E",
/* Clear Screen */
"\033[2J",
/* Foreground colour */
"\033[1;30m","\033[1;31m","\033[1;32m","\033[1;33m",
"\033[1;34m","\033[1;35m","\033[1;36m","\033[1;37m"
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
"CB","CM","CT","CW"
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

#define MAX_WHOS 	6

char *who_list_style[MAX_WHOS+1]={
"~OL~FRNone (System Default)",
"~OL~FMShort Who ~FB-> ~FGNames Only",
"~OL~FMHouse Of Pleasure Who List Style",
"~OL~FYStairway To Heaven Who List Style",
"~OL~FGMoenuts Byroom Who List Style",
"~OL~FMDragons Cove Who List (n/a)",
"~OL~FTNcohafmuta Who        (n/a)",
};

/* BDSM Types */

#define MAX_BDSM_TYPES 	6

char *bdsm_types[MAX_BDSM_TYPES]={
"Not Defined","Dominant","submissive","slave","switch","vanilla"
};

char *bdsm_type_desc[MAX_BDSM_TYPES]={
"N/A",
"A Dominant person.",
"A submissive person.",
"A Dom/Domme's slave.",
"Both dominant/submissive (switches)",
"Someone who isn't into the BDSM Lifestyle."
};

char file[ARR_SIZE];
char verification[SERV_NAME_LEN+1];
char wizrevbuff[REVIEW_LINES][REVIEW_LEN+2];
char shoutrevbuff[REVIEW_LINES][REVIEW_LEN+2];
char lastlogbuff[LASTLOG_LINES][LASTLOG_LEN+2];
char text[ARR_SIZE*2];
char afktext[ARR_SIZE];
char word[MAX_WORDS][WORD_LEN+1];
char wrd[8][81];
char progname[40],confile[40];
char tempstr[(ARR_SIZE*2)+1];
char newstopic[TOPIC_LEN+2];
time_t boot_time, rs_announce,rs_which;
jmp_buf jmpvar;

int port[2],listen_sock[2],wizport_level,minlogin_level;
int colour_def,password_echo,ignore_sigterm,wrevline,srevline;
int max_users,max_clones,num_of_users,num_of_logins,heartbeat;
int login_idle_time,user_idle_time,config_line,word_count;
int tyear,tmonth,tday,tmday,twday,thour,tmin,tsec;
int mesg_life,system_logging,prompt_def,no_prompt;
int force_listen,gatecrash_level,min_private_users;
int ignore_mp_level,rem_user_maxlevel,rem_user_deflevel;
int destructed,mesg_check_hour,mesg_check_min;
int keepalive_interval,ban_swearing,crash_action;
int time_out_afks,allow_caps_in_name,kill_idle_users,rs_countdown;
int charecho_def,time_out_maxlevel,num_maps,user_count,atmos_trigger;
int lastlogline,sys_allow_autopromote,sys_allow_suicide;
int SYS_FORTUNE=USE_FORTUNE; /* Set Fortune Flag */

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
  struct po_player_hist *hist;
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
struct po_player_hist {
  char name[USER_NAME_LEN+1];
  int total;
  int given;
};

struct po_player_hist *po_hist[50];
int max_po_hist;
int max_hist;

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
  {"~BW~FK.-------.~RS",
   "~BW~FK|       |~RS",
   "~BW~FK|   *   |~RS",
   "~BW~FK|       |~RS",
   "~BW~FK`-------'~RS"},
  {"~BW~FK.-------.~RS",
   "~BW~FK| *     |~RS",
   "~BW~FK|       |~RS",
   "~BW~FK|     * |~RS",
   "~BW~FK`-------'~RS"},
  {"~BW~FK.-------.~RS",
   "~BW~FK| *     |~RS",
   "~BW~FK|   *   |~RS",
   "~BW~FK|     * |~RS",
   "~BW~FK`-------'~RS"},
  {"~BW~FK.-------.~RS",
   "~BW~FK| *   * |~RS",
   "~BW~FK|       |~RS",
   "~BW~FK| *   * |~RS",
   "~BW~FK`-------'~RS"},
  {"~BW~FK.-------.~RS",
   "~BW~FK| *   * |~RS",
   "~BW~FK|   *   |~RS",
   "~BW~FK| *   * |~RS",
   "~BW~FK`-------'~RS"},
  {"~BW~FK.-------.~RS",
   "~BW~FK| * * * |~RS",
   "~BW~FK|       |~RS",
   "~BW~FK| * * * |~RS",
   "~BW~FK`-------'~RS"}
};

static char *ansidice[6][DICE_HEIGHT]={
  {"~BW~FKÚÄÄÄÄÄÄÄ¿~RS",
   "~BW~FK³       ³~RS",
   "~BW~FK³   *   ³~RS",
   "~BW~FK³       ³~RS",
   "~BW~FKÀÄÄÄÄÄÄÄÙ~RS"},
  {"~BW~FKÚÄÄÄÄÄÄÄ¿~RS",
   "~BW~FK³ *     ³~RS",
   "~BW~FK³       ³~RS",
   "~BW~FK³     * ³~RS",
   "~BW~FKÀÄÄÄÄÄÄÄÙ~RS"},
  {"~BW~FKÚÄÄÄÄÄÄÄ¿~RS",
   "~BW~FK³ *     ³~RS",
   "~BW~FK³   *   ³~RS",
   "~BW~FK³     * ³~RS",
   "~BW~FKÀÄÄÄÄÄÄÄÙ~RS"},
  {"~BW~FKÚÄÄÄÄÄÄÄ¿~RS",
   "~BW~FK³ *   * ³~RS",
   "~BW~FK³       ³~RS",
   "~BW~FK³ *   * ³~RS",
   "~BW~FKÀÄÄÄÄÄÄÄÙ~RS"},
  {"~BW~FKÚÄÄÄÄÄÄÄ¿~RS",
   "~BW~FK³ *   * ³~RS",
   "~BW~FK³   *   ³~RS",
   "~BW~FK³ *   * ³~RS",
   "~BW~FKÀÄÄÄÄÄÄÄÙ~RS"},
  {"~BW~FKÚÄÄÄÄÄÄÄ¿~RS",
   "~BW~FK³ * * * ³~RS",
   "~BW~FK³       ³~RS",
   "~BW~FK³ * * * ³~RS",
   "~BW~FKÀÄÄÄÄÄÄÄÙ~RS"}
};
