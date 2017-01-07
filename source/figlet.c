#include <ctype.h>
#define _FIGLET_SOURCE
  #include "figlet.h"
  #include "figlet.protos.h"
#undef _FIGLET_SOURCE

/* Write text figlets and introductions. */
void write_textfig(char *figtext,char *name,char *font)
{
UR_OBJECT u;
char fig1[ARR_SIZE],fig2[ARR_SIZE];

if (strcasecmp(font,"standard.flf")) sprintf(fig1,"~OL~FMFiglet from ~FG%s ~OL~FW<~FRfont = %s~FW>~FT: ~RS%s\n",name,font,figtext);
else sprintf(fig1,"~FTFiglet from ~FG%s~FT: ~RS%s\n",name,figtext);
record(room_first,fig1);
sprintf(fig2,"~FT Figlet from ~FG%s~FT:~RS\n",name);

for (u=user_first;u!=NULL;u=u->next) {
	if (u->login) continue;
	if (u->type==CLONE_TYPE) continue;
	write_user(u,fig2);
	}
}

/* Write figlet lines to those users who want them. */
void write_bcfig(char *fig_text)
{
UR_OBJECT u;

for (u=user_first;u!=NULL;u=u->next) {
	if (u->login) continue;
	if (u->type==CLONE_TYPE) continue;
/*	if (u->ignore & FIGLETS) continue; */
	write_user(u,fig_text);
	}
}

/* Start of figlet functions. */
#ifdef __STDC__
  char *myalloc(size_t size)
#else
  char *myalloc(int size)
#endif
{
char *ptr;

#ifndef __STDC__
extern void *malloc();
#endif
if ((ptr=(char*)malloc(size))==NULL) {
	write_syslog("ERROR: Malloc FAILED in myalloc().\n",1);
	return NULL;
	}
return ptr;
}

/* Skips to the end of a line, given a stream. */
void skiptoeol(FILE *fp)
{
int dummy;
while (dummy=getc(fp),dummy!='\n' && dummy!=EOF);
}

/* Clears both the input, (inchrline) and output (outline) storage. */
void fclearline(void)
{
int i;

for (i=0;i<charheight;i++) {
	outline[i][0]='\0';
	}
outlinelen=0;
inchrlinelen=0;
}

/* Reads a font character from the font file, and places it in a newly -
 - allocated entry in the list.                                       */
void readfontchar(FILE *file,inchr theord,char *line,int maxlen)
{
int row,k;
char endchar;
fcharnode *fclsave;

fclsave=fcharlist;
fcharlist=(fcharnode*)myalloc(sizeof(fcharnode));
fcharlist->ord=theord;
fcharlist->thechar=(char**)myalloc(sizeof(char*)*charheight);
fcharlist->next=fclsave;

for (row=0;row<charheight;row++) {
	if (fgets(line,maxlen+1,file)==NULL) {
		line[0]='\0';
		}
	k=MYSTRLEN(line)-1;
	while (k>=0 && isspace(line[k])) {
		k--;
		}
	if (k>=0) {
		endchar=line[k];
		while (k>=0?line[k]==endchar:0) {
			k--;
			}
		}
	line[k+1]='\0';
	fcharlist->thechar[row]=(char*)myalloc(sizeof(char)*(k+2));
	strcpy(fcharlist->thechar[row],line);
	}
}

/* Allocates memory, initializes variables, and reads in the font. Called -
 - near beginnging of main().                                            */
int readfont(char *fontname)
{
#define MAXFIRSTLINELEN 1000
FILE *fontfile;
int i,row,numsread,maxlen,cmtlines,ffright2left;
inchr theord;
char *fileline,magicnum[5],fontpath[256];

sprintf(fontpath,"%s/%s",FONT_DIR,fontname);
fontfile=fopen(fontpath,"r");
if (!fontfile) return -1;
fscanf(fontfile,"%4s",magicnum);
fileline=(char*)myalloc(sizeof(char)*(MAXFIRSTLINELEN+1));
if (fgets(fileline,MAXFIRSTLINELEN+1,fontfile)==NULL) {
	fileline[0]='\0';
	}
if (MYSTRLEN(fileline)>0?fileline[MYSTRLEN(fileline)-1]!='\n':0) {
	skiptoeol(stdin);
	}
numsread=sscanf(fileline,"%*c%c %d %*d %d %d %d%*[ \t]%d",&hardblank,&charheight,&maxlen,&defaultmode,&cmtlines,&ffright2left);
free(fileline);
for (i=1;i<=cmtlines;i++) {
	skiptoeol(fontfile);
	}
if (numsread<6) {  ffright2left=0;  }
if (charheight<1) {  charheight=1;  }
if (maxlen<1) {  maxlen=1;  }
maxlen+=100; /* Gives us some extra room. */
if (right2left<0) {  right2left=ffright2left;  }
if (justification<0) {  justification=2*right2left;  }
fileline=(char*)myalloc(sizeof(char)*(maxlen+1));
/* Allocate 'missing' character. */
fcharlist=(fcharnode*)myalloc(sizeof(fcharnode));
fcharlist->ord=0;
fcharlist->thechar=(char**)myalloc(sizeof(char*)*charheight);
fcharlist->next=NULL;
for (row=0;row<charheight;row++) {
	fcharlist->thechar[row]=(char*)myalloc(sizeof(char));
	fcharlist->thechar[row][0]='\0';
	}
for (theord=' ';theord<='~';theord++) {
	readfontchar(fontfile,theord,fileline,maxlen);
	}
for (theord= -255;theord<= -249;theord++) {
	readfontchar(fontfile,theord,fileline,maxlen);
	}
while (fgets(fileline,maxlen+1,fontfile)==NULL?0:sscanf(fileline,"%li",&theord)==1) {
	readfontchar(fontfile,theord,fileline,maxlen);
	}
fclose(fontfile);
free(fileline);
return 0;
}

/* Sets currchar to point to the font entry for the given character. Sets -
 - currcharwidth to the width of this character.                         */
void getletter(inchr c)
{
fcharnode *charptr;

for (charptr=fcharlist;charptr==NULL?0:charptr->ord!=c;charptr=charptr->next);
if (charptr!=NULL) {
	currchar=charptr->thechar;
	}
else {
	for (charptr=fcharlist;charptr==NULL?0:charptr->ord!=0;charptr=charptr->next) ;
	currchar=charptr->thechar;
	}
currcharwidth=MYSTRLEN(currchar[0]);
}

/* Attempts to add the given character onto the end of the current line. -
 - returns 1 if this can be done, 0 otherwise.                          */
int addchar(inchr c)
{
int smushamount,row;
char *templine;

getletter(c);
smushamount=0;
if (outlinelen+currcharwidth>outlinelenlimit || inchrlinelen+1>inchrlinelenlimit) {  return 0;  }
templine=(char*)myalloc(sizeof(char)*(outlinelenlimit+1));
for (row=0;row<charheight;row++) {
	if (right2left) {
		strcpy(templine,currchar[row]);
		strcat(templine,outline[row]+smushamount);
		strcpy(outline[row],templine);
		}
	else strcat(outline[row],currchar[row]+smushamount);
	}
free(templine);
outlinelen=MYSTRLEN(outline[0]);
inchrline[inchrlinelen++]=c;
return 1;
}

/* This function prints out the given null-terminated string, substituting -
 - blanks for hardblanks. If outputwidth is 1, prints the entire string.   -
 - Otherwise, it prints at most outputwidth-1 characters. Prints a newline -
 - at the end of the string. The string is left-justified, centered or     -
 - right-justified (taking outputwidth as the screen width) if             -
 - justification is 0,1, or 2 respectively.                               */
void putstring(char *string)
{
int i,j=0,len;
char t;

len=MYSTRLEN(string);
if (outputwidth>1) {
	if (len>outputwidth-1) {
		len=outputwidth-1;
		}
	if (justification>0) {
		for (i=1;(3-justification)*i+len+justification-2<outputwidth;i++) {
			text[j]=' ';
			j++;
			}
		}
	}
for (i=0;i<len;i++) {
	t=string[i]==hardblank?' ':string[i];
	text[j]=t;
	j++;
	}
text[j]='\n';
text[j+1]='\0';
write_bcfig(text);
}

/* This function prints outline using putstring, then clears the current line */
void printline(void)
{
int i;

for (i=0;i<charheight;i++) {
	putstring(outline[i]);
	}
fclearline();
}

/* This function splits inchrline at the last word break (bunch of consecutive -
 - blanks). Makes a new line out of the first part and prints it using print-  -
 - line. Makes a new line out of the second part and returns.                 */
void splitline(void)
{
int i,gotspace,lastspace,len1,len2;
inchr *part1,*part2;

lastspace=0;
part1=(inchr*)myalloc(sizeof(inchr)*(inchrlinelen+1));
part2=(inchr*)myalloc(sizeof(inchr)*(inchrlinelen+1));
gotspace=0;
for (i=inchrlinelen-1;i>=0;i--) {
	if (!gotspace && inchrline[i]==' ') {
		gotspace=1;
		lastspace=i;
		}
	if (gotspace && inchrline[i]!=' ') {
		break;
		}
	}
len1=i+1;
len2=inchrlinelen-lastspace-1;
for (i=0;i<len1;i++) {  part1[i]=inchrline[i];  }
for (i=0;i<len2;i++) {  part2[i]=inchrline[lastspace+1+i];  }
fclearline();
for (i=0;i<len1;i++) {  addchar(part1[i]);  }
printline();
for (i=0;i<len2;i++) {  addchar(part2[i]);  }
free(part1);
free(part2);
}

/* Ok, now we do the execution stuff. */
void figlet(UR_OBJECT user,char *inpstr)
{
inchr c;
int i=0,row,wordbreakmode,char_not_added;
char *p=inpstr,*name,fontname[256]="standard.flf";
fcharnode *fclsave;

if (user->muzzled & JAILED) {
	write_user(user,unknown_command);
	return;
	}
if (word_count<2) {
	write_user(user,"Usage: .figlet <text>\n");
	return;
	}
/* See if a font was specified. */
if (*p=='-') {
	while (*(p+i)!=' ') { /* Get size of fontname */
		i++;
		if (i==100) break;
		}
	strncpy(fontname,p+1,i);
	*(fontname+i-1)='\0';
	p=p+i+1;
	if (word_count<3) {
		write_user(user,"Usage: .figlet [-font] <text>\n");
		return;
		}
	}
justification=0;
right2left=-1;
outputwidth=80;
outlinelenlimit=outputwidth-1;
if (!strstr(fontname,".flf")) strcat(fontname,".flf");
i=readfont(fontname);
if (i==-1) {
	sprintf(text,"~OL~FB[~FT%s~FB]: ~FGfont was not found!\n",fontname);
	write_user(user,text);
	return;
	}
if (!user->vis || user->hidden) name=invisname; else name=user->recap;
write_textfig(p,name,fontname);
/* Line alloc. */
outline=(char**)myalloc(sizeof(char*)*charheight);
for (row=0;row<charheight;row++) {  outline[row]=(char*)myalloc(sizeof(char)*(outlinelenlimit+1));  }
inchrlinelenlimit=outputwidth*4+100;
inchrline=(inchr*)myalloc(sizeof(inchr)*(inchrlinelenlimit+1));
fclearline();
wordbreakmode=0;
while (*p) {
	c=*p;
	p=p+1;
	if (isascii(c) && isspace(c)) {  c=(c=='\t' || c==' ')?' ':'\n';  }
	if ((c>'\0' && c<' ' && c!='\n') || c==127) continue;
	/* NOTE the following code is complex and thoroughly tested... you -
	 - wanna fuck w/ it, you're digging your own grave.               */
	do {
		char_not_added=0;
		if (wordbreakmode==-1) {
			if (c==' ') {  break;  }
			else if (c=='\n') {
				wordbreakmode=0;
				break;
				}
			wordbreakmode=0;
			}
		if (c=='\n') {
			printline();
			wordbreakmode=0;
			}
		else if (addchar(c)) {
			if (c!=' ') {  wordbreakmode=(wordbreakmode>=2)?3:1;  }
			else {  wordbreakmode=(wordbreakmode>0)?2:0;  }
			}
		else if (outlinelen==0) {
			for (i=0;i<charheight;i++) {
				if (right2left && outputwidth>1) {
					putstring(currchar[i]+MYSTRLEN(currchar[i])-outlinelenlimit);
					}
				else {
					putstring(currchar[i]);
					}
				}
			wordbreakmode=-1;
			}
		else if (c==' ') {
			if (wordbreakmode==2) {  splitline();  }
			else {  printline();  }
			wordbreakmode=-1;
			}
		else {
			if (wordbreakmode>=2) {  splitline();  }
			else {  printline();  }
			wordbreakmode=(wordbreakmode==3)?1:0;
			char_not_added=1;
			}
		} while (char_not_added);
	}
if (outlinelen!=0) {  printline();  }
/* Now lets free up the memory we're hogging up. */
free(inchrline);
for (row=0;row<charheight;row++) free(outline[row]);
free(outline);
/* Free up the font memory. */
do {
	/* Save pointer to next node */
	fclsave=fcharlist->next;
	/* Free up memory used by this node. */
	for (row=0;row<charheight;row++) free(fcharlist->thechar[row]);
	free(fcharlist->thechar);
	free(fcharlist);
	fcharlist=fclsave;
	} while (fclsave!=NULL);
return;
}
