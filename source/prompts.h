/***************************************************************************

             Moenuts v1.73 Prompts Configuration Include File
             (C)1998-2004 Michael Irving, All Rights Reserved
                  (C)1998-2004 Moesoft Developments Canada
                           http://www.moenuts.com

 ***************************************************************************

   Please be CAREFUL when editing some of the prompts, especially the Style
   prompts because they MUST be in a specific order otherwise they will not
   work properly.  Especially mind the %s substitution variables as their
   order IS important!   And DO NOT change the prompt name!

 ***************************************************************************/

#define PROMPT_VERSION "Moenuts v1.73 Prompts"

/*   Prompt Name:           Value:                                       */

char *say_style            ="%s %ss~CW:~RS %s%s\n";
char *welcome_style        ="~CB(._.-(~CT %s ~RS~CB)-._.)\n";
char *show_style           ="~CGType~CY: ~CB\" ~RS%s ~CB\"\n";
char *examine_style        ="\n~CM=~CB-~CM=> ~RS%s ~RS%s ~RS\n";
char *help_levelname_style ="~CB#~CB#~CT# ~CY%s ~CT#~CB#~CB#";
char *all_clone_style      =" ~CM%-20.20s ~CW: ~CR%s \n";
char *site_style_dns_ip    ="~CT%s~CB's site is currently ~CM%s~CB, Their IP is ~CM%s\n";
char *site_style_dns       ="~CT%s~CB's site is currently ~CM%s~CB.\n";
char *site_style_offline   ="~CT%s~CB's last site was ~CM%s~CB.\n";
char *wizshout_style       ="~CG[~CGW~CYi~CGZ~RS~CG]~CB: ~CT[ ~RS%s ~RS~CT]~RS %s\n";
char *wizemote_style       ="~CG[~CGW~CYi~CGZ~RS~CG]~CB: %s %s\n";
char *greet_style          ="~CT%s ~RS~CMannounces~CT:\n\n";
char *autopromote_style    ="~CTEveryone, please welcome ~CG%s~CT to %s! :-)";

/* Miscelaneous Prompt Configuration */

char *syserror               ="~CMWARNING~CW:~CR ** A system error has occured **\n";
char *nosuchroom             ="~CTUm try that room again? That one doesn't exactly exist.\n";
char *nosuchuser             ="~CRWhat, where, WHO?..You must be confused.\n";
char *notloggedon            ="~CRThe person you are looking for isn't here now.\n";
char *invisenter             ="~CR< ~CRYou feel a sudden coldness around you.~CR>\n";
char *invisleave             ="~CR< ~CRYou feel the room get warmer.~CR>\n";
char *invisname              ="~CBA stalker~RS";
char *invisemote             ="~CBA stalker~RS";
char *hiddenname             ="~CBA stalker~RS";
char *noswearing             ="\n~CY~BR *** We don't like language like that here *** ~RS\n";
char *user_room_prompt       ="~CB-~CB=~CT+ ~RS~CGYou have been given a room called ~CB\"~CM%s~RS~CB\"~CG, use ~CR.home ~CG to enter it.\n";
char *enterprompt            ="~CG-=+ ~CTPress ~CB[~CYENTER~CB] ~CTTo Login ~CG+=-";
char *session_swap           ="~CB[ ~CTReCoNnEcT ~CB]~CG:";
char *announce_prompt        ="\n~CW~BM[~CG~BMEntering %s~CW~BM]~RS~CT:";
char *more_prompt            ="\n~CB[ ~CT.oO( ~CB[~CWENTER~CB] ~CW= Continue, ~CB[~CWE~CB] ~CW+ ~CB[~CWENTER~CB]~CW = Exit ~CT)Oo. ~CB]~RS \n";
char *unknown_command        ="~CRHuh?  I don't understand what it is you're trying to do!\n";
char *bad_command            ="~CB% ~RS%s?  What's that supposed to do?\n";
char *denied_command         ="~CB% ~RSYou want to use %s?  You can't do that! Stop trying to use things you don't have!\n";
char *no_message_prompt      ="~CMThe board is empty.\n";
char *single_message_prompt  ="~CMThere is only one message on the board.\n";
char *message_prompt         ="~CMThere is ~CT%d~CM message on the board.\n";
char *topic_prompt           ="~CTThe Current Topic Is~CW:~RS %s \n";
char *no_exits               ="~CRQuick get a shovel and dig yourself an exit.";
char *people_here_prompt     ="\n~CTYou are all alone here~CW:\n";
char *no_people_here_prompt  ="\n~CRYou are all alone here.\n";
char *room_not_found         ="~CB% ~CY\"~CG%s~CY\" ~CMdoes not match a room or matches more than one.\n";
char *already_in_room        ="~CGYou are already in the ~RS~CW%s!\n";
char *user_ustat_header      ="~FM-~CM= ~CTYour stats on %s ~CM=~FM-";
char *ustat_header           ="~FM-~CM= ~CT%s's Stats on %s ~CM=~FM-";
char *ustat_jail_levelname   ="~CRHells Guest";
char *help_header            ="~CGCommands available for level ~CT%s";
char *help_footer1           =" ~CTThere are ~CG%d ~CTcommands total.  ~CTYou have ~CG%d~CT commands available to you.\n";
char *help_footer2           =" ~CGFor help on a command, type:   ~CG.help <command>~CG.\n";
char *mash_prompt            ="~CW>>>>~RS %s ~RS~CRis being punished for their actions...\n";
char *mash_forgive           ="~CW>>>>~RS %s ~RS~CRhas been re-habilitated and forgiven!\n";
char *muzzle_victim_prompt   ="~CMYikes!  You've been Muzzled!\n";
char *muzzle_user_prompt     ="~CMYou throw a muzzle on %s.\n";
char *unmuzzle_victim_prompt ="~CGWooHoo!  Your Muzzle Has Been Removed!\n";
char *unmuzzle_user_prompt   ="~CGYou take off %s~CG's muzzle.\n";
char *arrest_user_prompt     ="~CRYou send %s ~RS~CRto jail.\n";
char *arrest_room_prompt     ="~CR%s~CR yells loudly and ~RS%s~CR is moved magically and\n~CRends up in jail!\n";
char *arrest_smail_message   ="~CMYou hear shouting and you suddenly appear in jail!!!\n";
char *unarrest_user_prompt   ="~CGYou have released %s~CG from jail.\n";
char *unarrest_victim_prompt ="~CGYou wake up just as you get tossed out of jail!\n";
char *suicide_prompt         ="\07~CB-~CB=~CT+ ~RS%s~CR is threw with this world and is moving on to another life... ~CT+~CB=~CB-\n";

/* Idle Prompts */

char *idle_user_timeout ="\n\n~CR!!! ~CTYou blew it!! ~CR!!!\n\n";
char *idle_kill_prompt1 ="~CYYou see a stray bullet hit %s. Wonder if that was aimed...~RS~CY...\n";
char *idle_kill_prompt2 ="~CYAn evil spell is heard.\n~CRYou shouldn't be idling so long! ;-)\n";
char *idle_user_drift   ="~CYYou havent been talking quite enough? oh well...\n~CTYou have been forced AFK by the system!\n";
char *idle_room_drift   ="%s~CY suddenly falls off and is pronouced dead...Quick call an ambulance.\n";
char *idle_user_warning ="\n~CM!!! Quick! You must Input something within two minutes or be disconnected !!!\n\n";

/* Wake Prompts */

char *wake_victim_prompt ="\n~BP~CM>> ~RS%s~CT nudges you... Pssst.... Wake Up!!! ~CM<<\n\n";
char *wake_user_prompt   ="~CMYou nudge ~RS%s~CT...  Pssst...Wake Up!!!\n";

/* Promotion Prompts */

char *promote_user_prompt ="~CYYou find %s worthy of being a ~CG%s!  Congratulations!\n";
char *promote_prompt      ="~CY%s finds %s worthy of being a ~CG%s!  Congratulations!\n";
char *demote_user_prompt  ="~CMYou demote %s~RS~CM to a %s.\n";
char *demote_prompt       ="~CM%s demotes %s~RS~CM to a ~CM%s.\n";

/* Visibility Prompts */

char *appear_prompt        ="~CT%s suddenly appears out of no where...\n";
char *disapear_prompt      ="~CB%s fades into the background...\n";
char *disapear_user_prompt ="~CTYou chant a mystical spell and fade into the background...\n";

/* Kill Prompts */

char *kill_user_chant  ="~CMYou get a glint in your eye.\n";
char *kill_room_chant  ="~CM%s gets an evil glint in their eye..\n";
char *kill_victim_text ="~CMSuddenly a bomb is dropped on your head. Kaboom!!!\n";
char *kill_room_text   ="~CMSuddenly a bomb is dropped from the sky and lands on %s. Kaboom!!!\n";
char *kill_all_text    ="~CR**Kaboom!**\nA cleanup crew appears and cleans up the mess!~CB\n";

/* Clone Prompts */

char *clone_here_prompt   ="\n~CMYou drink something from a green test tube and a clone of yourself appears.\n";
char *clone_prompt        ="\n~CMYou drink something from a green test tube and a clone of yourself appears in %s.\n";
char *clone_user_destroy  ="~CMYou convulse slightly as your clone disappears out of sight..\n";
char *clone_room_destroy  ="~CT%s suddenly becomes more life-like...\n";
char *clone_switch_prompt ="~CBYou experience a strange sensation and switch places with your clone...\n";

/* General Mail Prompts */

char *newmail_prompt ="~CM~BM!! ~CYThere is New Mail In Your Mailbox! !!~RS\07\n";
char *nomail_prompt  ="~CBSorry, but you don't have any mail.\n";
char *mailbox_total  ="~CGThere was a total of %d messages found in your mailbox.\n";

/* Smail Prompts */

char *sendmail_prompt   ="~CTYou hold your letter to %s out.  Just then...\n~CBSWOOOOOOOP...The pony express appears And takes your letter for delivery...\n";
char *smail_edit_header ="\n~FM-~CM=~CR[ ~CTWriting mail message to ~CG%s ~CR]~CM=~FM-\n\n";

/* Dmail Prompts */

char *dmail_nomail        ="~CRYou have no mail to delete.\n";
char *dmail_all           ="~CRAll your messages have been deleted.\n";
char *dmail_too_many      ="~CRThere were only ~CM%d ~CRmessages in your mailbox, all now deleted.\n";
char *dmail_some          ="~CT%d ~CBmessages deleted from the top of your mailbox.\n";
char *dmail_bottom        ="~CR%d messages deleted from the bottom of your mailbox.\n";
char *dmail_no_number     ="~CRThere were only %d messages in your mailbox. There is not a #%d message to delete.\n";
char *dmail_one_message   ="~CROne message deleted from your mailbox.\n";
char *dmail_some_messages ="~CR%d message deleted from your mailbox.\n";

/* Message Board Read Prompts */

char *message_board_header   ="~FM-~CM=~CR[ ~CTThe ~CG%s~CT message board ~CR]~CM=~FM-";
char *read_no_messages       ="~CGYou look up, but to your amazement, the %s~CG board is empty!";
char *user_read_board_prompt ="~CM%s~RS~CT looks up and decides to read the message board.\n";

/* Message Board Write Prompts */

char *write_edit_header ="\n~FM-~CM=~CR[ ~CGWriting A Messay On The Board! ~CR]~CM=~FM-\n\n";
char *user_write_end    ="~CGYou scribble something on the board...\n";
char *room_write_end    ="%s ~RS~CGwrites some scribble on the board.\n";

/* Message Wipe Prompts */

char *wipe_empty_board       ="~CRThe message board is empty.\n";
char *wipe_user_all_deleted  ="~CRAll messages on the message board have been deleted.\n";
char *wipe_room_all_deleted  ="~CM%s ~RS~CMwipes the message board clean!\n";
char *wipe_too_many          ="~CRThere were only ~CW%d ~CRmessages on the board, all messages deleted!\n";
char *wipe_deleted_top       ="~CW%d ~CMmessages deleted from the top.\n";
char *wipe_deleted_bottom    ="~CW%d ~CMmessages deleted from the bottom.\n";
char *wipe_deleted_some      ="~CW%s ~CMwipes %d messages from the wall.\n";
char *wipe_missing_number    ="~CRThere were only ~CW%d~CR messages on the board. There is not a #%d message to delete.\n";
char *wipe_user_one_message  ="~CMOne message deleted from board.\n";
char *wipe_room_one_message  ="~CW%s ~CMwiped one of the messages from the wall.\n";
char *wipe_user_delete_range ="~CW%d ~CMmessages deleted from board.\n";

/* Profile Prompts */

char *profile_start       ="~CT%s decides to write a profile...\n";
char *profile_end         ="~CT%s stands back and admires their profile...\n";
char *profile_edit_header ="~CS\n~CG-~CG=~CM]~CT You are writing a profile of yourself ~CM[~CG=~CG-";
char *no_profile_prompt   ="~CMI is too lazy to write a profile right now!\n~CGEncourage this user to use .entpro\n";

/* Room Decsription Editor Prompts */

char *entroom_start       ="%s~CT gets out a pencil and decides to draw on this room  a little...\n";
char *entroom_edit_header ="~CG-~CG=~CM] ~CWEntering A Room Description ~CM[~CG=~CG-";
char *entroom_end         ="%s ~CThas finished drawing on this room...\n";

/* Room Entry Request Prompts */

char *user_knock_prompt      ="~CYYou yell loudly at the door of the %s.\n";
char *room_knock_prompt      ="%s~CY yells loudy at the door of this room.\n";
char *user_room_knock_prompt ="%s~CY yells loudly at the door of the %s.\n";
char *user_join_request      ="~CYYou yells loudy at the door.\n";
char *join_request           ="%s~CY wishes to join you!\n";

/* Various Review Buffer Prompts */

char *cbuff_prompt           ="~CM%s licked the review buffer clean with their tongue...\n~CREeewwww!  That's disgusting...\n";
char *out_of_room_cbuff      ="~CGReview buffer cleared in the %s room.\n";
char *wiz_cbuff_prompt       ="~CGYou lick the wiz review buffer clean.\n";
char *shout_cbuff_prompt     ="~CGYou lick the shout review buffer clean.\n";
char *no_review_prompt       ="~CMI see nothing here worth showing you...\n";
char *private_review_prompt  ="~CRThat room is currently private, you cannot review the conversation.\n";
char *personal_review_prompt ="~CRThat room is set to ~CTpersonal~CR you cannot review the conversation.\n";
char *review_header          ="~FM-~CM=~CR[ ~CTReview buffer for: ~CG%s ~CR]~CM=~FM-\n";

/* Tell Review */

char *tell_review_header    ="\n~FM-~CM=~CR[ ~CTReview Of Tells Sent To You ~CR]~CM=~FM-\n";
char *no_tell_review_prompt ="~CMYou Have No Tells To Review!\n";

/* Wiz Review */

char *wiz_review_header    ="~FM-~CM=~CR[ ~CTReview Of Tells Sent To The Staff ~CR]~CM=~FM-";
char *no_wiz_review_prompt ="~CMThe staff have been quiet lately...\n";

/* Shout Review */

char *shout_review_header    ="~FM-~CM=~CR[ ~CTReview Of Shouts and Shout Emotes ~CR]~CM=~FM-";
char *no_shout_review_prompt ="~CMThere ain't no shouts here to show ya!\n";

/* Move Prompts */

char *move_prompt_user      ="~CBYou get sucked into a black hole...\n";
char *move_old_room_prompt  ="~CM%s disappears into a black hole...\n";
char *move_new_room_prompt  ="~CM%s comes crashing through the door...\n";
char *illegal_move_prompt   ="~CT%s chants an evil spell...\n";
char *user_room_move_prompt ="~CM%s chants an ancient spell...\n";

/*  Default Prompt Config */

char *default_inphr       ="comes running into the room!";
char *default_outphr      ="leaves the room";
char *default_level_alias ="New";

/* Login / Connection Prompts */

char *login_timeout      ="\n\n~CRYou've had long enough to login!  ~CY* Thanks For Coming! *\n\n";
char *login_prompt       ="\n~CMLogin: ";
char *login_quit         ="\n~CT~CMYou turn around and leave...\n";
char *login_shortname    ="\n~CRName Too Short, Minimum of 3 Characters!\n\n";
char *login_longname     ="\n~CRThat name's too long!  How about a shorter one?\n\n";
char *login_lettersonly  ="\n~CRYou can only use letters in the name!\n\n";
char *login_welcome      ="\n~CYWelcome to ~CM%s~CT!\n";
char *login_attempts     ="\n~CRSorry, you took too many attempts to login!\n";
char *login_arrest1      ="~CR%s~CR is sent straight to jail...\n";
char *login_arrest2      ="~CRYou are sent straigh to jail...\n";
char *fairwell_prompt    ="~CGThank you for coming to ~CT%s~CG, ~RS%s~CG!\n~CYPlease come back soon!\n";
char *exit_prompt        ="~CW~BR[~CY~BRExiting %s~CW~BR]~RS~CW:";
char *user_banned_prompt ="\n\n~CRYou are not welcome here at %s~CR!\n\n";

/* Password Prompts */

char *password_prompt  ="~CBPassword~CT: ";
char *password_again   ="~CMPlease re-enter your ~CBpassword~CT: ";
char *password_wrong   ="~CRWrong Password!\n";
char *password_nomatch ="~CRThe Two Passwords Do Not Match!\n";
char *password_short   ="~CRSorry, your password must be atleast 3 characters long!\n";
char *password_long    ="~CRSorry, Your Passowrd Is Too Long!  Please choose a shorter one!\n";
