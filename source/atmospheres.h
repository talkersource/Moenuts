/******************************************************************************

     MoeNUTS v1.52 Atmospheres Library v1.0 (C)1998 Moesoft Developments.

       This library file may serve as an example, but may not be used
        in whole or in part without the expressed written concent of
     Moesoft Developments.  (C)1998 Michael Irving, All Rights Reserved.

******************************************************************************/

#define ATMOSLIBID   "Forgotten Island Atmospheres Library v1.01"
#define ATMOSPHERES  10   /* How Many Atmospheres Do You Allow Per Room?  */
#define ATMOS_LEN    160  /* Maximum length Of Atmospheres                */
#define ATMOS_FREQ   120  /* Frequency in which atmospheres are displayed */
#define ATMOS_STRING "\n~FY%s\n\n"

/** Default Atmospheres (If atmosphere file doesn't exist)        **/
/** Note, Personal rooms do NOT get assigned default atmospheres! **/

char *atmos[ATMOSPHERES]={
"A soft breaze brushes through your hair...",
"The sound of other dinosaurs can be heard in the distance...",
"A slight roar is heard way off in the distance...",
"The sound of young dinosaurs happily playing fills your ears.",
"A cool breaze surrounds your body...",
"You quickly step aside before you're stomped on by a tyranosaurus rex.",
"The beautiful smell of the wild flowers overwelms your sences...",
"If you need help, you can use .help, and .help <command> or ask a wiz!",
"Quick!!  DUCK!!",
"A scream is heard as one of the cave people get chased by a velociraptor."
};
