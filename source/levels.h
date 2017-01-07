
/******************************************************************/
/*              User Levels Used Through The Code                 */
/******************************************************************/
#define NEW     0 /* Define The Level For New Users               */
#define USER    1 /* Define The Level For Regular Users           */
#define MEMBER  1 /* Define The Level For Members                 */
#define WIZ     2 /* Define The Level For Wizards (Staff)         */
#define ARCH    3 /* Define The Level For Archs   (Staff)         */
#define GOD     4 /* Define The Level For Gods    (Staff)         */
/******************************************************************/
/* Level For Removing Most Restrictions 1+GOD = Keep Restrictions */
/******************************************************************/
#define OWNER   4 /* Define The Level The Owners Are Offically At */
/******************************************************************/

/* Male level names */

char *level_name[]={
"New","User","Wizard","Admin","God","*"
};
/* Female Level Names */

char *level_name_fem[]={
"New","Lady","Wizard","Admin","Godess","*"
};
