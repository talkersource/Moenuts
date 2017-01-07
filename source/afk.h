/****************************************************************************

 This AFK Library written EXCLUSIVELY for Rue DeSade.
 Nobody else may use this library without permission!

****************************************************************************/

/* Setup Library Variables */

#define MAX_AFK  6  /* How Many AFK  Messages Are There? */
#define MAX_BAFK 6  /* How Many BAFK Messages Are There? */
#define AFKLIBID "Rue DeSade AFK Library v1.0.0"

/* Use %s Where You Want The AFK/BAFK String To Be Inserted In The
   AFKSTRING and BAFKSTRING Variables. */

#define AFKSTRING  "\n~CR<- ~RS%s ~RS~CR->~CT[AFK]\n"
#define BAFKSTRING "\n~CY-> ~RS%s ~RS~CY<-\n"

/* Use %s Where You Want The user's name to appear in the AFK/BAFK String */

/* User AFK Messages */

char *afk_msg[MAX_AFK]={
"~CW*THUD*  %s's head just hit the keyboard... ~CB(~CTOut Cold!~CB)",
"~CT%s ~FGis doing something they don't want ~CYyou~CG to see...",
"~CG%s ~FBis bound in the dungeon at the moment...",
"~CR%s ~RS~CRslips into another reality...",
"~CW%s falls asleep at the keyboard.. ZZZZzzzzzzzzzzz...",
"~CG%s ~FMis currently indisposed at the moment..."
};

/* User Back from AFK Messages */

char *bafk_msg[MAX_AFK]={
"~CW%s awakes, dazed and confused... 'Where am I?'",
"~CT%s re-joins our regularily schedule'd reality...",
"~CG%s returns with key marks on their forehead...(That's gotta hurt)",
"~CM%s runs into the room screaming...",
"~CR%s escapes the shackles and chains to rejoin us!",
"~CB%s returns from AFK Land..."
};
