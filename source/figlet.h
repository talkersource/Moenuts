#define MYSTRLEN(x) ((int)strlen(x)) /* Eliminate ANSI problem. */
/* If your server already has figlets installed, you can use the fonts from
   there.. (Usually found in /usr/games/lib/figlet.dir ) */
/* #define FONT_DIR "fonts" */
#define FONT_DIR "/usr/games/lib/figlet.dir"
typedef long inchr; /* "char" read from stdin */
inchr *inchrline;   /* Alloc'd inchr inchrline[inchrlinelenlimit+1]; */
int inchrlinelen,inchrlinelenlimit;

typedef struct fc {
	inchr ord;
	char **thechar;  /* Alloc'd char thechar[charheight][]; */
	struct fc *next;
	} fcharnode;

fcharnode *fcharlist;
char **currchar,**outline;
int currcharwidth,outlinelen,justification,right2left,outputwidth;
int outlinelenlimit,charheight,defaultmode;
char hardblank;
