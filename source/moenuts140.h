 /****************** Header file for Moenuts version 1.40 A4 ***************/

#define DATAFILES "datafiles"
#define ROOMFILES "roomfiles"
#define USERFILES "userfiles"
#define HELPFILES "helpfiles"
#define MAILSPOOL "mailspool"
#define CONFIGFILE "config"
#define NEWSFILE "newsfile"
#define NEWBIEFILE "newbiefile"
#define STAFFFILE "stafffile"
#define SRULESFILE "srulesfile"
#define RULESFILE "rulesfile"
#define MAPFILE "mapfile"
#define MAPAFILE "mapafile"
#define MAPBFILE "mapbfile"
#define MAPCFILE "mapcfile"
#define MAPDFILE "mapdfile"
#define MAPEFILE "mapefile"
#define MAPFFILE "mapffile"
#define SITEBAN "siteban"
#define NEWSITEBAN "newban"
#define USERBAN "userban"
#define SYSLOG "syslog"
#define LOGINLOG "loginlog"
#define MOTD1 "motd1"
#define MOTD2 "motd2"
#define USERLIST "userlist"
#define TALKERFILE "talkerfile"
#define MUDFILE "mudfile"
#define EWTOOFILE "ewtoofile"
#define NUTSFILE "nutsfile"
#define PICLISTFILE "piclist"
#define PICFILES "pictures"
#define MAX_MOTD1 5
#define MAX_QUOTES 47
#define OUT_BUFF_SIZE 1000
#define MAX_WORDS 10
#define WORD_LEN 40
#define ARR_SIZE 1000
#define MAX_LINES 15
#define NUM_COLS 22
#define USER_NAME_LEN 12
#define USER_DESC_LEN 30
#define AFK_MESG_LEN 60
#define PHRASE_LEN 60
#define PASS_LEN 20 /* only the 1st 8 chars will be used by crypt() though */
#define BUFSIZE 1000
#define ROOM_NAME_LEN 20
#define ROOM_LABEL_LEN 5
/* #define ROOM_DESC_LEN 1558 18 lines of 80 chars each + 18 nl */
#define ROOM_DESC_LEN 12
#define TOPIC_LEN 60
#define MAX_LINKS 20
#define SERV_NAME_LEN 80
#define SITE_NAME_LEN 80
#define VERIFY_LEN 20
#define REVIEW_LINES 30
#define REVTELL_LINES 10
#define REVIEW_LEN 200
/* DNL (Date Number Length) will have to become 12 on Sun Sep 9 02:46:40 2001 
   when all the unix timers will flip to 1000000000 :) */
#define DNL 12
#define PUBLIC 0
#define PRIVATE 1
#define FIXED 2
#define FIXED_PUBLIC 2
#define FIXED_PRIVATE 3
#define PERSONAL 4
#define FIXED_PERSONAL 6

#define NEW 0
#define USER 1
#define REG 2
#define WIZ 3
#define CODER 4
#define GOD 5
#define OWNER 6

#define ZERO 0 
#define TRUE 1
#define SCUM 16
#define JAILED 32
#define FROZEN 64
#define FEMALE 4 
#define MALE 8
#define USERINVIS 4
#define USERVIS 8

#define ALL_TELLS 1
#define OUT_ROOM_TELLS 2
#define SHOUT_MSGS 4
#define LOGON_MSGS 8
#define BEEP_MSGS 16
#define BCAST_MSGS 32
#define ROOM_PICTURE 64
#define MOST_MSGS 126
#define WIZARD_MSGS 128

#define USER_TYPE 0
#define CLONE_TYPE 1
#define REMOTE_TYPE 2
#define CLONE_HEAR_NOTHING 0
#define CLONE_HEAR_SWEARS 1
#define CLONE_HEAR_ALL 2

/* The elements vis, ignall, prompt, command_mode etc could all be bits in 
   one flag variable as they're only ever 0 or 1, but I tried it and it
   made the code unreadable. Better to waste a few bytes */
struct user_struct {
	char name[USER_NAME_LEN+1];
	char desc[USER_DESC_LEN+1];
	char pass[PASS_LEN+6];
	char in_phrase[PHRASE_LEN+1],out_phrase[PHRASE_LEN+1],ignuser[USER_NAME_LEN];
	char buff[BUFSIZE],site[81],last_site[81],site_port[20],page_file[81];
	char mail_to[WORD_LEN+1],revbuff[REVTELL_LINES][REVIEW_LEN+2];
	char afk_mesg[AFK_MESG_LEN+1],inpstr_old[REVIEW_LEN+1],samesite_check_store[USER_NAME_LEN];
	struct room_struct *room,*invite_room;
	int type,port,login,socket,attempts,buffpos,filepos;
	int vis,ignall,prompt,command_mode,muzzled,gaged,charmode_echo; 
	int level,misc_op,remote_com,edit_line,charcnt,warned;
	int accreq,last_login_len,ignall_store,clone_hear,afk;
	int edit_op,colour,chkrev,ignore,revline,samesite_all_store;
	time_t last_input,last_login,total_login,read_mail;
	char *malloc_start,*malloc_end;
	struct netlink_struct *netlink,*pot_netlink;
	struct user_struct *prev,*next,*owner;
        int a1,a2,a3,a4,a5,a6;
        char c1[ARR_SIZE],c2[ARR_SIZE];
	int first;
	char array[10];
	int rules,news,default_wrap,shackled,shackle_level,hidden;	
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
	int inlink; /* 1 if room accepts incoming net links */
	int access; /* public , private etc */
	int revline; /* line number for review */
	int mesg_cnt;
	char netlink_name[SERV_NAME_LEN+1]; /* temp store for config parse */
	char link_label[MAX_LINKS][ROOM_LABEL_LEN+1]; /* temp store for parse */
	struct netlink_struct *netlink; /* for net links, 1 per room */
	struct room_struct *link[MAX_LINKS];
	struct room_struct *next;
	};

typedef struct room_struct *RM_OBJECT;
RM_OBJECT room_first,room_last;
RM_OBJECT create_room();

/* Netlink stuff */
#define UNCONNECTED 0 
#define INCOMING 1 
#define OUTGOING 2
#define DOWN 0
#define VERIFYING 1
#define UP 2
#define ALL 0
#define IN 1
#define OUT 2

/* Structure for net links, ie server initiates them */
struct netlink_struct {
	char service[SERV_NAME_LEN+1];
	char site[SITE_NAME_LEN+1];
	char verification[VERIFY_LEN+1];
	char buffer[ARR_SIZE*2];
	char mail_to[WORD_LEN+1];
	char mail_from[WORD_LEN+1];
	FILE *mailfile;
	time_t last_recvd; 
	int port,socket,type,connected;
	int stage,lastcom,allow,warned,keepalive_cnt;
	int ver_major,ver_minor,ver_patch;
	struct user_struct *mesg_user;
	struct room_struct *connect_room;
	struct netlink_struct *prev,*next;
	};

typedef struct netlink_struct *NL_OBJECT;
NL_OBJECT nl_first,nl_last;
NL_OBJECT create_netlink();

char *syserror="~OLWARNING:~FR ** A system error has occured **~RS\n";
char *nosuchroom="No room like that in this heaven...\n";
char *nosuchuser="There isn't anyone like that in this heaven...\n";
char *notloggedon="That person has left this heaven...\n";
char *invisenter="A presence arives in the room...\n";
char *invisleave="A presence leaves this room...\n";
char *invisname="Someone";
char *invisemote="Someone";
char *noswearing="\n*** We don't like language like that here ***\n";

/* Male level names */
char *level_name[]={
"BEING","MORTAL","IMMORTAL","GATEKEEPER","ANGEL","GOD","BEYOND","*"
};

/* Female Level Names */
char *level_name_fem[]={
"BEING","MORTAL","IMMORTAL","GATEKEEPER","ANGEL","GODDESS","BEYOND","*"
};

char *command[]={
"quit",    "look",      "mode",      "say",    "shout",
"tell",    "emote",     "semote",    "pemote", "echo",
"go",      "ignore",    "prompt",    "lobby",  "listen",
"ignall",  "public",    "private",   "knock",  "invite",
"to",      "move",      "bcast",     "who",    "people",
"help",    "shutdown",  "news",      "read",   "write",
"wipe",    "search",    "review",    "nethome",   "status",
"version", "rmail",     "smail",     "dmail",  "from",
"entpro",  "examine",   "rooms",      "rmsn",   "netstat",
"netdata", "connect",   "disconnect","passwd", "kill",
"promote", "demote",    "listbans",  "ban",    "unban",
"vis",     "invis",     "site",      "wake",   "wiztell",
"muzzle",  "unmuzzle",  "map",       "logging","minlogin",
"system",  "charecho",  "clearline", "fix",    "unfix",
"viewlog", "viewsys",   "cbuff",     "clone",  "destroy",
"myclones","allclones", "switch",    "csay",   "chear",
"cemote",  "swban",     "desc",      "cls",    "color",
"inphr",   "outphr",    "suicide",   "nuke",   "reboot",
"recount", "revtell",   "think",     "wemote", "imnew", 
"staff",   "management","rstat",     "entroom","topic",
"accreq",  "boot",      "arrest",    "unarrest","gender",  
"rules",   "srules",    "record",    "change",  "revwiz",
"reload",  "beep",      "afk",       "join",    "mash", 
"freeze",  "unfreeze",  "sing",      "naked",   "ssing",
"doh",     "qecho",     "hug",       "force",   "kiss",
"french",  "netsex",    "netsextwo", "yell",    "logoff",
"clsall",  "welcome",   "time",      "ranks",   "greet",
"samesite","wrap",      "uninvite",  "streak",  "lick",
"bop",     "set",       "paddle",    
"talkers",   "plist",     "ptell",   "rpic",
"vpic",    "wcbuff",    "scbuff",    "revshout","home",
"edit",    "rmkill",    "rmban",     "rmunban", "setposs",
"muds",    "ewtoo",     "nuts",      "shackle", "unshackle",
"makeinvis","makevis",  "hide",      "*"
};

/* Values of commands , used in switch in exec_com() */
enum comvals {
QUIT,      LOOK,       MODE,      SAY,     SHOUT,
TELL,      EMOTE,      SEMOTE,    PEMOTE,  ECHO,
GO,        IGNORE,     PROMPT,    LOBBY,   LISTEN,
IGNALL,    PUBCOM,     PRIVCOM,   KNOCK,   INVITE,
TO,        MOVE,       BCAST,     WHO,     PEOPLE,
HELP,      SHUTDOWN,   NEWS,      READ,    WRITE,
WIPE,      SEARCH,     REVIEW,    HOME,    STATUS,
VER,       RMAIL,      SMAIL,     DMAIL,   FROM,
ENTPRO,    EXAMINE,    RMST,      RMSN,    NETSTAT,
NETDATA,   CONN,       DISCONN,   PASSWD,  KILL,
PROMOTE,   DEMOTE,     LISTBANS,  BAN,     UNBAN,
VIS,       INVIS,      SITE,      WAKE,    WIZTELL,
MUZZLE,    UNMUZZLE,   MAP,       LOGGING, MINLOGIN,
SYSTEM,    CHARECHO,   CLEARLINE, FIX,     UNFIX,
VIEWLOG,   VIEWSYS,    CBUFF,     CREATE,  DESTROY,
MYCLONES,  ALLCLONES,  SWITCH,    CSAY,    CHEAR,
CEMOTE,    SWBAN,      DESC,      CLS,     COLOR,
INPHRASE,  OUTPHRASE,  SUICIDE,   NUKE,    REBOOT,
RECOUNT,   REVTELL,    THINK,     WEMOTE,  IMNEW,
STAFF,     MANAGEMENT, RSTAT,     ENTROOM, TOPIC,
ACCREQ,    BOOT,       ARREST,    UNARREST,GENDER,
RULES,     SRULES,     RECORD,    CHANGE,  REVWIZ,
RELOAD,    BEEP,       AFK,       JOIN,    GAG,
FREEZE,    UNFREEZE,   SING,      NAKED,   SSING,
DOH,       QECHO,      HUG,       FORCE,   KISS,
FRENCH,    NETSEX,     NETSEXTWO, YELL,    LOGOFF,
CLSALL,    WELCOME,    TIME,      RANKS,   GREET,
SAMESITE,  WRAP,       UNINVITE,  STREAK,  LICK,
BOP,       SET,        PADDLE,    
TALKERS,    PICLIST,   PICTELL,   ROOMPIC,
VIEWPIC,   WREVCLR,    SREVCLR,   REVSHOUT,MYROOM,
EDIT,      RMKILL,     RMBAN,     RMUNBAN, SETPOSS,
MUDS,      EWTOO,      NUTS,      SHACKLE,  UNSHACKLE,
MAKEINVIS, MAKEVIS,    HIDE
} com_num;


/* These are the minimum levels at which the commands can be executed. 
   Alter to suit. */
int com_level[]={
NEW,         NEW,         NEW,         NEW,         USER,
USER,        USER,        USER,        USER,        USER,
USER,        USER,        NEW,         NEW,         USER,
USER,        USER,        USER,        USER,        USER,
USER,        WIZ,         WIZ,         NEW,         WIZ,
NEW,         GOD,         USER,        NEW,         USER,
WIZ,         REG,         USER,        GOD,         NEW,
NEW,         NEW,         USER,        USER,        USER,
NEW,         USER,        USER,        USER,        WIZ,
CODER,       GOD,         GOD,         USER,        CODER,
CODER,       CODER,       WIZ,         WIZ,         WIZ,
REG,         REG,         WIZ,         WIZ,         WIZ,
WIZ,         WIZ,         USER,        GOD,         GOD,
GOD,         NEW,         WIZ,         GOD,         GOD,
WIZ,         GOD,         USER,        WIZ,         WIZ,
WIZ,         WIZ,         WIZ,         WIZ,         WIZ,
WIZ,         CODER,       NEW,         NEW,         NEW,
USER,        USER,        OWNER,       CODER,       GOD,
CODER,       USER,        USER,        WIZ,         NEW,
NEW,         NEW,         CODER,       WIZ,         NEW,
NEW,         WIZ,         WIZ,         WIZ,         NEW,
NEW,         WIZ,         WIZ,         GOD,         WIZ,
GOD,         USER,        USER,        USER,        WIZ,
CODER,       CODER,       USER,        USER,        REG,
USER,        GOD,         USER,        CODER,       USER,
USER,        USER,        USER,        WIZ,         WIZ,
WIZ,         REG,         NEW,         NEW,         REG,
WIZ,         NEW,         USER,        USER,        USER,
USER,        NEW,         USER,       
USER,        USER,        USER,        WIZ,
USER,        GOD,         WIZ,         USER,        USER,
USER,        OWNER,       OWNER,       OWNER,       OWNER,
USER,        USER,        USER,        WIZ,         WIZ,
WIZ,         WIZ,         REG
};

/* 
Colcode values equal the following:
RESET,BOLD,BLINK,REVERSE

Foreground & background colours in order..
BLACK,RED,GREEN,YELLOW/ORANGE,
BLUE,MAGENTA,TURQUIOSE,WHITE,BEEP
*/

char *colcode[NUM_COLS]={
/* Standard stuff */
"\033[0m", "\033[1m", "\033[4m", "\033[5m", "\033[7m",
/* Foreground colour */
"\033[30m","\033[31m","\033[32m","\033[33m",
"\033[34m","\033[35m","\033[36m","\033[37m",
/* Background colour */
"\033[40m","\033[41m","\033[42m","\033[43m",
"\033[44m","\033[45m","\033[46m","\033[47m",
"\07"
};

/* Codes used in a string to produce the colours when prepended with a '~' */
char *colcom[NUM_COLS]={
"RS","OL","UL","LI","RV",
"FK","FR","FG","FY",
"FB","FM","FT","FW",
"BK","BR","BG","BY",
"BB","BM","BT","BW",
"BP"
};

/* 47 Quotes */
char *quote_of_the_day[MAX_QUOTES]={
"Daddy, why doesn't this magnet pick up this floppy disk?\n",
"Give me ambiguity or give me something else.\n",
"I.R.S.: We've got what it takes to take what you've got!\n",
"We are born naked, wet and hungry.  Then things get worse.\n",
"Pentiums melt in your PC, not in your hand.\n",
"Suicidal twin kills sister by mistake!\n",
"The secret of the universe is @*&^^^ NO CARRIER\n",
"Did anyone see my lost carrier?\n",
"Make it idiot proof and someone will make a better idiot.\n",
"I'm not a complete idiot, some parts are missing!\n",
"He who laughs last thinks slowest!\n",
"Always remember you're unique, just like everyone else.\n",
"'More hay, Trigger?'  'No thanks, Roy, I'm stuffed!'\n",
"A flashlight is a case for holding dead batteries.\n",
"Lottery: A tax on people who are bad at math.\n",
"Error, no keyboard - press F1 to continue.\n",
"There's too much blood in my caffeine system.\n",
"Artificial Intelligence usually beats real stupidity.\n",
"I wouldn't be caught dead with a necrophiliac.\n",
"Learn from your parents' mistakes - use birth control!\n",
"Hard work has a future payoff.  Laziness pays off now.\n",
"Friends help you move.  Real friends help you move bodies.\n",
"I won't rise to the occaasion, but I'll slide over to it.\n",
"Ever notice how fast Windows runs?  Neither did I.\n",
"Double your drive space - delete Windows!\n",
"What is a 'free' gift ?  Aren't all gifts free?\n",
"Assassins do it from behind.\n",
"If ignorance is bliss, you must be orgasmic.\n",
"'Very funny, Scotty.  Now beam down my clothes.'\n",
"Puritanism: The haunting fear that someone, somewhere may be happy.\n",
"Consciousness: that annoying time between naps.\n",
"Oops.  My brain just hit a bad sector.\n",
"I used to have a handle on life, then it broke.\n",
"Don't take life too seriously, you won't get out alive.\n",
"I don't suffer from insanity.  I enjoy every minute of it.\n",
"The sex was so good that even the neighbors had a cigarette.\n",
"Better to understand a little than to misunderstand a lot.\n",
"The gene pool could use a little chlorine.\n",
"When there's a will, I want to be in it.\n",
"Okay, who put a 'stop payment' on my reality check?\n",
"Few women admit their age.  Few men act theirs.\n",
"I'm as confused as a baby in a topless bar.\n",
"We have enough youth, how about a fountain of SMART?\n",
"All generalizations are false, including this one.\n",
"Change is inevitable, except from a vending machine.\n",
"C program run.  C program crash.  C programmer quit.\n",
"'Criminal Lawyer' is a redundancy.\n"
};

char *month[12]={
"January","February","March","April","May","June",
"July","August","September","October","November","December"
};

char *day[7]={
"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"
};

char *noyes1[]={ " NO","YES" };
char *noyes2[]={ "NO ","YES" };
char *offon[]={ "OFF","ON " };

/* These MUST be in lower case - the contains_swearing() function converts
   the string to be checked to lower case before it compares it against
   these. Also even if you dont want to ban any words you must keep the 
   star as the first element in the array. */
char *swear_words[]={
"nigger","nigga","niger","niga","niggr","nigr","neger","negger","negr",
"neggr","*"
};

char verification[SERV_NAME_LEN+1];
char wizrevbuff[REVIEW_LINES][REVIEW_LEN+2];
char shoutrevbuff[REVIEW_LINES][REVIEW_LEN+2];
char text[ARR_SIZE*2];
char word[MAX_WORDS][WORD_LEN+1];
char wrd[8][81];
char progname[40],confile[40];
time_t boot_time;
jmp_buf jmpvar;

int port[3],listen_sock[3],wizport_level,minlogin_level;
int colour_def,password_echo,ignore_sigterm,wrevline,srevline;
int max_users,max_clones,num_of_users,num_of_logins,heartbeat;
int login_idle_time,user_idle_time,config_line,word_count;
int tyear,tmonth,tday,tmday,twday,thour,tmin,tsec;
int mesg_life,system_logging,prompt_def,no_prompt;
int force_listen,gatecrash_level,min_private_users;
int ignore_mp_level,rem_user_maxlevel,rem_user_deflevel;
int destructed,mesg_check_hour,mesg_check_min,net_idle_time;
int keepalive_interval,auto_connect,ban_swearing,crash_action;
int time_out_afks,allow_caps_in_name,rs_countdown;
int charecho_def,time_out_maxlevel,num_maps,user_count;
time_t rs_announce,rs_which;
UR_OBJECT rs_user;

/* extern char *sys_errlist[]; */
char *long_date();

/* Big letter array map - for greet() */
int biglet[26][5][5] = {
    0,1,1,1,0,1,0,0,0,1,1,1,1,1,1,1,0,0,0,1,1,0,0,0,1,
    1,1,1,1,0,1,0,0,0,1,1,1,1,1,0,1,0,0,0,1,1,1,1,1,0,
    0,1,1,1,1,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,1,1,1,1,
    1,1,1,1,0,1,0,0,0,1,1,0,0,0,1,1,0,0,0,1,1,1,1,1,0,
    1,1,1,1,1,1,0,0,0,0,1,1,1,1,0,1,0,0,0,0,1,1,1,1,1,
    1,1,1,1,1,1,0,0,0,0,1,1,1,1,0,1,0,0,0,0,1,0,0,0,0,
    0,1,1,1,0,1,0,0,0,0,1,0,1,1,0,1,0,0,0,1,0,1,1,1,0,
    1,0,0,0,1,1,0,0,0,1,1,1,1,1,1,1,0,0,0,1,1,0,0,0,1,
    0,1,1,1,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,1,1,1,0,
    0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,1,0,0,0,1,0,1,1,1,0,
    1,0,0,0,1,1,0,0,1,0,1,0,1,0,0,1,0,0,1,0,1,0,0,0,1,
    1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,1,1,1,1,
    1,0,0,0,1,1,1,0,1,1,1,0,1,0,1,1,0,0,0,1,1,0,0,0,1,
    1,0,0,0,1,1,1,0,0,1,1,0,1,0,1,1,0,0,1,1,1,0,0,0,1,
    0,1,1,1,0,1,0,0,0,1,1,0,0,0,1,1,0,0,0,1,0,1,1,1,0,
    1,1,1,1,0,1,0,0,0,1,1,1,1,1,0,1,0,0,0,0,1,0,0,0,0,
    0,1,1,1,0,1,0,0,0,1,1,0,1,0,1,1,0,0,1,1,0,1,1,1,0,
    1,1,1,1,0,1,0,0,0,1,1,1,1,1,0,1,0,0,1,0,1,0,0,0,1,
    0,1,1,1,1,1,0,0,0,0,0,1,1,1,0,0,0,0,0,1,1,1,1,1,0,
    1,1,1,1,1,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,
    1,0,0,0,1,1,0,0,0,1,1,0,0,0,1,1,0,0,0,1,1,1,1,1,1,
    1,0,0,0,1,1,0,0,0,1,0,1,0,1,0,0,1,0,1,0,0,0,1,0,0,
    1,0,0,0,1,1,0,0,0,1,1,0,1,0,1,1,1,0,1,1,1,0,0,0,1,
    1,0,0,0,1,0,1,0,1,0,0,0,1,0,0,0,1,0,1,0,1,0,0,0,1,
    1,0,0,0,1,0,1,0,1,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,
    1,1,1,1,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,1,1,1,1
    };

/* Symbol array map - for greet() */
int bigsym[32][5][5] = {
    0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,
    0,1,0,1,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,1,0,1,0,1,1,1,1,1,0,1,0,1,0,1,1,1,1,1,0,1,0,1,0,
    0,1,1,1,1,1,0,1,0,0,0,1,1,1,0,0,0,1,0,1,1,1,1,1,0,
    1,1,0,0,1,1,1,0,1,0,0,0,1,0,0,0,1,0,1,1,1,0,0,1,1,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,1,1,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,1,1,0,
    0,1,1,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,1,1,0,0,
    1,0,1,0,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,0,1,0,1,0,1,
    0,0,1,0,0,0,0,1,0,0,1,1,1,1,1,0,0,1,0,0,0,0,1,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,1,0,0,0,
    0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,1,1,0,0,0,
    0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,
    0,1,1,1,0,1,0,0,1,1,1,0,1,0,1,1,1,0,0,1,0,1,1,1,0,
    0,0,1,0,0,0,1,1,0,0,0,0,1,0,0,0,0,1,0,0,0,1,1,1,0,
    1,1,1,1,0,0,0,0,0,1,0,1,1,1,0,1,0,0,0,0,1,1,1,1,1,
    1,1,1,1,0,0,0,0,0,1,0,1,1,1,0,0,0,0,0,1,1,1,1,1,0,
    0,0,1,1,0,0,1,0,0,0,1,0,0,1,0,1,1,1,1,1,0,0,0,1,0,
    1,1,1,1,1,1,0,0,0,0,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,
    0,1,1,1,0,1,0,0,0,0,1,1,1,1,0,1,0,0,0,1,0,1,1,1,0,
    1,1,1,1,1,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,
    0,1,1,1,0,1,0,0,0,1,0,1,1,1,0,1,0,0,0,1,0,1,1,1,0,
    1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,0,0,0,0,1,0,0,0,0,1,
    0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,
    0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1,0,
    0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,
    0,1,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,
    0,1,1,1,1,0,0,0,0,1,0,0,1,1,1,0,0,0,0,0,0,0,1,0,0,
    0,1,0,0,0,1,0,1,1,1,1,0,1,0,1,1,0,1,1,1,0,1,1,1,0
    };
