/*****************************************************************************
       Object system version 1.1.1 - by Kevin Wojtysiak (Magius)
                        Last update: 16th November, 2001

                                wojtyk@home.com

                        compatible with Amnuts 2.2.1 by
                                Andrew Collington
                 amnuts@talker.com  |  http://amnuts.talker.com/
*****************************************************************************/

/* definitions used */

#define OBJECT_NAME_LEN 255
#define OBJECT_DESC_LEN 5000
#define MAX_LABELS 10
#define OBJECT_LABEL_LEN 30
#define MAX_NUMOF_USER_OBJECTS 10
#define MAX_NUMOF_ROOM_OBJECTS 10

/* object information structure */
struct object_struct {
  int id;
  char name[OBJECT_NAME_LEN+1];
  char desc[OBJECT_DESC_LEN+1];
  int cost,fixed,worn,drinkable,edible,rare,level,disabled,age,pet,locked;
  char label[MAX_LABELS][OBJECT_LABEL_LEN+1];
  char owner[USER_NAME_LEN+1];
  char petname[USER_RECAP_LEN+1];
  int expire_time;
  struct object_struct *next,*prev;
  };
typedef struct object_struct *O_OBJECT;
O_OBJECT object_first,object_last;
