/*****************************************************************************
       Object system version 1.1.1 - by Kevin Wojtysiak (Magius)
                        Last update: 16th November, 2001

                                wojtyk@home.com

                        compatible with Amnuts 2.2.1 by
                                Andrew Collington
                 amnuts@talker.com  |  http://amnuts.talker.com/
*****************************************************************************/

void use(UR_OBJECT user)
{
O_OBJECT o;
        
if (word_count<2) {
	if (com_num==DRINK) { write_user(user,"Usage: drink <object>\n"); }
	else if (com_num==EAT) { write_user(user,"Usage: eat <object>\n"); }
	else if (com_num==WEAR) { write_user(user,"Usage: wear <object>\n"); }
	else { write_user(user,"Usage: use <object>\n"); }
        return;
        }
        
o = find_object_on_user(user,word[1],1);
if (o==NULL) { o = find_object_in_room(user->room,word[1],1); }
if (o==NULL) { write_user(user,"That object is not in this room or in your possession.\n"); return; }
if (o->level>user->level) {
	sprintf(text,"~CR<~CY!~CR>~CM-> ~CR%s ~CRgets shocked by %s~CR.\n",user->recap,o->name);
	write_room(NULL,text);
	return;
	}
if (com_num==EAT && o->pet) {
	write_user(user,"~CR<~CY!~CR> ~CM-> ~CTYou Sicko you can't eat pets!\n");
	sprintf(text,"~CR<~CY!~CR> ~CM-> ~CT%s is a sicko.  Trying to eat a pet...!\n",user->recap);
	write_room_except(user->room,text,user);
	return;
	}
if (com_num==EAT && !o->edible) {
	sprintf(text,"~CR<~CY!~CR> ~CM-> ~CT%s probably doesn't taste very good.\n",o->name);
	write_user(user,text);
	if (o->drinkable) { write_user(user,"~CR<~CY!~CR> ~CM-> ~CTTry drinking it instead.\n"); }
	else if (o->worn) { write_user(user,"~CR<~CY!~CR> ~CM-> ~CTTry wearing it instead.\n"); }
	else {  write_user(user,"~CR<~CY!~CR> ~CM-> ~CTTry using it instead.\n"); }
	return;
	}
if (com_num==EAT && o->drinkable) {
	write_user(user,"~CR<~CY!~CR> ~CM-> ~CTIt's not edible, but is drinkable.\n");
	return;
	}
if (com_num==EAT && o->worn) {
	write_user(user,"~CR<~CY!~CR> ~CM-> ~CTYou should try to wear this instead of eat it.\n");
	return;
	}
if (com_num==DRINK && !o->drinkable) {
	write_user(user,"~CR<~CY!~CR> ~CM-> ~CTIt's not drinkable.\n");
	return;
	}
if (com_num==WEAR && !o->worn) {
	write_user(user,"~CR<~CY!~CR> ~CM-> ~CTYou cannot wear it!  Altho it might look funny :)\n");
	return;
	}
switch(o->id) {
	case 1: // Coffee (Object #1)
	  if (com_num!=DRINK) {
		write_user(user,"~CR<~CY!~CR> ~CM-> Coffee should be drank.\n");
		return;
		}
	  if (remove_object_from_user(o,user,1)) {
		write_user(user,"~CM& ~CTYou drink your steaming cup of hot coffee, just what you need to pick you up!  Ahhh...\n");
		sprintf(text,"~CM& ~CT%s~CT drinks a steaming cup of hot coffee, just what %s needs to pick %s up.\n",user->recap,heshe[user->gender],himher[user->gender]);
		write_room_except(user->room,text,user);
		}
	  else { write_user(user,"Hmm, your coffee seems to be missing.\n"); }
	break;

	case 2: // Welcome Sign (object #2)
	  if (com_num==EAT) {
		write_user(user,"~CM& ~CTThe sign doesn't taste very good.\n");
		sprintf(text,"~CM& ~CT%s ~CTtries to eat the sign but finds it a little too signy tasting.\n",user->name);
		write_room_except(user->room,text,user);
		record(user->room,text);
		return;
		}
		/* What use does a sign really have except to read it so */
		/* lets shock them instead if they try and use it. ;)    */
		sprintf(text,"~CB<~CY!~CB> ~CR%s~CR gets socked by %s.\n",user->recap,o->name);
		write_room(user->room,text);
		break;
	case 3: // Object ID #3
		// insert use code here
	        break;

	default:  /* no use coded so lets use generic stuff. */
	  if (com_num==EAT && o->edible) {
	  	if (remove_object_from_user(o,user,1)) {
			sprintf(text,"~CM& ~CTYou eat %s~CT.\n",o->name);
			write_user(user,text);
			sprintf(text,"~CM& %s~CT eats %s~CT.\n",user->recap,o->name);
			write_room_except(user->room,text,user);
			return;
			}
			else { write_user(user,"Unable to remove object\n"); }
	  	}
	  else if (com_num==DRINK && o->drinkable) {
		  if (remove_object_from_user(o,user,1)) {
			sprintf(text,"~CM& ~CTYou drink %s~CT.\n",o->name);
			write_user(user,text);
			sprintf(text,"~CM& ~CT%s~CT drinks %s~CT.\n",user->recap,o->name);
			write_room_except(user->room,text,user);
			return;
			}
		  else { write_user(user,"Could not remove object\n"); } 
        	  }
	  else if (com_num==USE && o->pet) { /* Pet a pet that has no action */
		sprintf(text,"~CM& ~CTYou pet %s~CT loveingly.\n",o->name);
		write_user(user,text);
		sprintf(text,"~CM& ~CT%s~CT pets %s lovingly.\n",user->recap,o->name);
		write_room_except(user->room,text,user);
		return;
		}
	  else { write_user(user,"~CR<~CY!~CR> ~CM-> ~CTThat object does not have a use.\n"); }
	  break;
  }
}

/* Cause a user to puke if their drunk enough, but sober them up afterwords */
void puke(UR_OBJECT user)
{
if (user->drunk>5) {
	sprintf(text,"~CM& ~CTYou have a little too much to drink and ~CGp~FGu~CYk~FYe~CGs~CR all over the floor.\n");
	write_user(user,text);
	sprintf(text,"~CM& ~CT%s ~CRhas a little too much to drink and ~CGp~FGu~CYk~FYe~CGs~CR all over the floor.\n",user->recap);
	write_room_except(user->room,text,user);
	user->drunk=0;
	}
}

