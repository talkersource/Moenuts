/***************************************************************************

             Moenuts v1.72 Prompts Configuration Include File
             (C)1998-2004 Michael Irving, All Rights Reserved
                  (C)1998-2004 Moesoft Developments Canada
                           http://www.moenuts.com

 ***************************************************************************

   Please be CAREFUL when editing some of the prompts, especially the Style
   prompts because they MUST be in a specific order otherwise they will not
   work properly.  Especially mind the %s substitution variables as their
   order IS important!   And DO NOT change the prompt name!

 ***************************************************************************/

#define PROMPT_VERSION "Moenuts v1.72 Prompts Library"

/*   Prompt Name:           Value:                                       */

/* Style Prompts ** BE CAREFUL WITH THESE ** They can be touchy! */

char *say_style            ="%s %ss~CW:~RS %s%s\n";
char *welcome_style        ="~CB(._.-(~CT %s ~RS~CB)-._.)\n";
char *show_style           ="~FGType~FY: ~FB\" ~RS%s ~FB\"\n";
char *examine_style        ="\n~FM=~FB-~FM=> ~RS%s ~RS%s ~RS\n";
char *help_levelname_style ="~FB#~CB#~FT# ~CY%s ~FT#~CB#~FB#";
char *all_clone_style      =" ~CM%-20.20s ~FW: ~FR%s \n";
char *site_style_dns_ip    ="~CT%s~FB's site is currently ~FM%s~FB, Their IP is ~FM%s\n";
char *site_style_dns       ="~CT%s~FB's site is currently ~FM%s~FB.\n";
char *site_style_offline   ="~CT%s~FB's last site was ~FM%s~FB.\n";
char *wizshout_style       ="~FG[~CGW~FYi~FGZ~RS~FG]~FB: ~FT[ ~RS%s ~RS~FT]~RS %s\n";
char *wizemote_style       ="~FG[~CGW~FYi~FGZ~RS~FG]~FB: %s %s\n";
char *greet_style          ="~CT%s ~RS~CMannounces~FT:\n\n";
char *autopromote_style    ="~CTEveryone, please welcome ~FG%s~FT to %s! :-)";

/* Miscelaneous Prompt Configuration */

char *syserror               ="~CMWARNING~CW:~CR ** A system error has occured **\n";
char *nosuchroom             ="~FTThat room wasn't thought of at this point in time!\n";
char *nosuchuser             ="~FRNobody like that has ever set foot in this talker before...\n";
char *notloggedon            ="~FRThat person isn't on the talker right now...\n";
char *invisenter             ="~FR< ~CRYou hear strange foot steps getting closer... ~FR>\n";
char *invisleave             ="";
char *invisname              ="~CBSomeone~RS";
char *invisemote             ="~CBSomeone~RS";
char *hiddenname             ="~CBSomeone~RS";
char *noswearing             ="\n~CY~BR *** We don't like language like that here *** ~RS\n";
char *user_room_prompt       ="~FB-~CB=~CT+ ~RS~FGYou have been given a room called ~FB\"~CM%s~RS~FB\"~FG, use ~FR.home ~FG to enter it.\n";
char *enterprompt            ="~CG-=+ ~FTPress ~FB[~FYENTER~FB] ~FTTo Login ~FG+=-";
char *session_swap           ="~CB[ ~FTReCoNnEcT ~CB]~FG:";
char *announce_prompt        ="\n~CW~BM[~CG~BMEntering %s~CW~BM]~RS~CT:";
char *more_prompt            ="\n~CB[ ~CT.oO( ~CB[~CWENTER~CB] ~CW= Continue, ~CB[~CWE~CB] ~CW+ ~CB[~CWENTER~CB]~CW = Exit ~CT)Oo. ~CB]~RS \n";
char *unknown_command        ="~CRHuh?  I don't understand what it is you're trying to do!\n";
char *bad_command            ="~CB% ~RS%s?  What's that supposed to do?\n";
char *denied_command         ="~CB% ~RSYou want to use %s?  Your not permitted to use such a command.\n";
char *no_message_prompt      ="~CMThere are no writings on the wall.\n";
char *single_message_prompt  ="~CMThere is only one writing on the wall.\n";
char *message_prompt         ="~CMThere is ~FT%d~FM writings on the wall.\n";
char *topic_prompt           ="~CTThe current topic is~FW:~RS %s \n";
char *no_exits               ="~CRIt seems you are trapped in here!  There is no way out.";
char *people_here_prompt     ="\n~CTOthers people here with you~FW:\n";
char *no_people_here_prompt  ="\n~FRThere are no other people here with you.\n";
char *room_not_found         ="~CB% ~FY\"~FG%s~FY\" ~FMdoes not match a room or matches more than one.\n";
char *already_in_room        ="~CGYou are already in the ~RS~OL%s!\n";
char *user_ustat_header      ="~FM-~CM= ~FTYour stats on %s ~CM=~FM-";
char *ustat_header           ="~FM-~CM= ~FT%s's Stats on %s ~CM=~FM-";
char *ustat_jail_levelname   ="~CRHells Guest";
char *help_header            ="~FGCommands available for level ~CT%s";
char *help_footer1           =" ~FTThere are ~FG%d ~FTcommands total.  ~FTYou have ~FG%d~FT commands available to you.\n";
char *help_footer2           =" ~FGFor help on a command, type:   ~CG.help <command>~FG.\n";
char *mash_prompt            ="~CW>>>>~RS %s ~RS~CRis being punnished for their actions...\n";
char *mash_forgive           ="~CW>>>>~RS %s ~RS~CRhas been re-habilitated and forgiven!\n";
char *muzzle_victim_prompt   ="~CMYikes!  You've been Muzzled!\n";
char *muzzle_user_prompt     ="~CMYou throw a muzzle on %s.\n";
char *unmuzzle_victim_prompt ="~CGWooHoo!  Your Muzzle Has Been Removed!\n";
char *unmuzzle_user_prompt   ="~CGYou take off %s~CG's muzzle.\n";
char *arrest_user_prompt     ="~CRYou damn %s ~RS~CRto hell.\n";
char *arrest_room_prompt     ="~CR%s~CR makes a phone call and ~RS%s~CR is picked up by the mafia and\n~CRstuffed into the trunk of a car!\n";
char *arrest_smail_message   ="~CMAn angry mob grab you and carry you off!!!\n";
char *unarrest_user_prompt   ="~CGYou have released %s~CG from jail.\n";
char *unarrest_victim_prompt ="~CGYou wake up just as someone opens the trunk of the car!\n";
char *suicide_prompt         ="\07~FB-~CB=~CT+ ~RS%s~CR is caught in a hail of bullets from a drive by shooting and is killed... ~CT+~CB=~FB-\n";

/* Idle Prompts */

char *idle_user_timeout ="\n\n~CR!!! ~CTYou had your chance, See Ya! ~CR!!!\n\n";
char *idle_kill_prompt1 ="~CYA bolt of lightning strikes %s~RS~CY...\n";
char *idle_kill_prompt2 ="~CYA swift breeze blows ashes through the halls...\n~CRThat'll teach'em to idle too long! ;-)\n";
char *idle_user_drift   ="~FYYou have been idling too long...\n~CTYou have been forced AFK by the system!\n";
char *idle_room_drift   ="%s~CY falls asleep and drifts off into another reality...\n";
char *idle_user_warning ="\n~CM!!! You must Input something within two minutes or be disconnected !!!\n\n";

/* Wake Prompts */

char *wake_victim_prompt ="\n~BP~FM>> ~RS%s~CT nudges you... Pssst.... Wake Up!!! ~CM<<\n\n";
char *wake_user_prompt   ="~CMYou nudge ~RS%s~CT...  Pssst...Wake Up!!!\n";

/* Promotion Prompts */

char *promote_user_prompt ="~CYYou find %s worthy of being a ~FG%s!  Congratulations!\n";
char *promote_prompt      ="~CY%s finds %s worthy of being a ~FG%s!  Congratulations!\n";
char *demote_user_prompt  ="~CMYou demote %s~RS~CM to a %s.\n";
char *demote_prompt       ="~CM%s demotes %s~RS~CM to a ~FM%s.\n";

/* Visibility Prompts */

char *appear_prompt        ="~CT%s materializes right before your eyes...\n";
char *disapear_prompt      ="~CB%s fades into the background...\n";
char *disapear_user_prompt ="~CTYou chant a mystical spell and fade into the background...\n";

/* Kill Prompts */

char *kill_user_chant  ="~CMYou whistle a strange little tune...\n";
char *kill_room_chant  ="~CM%s whistles a strange little tune...\n";
char *kill_victim_text ="~CMFrom out of nowhere, a huge dinosaur comes running upto you and devours you!!!\n";
char *kill_room_text   ="~CMFrom out of nowhere, a huge dinosaur comes running and devours %s!!!\n";
char *kill_all_text    ="~CR**BUUUURRRP!**\n~CBThe Dinosaur blushes, excuses himself, then wanders off...\n";

/* Clone Prompts */

char *clone_here_prompt   ="\n~CMYou whisper a haunting spell and create a clone of yourself here.\n";
char *clone_prompt        ="\n~CMYou whisper a haunting spell create a clone of yourself in the %s.\n";
char *clone_user_destroy  ="~CMYou whisper a sharp spell and the clone is destroyed.\n";
char *clone_room_destroy  ="~CT%s gets sucked into a black hole...\n";
char *clone_switch_prompt ="~CBYou experience a strange sensation and switch places with your clone...\n";

/* General Mail Prompts */

char *newmail_prompt ="~CM~BM!! ~FYThere is New Mail In Your Mailbox! !!~RS\07\n";
char *nomail_prompt  ="~CBSorry, but you don't have any mail.\n";
char *mailbox_total  ="~CGThere was a total of %d messages found in your mailbox.\n";

/* Smail Prompts */

char *sendmail_prompt   ="~CTYou put your letter to %s in the mailbox.  Just then...\n~CBZooooooooooom...Pony Express Zooms By And Takes Your Letter For Delivery...\n";
char *smail_edit_header ="\n~FM-~CM=~CR[ ~FTWriting mail message to ~FG%s ~CR]~CM=~FM-\n\n";

/* Dmail Prompts */

char *dmail_nomail        ="~FRYou have no mail to delete.\n";
char *dmail_all           ="~CRAll your messages have been deleted.\n";
char *dmail_too_many      ="~CRThere were only ~CM%d ~CRmessages in your mailbox, all now deleted.\n";
char *dmail_some          ="~CT%d ~FBmessages deleted from the top of your mailbox.\n";
char *dmail_bottom        ="~CR%d messages deleted from the bottom of your mailbox.\n";
char *dmail_no_number     ="~CRThere were only %d messages in your mailbox. There is not a #%d message to delete.\n";
char *dmail_one_message   ="~CROne message deleted from your mailbox.\n";
char *dmail_some_messages ="~CR%d message deleted from your mailbox.\n";

/* Message Board Read Prompts */

char *message_board_header   ="~FM-~CM=~CR[ ~CTThe ~FG%s~CT message board ~CR]~CM=~FM-";
char *read_no_messages       ="~CGYou look up, but to your amazement, the %s~CG board is empty!";
char *user_read_board_prompt ="~CM%s~RS~FT looks up and decides to read the message board.\n";

/* Message Board Write Prompts */

char *write_edit_header ="\n~FM-~CM=~CR[ ~FGWriting Babble On The Board! ~CR]~CM=~FM-\n\n";
char *user_write_end    ="~CGYou scribble something on the board...\n";
char *room_write_end    ="%s ~RS~FGwrites some scribble on the board.\n";

/* Message Wipe Prompts */

char *wipe_empty_board       ="~CRThe message board is empty.\n";
char *wipe_user_all_deleted  ="~CRAll messages on the message board have been deleted.\n";
char *wipe_room_all_deleted  ="~CM%s ~RS~FMwipes the message board clean!\n";
char *wipe_too_many          ="~CRThere were only ~CW%d ~CRmessages on the board, all messages deleted!\n";
char *wipe_deleted_top       ="~CW%d ~CMmessages deleted from the top.\n";
char *wipe_deleted_bottom    ="~CW%d ~CMmessages deleted from the bottom.\n";
char *wipe_deleted_some      ="~CW%s ~CMwipes %d messages from the wall.\n";
char *wipe_missing_number    ="~CRThere were only ~CW%d~CR messages on the board. There is not a #%d message to delete.\n";
char *wipe_user_one_message  ="~CMOne message deleted from board.\n";
char *wipe_room_one_message  ="~CW%s ~CMwiped one of the messages from the wall.\n";
char *wipe_user_delete_range ="~CW%d ~CMmessages deleted from board.\n";

/* Profile Prompts */

char *profile_start       ="~CT%s decides to write a little somthing about themselves...\n";
char *profile_end         ="~CT%s stands back and admires their crayon scribbles...\n";
char *profile_edit_header ="~CS\n~FG-~CG=~CM]~FT You are writing a profile of yourself ~CM[~CG=~FG-";
char *no_profile_prompt   ="~CMI is too lazy to write a profile right now!\n~CGEncourage this user to use .entpro\n";

/* Room Decsription Editor Prompts */

char *entroom_start       ="%s~CT gets out some paint and decides to decorate this room  a little...\n";
char *entroom_edit_header ="~FG-~CG=~CM] ~CWEntering A Room Description ~CM[~CG=~FG-";
char *entroom_end         ="%s ~CThas finished decorating this room...\n";

/* Room Entry Request Prompts */

char *user_knock_prompt      ="~CYYou grab a big stick and begin to tap on the stone door of the %s.\n";
char *room_knock_prompt      ="%s~CY grabs a big stick and begins to tap on the stone door of this room.\n";
char *user_room_knock_prompt ="%s~CY grabs a big stick and begins to tap on the stone door of the %s.\n";
char *user_join_request      ="~CYYou pick up a big stick and bang on the door.\n";
char *join_request           ="%s~CY wishes to join you!\n";

/* Various Review Buffer Prompts */

char *cbuff_prompt           ="~CM%s licked the review buffer clean with their tongue...\n~CREeewwww!  That's disgusting...\n";
char *out_of_room_cbuff      ="~CGReview buffer cleared in the %s room.\n";
char *wiz_cbuff_prompt       ="~CGYou lick the wiz review buffer clean.\n";
char *shout_cbuff_prompt     ="~CGYou lick the shout review buffer clean.\n";
char *no_review_prompt       ="~CMI see nothing here worth showing you...\n";
char *private_review_prompt  ="~CRThat room is currently private, you cannot review the conversation.\n";
char *personal_review_prompt ="~CRThat room is set to ~FTpersonal~FR you cannot review the conversation.\n";
char *review_header          ="~FM-~CM=~CR[ ~FTReview buffer for: ~FG%s ~CR]~CM=~FM-\n";

/* Tell Review */

char *tell_review_header    ="\n~FM-~CM=~CR[ ~FTReview Of Tells Sent To You ~CR]~CM=~FM-\n";
char *no_tell_review_prompt ="~CMYou Have No Tells To Review!\n";

/* Wiz Review */

char *wiz_review_header    ="~FM-~OL=~FR[ ~FTReview Of Tells Sent To The Staff ~FR]~FM=~RS~FM-";
char *no_wiz_review_prompt ="~CMThe Wizards have been quiet lately...\n";

/* Shout Review */

char *shout_review_header    ="~FM-~CM=~CR[ ~FTReview Of Shouts and Shout Emotes ~CR]~CM=~FM-";
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
char *default_level_alias ="Visitor";

/* Login / Connection Prompts */

char *login_timeout      ="\n\n~CRYou've had long enough to login!  ~FY* Thanks For Coming! *\n\n";
char *login_prompt       ="\n~CMWho are you? ";
char *login_quit         ="\n~CTOh, Well ok then.  Hope you come back soon!\n~CMYou change your mind, and turn around and walk away...\n";
char *login_shortname    ="\n~CRYou'll have to choose a longer name than that!\n\n";
char *login_longname     ="\n~CRUmm... That name is a little toooo long...  How about a shorter one?\n\n";
char *login_lettersonly  ="\n~CRYou can only use letters in the name!\n\n";
char *login_welcome      ="\n~CYHello!  ~FGAnd welcome to ~FM%s~FT!\n";
char *login_attempts     ="\n~CRSorry, you took too many attempts to login!\n";
char *login_arrest1      ="~CRLThe earth opens up and ~RS%s~CR falls straight into hell...\n";
char *login_arrest2      ="~CRThe earth opens up and you fall straight into hell...\n";
char *fairwell_prompt    ="~CGThank you for comming to ~FT%s~CG, ~RS%s~CG!\n~CYWe hope you enjoyed your stay here, Please come back soon!\n~CTPlease tell all your friends about this place!\n";
char *exit_prompt        ="~CW~BR[~CY~BRLeaving %s~CW~BR]~RS~CW:";
char *user_banned_prompt ="\n\n~CRYou are not welcome here at %s~CR!\n~CRYour Connection Is Being Disconnected.\n\n";

/* Password Prompts */

char *password_prompt  ="~CMAnd what is thy ~FBsecret~FM word~FG? ~FT";
char *password_again   ="~CMPlease re-enter thy ~FBsecret~FM word~FT: ";
char *password_wrong   ="~CROopsie!  Wrong Password!\n";
char *password_nomatch ="~CRThe Two Passwords Do Not Match!\n";
char *password_short   ="~CRSorry, your password must be atleast 3 characters long!\n";
char *password_long    ="~CRSorry, Your Passowrd Is Too Long!  Please choose a shorter one!\n";

