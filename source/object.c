#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#define _OBJECT_SOURCE
  #include "prototypes.h"
#undef _OBJECT_SOURCE

/* Much of this code mimics Amnuts user/room struct functions (thanks Andy ;) ) */

/*** Construct object object (kinda redundant, huh?)  :> ***/
O_OBJECT create_object(void)
{
O_OBJECT o;

if ((o=(O_OBJECT)malloc(sizeof(struct object_struct)))==NULL) {
  write_syslog("ERROR: Memory allocation failure in create_object().\n",0);
  return NULL;   
  }
/* Append object into linked list. */
if (object_first==NULL) {
  object_first=o;  o->prev=NULL;
  }
else {
  object_last->next=o;  o->prev=object_last;
  }
o->next=NULL;
object_last=o;

reset_object(o);
return o;
}

void reset_object(O_OBJECT object)
{
int i=0;
object->name[0] = '\0';
object->desc[0] = '\0';
object->cost=0;
object->fixed=1;
object->id=0;
object->pet=0;
object->edible=0;
object->drinkable=0;
object->locked=0;
object->rare=0;
object->level=0;
object->disabled=0;
object->age=0;
for (i=0;i<MAX_LABELS;++i) object->label[i][0]='\0';
}

/*** Destruct an object. ***/
void destruct_object(O_OBJECT object)
{
/* Remove from linked list */
if (object==object_first) {
  object_first=object->next;
  if (object==object_last) object_last=NULL;
  else object_first->prev=NULL;
  }
else {
  object->prev->next=object->next;
  if (object==object_last) {
    object_last=object->prev;  object_last->next=NULL;
    }
  else object->next->prev=object->prev;
  }   
free(object);
}

/* Load object info */
void init_object(O_OBJECT object)
{
FILE *fp;
char c,filename[80],line[ARR_SIZE];
int i;

  sprintf(filename,"%s/%d.O",OBJFILES,object->id);
  if (!(fp=fopen(filename,"r"))) {
    fprintf(stderr,"|> Moenuts: Can't open object file for object %d.\n",object->id);
    sprintf(text,"ERROR: Couldn't open object file for object %d.\n",object->id);
    write_syslog(text,0);
    return;
    }
  i=0;
fgets(line,OBJECT_NAME_LEN-1,fp);  line[strlen(line)-1]=0;  strcpy(object->name,line);
fgets(line,ARR_SIZE-1,fp);  line[strlen(line)-1]=0;
sscanf(line,"%s %s %s %s %s %s %s %s %s %s %s",wrd[0],wrd[1],wrd[2],wrd[3],wrd[4],wrd[5],wrd[6],wrd[7],wrd[8],wrd[9],wrd[10]);
object->cost=atoi(wrd[0]);
object->fixed=atoi(wrd[1]);
object->worn=atoi(wrd[2]);
object->drinkable=atoi(wrd[3]);
object->edible=atoi(wrd[4]);
object->rare=atoi(wrd[5]);
object->level=atoi(wrd[6]);
object->disabled=atoi(wrd[7]);
object->age=atoi(wrd[8]);
object->pet=atoi(wrd[9]);
object->locked=atoi(wrd[10]);
line[0]='\0';
while(line[0]!='*')
{
fgets(line,OBJECT_LABEL_LEN-1,fp);  line[strlen(line)-1]=0;  strcpy(object->label[i],line);
i++;
}

i=0;
  c=getc(fp);  
  while(!feof(fp)) {
    if (i==OBJECT_DESC_LEN) {
      fprintf(stderr,"|> Objects: Description too long for object %d.\n",object->id);      
      sprintf(text,"Object Error: Description too long for object %d.\n",object->id);
      write_syslog(text,0);
      break;
      }
    object->desc[i]=c;
    c=getc(fp);
    ++i;
    }
  object->desc[i]='\0';
  fclose(fp);
}

/* Load and parse all objects in the OBJFILES dir */
void parse_objects(void) {
DIR *dirp;
struct dirent *dp;
char dirname[80];
O_OBJECT o;

sprintf(dirname,"%s",OBJFILES);
if (!(dirp=opendir(dirname))) {
  fprintf(stderr,"|> Moenuts: Directory open failure in parse_objects().\n");
  boot_exit(19);
  }
/* parse the names of the files but don't include . and .. */
while((dp=readdir(dirp))!=NULL) {
  if (!strcmp(dp->d_name,".") || !strcmp(dp->d_name,"..")) continue;
  if (strstr(dp->d_name,".O")) {
  sscanf(dp->d_name,"%s.%s",wrd[0],wrd[1]);
	if ((o=create_object())!=NULL) {
	    o->id = atoi(wrd[0]);
	    init_object(o);
	}
    }
  }
(void) closedir(dirp);
}

/*** Parse the room objects ***/
void parse_room_objects(void) {
RM_OBJECT rm;

for(rm=room_first;rm!=NULL;rm=rm->next) {
  room_object_store(rm->name,0,rm);
  }
}

/*** Show talker objects ***/
void objects(UR_OBJECT user, int wrap)
{
O_OBJECT o;
int obj_cnt;
char text2[ARR_SIZE+1];
char temp[OBJECT_NAME_LEN];

  if (!wrap) user->wrap_object=object_first;
  write_user(user,"\n~CB.-----.-----------------------------------.--------------------------------.\n");
  write_user(user,"~CB| ~CMID# ~CB| ~CTObject Description                ~CB|  ~CGCost  ~CYF W D E R LEV X AGE P ! ~CB|\n");
  write_user(user,"~CB|-----|-----------------------------------|--------------------------------|\n");
  obj_cnt=0;
  for(o=user->wrap_object;o!=NULL;o=o->next) {
    strcpy(temp,colour_com_strip(o->name)); temp[70]='\0';
    sprintf(text,"~CB| ~CM%3.3d ~CB| ~RS%-33.33s ~CB| ~CG%6.6d ~CY%s %s %s %s %s %3.3d %s %3.3d %s %s ~CB|\n",o->id,temp,o->cost,yn(o->fixed),yn(o->worn),yn(o->drinkable),yn(o->edible),yn(o->rare),o->level,yn(o->disabled),o->age,petmf(o->pet),yn(o->locked));
    write_user(user,text);
    ++obj_cnt;  user->wrap_object=o->next;
    }
  user->misc_op=0;
  write_user(user,"~CB|-----'-----------------------------------'--------------------------------|\n");
  sprintf(text,"There %s a total of %3.3d objects.",(obj_cnt==1?"is":"are"),obj_cnt);
  sprintf(text2,"~CB| ~CG%-72.728s ~CB|\n",text);
  write_user(user,text2);
  write_user(user,"~CB| ~CTFlags: ~CY[C]ost, [F]ixed, [W]earable,   [D]rinkable,  [E]dible             ~CB|\n");
  write_user(user,"~CB| ~FT------ ~CY[R]are, [L]evel, [X]-Disabled, [A]ge, [P]et, [!]-Lock             ~CB|\n");
  write_user(user,"~CB`--------------------------------------------------------------------------'\n");
  return;
}

/*** Get object struct pointer from id ***/
O_OBJECT get_object(int id) {
O_OBJECT o;

/* Search for id  */
for(o=object_first;o!=NULL;o=o->next) {
  if (id==o->id)  return o;
  }
return NULL;
}

/* function for creating/destroying an object in current room */
void handle_room_object(UR_OBJECT user)
{
O_OBJECT o;

if (word_count<3) {
  write_user(user,"Usage: roomobj create/destroy <obj_id>\n");
  return;
  }
if ((o=get_object(atoi(word[2])))==NULL) {
  write_user(user,nosuchobject);
  return;  
  }
if (!strcasecmp("create",word[1])) {
	if (add_object_to_room(o,user->room,1))
	{
	sprintf(text,"You get %s~RS and drops it in the room.\n",o->name);
	write_user(user,text);
	sprintf(text,"%s~RS gets %s~RS and drops it in the room.\n",user->recap,o->name);
	write_room_except(user->room,text,user);
	}
	else
	{
	write_user(user,"Sorry, adding another object would exceed max limit.\n");
	}
  return;
  }
else if (!strcasecmp("destroy",word[1])) {
        if (remove_object_from_room(o,user->room,1))
        {
        sprintf(text,"~CRYou place %s~CR in a black hole and it vanishes from existance.\n",o->name);
	write_user(user,text);
	sprintf(text,"~CR<~CY!~CR>~CM -> ~CT%s~CT places %s~CT into a black hole and it vanishes from existance.\n",user->recap,o->name);
	write_room_except(user->room,text,user);
        }
        else
        {   
        write_user(user,"~CR<~CY!~CR> ~CM-> ~CTPick something that exists next time.\n");
        }
  return;
  }
else {
  write_user(user,"Usage: roomobj create/destroy <obj_id>\n");  
}
}

/* function for creating/destroying an object in a given user struct */
void handle_user_object(UR_OBJECT user)
{
O_OBJECT o;
UR_OBJECT u;

if (word_count<4) {
  write_user(user,"Usage: userobj create/destroy <user> <obj_id>\n");
  return;
  }
if ((o=get_object(atoi(word[3])))==NULL) {
  write_user(user,nosuchobject);
  return;  
  }
if (!(u=get_user(word[2]))) {
  write_user(user,notloggedon);  return;
  }
if (!strcasecmp("create",word[1])) {
	if (add_object_to_user(o,u,1))
	{
		if (u==user)
		{
		sprintf(text,"~CM& ~CTYou get %s~CT.\n",o->name);
		write_user(user,text);
		sprintf(text,"~CM& ~CT%s~CT gets %s~CT.\n",user->recap,o->name);
		write_room_except(user->room,text,user);
		}
		else
		{
		sprintf(text,"~CM& ~CTYou get %s~CT and give it to %s~CT.\n",o->name,u->recap);
		write_user(user,text);
		sprintf(text,"~CM& ~CT%s~CT gets %s~CT and gives it to %s~CT.\n",user->recap,o->name,u->recap);
		write_room_except(user->room,text,user);
		}
	}
	else
	{
	write_user(user,"Sorry, adding another object would exceed max limit.\n");
	}
  return;
  }
else if (!strcasecmp("destroy",word[1])) {
        if (remove_object_from_user(o,u,1))
        {
		if (u==user)
		{
        	sprintf(text,"~CR& ~CTYou snap your fingers and your %s~CT disappears.\n",o->name);
		write_user(user,text);
		sprintf(text,"~CR& ~CT%s~CT snaps their fingers and their %s~CT disappears from existence.\n",user->recap,o->name);
		write_room_except(user->room,text,user);
		}
		else
		{
	        sprintf(text,"~CR& ~CTYou point at %s~CT and their %s~CT disappears.\n",u->recap,o->name);
		write_user(user,text);
		sprintf(text,"~CR& ~CT%s~CT points at %s~CT and their %s~CT disappears from existence.\n",user->recap,u->recap,o->name);
		write_room_except(user->room,text,user);
		}
        }
        else
        {   
        write_user(user,"~CR<~CY!~CR> ~CM-> ~CTPick something that exists next time.\n");
        }
  return;
  }
else {
  write_user(user,"Usage: userobj create/destroy <obj_id> <user>\n");  
}
}

int add_object_to_room(O_OBJECT obj,RM_OBJECT rm,int save)
{
int i=0;

while (rm->objects[i])
{
	if (obj->id==rm->objects[i]->id)
	{
	if (rm->object_count[i]+1 > MAX_NUMOF_ROOM_OBJECTS) { return 0; }
	rm->object_count[i]++;
	if (save) { room_object_store(rm->name,1,rm); }
	return 1;
	}
i++;
}
if (i+1>MAX_ROOM_OBJECTS) { return 0; }
rm->objects[i] = obj;
rm->object_count[i] = 1;
if (save) { room_object_store(rm->name,1,rm); }
return 1;
}

int remove_object_from_room(O_OBJECT obj,RM_OBJECT rm,int delete)
{
int i=0,tmp1,tmp2,retval=0,newfile=0;
FILE *infp,*outfp;
char filename[80];

while (rm->objects[i])
{
	if (obj->id==rm->objects[i]->id)
	{
		if (rm->object_count[i] > 1)
		{
		rm->object_count[i]--;
		retval = 1; break;
		}
		else if (rm->object_count[i] == 1)
		{
			while (rm->objects[i])
			{
			rm->objects[i] = rm->objects[i+1];
			rm->object_count[i] = rm->object_count[i+1];
			i++;
			}
		retval = 1; break;
		}
	}
i++;
}
if (delete) // also delete from savefile
{
sprintf(filename,"%s/%s.O",DATAFILES,rm->name);
if (!(infp=fopen(filename,"r"))) return 0;
if ((outfp=fopen("tempfile","w"))) {
  while(!feof(infp))
  {
  fscanf(infp,"%d %d\n",&tmp1,&tmp2);
    if (obj->id==tmp1) { tmp2--; }
    if (tmp2) { newfile = 1; fprintf(outfp,"%d %d\n",tmp1,tmp2); }
  }
  fclose(infp);
  fclose(outfp);
  if (newfile)
  {
  rename("tempfile",filename);
  unlink("tempfile");
  }
  else
  {
  unlink("tempfile");
  unlink(filename);
  }
}
}
return retval;
}

int add_object_to_user(O_OBJECT obj,UR_OBJECT u,int save)
{
int i=0;

while (u->objects[i])
{
	if (obj->id==u->objects[i]->id)
	{
	if (u->object_count[i]+1 > MAX_NUMOF_USER_OBJECTS) { return 0; }
	u->object_count[i]++;
        if (save) { user_object_store(u->name,1,u); }
	return 1;
	}
i++;
}
if (i+1>MAX_USER_OBJECTS) { return 0; }
u->objects[i] = obj;
u->object_count[i] = 1;
if (save) { user_object_store(u->name,1,u); }
return 1;
}

int remove_object_from_user(O_OBJECT obj,UR_OBJECT u,int delete)
{
int i=0,retval=0,tmp1,tmp2,newfile=0;
FILE *infp,*outfp;
char filename[80];

while (u->objects[i])
{
	if (obj->id==u->objects[i]->id)
	{
		if (u->object_count[i] > 1)
		{
		u->object_count[i]--;
		retval = 1; break;
		}
		else if (u->object_count[i] == 1)
		{
			while (u->objects[i])
			{
			u->objects[i] = u->objects[i+1];
			u->object_count[i] = u->object_count[i+1];
			i++;
			}
		retval = 1; break;
		}
	}
i++;
}
if (delete) // also delete from savefile
{
sprintf(filename,"%s/%s/%s.O",USERFILES,USEROBJECTS,u->name);
if (!(infp=fopen(filename,"r"))) return 0;
if ((outfp=fopen("tempfile","w"))) {
  while(!feof(infp))
  {
  fscanf(infp,"%d %d\n",&tmp1,&tmp2);
    if (obj->id==tmp1) { tmp2--; }
    if (tmp2) { newfile = 1; fprintf(outfp,"%d %d\n",tmp1,tmp2); }
  }
  fclose(infp);
  fclose(outfp);
  if (newfile)
  {
  rename("tempfile",filename);
  unlink("tempfile");
  }
  else
  {
  unlink("tempfile");
  unlink(filename);
  }
}
}
return retval;
}

/* save and load room object information
   if store=0 then read info from file else store.
   */
int room_object_store(char *name, int store, RM_OBJECT rm) {
FILE *fp;
char filename[80];
int i,count,id;
O_OBJECT o;

if (rm==NULL) return 0;
/* load the info */  
if (!store) {
  reset_room_objects(rm);
  sprintf(filename,"%s/%s.O",DATAFILES,name);
  if (!(fp=fopen(filename,"r"))) { return 0; }
  while(!feof(fp))
  {
  fscanf(fp,"%d %d\n",&id,&count);
    if ((o=get_object(id))==NULL) { continue; }
    for (i=0;i<count;i++)
    {
    add_object_to_room(o,rm,0);
    }
  }
  fclose(fp);
  return 1;
}
/* save info */
sprintf(filename,"%s/%s.O",DATAFILES,name);
if (!(fp=fopen(filename,"w"))) return 0;

i=0;
while(rm->objects[i])
{
fprintf(fp,"%d %d\n",rm->objects[i]->id,rm->object_count[i]);
i++;
}
fclose(fp);
return 1;
}

/* save and load room user information
   if store=0 then read info from file else store.
   */
int user_object_store(char *name, int store, UR_OBJECT u) {
FILE *fp;
char filename[80];
int i,id,count;
O_OBJECT o;

if (u==NULL) return 0;
/* load the info */  
if (!store) {
reset_user_objects(u);
  sprintf(filename,"%s/%s/%s.O",USERFILES,USEROBJECTS,name);
  if (!(fp=fopen(filename,"r"))) { return 0; }
  while(!feof(fp))
  {
  fscanf(fp,"%d %d\n",&id,&count);
    if ((o=get_object(id))==NULL) { continue; }
    for (i=0;i<count;i++)
    {
    add_object_to_user(o,u,0);
    }
  }
  fclose(fp);
  return 1;
}
/* save info */
sprintf(filename,"%s/%s/%s.O",USERFILES,USEROBJECTS,name);
if (!(fp=fopen(filename,"w"))) return 0;

i=0;
while(u->objects[i])
{
fprintf(fp,"%d %d\n",u->objects[i]->id,u->object_count[i]);
i++;
} 
fclose(fp);
return 1;
}

/* Reload object descriptions (if manual objectfile editing is done) */
/* (assumes object id order is consecutive) */

void reload_object_descriptions(UR_OBJECT user)
{
FILE *fp;
O_OBJECT o;
int i,count=0;
char filename[80];

if (word_count<2) {
  write_user(user,"Usage: rloadobj -a/<object id>\n");
  return;
  }
/* if reload all of the objects */
if (!strcmp(word[1],"-a"))
{
	for(o=object_first;o!=NULL;o=o->next) {
	init_object(o);
	count++;
	}

	i = count + 1;

	while(1)
	{
	  sprintf(filename,"%s/%d.O",OBJFILES,i);
	  if (!(fp=fopen(filename,"r"))) {
	    break;
	    }
	    fclose(fp);
		if ((o=create_object())!=NULL) {
		    o->id = i;
		    init_object(o);
		}
	i++;
	}
	sprintf(text,"%d object(s) reloaded, %d new object(s) loaded (%d total).\n",count,i-count-1,i-1);
	write_user(user,text);
}
else
{
	/* if it's just one object to reload */
	if ((o=get_object(atoi(word[1])))!=NULL) {
  	    sprintf(filename,"%s/%s.O",OBJFILES,word[1]);
	    if (!(fp=fopen(filename,"r"))) {
     	    write_user(user,nosuchobject);
	    return;
            }
	fclose(fp);
	init_object(o);
	sprintf(text,"Reloaded object %s (%s~RS).\n",word[1],o->name);
	write_user(user,text);
	return;
  	}
	write_user(user,"No such object in memory.\n");
}
}

void reset_user_objects(UR_OBJECT user)
{
int i=0;

while (i<MAX_USER_OBJECTS)
{
user->objects[i] = NULL;
user->object_count[i] = 0;
i++;
}
}

void reset_room_objects(RM_OBJECT room)
{
int i=0;
  
while (i<MAX_ROOM_OBJECTS)
{
room->objects[i] = NULL;
room->object_count[i] = 0;
i++; 
}
}

/* this is something of a wrapper function, needed to keep */
/* track of count from user to room lists (didnt feel like */
/* messing with "fake" pass-by-reference in C */

O_OBJECT find_object(UR_OBJECT user,RM_OBJECT rm,char *label,int count)
{
int i=0,j=0;
O_OBJECT o;

strtolower(label);
 
while (user->objects[i])
{
        while (user->objects[i]->label[j][0]!='\0')
        {
                if (!strcmp(label,user->objects[i]->label[j]))
                {
	                count--; 
                        if (!count)
                        {
        	                o = user->objects[i];
        	                return o;
                        }
                }
        	j++;
        }
	j=0;
	i++;
}
i=j=0;
while (rm->objects[i])
{
        while (rm->objects[i]->label[j][0]!='\0')
        {
                if (!strcmp(label,rm->objects[i]->label[j]))
                {
                count--;
                        if (!count)
                        {
                        o = rm->objects[i];
                        return o;
                        }
                }
        j++;
        }
j=0;
i++;
}
return NULL;
}

O_OBJECT find_object_on_user(UR_OBJECT user,char *label,int count)
{
int i=0,j=0;
O_OBJECT o;

strtolower(label);  

while (user->objects[i])
{
	while (user->objects[i]->label[j][0]!='\0')
	{
		if (!strcmp(label,user->objects[i]->label[j]))
		{
		count--;
			if (!count)
			{
			o = user->objects[i];
			return o;
			}
		}
	j++;
	}
j=0;
i++;
}
return NULL;
}

O_OBJECT find_object_in_room(RM_OBJECT rm,char *label,int count)
{
int i=0,j=0;
O_OBJECT o;

strtolower(label);

while (rm->objects[i])
{
	while (rm->objects[i]->label[j][0]!='\0')
	{
		if (!strcmp(label,rm->objects[i]->label[j]))
		{
		count--;
			if (!count)
			{
			o = rm->objects[i];
			return o;
			}
		}
	j++;
	}
j=0;
i++;
}
return NULL;
}

/* function for examining/inspecting an object */
void inspect_object(UR_OBJECT user)
{
int count;
O_OBJECT o;
   
if (word_count<2) {
  write_user(user,"Usage: inspect <object>\n");
  return;
  }
if (word_count<3) { count = 1; }
else { count=atoi(word[2]); }

o = find_object(user,user->room,word[1],count);
if (o==NULL) { write_user(user,"That object is not in this room or in your possession.\n"); return; }
sprintf(text,"%s~RS inspects %s~RS.\n",user->recap,o->name);
write_room_except(user->room,text,user);
sprintf(text,"You inspect %s~RS:\n",o->name);
write_user(user,text);
sprintf(text,"%s\n",o->desc);
write_user(user,text);
}

/* function for getting an object */
void get_object_from_room(UR_OBJECT user)
{
int count;
O_OBJECT o;
   
if (word_count<2) {
  write_user(user,"Usage: get <object>\n");
  return;
  }
if (word_count<3) { count = 1; }
else { count=atoi(word[2]); }

o = find_object_in_room(user->room,word[1],count);
if (o==NULL) { write_user(user,"That object is not in this room.\n"); return; }
if (o->fixed && user->level<GOD) { write_user(user,"You cannot pick up that object.\n"); return; }
        if (add_object_to_user(o,user,1))
        {
	remove_object_from_room(o,user->room,1);
	sprintf(text,"%s~RS picks up %s~RS.\n",user->recap,o->name);
	write_room_except(user->room,text,user);
	sprintf(text,"You pick up %s~RS.\n",o->name);
	write_user(user,text);
	}
        else
        {
        write_user(user,"Sorry, adding another object would exceed max limit.\n");
        }
}

/* function for dropping an object */
void drop_object_in_room(UR_OBJECT user)
{
int count;
O_OBJECT o;
   
if (word_count<2) {
  write_user(user,"Usage: get <object>\n");
  return;
  }
if (word_count<3) { count = 1; }
else { count=atoi(word[2]); }

o = find_object_on_user(user,word[1],count);
if (o==NULL) { write_user(user,"You are not carrying that object.\n"); return; }
if (o->fixed && user->level<GOD) { write_user(user,"You cannot drop that object.\n"); return; }
        if (add_object_to_room(o,user->room,1))
        {
	remove_object_from_user(o,user,1);
	sprintf(text,"%s~RS drops their %s~RS.\n",user->recap,o->name);
	write_room_except(user->room,text,user);
	sprintf(text,"You drop your %s~RS.\n",o->name);
	write_user(user,text);
	}
        else
        {
        write_user(user,"Sorry, adding another object would exceed max limit.\n");
        }
}

/* function for trashing an object */
void trash_object(UR_OBJECT user)
{
int count;
O_OBJECT o;
   
if (word_count<2) {
  write_user(user,"Usage: trash <object>\n");
  return;
  }
if (word_count<3) { count = 1; }
else { count=atoi(word[2]); }

o = find_object_on_user(user,word[1],count);
if (o==NULL) { write_user(user,"You are not carrying that object.\n"); return; }
if (o->fixed && user->level<GOD) { write_user(user,"You cannot trash that object.\n"); return; }
remove_object_from_user(o,user,1);
sprintf(text,"%s~RS trashes %s~RS.\n",user->recap,o->name);
write_room_except(user->room,text,user);
sprintf(text,"You trash %s~RS.\n",o->name);
write_user(user,text);
}

/* function for giving an object to another user */
void give_object(UR_OBJECT user)
{
int count;
O_OBJECT o;
UR_OBJECT u;
   
if (word_count<3) {
  write_user(user,"Usage: give <object> <recipient>\n");
  return;
  }
if (word_count<4) { count = 1; }
else { count=atoi(word[3]); }

if (!(u=get_user(word[2]))) {
  write_user(user,notloggedon);  return;
  }

if (user->room!=u->room) { sprintf(text,"You do not see %s~RS in this room.\n",u->recap); write_user(user,text); return; }
o = find_object_on_user(user,word[1],count);
if (o==NULL) { write_user(user,"You are not carrying that object.\n"); return; }
if (o->fixed && user->level<GOD) { write_user(user,"You cannot give away that object.\n"); return; }
        if (add_object_to_user(o,u,1))
        {
	remove_object_from_user(o,user,1);
	sprintf(text,"%s~RS gives %s~RS to %s~RS.\n",user->recap,o->name,u->recap);
	write_room_except(user->room,text,user);
	sprintf(text,"You give %s~RS to %s~RS.\n",o->name,u->recap);
	write_user(user,text);
	}
        else
        {
        write_user(user,"Sorry, adding another object would exceed max limit.\n");
        }
}

/* takes a 0 or 1 and returns a NO or YES string */
char *yesno(int input)
{
if (input==0)      return "~FRNO";
else if (input==1) return "~FGYES";
else               return "~FY???";
}

/* takes a 0 or 1 and returns a NO or YES string */
char *yn(int input)
{
if (input==0) return "N";
else return "Y";
}

/* takes a 0 or 1 and returns a NO or YES string */
char *petmf(int input)
{
if (input==0) return "N";
else if (input==1) return "M";
else if (input==2) return "F";
else return "I";
}

int user_has_object(UR_OBJECT user,int test)
{
int i=0;
while (user->objects[i])
{
	if (user->objects[i]->id==test) { return 1; }
i++;
}
return 0;
}

/* function for examining/inspecting an object */
void object_info(UR_OBJECT user)
{
int count;
O_OBJECT o;
int objid=0,l=0,isanumber();
   
if (word_count<2) {
  write_user(user,"Usage: objinfo <object id>\n");
  return;
  }
if (word_count<3) { count = 1; }
else { count=atoi(word[2]); }
o = find_object(user,user->room,word[1],count);
if (isanumber(word[1])) { objid=atoi(word[1]); }
if (o==NULL) {
	if (objid) { o=get_object(objid); }
	}
if (o==NULL) { write_user(user,"That object could not be found.\n"); return; }

/* Show The Object's Properties To The User */
write_user(user,"\n");
write_user(user,SEPERATOR1);
write_user(user,"\n");
sprintf(text,"~CTObject Name.........~CW:~RS %s",o->name);
box_write(user,text);
sprintf(text,"~CTObject Cost.........~CW:~RS $%d.00",o->cost);
box_write(user,text);
sprintf(text,"~CTIs Object Fixed.....~CW:~RS %s",yesno(o->fixed));
box_write(user,text);
sprintf(text,"~CTIs Object Worn......~CW:~RS %s",yesno(o->worn));
box_write(user,text);
sprintf(text,"~CTIs Object Drinkable.~CW:~RS %s",yesno(o->drinkable));
box_write(user,text);
sprintf(text,"~CTIs Object Edible....~CT:~RS %s",yesno(o->edible));
box_write(user,text);
sprintf(text,"~CTIs Object Rare......~CT:~RS %s",yesno(o->rare));
box_write(user,text);
sprintf(text,"~CTIs Object Disabled..~CT:~RS %s",yesno(o->disabled));
box_write(user,text);
sprintf(text,"~CTIs Object A Pet.....~CT:~RS %s",yesno(o->pet));
box_write(user,text);
sprintf(text,"~CTIs Object Locked....~CW:~RS %s",yesno(o->locked));
box_write(user,text);
sprintf(text,"~CTObject's Level Is...~CW:~RS %d [%s]",o->level,level_name[o->level]);
box_write(user,text);
sprintf(text,"~CTObject's Minimum Age~CW:~RS %d Years.",o->age);
box_write(user,text);
sprintf(text,"~CTObject's Keywords...~CW:~RS ");
for(l=0;l<MAX_LABELS-1;l++) {
	if(strcmp(o->label[l],"*") && l>=1) { strcat(text,", "); }
	if(!strcmp(o->label[l],"*")) { ;break; }
	strcat(text,o->label[l]);
	}
box_write(user,text);
write_user(user,SEPERATOR2);
write_user(user,"\n");
box_write(user,center("~CTObject's Description~CW:",74));
write_user(user,SEPERATOR2);
sprintf (text,"\n%s\n%s\n\n",o->desc,SEPERATOR3);
write_user(user,text);
}
