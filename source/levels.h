/*******************************************************************/
/*              User Levels Used Through The Code                  */
/*******************************************************************/
#define NEW      0 /* Define The Level For New Users               */
#define USER     1 /* Define The Level For Regular Users           */
#define MEMBER   1 /* Define The Level For Members                 */
#define WIZ      2 /* Define The Level For Wizards (Staff)         */
#define GAMES    3 /* Define The Level For Archs   (Staff)         */
#define ARCH     4 /* Define The Level For Archs   (Staff)         */
#define GOD      5 /* Define The Level For Gods    (Staff)         */
/*******************************************************************/
/* Level For Removing Most Restrictions 1+GOD = Keep Restrictions  */
/*******************************************************************/
#define OWNER    5 /* Define The Level The Owners Are Offically At */
#define BEYOND   5 /* Don't Want PPL Doing Stuff Theyre Not Allowed*/
#define NEWLEVEL 0 /* New User Level                               */
/*******************************************************************/

/* Male level names */

char *level_name[]={
"New","User","Wizard","Games","Arch","God","*"
};

/* Female Level Names */

char *level_name_fem[]={
"New","User","Wizard","Games","Arch","God","*"
};

