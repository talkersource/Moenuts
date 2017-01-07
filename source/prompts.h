/***************************************************************************

            Moenuts v1.58b1 Prompts Configuration Include File
               (C)1998 Michael Irving, All Rights Reserved.
                   (C)1998 Moesoft Developments Canada.
                       http://moesoft.talkernet.net

 ***************************************************************************

   Please be CAREFUL when editing some of the prompts, especially the Style
   prompts because they MUST be in a specific order otherwise they will not
   work properly.  Especially mind the %s substitution variables as their
   order IS important!   And DO NOT change the prompt name!

 ***************************************************************************/

#define PROMPT_VERSION "The Forgotten Island Prompts v1.61c"

/*   Prompt Name:           Value:                                       */

/* Style Prompts ** BE CAREFUL WITH THESE ** They can be touchy! */

char *say_style            ="%s %ss~RS~FY:~RS %s\n";
char *welcome_style        ="~OL~FB(._.-(~OL~FT %s ~RS~OL~FB)-._.)\n";
char *show_style           ="~FGType~FY: ~FB\" ~RS%s ~FB\"\n";
char *examine_style        ="\n~FM=~FB-~FM=> ~RS%s ~RS%s ~RS\n";
char *help_levelname_style ="~FB#~OL#~FT# ~FY%s ~FT#~FB#~RS~FB#";
char *all_clone_style      =" ~OL~FM%-20.20s ~FW: ~FR%s \n";
char *site_style_dns_ip    ="~OL~FT%s~FB's site is currently ~FM%s~FB, Their IP is ~FM%s\n";
char *site_style_dns       ="~OL~FT%s~FB's site is currently ~FM%s~FB.\n";
char *site_style_offline   ="~OL~FT%s~FB's last site was ~FM%s~FB.\n";
char *wizshout_style       ="~FG[~OL~FGW~FYi~FGZ~RS~FG]~FB: ~FT[ ~RS%s ~RS~FT]~RS %s\n";
char *wizemote_style       ="~FG[~OL~FGW~FYi~FGZ~RS~FG]~FB: %s %s\n";
char *greet_style          ="~OL~FT%s ~RS~OL~FMannounces~FT:\n\n";
char *autopromote_style    ="~OL~FTEveryone, please welcome ~FG%s~FT to %s! :-)";

/* Miscelaneous Prompt Configuration */

char *syserror               ="~OLWARNING:~FR ** A system error has occured **\n";
char *nosuchroom             ="~FTThat room wasn't thought of at this point in time!\n";
char *nosuchuser             ="~FRNobody like that has ever set foot on the island before...\n";
char *notloggedon            ="~FRThat person isn't on the island right now...\n";
char *invisenter             ="~FR< ~OLYou hear strange foot steps getting closer... ~RS~FR>\n";
char *invisleave             ="";
char *invisname              ="~OL~FBSomeone~RS";
char *invisemote             ="~OL~FBSomeone~RS";
char *hiddenname             ="~OL~FBSomeone~RS";
char *noswearing             ="\n~OL~FY~BR *** ~LIWe don't like language like that here~RS~OL~FY~BR *** ~RS\n";
char *user_room_prompt       ="~OL~FB-~OL=~FT+ ~RS~FGYou have been given a room called ~FB\"~OL~FM%s~RS~FB\"~FG, use ~FR.home ~FG to enter it.\n";
char *enterprompt            ="~OL~FG-=+ ~FTPress ~FB[~FYENTER~FB] ~FTTo Login ~FG+=-";
char *session_swap           ="~OL~FB[ ~FTReCoNnEcT ~FB]~FG:";
char *announce_prompt        ="\n~OL~FW~BM[~FGEntering %s~FW]~RS~OL~FT:";
char *more_prompt            ="\n~FB[ ~OL~FT.oO( [~FWENTER~FT] = Continue, [~FWE~FT] + [~FWENTER~FT] = Exit )Oo. ~RS~FB]~RS\n";
char *unknown_command        ="~OL~FRHuh?  I don't understand what it is you're trying to do!\n";
char *bad_command            ="~OL~FB% ~RS%s?  What's that suposed to do?\n";
char *denied_command         ="~OL~FB% ~RSYou want to use %s?  Your not permitted to use such a command.\n";
char *no_message_prompt      ="~OL~FMThere are no writings on the wall.\n";
char *single_message_prompt  ="~OL~FMThere is only one writing on the wall.\n";
char *message_prompt         ="~OL~FMThere is ~FT%d~FM writings on the wall.\n";
char *topic_prompt           ="~OL~FTThe Buzz Of The Island Is~FW:~RS %s \n";
char *no_exits               ="~OL~FRIt seems you are trapped in here!  There is no way out.";
char *people_here_prompt     ="\n~OL~FTOthers people here with you~FW:\n";
char *no_people_here_prompt  ="\n~FRThere are no other people here with you.\n";
char *room_not_found         ="~OL~FB% ~FY\"~FG%s~FY\" ~FMdoes not match a room or matches more than one.\n";
char *already_in_room        ="~OL~FGYou are already in the ~RS~OL%s!\n";
char *user_ustat_header      ="~FM-~OL= ~FTYour stats on %s ~FM=~RS~FM-";
char *ustat_header           ="~FM-~OL= ~FT%s's Stats on %s ~FM=~RS~FM-";
char *ustat_jail_levelname   ="~FRHells Guest";
char *help_header            ="~FGCommands available for level ~OL~FT%s";
char *help_footer1           =" ~OL~FTThere are ~FG%d ~FTcommands total.  ~FTYou have ~FG%d~FT commands available to you.\n";
char *help_footer2           =" ~FGFor help on a command, type:   ~OL.help <command>~RS~FG.\n";
char *mash_prompt            ="~OL~FW>>>>~RS %s ~RS~OL~FRis being punnished for their actions...\n";
char *mash_forgive           ="~OL~FW>>>>~RS %s ~RS~OL~FRhas been re-habilitated and forgiven!\n";
char *muzzle_victim_prompt   ="~OL~FMYikes!  You've been Muzzled!\n";
char *muzzle_user_prompt     ="~OL~FMYou throw a muzzle on ~RS~OL%s.\n";
char *unmuzzle_victim_prompt ="~OL~FGWooHoo!  Your Muzzle Has Been Removed!\n";
char *unmuzzle_user_prompt   ="~OL~FGYou take off %s's muzzle.\n";
char *arrest_user_prompt     ="~OL~FRYou damn %s ~RS~OL~FRto hell.\n";
char *arrest_room_prompt     ="~OL~FR%s~RS~OL~FR makes a phone call and %s~RS~OL~FR is picked up by the mafia and\n~OL~FRstuffed into the trunk of a car!\n";
char *arrest_smail_message   ="~OL~FMAn angry mob grab you and carry you off!!!\n";
char *unarrest_user_prompt   ="~OL~FGYou have released %s ~RS~OL~FGfrom jail.\n";
char *unarrest_victim_prompt ="~OL~FGYou wake up just as someone opens the trunk of the car!\n";
char *suicide_prompt         ="\07~FB-~OL=~FT+ ~FR%s ~RS~OL~FRis caught in a hail of bullets from a drive by shooting and is killed... ~FT+~FB=~RS~FB-\n";

/* Idle Prompts */

char *idle_user_timeout ="\n\n\07~FR!!! ~FTYou had your chance, See Ya! ~FR!!!\n\n";
char *idle_kill_prompt1 ="~OL~FYA bolt of lightning strikes %s~RS~OL~FY...\n";
char *idle_kill_prompt2 ="~OL~FYA swift breeze blows ashes through the halls...\n~OL~FRThat'll teach'em to idle too long! ;-)";
char *idle_user_drift   ="~OL~FYYou have been idling too long...\n~OL~FTYou have been forced AFK by the system!\n";
char *idle_room_drift   ="~OL~FY%s~RS~OL~FY falls asleep and drifts off into another reality...\n";
char *idle_user_warning ="\n\07~FM!!! You must Input something within two minutes or be disconnected !!!\n\n";

/* Wake Prompts */

char *wake_victim_prompt ="\07\n~FM>> ~OL~FM%s~RS~OL~FM nudges you... Pssst.... Wake Up!!! <<\n\n";
char *wake_user_prompt   ="~OL~FMYou nudge %s...  ~RS~OL~FMPssst...Wake Up!!!\n";

/* Promotion Prompts */

char *promote_user_prompt ="~OL~FYYou find %s worthy of being a ~FG%s!  Congratulations!\n";
char *promote_prompt      ="~OL~FY%s finds %s worthy of being a ~FG%s!  Congratulations!\n";
char *demote_user_prompt  ="~OL~FMYou demote %s~RS~OL~FM to a %s.\n";
char *demote_prompt       ="~OL~FM%s demotes %s~RS~OL~FM to a ~FM%s.\n";

/* Visibility Prompts */

char *appear_prompt        ="~OL~FT%s materializes right before your eyes...\n";
char *disapear_prompt      ="~OL~FB%s fades into the background...\n";
char *disapear_user_prompt ="~OL~FTYou chant a mystical spell and fade into the background...\n";

/* Kill Prompts */

char *kill_user_chant  ="~OL~FMYou whistle a strange little tune...\n";
char *kill_room_chant  ="~OL~FM%s whistles a strange little tune...\n";
char *kill_victim_text ="~OL~FMFrom out of nowhere, a huge dinosaur comes running upto you and devours you!!!\n";
char *kill_room_text   ="~OL~FMFrom out of nowhere, a huge dinosaur comes running and devours %s!!!\n";
char *kill_all_text    ="~OL~FR**BUUUURRRP!**\n~OL~FBThe Dinosaur blushes, excuses himself, then wanders off...\n";

/* Clone Prompts */

char *clone_here_prompt   ="\n~FM~OLYou whisper a haunting spell and create a clone of yourself here.\n";
char *clone_prompt        ="\n~FM~OLYou whisper a haunting spell create a clone of yourself in the %s.\n";
char *clone_user_destroy  ="~FM~OLYou whisper a sharp spell and the clone is destroyed.\n";
char *clone_room_destroy  ="~OL~FT%s gets sucked into a black hole...\n";
char *clone_switch_prompt ="~FB~OLYou experience a strange sensation and switch places with your clone...\n";

/* General Mail Prompts */

char *newmail_prompt ="~OL~FM~BM!! ~FYThere is New Mail In Your Mailbox! !!~RS\07\n";
char *nomail_prompt  ="~OL~FBSorry, but you don't have any mail.\n";
char *mailbox_total  ="~OL~FGThere was a total of %d messages found in your mailbox.\n";

/* Smail Prompts */

char *sendmail_prompt   ="~FT~OLYou put your letter to %s in the mailbox.  Just then...\n~OL~FBZooooooooooom...Pony Express Zooms By And Takes Your Letter For Delivery...\n";
char *smail_edit_header ="\n~FM-~OL=~FR[ ~FTWriting mail message to ~FG%s ~FR]~FM=~RS~FM-\n\n";
/* Dmail Prompts */

char *dmail_nomail        ="~FRYou have no mail to delete.\n";
char *dmail_all           ="~OL~FRAll your messages have been deleted.\n";
char *dmail_too_many      ="~OL~FRThere were only ~RS~OL%d ~FRmessages in your mailbox, all now deleted.\n";
char *dmail_some          ="~OL~FT%d ~FBmessages deleted from the top of your mailbox.\n";
char *dmail_bottom        ="~OL%d ~FMmessages deleted from the bottom of your mailbox.\n";
char *dmail_no_number     ="~OL~FRThere were only %d messages in your mailbox. There is not a #%d message to delete.\n";
char *dmail_one_message   ="~OL~FROne message deleted from your mailbox.\n";
char *dmail_some_messages ="~OL~FR%d message deleted from your mailbox.\n";

/* Message Board Read Prompts */

char *message_board_header   ="~FM-~OL=~FR[ ~OL~FTThe ~FG%s~FT message board ~FR]~FM=~RS~FM-";
char *read_no_messages       ="~OL~FGYou look up, but to your amazement, the %s board is empty!";
char *user_read_board_prompt ="~FM~OL%s~RS~FT looks up and decides to read the message board.\n";

/* Message Board Write Prompts */

char *write_edit_header ="\n~FM-~OL=~FR[ ~FGWriting Babble On The Board! ~FR]~FM=~RS~FM-\n\n";
char *user_write_end    ="~OL~FGYou scribble something on the board...\n";
char *room_write_end    ="~OL~FM%s ~RS~FGwrites some scribble on the board.\n";

/* Message Wipe Prompts */

char *wipe_empty_board       ="~OL~FRThe message board is empty.\n";
char *wipe_user_all_deleted  ="~OL~FRAll messages on the message board have been deleted.\n";
char *wipe_room_all_deleted  ="~OL~FM%s ~RS~FMwipes the message board clean!\n";
char *wipe_too_many          ="~OL~FRThere were only ~RS~OL%d ~FRmessages on the board, all messages deleted!\n";
char *wipe_deleted_top       ="~OL%d ~FMmessages deleted from the top.\n";
char *wipe_deleted_bottom    ="~OL%d ~FMmessages deleted from the bottom.\n";
char *wipe_deleted_some      ="~OL%s ~FMwipes %d messages from the wall.\n";
char *wipe_missing_number    ="~OL~FRThere were only %d messages on the board. There is not a #%d message to delete.\n";
char *wipe_user_one_message  ="~OL~FMOne message deleted from board.\n";
char *wipe_room_one_message  ="~OL%s ~FMwiped one of the messages from the wall.\n";
char *wipe_user_delete_range ="~OL%d ~FMmessages deleted from board.\n";

/* Profile Prompts */

char *profile_start       ="~OL~FT%s decides to write a little somthing about themselves...\n";
char *profile_end         ="~OL~FT%s stands back and admires their crayon scribbles...\n";
char *profile_edit_header ="~CS\n~FG-~OL~FG=~FM]~FT You are writing a profile of yourself ~FM[~FG=~RS~FG-";
char *no_profile_prompt   ="~OL~FMI is too lazy to write a profile right now!\n~OL~FGEncourage this user to use .entpro\n";

/* Room Decsription Editor Prompts */

char *entroom_start       ="~OL~FT%s gets out some paint and decides to decorate this room  a little...\n";
char *entroom_edit_header ="~FG-~OL=~FM] ~FWEntering A Room Description ~FM[~FG=~RS~FG-";
char *entroom_end         ="~OL~FT%s has finished decorating this room...\n";

/* Room Entry Request Prompts */

char *user_knock_prompt      ="~OL~FBYou grab a big stick and begin to tap on the stone door of the %s.\n";
char *room_knock_prompt      ="~OL~FBY%s ~RS~OL~FBgrabs a big stick and begins to tap on the stone door of this room.\n";
char *user_room_knock_prompt ="~OL~FBY%s ~RS~OL~FBgrabs a big stick and begins to tap on the stone door of the %s.\n";
char *user_join_request      ="~OL~FBYou pick up a big stick and bang on the door.\n";
char *join_request           ="~OL~FM%s ~OL~FRwishes to join you!\n";

/* Various Review Buffer Prompts */

char *cbuff_prompt           ="~OL~FM%s licked the review buffer clean with their tonge...\n~OL~FREeewwww!  That's discusting...\n";
char *out_of_room_cbuff      ="~OL~FGReview buffer cleared in the %s room.\n";
char *wiz_cbuff_prompt       ="~OL~FGYou lick the wiz review buffer clean.\n";
char *shout_cbuff_prompt     ="~OL~FGYou lick the shout review buffer clean.\n";
char *no_review_prompt       ="~OL~FMI see nothing here worth showing you...\n";
char *private_review_prompt  ="~OL~FRThat room is currently private, you cannot review the conversation.\n";
char *personal_review_prompt ="~OL~FRThat room is set to ~FTpersonal~FR you cannot review the conversation.\n";
char *review_header          ="~FM-~OL=~FR[ ~FTReview buffer for: ~FG%s ~FR]~FM=~RS~FM-\n";

/* Tell Review */

char *tell_review_header    ="\n~FM-~OL=~FR[ ~FTReview Of Tells Sent To You ~FR]~FM=~RS~FM-\n";
char *no_tell_review_prompt ="~OL~FMYou Have No Tells To Review!\n";

/* Wiz Review */

char *wiz_review_header    ="~FM-~OL=~FR[ ~FTReview Of Tells Sent To The Staff ~FR]~FM=~RS~FM-";
char *no_wiz_review_prompt ="~OL~FMThe Wizards have been quiet lately...\n";

/* Shout Review */

char *shout_review_header    ="~FM-~OL=~FR[ ~FTReview Of Shouts and Shout Emotes ~FR]~FM=~RS~FM-";
char *no_shout_review_prompt ="~OL~FMThere ain't no shouts here to show ya!\n";

/* Move Prompts */

char *move_prompt_user      ="~OL~FBYou get sucked into a black hole...\n";
char *move_old_room_prompt  ="~OL~FM%s disappears into a black hole...\n";
char *move_new_room_prompt  ="~OL~FM%s comes crashing through the door...\n";
char *illegal_move_prompt   ="~OL~FT%s chants an evil spell...\n";
char *user_room_move_prompt ="~OL~FM%s chants an anchient spell...\n";

/*  Default Prompt Config */

char *default_inphr       ="walks comes running into the room!";
char *default_outphr      ="leaves the room";
char *default_level_alias ="Visitor";

/* Login / Connection Prompts */

char *login_timeout      ="\n\n~OL~FRYou've had long enough to login!  ~FY* Thanks For Coming! *\n\n";
char *login_prompt       ="\n~OLHow may I announce you to the Island locals? ";
char *login_quit         ="\n~OL~FTOh, Well ok then.  Hope you come back soon!\n~OL~FMYou change your mind, and turn around and walk away...\n";
char *login_shortname    ="\n~OLYou'll have to choose a longer name than that!\n\n";
char *login_longname     ="\n~OLUmm... That name is a little toooo long...  How about a shorter one?\n\n";
char *login_lettersonly  ="\n~OLYou can only use letters in the name!\n\n";
char *login_welcome      ="\n~OL~FYHello!  ~FGAnd welcome to ~FM%s~FT!\n";
char *login_attempts     ="\n~OL~FRSorry, you took too many attempts to login!\n";
char *login_arrest1      ="~FR~OLThe earth opens up and %s~RS~OL~FR falls straight into hell...\n";
char *login_arrest2      ="~FR~OLThe earth opens up and you fall straight into hell...\n";
char *fairwell_prompt    ="~OL~FGThank you for comming to ~FT%s~FG, %s~RS~OL~FG!\n~OL~FYWe hope you enjoyed your stay here, Please come back soon!\n~OL~FTPlease tell all your friends about this place!\n";
char *exit_prompt        ="~OL~FW~BR[~FYLeaving %s~FW]~RS~OL~FW:";
char *user_banned_prompt ="\n\n~OL~FRYou are not welcome here at %s!\nYour Connection Is Being Disconnected.\n\n";

/* Password Prompts */

char *password_prompt  ="~OL~FMAnd what is thy ~FBsecret~FM word~FG? ~FT";
char *password_again   ="~OL~FMPlease re-enter thy ~FBsecret~FM word~FT: ";
char *password_wrong   ="~OL~FROopsie!  Wrong Password!\n";
char *password_nomatch ="~OL~FRThe Two Passwords Do Not Match!\n";
char *password_short   ="~OL~FRSorry, your password must be atleast 3 characters long!\n";
char *password_long    ="~OL~FRSorry, Your Passowrd Is Too Long!  Please choose a shorter one!\n";

