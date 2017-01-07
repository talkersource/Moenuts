/****************************************************************************

 This AFK Library written EXCLUSIVELY for Angel and Moe's Forgotten Island
 Telnet Chat Server!  Nobody else may use this library without permission!

****************************************************************************/

/* Setup Library Variables */

#define MAX_AFK  6  /* How Many AFK  Messages Are There? */
#define MAX_BAFK 6  /* How Many BAFK Messages Are There? */
#define AFKLIBID "Forgotten Island AFK Library v1.01"

/* Use %s Where You Want The AFK/BAFK String To Be Inserted In The
   AFKSTRING and BAFKSTRING Variables. */

#define AFKSTRING  "\n~OL~FR<- ~RS%s ~RS~OL~FR->~FT(AFK)\n"
#define BAFKSTRING "\n~OL~FY-> ~RS%s ~RS~OL~FY<-\n"

/* Use %s Where You Want The user's name to appear in the AFK/BAFK String */

/* User AFK Messages */

char *afk_msg[MAX_AFK]={
"~OL~FWSnap...Snap...Snap...Hmmm...Strange... ~FT%s ~FMseems to be out cold...",
"~OL~FT%s ~FGis doing something they don't want ~FYyou~FG to see...",
"~OL~FG%s ~FBis roaming the caves, or is dinner to a dino or something...",
"~OL~FR%s ~RS~FRslips into a coma...",
"~OL~FWACK!! The Boss!!  ~RS~FT%s pretends to be working...",
"~OL~FG%s ~FMis currently indisposed at the moment..."
};

/* User Back from AFK Messages */

char *bafk_msg[MAX_AFK]={
"~OL~FW%s awakens in a dazed and confused state... 'HUH?  Where am I?', 'Who are you?'",
"~OL~FT%s re-joins our regularily schedule'd program already in progress...",
"~OL~FG%s returns from the magical mystical world of AFK land...",
"~OL~FM%s runs into the room screaming, 'HELP HELP!!! THE AFK MONSTER'S AFTER ME!!'",
"~OL~FR%s manages to get away from the nasty dino before they are lunch!",
"~OL~FB%s barely escapes the grasp of the AFK monster to make it back here..."
};
