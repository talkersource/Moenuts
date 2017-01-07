/******************************************************************************

     MoeNUTS v1.52 Atmospheres Library v1.0 (C)1998 Moesoft Developments.

       This library file may serve as an example, but may not be used
        in whole or in part without the expressed written consent of
     Moesoft Developments.  (C)1998 Michael Irving, All Rights Reserved.

******************************************************************************/

#define ATMOSLIBID   "Rue DeSade Atmospheres Library"
#define ATMOSPHERES  15   /* How Many Atmospheres Do You Allow Per Room?  */
#define ATMOS_LEN    160  /* Maximum length Of Atmospheres                */
#define ATMOS_FREQ   360  /* Frequency in which atmospheres are displayed */
#define ATMOS_STRING "\n~FR=] ~FM%s ~FR[=\n\n"

/** Default Atmospheres (If atmosphere file doesn't exist (room.RA))     **/
/** Note, Personal rooms do NOT get assigned default atmospheres!        **/

char *atmos[ATMOSPHERES]={
"Faint screams of pleasure and pain are heard in the distance.",
"The smell of lust fills the air.",
"The crack of a whip can be heard off in the distance.",
"The sense of diciplin around you makes you alert.",
"A cool breeze brushes over your naked skin making you shiver.",
"The faint rattle of chains can be heard.",
"'Yes Master' can be heard muttered by a slave to her master.",
"'As you wish' can be heard muttered by a slave to his domme.",
"Someone snickers, 'Sticks and stones may break my bones, but whips and chains excite me!'",
"You become more aware of your surroundings as sounds of diciplin are heard in the background.",
"x",
"x",
"x",
"x",
"x"
};
