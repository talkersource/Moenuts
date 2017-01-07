/*******************************************************************
 *  This AFK Library written for the Moenuts v1.73 Public Release  *
 *******************************************************************/

/* Setup Library Variables */

#define MAX_AFK  6  /* How Many AFK  Messages Are There? */
#define MAX_BAFK 6  /* How Many BAFK Messages Are There? */
#define AFKLIBID "Moenuts v1.73 Default AFK Prompts Library"

/* Use %s Where You Want The AFK/BAFK String To Be Inserted In The
   AFKSTRING and BAFKSTRING Variables. */

#define AFKSTRING  "\n~CRAFK ~CY: ~CR%s~RS \n"
#define BAFKSTRING "\n~CGBAFK~CY: ~CG%s~RS \n"

/* Use %s Where You Want The user's name to appear in the AFK/BAFK String */

/* User AFK Messages */

char *afk_msg[MAX_AFK]={
"%s passes out at the keyboard.",
"%s has gone AFK.",
"%s goes AFK.",
"%s leaves the keyboard for a moment...",
"%s falls asleep at the keyboard.",
"%s has gone potty, maybe?"
};

/* User Back from AFK Messages */

char *bafk_msg[MAX_AFK]={
"%s wakes up, dazed and confused.",
"%s has returned.",
"%s returns.",
"%s returns to the keyboard.",
"%s wakes up.",
"%s returns from all potty activities."
};
